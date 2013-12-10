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
{
	// Add map object creation callback.
	mMap.SetNewMapObjectCB( &SpawnObjectFromXml );
}


Game::~Game() { }


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
			Vec2f topLeft = mMap.TileToWorld( it->second.tilePos );
			topLeft -= mCamera->GetPosition();

			DrawRect( topLeft.x, topLeft.y, mMap.GetTileWidth(), mMap.GetTileHeight(), Color( 0x8888AAFF ) );
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
		// Deselect the previously selected Unit.
		mSelectedUnit = nullptr;

		// Clear all selected tiles.
		mReachableTiles.clear();
	}
}


void Game::SelectReachableTilesForUnit( Unit* unit, const Vec2i& tilePos, int totalCostToEnter, CardinalDirection previousTileDirection, int movementRange )
{
	// Select the current tile.
	int tileIndex = ( tilePos.x + ( tilePos.y * mMap.GetTileWidth() ) );

	TileInfo tileInfo;
	tileInfo.tilePos = tilePos;
	tileInfo.bestTotalCostToEnter = totalCostToEnter;
	tileInfo.previousTileDirection = previousTileDirection;

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
			// Calculate the cost to enter the adjacent tile.
			// TODO: Actually make this use real cost.
			TerrainType* adjacentType = GetTerrainTypeOfTile( adjacentPos );
			int costToEnterAdjacentTile = 1;

			if( costToEnterAdjacentTile <= movementRange )
			{
				int totalCostToEnterAdjacentTile = ( totalCostToEnter + costToEnterAdjacentTile );

				// Search for an existing cost entry for this tile.
				int adjacentIndex = ( adjacentPos.x + ( adjacentPos.y * mMap.GetTileWidth() ) );
				auto existingTileInfo = mReachableTiles.find( adjacentIndex );

				if( existingTileInfo == mReachableTiles.end() ||
					totalCostToEnterAdjacentTile < existingTileInfo->second.bestTotalCostToEnter )
				{
					// If the unit is able to enter this tile, add it to the list
					// of reachable tiles and keep searching.
					SelectReachableTilesForUnit( unit, adjacentPos, totalCostToEnterAdjacentTile, GetOppositeDirection( direction ), movementRange - costToEnterAdjacentTile );
				}
			}
		}
	}
}


void Game::OnTouchEvent( float x, float y )
{
	static void( *fn )( Unit* ) = []( Unit* unit ) -> void { unit->DrawSelected = false; };
	MapObject* obj = mMap.GetFirstObjectAt( Vec2f( x, y ) + mCamera->GetPosition() );
	mMap.ForeachObjectOfType( fn );
	if ( obj && obj->IsExactly( Unit::TYPE ) )
	{
		// If the user taps on a Unit, select it.
		Unit* unit = (Unit*) obj;
		SelectUnit( unit );
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
	return gDatabase->TerrainTypes.FindByName( "City" );
}
