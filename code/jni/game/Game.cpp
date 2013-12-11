#include "androidwars.h"

using namespace mage;


const char* Game::MAPS_FOLDER_PATH = "maps";
const char* Game::MAP_FILE_EXTENSION = "tmx";


Game* Game::Create( int numPlayers, const std::string& mapName )
{
	// Create a new Game.
	Game* game = new Game();

	// Set the map for this Game.
	game->SetMapName( mapName );

	for( int i = 0; i < numPlayers; ++i )
	{
		// Create new players for the Game.
		Player* player = new Player();
		game->AddPlayer( player );
	}

	return game;
}


std::string Game::FormatMapPath( const std::string& mapName )
{
	std::stringstream formatter;
	formatter << MAPS_FOLDER_PATH << "/" << mapName << "." << MAP_FILE_EXTENSION;
	return formatter.str();
}


Game::Game()
	: mNextPlayerIndex( 0 )
	, mStatus( STATUS_NOT_STARTED )
	, mCamera( nullptr )
	, mSelectedUnit( nullptr )
{
	// Add map object creation callback.
	mMap.SetNewMapObjectCB( &SpawnObjectFromXml );

	// Create dialogs
	mMoveDialog = Widget::LoadWidget( "ui/move_dialog.xml" );

	// Hide them
	HideAllDialogs();

	// Register events
	RegisterObjectEventFunc( Game, ConfirmMoveEvent );
	RegisterObjectEventFunc( Game, CancelMoveEvent );
}


Game::~Game()
{
	EventManager::UnregisterObjectForAllEvent( *this );
}


void Game::Start()
{
	int numPlayers = GetNumPlayers();

	DebugPrintf( "Starting Game with %d players...", numPlayers );

	// Make sure a valid map was specified.
	assertion( !mMapName.empty(), "Cannot start Game because no map name was specified!" );

	// Make sure the number of Players makes sense.
	assertion( numPlayers >= MIN_PLAYERS, "Cannot start Game with fewer than %d players! (%d requested)", MIN_PLAYERS, numPlayers );
	assertion( numPlayers <= MAX_PLAYERS, "Cannot start Game with more than %d players! (%d requested)", MAX_PLAYERS, numPlayers );

	// Make sure the game hasn't been started yet.
	assertion( IsNotStarted(), "Cannot start Game that has already been started!" );

	// Start the game.
	mStatus = STATUS_IN_PROGRESS;

	// Create a TileMap for this Game.
	std::string pathToMapFile = FormatMapPath( mMapName );
	mMap.Load( pathToMapFile.c_str() );

	mCamera->SetWorldBounds( mMap.GetMapBounds() );

	// Spawn a test Unit. TODO remove this
	UnitType* unitType = gDatabase->UnitTypes.FindByName( "Infantry" );
	Unit* testUnit = SpawnUnit( unitType, 5, 5 );
	mMap.AddMapObject( testUnit );

}


void Game::OnUpdate( float dt )
{
	if( IsInProgress() )
	{
		// Update the world.
		mMap.OnUpdate( dt );
	}
}


void Game::OnDraw()
{
	if( IsInProgress() && mCamera )
	{
		// Draw the world.
		mMap.OnDraw( *mCamera );

		for( auto it = mReachableTiles.begin(); it != mReachableTiles.end(); ++it )
		{
			// Draw the currently selected tiles.
			Vec2f topLeft = ( mMap.TileToWorld( it->second.tilePos ) - mCamera->GetPosition() );
			DrawRect( topLeft.x, topLeft.y, mMap.GetTileWidth(), mMap.GetTileHeight(), Color( 0x8888AAFF ) );
		}

		if( mSelectedPath.IsValid() )
		{
			for( size_t i = 0, numWaypoints = mSelectedPath.GetNumWaypoints(); i < numWaypoints; ++i )
			{
				// Draw all tiles in the currently selected path.
				Vec2i tilePos = mSelectedPath.GetWaypoint( i );
				Vec2f topLeft = ( mMap.TileToWorld( tilePos ) - mCamera->GetPosition() );
				DrawRect( topLeft.x, topLeft.y, mMap.GetTileWidth(), mMap.GetTileHeight(), Color( 0x88FF0000 ) );
			}
		}
	}
}


void Game::AddPlayer( Player* player )
{
	// Make sure the Player is not in the Game already.
	assertion( !HasPlayer( player ), "Cannot add Player to Game that is already part of the Game!" );
	assertion( !player->HasGame(), "Cannot add Player to Game that is already part of another Game!" );

	// Add the player to the list of Players.
	mPlayers.push_back( player );

	// Notify the Player that it is part of the Game and assign it a unique index.
	player->mGame = this;
	player->mIndex = mNextPlayerIndex;
	++mNextPlayerIndex;
}


