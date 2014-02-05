#include "androidwars.h"

#   include <EGL/egl.h>
#   include <GLES/gl.h>
#   include <GLES2/gl2.h>
#   include <GLES2/gl2ext.h>

using namespace mage;


const char* Game::MAPS_FOLDER_PATH = "maps";
const char* Game::MAP_FILE_EXTENSION = "tmx";
const float Game::GAME_MESSAGE_LENGTH = 5;	// in sec


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
	, mDatabase( nullptr )
	, mCamera( nullptr )
	, mSelectedUnit( nullptr )
	, mTargetUnit( nullptr )
	, mCurrentTurnIndex( -1 )
	, mCurrentPlayerIndex( -1 )
	, mUnitMotionInProgress( false )
{
	// Create the game Database.
	mDatabase = new Database();

	// Add map object creation callback.
	mMap.SetNewMapObjectCB( &SpawnObjectFromXml );

	// Create dialogs
	mMoveDialog     = Widget::LoadWidget( "ui/move_dialog.xml" );
	mAttackDialog   = Widget::LoadWidget( "ui/attack_dialog.xml" );
	mCaptureDialog  = Widget::LoadWidget( "ui/capture_dialog.xml" );
	mGameOverDialog = Widget::LoadWidget( "ui/game_end.xml" );
	mUnitDialog     = Widget::LoadWidget( "ui/unit_dialog.xml" );

	// Hide them
	HideAllDialogs();
	mCaptureDialog->Hide();
	mUnitDialog->Hide();

	// Register events
	RegisterObjectEventFunc( Game, ConfirmMoveEvent );
	RegisterObjectEventFunc( Game, CancelMoveEvent );
	RegisterObjectEventFunc( Game, ConfirmAttackEvent );
	RegisterObjectEventFunc( Game, CancelAttackEvent );
	RegisterObjectEventFunc( Game, ConfirmCaptureEvent );
	RegisterObjectEventFunc( Game, BuyEnforcementsEvent );

	mDefaultFont = new BitmapFont( "fonts/small.fnt" );
}


Game::~Game()
{
	// Clean up the Game (to be safe).
	Destroy();
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

	// Reset the game to the first turn and set the starting Player index.
	mCurrentTurnIndex = -1;
	mCurrentPlayerIndex = -1;

	// Create a TileMap for this Game.
	std::string pathToMapFile = FormatMapPath( mMapName );
	mMap.Load( pathToMapFile.c_str() );

	mMap.ForeachObjectOfType< Unit >( [this]( Unit* unit )
	{
		if( !unit->IsInitialized() )
		{
			// Initialize all Units loaded from the TileMap.
			unit->Init( this );
		}
	});

	mCamera->SetWorldBounds( mMap.GetMapBounds() );

	// TODO this should be more flexible with data in a perfect world
	ArrayList< TileMap::MapTile* > tn, tr, tb;
	int n = mMap.GetTilesById( tn, CITY_N_ID, TERRAIN_LAYER_INDEX );
	int r = mMap.GetTilesById( tr, CITY_R_ID, TERRAIN_LAYER_INDEX );
	int b = mMap.GetTilesById( tb, CITY_B_ID, TERRAIN_LAYER_INDEX );
	DebugPrintf( "Cities : n=%d r=%d b=%d\n", n, r, b );
	mPlayers[0]->CitiesOwned = r;
	mPlayers[1]->CitiesOwned = b;

	/*UnitType* unitType = gDatabase->UnitTypes.FindByName( "Infantry" );
	Player* firstPlayer = GetPlayer( 0 );
	Unit* testUnit = SpawnUnit( unitType, firstPlayer, 5, 5 );
	mMap.AddMapObject( testUnit );*/

	PostMessage( "Game started!" );

	// Start the first turn.
	NextTurn();

	// Drain all AP for P2
	Player* player = GetPlayer( 1 );
	mMap.ForeachObjectOfType< Unit >( [player]( Unit* unit )
	{
		if ( unit->GetOwner() == player )
		{
			unit->ConsumeAP( 9999 );
		}
	});
}


void Game::Destroy()
{
	// Destroy all event bindings for this Game.
	EventManager::UnregisterObjectForAllEvent( *this );

	// Unload all game data.
	mDatabase->ClearData();
}


void Game::OnStartTurn()
{
	DebugPrintf( "Starting turn %d. It is Player %d's turn.", mCurrentTurnIndex, mCurrentPlayerIndex );

	Player* player = GetCurrentPlayer();
	assertion( player, "Player does not exist!" );

	// Re-gen AP
	mMap.ForeachObjectOfType< Unit >( [player]( Unit* unit )
	{
		if ( unit->GetOwner() == player )
		{
			unit->ResetAP();
		}
	});

	if ( mCurrentPlayerIndex == 0 )
		PostMessage( "It is REDS turn!", Color::RED );
	else
		PostMessage( "It is BLUES turn!", Color::BLUE );
}


void Game::OnEndTurn()
{
	DebugPrintf( "Ending turn %d.", mCurrentTurnIndex );
	Player* player = GetCurrentPlayer();
	// Get money
	player->GenerateFunds();
	// Drain all AP

	mMap.ForeachObjectOfType< Unit >( [player]( Unit* unit )
	{
		if ( unit->GetOwner() == player )
		{
			unit->ConsumeAP( 9999 );
		}
	});

	SelectUnit( 0 );
}


void Game::OnUpdate( float dt )
{
	if( IsInProgress() )
	{
		// Update the world.
		mMap.OnUpdate( dt );

		for( auto it = mUnitsToRemove.begin(); it != mUnitsToRemove.end(); ++it )
		{
			// Remove all destroyed Units.
			Unit* unit = ( *it );
			DebugPrintf( "Removing %s from the game.", unit->ToString() );

			MapObject* unitAsMapObject = (MapObject*) unit;
			mMap.RemoveObject( unitAsMapObject, true );
		}

		mUnitsToRemove.clear();

	}
	else if ( mStatus == STATUS_GAME_OVER )
	{
	}

	UpdateMessages( dt );
}


void Game::OnDraw()
{
	if( IsInProgress() && mCamera )
	{
		// Draw the world.
		mMap.OnDraw( *mCamera );

		if ( !mUnitMotionInProgress && !mReachableTiles.empty() )
		{
			// Darken screen
			DrawRect( 0, 0, 1200, 800, Color( 0x88000000 ) );
			SetAdditiveBlend();
			// Draw overlay on tiles
			for( auto it = mReachableTiles.begin(); it != mReachableTiles.end(); ++it )
			{
				// Draw the currently selected tiles.
				Vec2f topLeft = ( mMap.TileToWorld( it->second.tilePos ) - mCamera->GetPosition() );
				DrawRectOutlined( topLeft.x, topLeft.y, mMap.GetTileWidth(), mMap.GetTileHeight(), Color( 0x8888AAFF ), 1.0f, Color( 0xF888AAFF ) );
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
			// Set blending back to default
			SetDefaultBlend();
		}

		// UI
		BitmapFont* fnt = GetDefaultFont();
		for ( int i =0; i < GetNumPlayers(); ++i )
		{
			Player* player = mPlayers[i];

			int f = player->GetFunds();
			DrawTextFormat( 0, 128 + i * fnt->GetLineHeight(), fnt, player->GetPlayerColor(), "%cFunds: $%d\n", mCurrentPlayerIndex == i ? '*' : ' ', f );
		}

	}
	else if ( mStatus == STATUS_GAME_OVER )
	{
	}

	DrawMessages();
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
		// Spawn a new unit with the specified name.
		Unit* unit = new Unit( name );

		// Load generic properties
		TileMap::LoadDefaultMapObjectFields( *unit, xmlIterator );

		// Return the newly created Unit.
		result = unit;
	}
	// Object is unknown - just use the default loading
	else
	{
		result = TileMap::DefaultNewMapObjectFn( xmlIterator );
	}

	return result;
}