MapObject* Game::SpawnObjectFromXml( const XmlReader::XmlReaderIterator& xmlIterator )
{
	MapObject* result = nullptr;

	// Get the name of the element in question.
	std::string name = xmlIterator.GetAttributeAsString( "name", "" );
	std::string type = xmlIterator.GetAttributeAsString( "type", "" );

	if( type == "Unit" )
	{
		// Spawn a new unit with the specified UnitType.
		result = new Unit( name );

		// Load generic properties
		TileMap::LoadDefaultMapObjectFields( *result, xmlIterator );
	}
	// Object is unknown - just use the default loading
	else
	{
		result = TileMap::DefaultNewMapObjectFn( xmlIterator );
	}

	return result;
}


Unit* Game::SpawnUnit( UnitType* unitType, int x, int y )
{
	// Create a new Unit of the specified type at the specified location on the map.
	Unit* unit = new Unit( unitType );
	unit->SetTilePos( x, y );

	// Initialize the Unit.
	unit->Init();

	return unit;
}


void Game::SelectUnit( Unit* unit )
{
	if( unit )
	{
		// Select the unit.
		unit->DrawSelected = true;
		mSelectedUnit = unit;

		// Select all reachable tiles from this Unit's position.
		SelectReachableTilesForUnit( unit, unit->GetTilePos(), 0, INVALID_DIRECTION, unit->GetMovementRange() );

		DebugPrintf( "Selected unit \"%s\"", unit->GetName().c_str() );

		for( auto it = mReachableTiles.begin(); it != mReachableTiles.end(); ++it )
		{
			TileInfo& info = it->second;
			DebugPrintf( "Tile (%d, %d) is reachable (best cost of %d)!", info.tilePos.x, info.tilePos.y, info.bestTotalCostToEnter );
		}
	}
	else
	{
		if ( mSelectedUnit )
		{
			mSelectedUnit->DrawSelected = false;
		}
		// Deselect the previously selected Unit.
		mSelectedUnit = nullptr;

		// Clear all selected tiles.
		mReachableTiles.clear();

		// Clear the currently selected path.
		mSelectedPath.Clear();
	}
}


void Game::SelectReachableTilesForUnit( Unit* unit, const Vec2i& tilePos, int totalCostToEnter, CardinalDirection previousTileDirection, int movementRange )
{
	// Select the current tile.
	int tileIndex = GetIndexOfTile( tilePos );

	TileInfo tileInfo;
	tileInfo.tilePos = tilePos;
	tileInfo.bestTotalCostToEnter = totalCostToEnter;
	tileInfo.previousTileDirection = previousTileDirection;

	// Replace any existing tile info with the updated info.
	mReachableTiles[ tileIndex ] = tileInfo;

	for( int i = FIRST_VALID_DIRECTION; i <= LAST_VALID_DIRECTION; ++i )
	{
		CardinalDirection direction = (CardinalDirection) i;

		// Don't search the previous tile again.
		if( direction == previousTileDirection )
			continue;

		// Search all surrounding tiles to see if they are pathable.
		Vec2i adjacentPos = GetAdjacentTilePos( tilePos, direction );
		MapTile& adjacentTile = GetTile( adjacentPos );

		if( adjacentTile != TileMap::INVALID_TILE )
		{
			// Get the TerrainType of the adjacent tile.
			TerrainType* adjacentType = GetTerrainTypeOfTile( adjacentPos );

			// Calculate the cost to enter the adjacent tile.
			int costToEnterAdjacentTile = unit->GetMovementCostAcrossTerrain( adjacentType );
			DebugPrintf( "Unit %s move into tile (%d, %d) with cost %d (TerrainType \"%s\").", ( unit->CanMoveAcrossTerrain( adjacentType ) ? "CAN" : "CANNOT" ), adjacentPos.x, adjacentPos.y, costToEnterAdjacentTile, adjacentType->GetName().GetString().c_str() );

			if( unit->CanMoveAcrossTerrain( adjacentType ) )
			{
				if( costToEnterAdjacentTile <= movementRange )
				{
					int totalCostToEnterAdjacentTile = ( totalCostToEnter + costToEnterAdjacentTile );

					// Search for an existing cost entry for this tile.
					TileInfo* existingTileInfo = GetReachableTileInfo( adjacentPos );

					if( existingTileInfo == nullptr || totalCostToEnterAdjacentTile < existingTileInfo->bestTotalCostToEnter )
					{
						// If the unit is able to enter this tile and this way is optimal,
						// add it to the list of reachable tiles and keep searching.
						SelectReachableTilesForUnit( unit, adjacentPos, totalCostToEnterAdjacentTile, GetOppositeDirection( direction ), movementRange - costToEnterAdjacentTile );
					}
				}
			}
		}
	}
}