Unit* Game::SpawnUnit( UnitType* unitType, Player* owner, int x, int y )
{
	// Create a new Unit of the specified type at the specified location on the map.
	Unit* unit = new Unit();
	unit->SetUnitType( unitType );
	unit->SetOwner( owner );
	unit->SetTilePos( x, y );

	// Initialize the Unit.
	unit->Init( this );

	return unit;
}


void Game::SelectUnit( Unit* unit )
{
	if( unit )
	{
		Player* currentPlayer = GetCurrentPlayer();

		if ( unit->IsOwnedBy( currentPlayer ) )
		{
			// Check AP
			if ( unit->GetRemainingAP() == 0 )
				return;

			// Select the unit.
			unit->Select();
			mSelectedUnit = unit;

			Vec2f tilePos = unit->GetTilePos();
			TileMap::MapTile& tile = mMap.GetTile( tilePos.x, tilePos.y, Game::TERRAIN_LAYER_INDEX );
			int id = tile.GetTileId();
			int playerId = currentPlayer->GetIndex();
			if ( id == CITY_N_ID )
				ShowCaptureDialog();
			else if ( id == CITY_B_ID )
			{
				if ( playerId == 0 )
				{
					((Button*)mCaptureDialog->GetChildByName( "button" ))->Show();
				}
				else
					((Button*)mCaptureDialog->GetChildByName( "button" ))->Hide();
				ShowCaptureDialog();
			}
			else if ( id == CITY_R_ID )
			{
				if ( playerId == 1 )
				{
					((Button*)mCaptureDialog->GetChildByName( "button" ))->Show();
				}
				else
					((Button*)mCaptureDialog->GetChildByName( "button" ))->Hide();
				ShowCaptureDialog();
			}
			else
				mCaptureDialog->Hide();

			// Show unit info
			ShowUnitDialog();

			// Select all reachable tiles from this Unit's position.
			SelectReachableTilesForUnit( unit, unit->GetTilePos(), 0, CARDINAL_DIRECTION_NONE, unit->GetMovementRange() );

			DebugPrintf( "Selected unit %s", unit->ToString() );

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
				if ( mSelectedUnit->CanAttack( *unit ) )
				{
					mTargetUnit = unit;
					mTargetUnit->Select();
					ShowAttackDialog();
				}
			}
			else
				DebugPrintf( "No selected unit!" );
		}
	}
	else
	{
		if ( mSelectedUnit )
		{
			mSelectedUnit->Deselect();
		}
		// Deselect the previously selected Unit.
		mSelectedUnit = nullptr;
		mTargetUnit = nullptr;

		// Clear all selected tiles.
		mReachableTiles.clear();

		// Clear the currently selected path.
		mSelectedPath.Clear();

		// Hide capture dialog
		mCaptureDialog->Hide();
		mUnitDialog->Hide();
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
		 tileInfo != nullptr && tileInfo->previousTileDirection != CARDINAL_DIRECTION_NONE;
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
	unit->ConsumeAP( 1 );
	mUnitMotionInProgress = true;
	mNextPathIndex = 0;
	OnUnitReachedDestination( unit );
//	unit->SetTilePos( tilePos );
	PostMessage( "On my way!", GetCurrentPlayer()->GetPlayerColor() );
	DebugPrintf( "Moving %s to tile (%d, %d).", unit->ToString(), tilePos.x, tilePos.y );
}

void Game::OnUnitReachedDestination( Unit* unit )
{
	if ( mNextPathIndex >= mSelectedPath.GetNumWaypoints() )
	{
		mUnitMotionInProgress = false;
		SelectUnit( 0 );
		SelectUnit( unit );
	}
	else
	{
		unit->SetDestination( mSelectedPath.GetWaypoint( mNextPathIndex ) );
		++mNextPathIndex;
	}
}