void Game::FindBestPathToTile( const Vec2i& tilePos, Path& result ) const
{
	// Make sure the destination tile is currently selected.
	assertion( TileIsReachable( tilePos ), "Cannot get best path to unreachable tile (%d, %d)!", tilePos.x, tilePos.y );

	// Clear the return variable.
	result.Clear();

	for( const TileInfo* tileInfo = GetReachableTileInfo( tilePos );
		 tileInfo != nullptr && tileInfo->previousTileDirection != INVALID_DIRECTION;
		 tileInfo = GetReachableTileInfo( GetAdjacentTilePos( tileInfo->tilePos, tileInfo->previousTileDirection ) ) )
	{
		// Construct a path through the selected tiles back to the starting tile.
		result.AddWaypoint( tileInfo->tilePos );
		DebugPrintf( "Tracing path through selected tiles: (%d, %d)", tileInfo->tilePos.x, tileInfo->tilePos.y );
	}

	// Make sure the path that was found is valid.
	assertion( result.IsValid(), "Could not find valid Path to tile (%d, %d) through selected tiles!", tilePos.x, tilePos.y );
}


void Game::MoveUnitToTile( Unit* unit, const Vec2i& tilePos )
{
	// Move the unit to the destination tile.
	// TODO: Apply fuel penalty.
	// TODO: Play movement animation.
	unit->SetTilePos( tilePos );
	DebugPrintf( "Moving Unit \"%s\" to tile (%d, %d).", unit->GetName().c_str(), tilePos.x, tilePos.y );
}


void Game::OnTouchEvent( float x, float y )
{
	// A widget is blocking input
	if ( WidgetIsOpen() )
		return;

	// Get the position of the tile that was tapped.
	Vec2f worldPos = ( Vec2f( x, y ) + mCamera->GetPosition() );
	Vec2i tilePos = mMap.WorldToTile( worldPos );
	MapTile tile = GetTile( tilePos );

	// See if a Unit was tapped.
	MapObject* obj = mMap.GetFirstObjectAt( worldPos );

	if ( obj && obj->IsExactly( Unit::TYPE ) )
	{
		// If the user taps on a Unit, deselect the currently selected unit (if any).
		SelectUnit( nullptr );

		// Select the unit that was tapped.
		Unit* unit = (Unit*) obj;
		SelectUnit( unit );
	}
	else if( tile != TileMap::INVALID_TILE && TileIsReachable( tilePos ) )
	{
		// Find the best path to this tile.
		FindBestPathToTile( tilePos, mSelectedPath );

		// Show the confirm dialog.
		ShowMoveDialog();
	}
	else
	{
		// Deselect the currently selected unit (if any).
		SelectUnit( nullptr );
	}
}


TerrainType* Game::GetTerrainTypeOfTile( int x, int y )
{
	MapTile tile = mMap.GetTile( x, y, TERRAIN_LAYER_INDEX );
	assertion( tile != TileMap::INVALID_TILE, "Cannot get TerrainType of invalid Tile (%d, %d)!", x, y );

	// TODO: Make this actually use the properties of the Tile to determine TerrainType.
	HashString terrainTypeName = tile.GetPropertyAsString( "TerrainType" );
	return gDatabase->TerrainTypes.FindByName( terrainTypeName );
}

bool Game::WidgetIsOpen() const
{
	// Put all the dialogs that block game input here...
	return mMoveDialog->Visible;
}

void Game::HideAllDialogs()
{
	mMoveDialog->Hide();
}

// Events

ObjectEventFunc( Game, ConfirmMoveEvent )
{
	// Move the unit to its intended destination.
	MoveUnitToTile( mSelectedUnit, mSelectedPath.GetDestination() );

	// Clear the currently selected unit.
	SelectUnit( nullptr );

	// Hide the movement dialog.
	mMoveDialog->Hide();
}

ObjectEventFunc( Game, CancelMoveEvent )
{
	// Clear the currently selected path.
	mSelectedPath.Clear();

	// Hide the movement dialog.
	mMoveDialog->Hide();
}