void Game::CheckVictory()
{
	for ( int i = 0; i < mPlayers.size(); ++i )
	{
		if ( mPlayers[i]->HasLost() )
		{
			OnGameOver();
			break;
		}
	}
}


void Game::OnGameOver()
{
	Label* winnerText = (Label*)mGameOverDialog->GetChildByName( "winnerTxt" );
	if ( mPlayers[0]->HasLost() )
	{
		winnerText->SetText( "BLUE wins!" );
		winnerText->TextColor = Color::BLUE;
		PostMessage( "BLUE wins!", Color::BLUE );
	}
	else if ( mPlayers[1]->HasLost() )
	{
		winnerText->SetText( "RED wins!" );
		winnerText->TextColor = Color::RED;
		PostMessage( "RED wins!", Color::RED );
	}
	mGameOverDialog->Show();
}


void Game::OnTouchEvent( float x, float y )
{
	// A widget is blocking input
	if ( WidgetIsOpen() )
		return;

	// Unit is animating motion
	if ( mUnitMotionInProgress )
		return;

	// Get the position of the tile that was tapped.
	Vec2f worldPos = ( Vec2f( x, y ) + mCamera->GetPosition() );
	Vec2i tilePos = mMap.WorldToTile( worldPos );
	MapTile tile = GetTile( tilePos );

	// See if a Unit was tapped.
	MapObject* obj = mMap.GetFirstObjectAt( worldPos );

	if ( obj && obj->IsExactly( Unit::TYPE ) )
	{
		// Select the unit that was tapped.
		Unit* unit = (Unit*) obj;
		if ( unit->IsOwnedBy( GetCurrentPlayer() ) )
			SelectUnit( 0 );
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


void Game::NextTurn()
{
	assertion( mStatus == STATUS_IN_PROGRESS, "Cannot advance turn for Game that is not in progress!" );

	if( mCurrentTurnIndex > -1 )
	{
		// If this isn't the first turn, end the previous turn.
		OnEndTurn();
	}

	// Increment the turn counter.
	++mCurrentTurnIndex;

	// Choose the next Player to take a turn.
	mCurrentPlayerIndex = ( ( mCurrentPlayerIndex + 1 ) % GetNumPlayers() );

	// Start the next turn.
	OnStartTurn();
}


TerrainType* Game::GetTerrainTypeOfTile( int x, int y )
{
	MapTile tile = mMap.GetTile( x, y, TERRAIN_LAYER_INDEX );
	assertion( tile != TileMap::INVALID_TILE, "Cannot get TerrainType of invalid Tile (%d, %d)!", x, y );

	// TODO: Make this actually use the properties of the Tile to determine TerrainType.
	HashString terrainTypeName = tile.GetPropertyAsString( "TerrainType" );
	return mDatabase->TerrainTypes.FindByName( terrainTypeName );
}

bool Game::WidgetIsOpen() const
{
	// Put all the dialogs that block game input here...
	return mMoveDialog->Visible || mAttackDialog->Visible || mGameOverDialog->Visible;
}

void Game::HideAllDialogs()
{
	mMoveDialog->Hide();
	mAttackDialog->Hide();
	mGameOverDialog->Hide();
}


void Game::RemoveUnit( Unit* unit )
{
	// Add the Unit to the list of Units to remove.
	mUnitsToRemove.push_back( unit );
}


// Events

ObjectEventFunc( Game, ConfirmMoveEvent )
{
	// Move the unit to its intended destination.
	MoveUnitToTile( mSelectedUnit, mSelectedPath.GetDestination() );

	// Clear the currently selected unit.
//	SelectUnit( nullptr );

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

ObjectEventFunc( Game, ConfirmAttackEvent )
{
	assertion( mSelectedUnit, "Cannot initiate attack because no Unit is selected!" );
	assertion( mTargetUnit, "Cannot initiate attack because no target Unit was selected!" );

	PostMessage( "To victory!", GetCurrentPlayer()->GetPlayerColor() );

	// Allow the selected unit to attack the target.
	DebugPrintf( "INITIAL ATTACK:" );
	mSelectedUnit->Attack( *mTargetUnit );

	// Determine whether the target can counter-attack.
	bool targetCanCounterAttack = mTargetUnit->CanAttack( *mSelectedUnit );
	DebugPrintf( "%s %s counter-attack.", mTargetUnit->ToString(), ( targetCanCounterAttack ? "CAN" : "CANNOT" ) );

	if( targetCanCounterAttack )
	{
		// Allow the target unit to counter-attack, if possible.
		DebugPrintf( "COUNTER-ATTACK:" );
		mTargetUnit->Attack( *mSelectedUnit );
	}

	// Reset game state.
	mTargetUnit->Deselect();
	SelectUnit( 0 );
	mAttackDialog->Hide();
}

ObjectEventFunc( Game, CancelAttackEvent )
{
	mTargetUnit->Deselect();
	SelectUnit( 0 );
	mAttackDialog->Hide();
}

ObjectEventFunc( Game, ConfirmCaptureEvent )
{
	if ( mSelectedUnit )
	{
		Vec2i tilePos = mSelectedUnit->GetTilePos();
		TileMap::MapTile& tile = mMap.GetTile( tilePos.x, tilePos.y, TERRAIN_LAYER_INDEX );
		int tileId = tile.GetTileId();
		int playerId = mCurrentPlayerIndex;
		Player* player = GetCurrentPlayer();
		Player* other = mPlayers[ (mCurrentPlayerIndex + 1) % mPlayers.size() ];
//		DebugPrintf( "Capturing city: t=%d p=%d", tileId, playerId );
		if ( tileId == CITY_N_ID )
		{
			if ( playerId == 0 )
				mMap.SetTileId( CITY_R_ID + 1, tile );
			else
				mMap.SetTileId( CITY_B_ID + 1, tile );
			player->CitiesOwned++;
			PostMessage( "City Captured!", player->GetPlayerColor() );
			mSelectedUnit->ConsumeAP( 1 );
		}
		else if ( tileId == CITY_R_ID )
		{
			if ( playerId == 1 )
			{
				mMap.SetTileId( CITY_N_ID + 1, tile );
				other->CitiesOwned--;
				PostMessage( "City Neutralized!", Color::GREY );
				mSelectedUnit->ConsumeAP( 1 );
			}
		}
		else if ( tileId == CITY_B_ID )
		{
			if ( playerId == 0 )
			{
				mMap.SetTileId( CITY_N_ID + 1, tile );
				other->CitiesOwned--;
				PostMessage( "City Neutralized!", Color::GREY );
				mSelectedUnit->ConsumeAP( 1 );
			}
		}


	}
	mCaptureDialog->Hide();
	SelectUnit( 0 );
}

void Game::PostMessage( const std::string& msg, const Color& color )
{
	GameMessage gmsg;
	gmsg.msg = msg;
	gmsg.color = color;
	gmsg.age = GAME_MESSAGE_LENGTH;
	mMessageQueue.push_back( gmsg );
}

void Game::PostMessageFormat( const Color& color, const char* msg, ... )
{
	char textFormatBuffer[ 1024 ];

	// Apply text formating
	va_list vargs;
	va_start( vargs, msg );
	vsprintf_s( textFormatBuffer, msg, vargs );
	va_end( vargs );

	PostMessage( textFormatBuffer, color );
}

void Game::DrawMessages()
{
	BitmapFont* fnt = GetDefaultFont();
	float dy = 0;
	float h = fnt->GetLineHeight( 0.75f );
	DrawRectOutlined( 1, 552 - h * 10, 256, h * 10, Color( 0x70000000 ), 1.0f, Color::BLACK );
	int n = mMessageQueue.size();
	n = n > 10 ? 10 : n;
	for ( int i =  n - 1; i >= 0; --i )
	{
		GameMessage& gmsg = mMessageQueue[ i ];
		int a = 255;
		if ( gmsg.age <= 1.0f )
			a = (uint8) ( gmsg.age / 1.0f * 255 );
		Color c = gmsg.color;
		c.a = a;
		dy += fnt->GetLineHeight( gmsg.msg.c_str(), 0.75f, 256 );
		DrawText( 1, 552 - dy, fnt, c, 0.75f, 256, gmsg.msg.c_str() );
	}
}

void Game::UpdateMessages( float dt )
{
	int n = mMessageQueue.size();
	n = n > 10 ? 10 : n;
	for ( int i = 0; i < n; ++i )
	{
		GameMessage& gmsg = mMessageQueue[ i ];
		gmsg.age -= dt;
	}

	mMessageQueue.erase(
		std::remove_if( mMessageQueue.begin(), mMessageQueue.end(), [&]( const GameMessage& gmsg )
		{
			return gmsg.age <= 0;
		}), mMessageQueue.end() );
}

void Game::ShowCaptureDialog() const
{
	if ( mSelectedUnit )
	{
		Label* ownerText = (Label*) mCaptureDialog->GetChildByName( "ownerTxt" );
		Vec2i tilePos = mSelectedUnit->GetTilePos();
		TileMap::MapTile& tile = mMap.GetTile( tilePos.x, tilePos.y, TERRAIN_LAYER_INDEX );
		int tileId = tile.GetTileId();
		if ( tileId == CITY_N_ID )
		{
			ownerText->SetText( "None" );
			ownerText->TextColor = Color::GREY;
		}
		else if ( tileId == CITY_R_ID )
		{
			ownerText->SetText( "Red" );
			ownerText->TextColor = Color::RED;
		}
		else if ( tileId == CITY_B_ID )
		{
			ownerText->SetText( "Blue" );
			ownerText->TextColor = Color::BLUE;
		}
	}
	mCaptureDialog->Show();
}

void Game::ShowUnitDialog() const
{
	Label* nameText = (Label*)mUnitDialog->GetChildByName( "nameTxt" );
	Label* hpText   = (Label*)mUnitDialog->GetChildByName( "hpTxt" );
	Label* apText   = (Label*)mUnitDialog->GetChildByName( "apTxt" );
	Player* player  = GetCurrentPlayer();

	if ( mSelectedUnit )
	{
		if ( player->GetFunds() < 100 || mSelectedUnit->GetRemainingAP() == 0 || mSelectedUnit->GetHP() == mSelectedUnit->GetTotalHP() )
			((Button*)mUnitDialog->GetChildByName( "button" ))->Disable();
		else
			((Button*)mUnitDialog->GetChildByName( "button" ))->Enable();

		nameText->SetText( mSelectedUnit->ToString() );
		nameText->TextColor = player->GetPlayerColor();
		hpText->SetText( std::string( "HP: " ) + StringUtil::ToString( mSelectedUnit->GetHP() ) + "/" + StringUtil::ToString( mSelectedUnit->GetTotalHP() ) );
		apText->SetText( std::string( "AP: " ) + StringUtil::ToString( mSelectedUnit->GetRemainingAP() ) + "/" + StringUtil::ToString( mSelectedUnit->GetTotalAP() ) );
	}

	mUnitDialog->Show();
}

ObjectEventFunc( Game, BuyEnforcementsEvent )
{
	Player* player = GetCurrentPlayer();
	player->AddFunds( -100 );
	mSelectedUnit->TakeDamage( -1 );
	mSelectedUnit->ConsumeAP( 1 );
	Unit* u = mSelectedUnit;
	SelectUnit( 0 );
	SelectUnit( u );
}
