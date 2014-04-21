#include "androidwars.h"

using namespace mage;


Game::Game() :
	mStatus( STATUS_NOT_STARTED ),
	mCamera( nullptr ),
	mCurrentTurnIndex( -1 ),
	mCurrentPlayerIndex( -1 )
{ }


Game::~Game()
{
	// Clean up the Game (to be safe).
	Destroy();
}


void Game::Init( Map* map )
{
	assertion( !IsInitialized(), "Cannot initialize Game that has already been initialized!" );

	// Store a pointer to the Map for this Game.
	mMap = map;
	assertion( mMap, "Cannot create game without a valid Map!" );

	for( int i = 0; i < mPlayers.size(); ++i )
	{
		// Validate Players.
		Player* player = GetPlayerByIndex( i );
		Faction* faction = player->GetFaction();

		assertion( player->GetFaction(), "Cannot initialize Game because Player %d does not control a Faction!", i );
		assertion( faction->GetMap() == mMap, "Cannot initialize Game because Player %d controls a Faction that is not part of the current Map!", i );
	}

	// Make sure the number of Players makes sense.
	size_t playerCount = GetPlayerCount();
	size_t maxPlayerCount = mMap->GetFactionCount();

	assertion( maxPlayerCount >= MIN_PLAYER_COUNT, "Cannot start Game with fewer than %d players! (%d requested)", MIN_PLAYER_COUNT, maxPlayerCount );
	assertion( maxPlayerCount <= maxPlayerCount, "Cannot start Game with %d players because there are only %d Factions for the current Map!", playerCount, maxPlayerCount );

	// Make sure the game hasn't been started yet.
	assertion( IsNotStarted(), "Cannot start Game that has already been started!" );

	// Start the game.
	mStatus = STATUS_IN_PROGRESS;

	// Reset the game to the first turn and set the starting Player index.
	mCurrentTurnIndex = -1;
	mCurrentPlayerIndex = -1;

	// Start the first turn.
	NextTurn();
}


bool Game::IsInitialized() const
{
	return ( mMap != nullptr );
}


Game::Status Game::GetStatus() const
{
	return mStatus;
}


bool Game::IsNotStarted() const
{
	return ( mStatus == STATUS_NOT_STARTED );
}


bool Game::IsInProgress() const
{
	return ( mStatus == STATUS_IN_PROGRESS );
}


bool Game::IsGameOver() const
{
	return ( mStatus == STATUS_GAME_OVER );
}


void Game::Destroy()
{
	// TODO
}


void Game::LoadState( const rapidjson::Document& gameData )
{
	// TODO: Load turn info.

	if( gameData.HasMember( "map" ) )
	{
		const rapidjson::Value& mapObject = gameData[ "map" ];

		if( mapObject.IsObject() )
		{
			// Load Map state.
			mMap->LoadFromJSON( gameData );
		}
		else
		{
			WarnFail( "Could not load Map data from JSON the \"map\" property is not an object!" );
		}
	}
	else
	{
		WarnFail( "Could not load Map data from JSON because no \"map\" object was found!" );
	}
}


void Game::SaveState( rapidjson::Document& result )
{
	// TODO: Save Game info.
	rapidjson::Value mapData;
	mapData.SetObject();
	result.AddMember( "map", mapData, result.GetAllocator() );

	// Save the Map.
	mMap->SaveToJSON( result, mapData );
}

/*
void Game::PostTurn()
{
	// Serialize the game.
	rapidjson::Document gameState;
	gameState.SetObject();
	SaveState( gameState );
	DebugPrintf( ConvertJSONToString( gameState ).c_str() );

	// Create a new turn on the service.
	gOnlineGameClient->PostTurn( ConvertJSONToString( gameState ) );
}


void Game::FetchTurn()
{

}
*/


void Game::StartTurn()
{
	DebugPrintf( "Starting turn %d. It is Player %d's turn.", mCurrentTurnIndex, mCurrentPlayerIndex );

	// Get the current player.
	Player* player = GetCurrentPlayer();
	assertion( player, "Cannot start turn because current Player index (%d) is invalid!", mCurrentPlayerIndex );

	// Let the current Player's Faction process the next turn.
	Faction* faction = player->GetFaction();
	faction->OnTurnStart( mCurrentTurnIndex );
}


void Game::EndTurn()
{
	DebugPrintf( "Ending turn %d.", mCurrentTurnIndex );

	// Get the current player.
	Player* player = GetCurrentPlayer();
	assertion( player, "Cannot end turn because current Player index (%d) is invalid!", mCurrentPlayerIndex );

	// Notify the current Player's Faction that the turn is over.
	Faction* faction = player->GetFaction();
	faction->OnTurnEnd( mCurrentTurnIndex );
}


Player* Game::CreatePlayer( Faction* faction )
{
	assertion( faction, "Cannot create Player because no Faction to control was specified!" );

	// Create a new Player.
	Player* player = new Player( this, faction );

	// Add the player to the list of Players.
	mPlayers.push_back( player );

	return player;
}


Player* Game::GetPlayerByIndex( int index ) const
{
	Player* result = nullptr;

	if( index >= 0 && index < (int) mPlayers.size() )
	{
		result = mPlayers[ (size_t) index ];
	}

	return result;
}


int Game::GetCurrentPlayerIndex() const
{
	return mCurrentPlayerIndex;
}


Player* Game::GetCurrentPlayer() const
{
	return GetPlayerByIndex( mCurrentPlayerIndex );
}


size_t Game::GetPlayerCount() const
{
	return mPlayers.size();
}


/*
void Game::SelectReachableTilesForUnit( Unit* unit, const Vec2i& tilePos, int totalCostToEnter, CardinalDirection previousTileDirection, int movementRange )
{
	assertion( unit, "Cannot select tiles for null Unit!" );

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
			//DebugPrintf( "Adjacent tile is valid!" );

			// Get the TerrainType of the adjacent tile.
			TerrainType* adjacentType = GetTerrainTypeOfTile( adjacentPos );
			assertion( adjacentType, "Could not find terrain type of tile (%d,%d)!", adjacentPos.x, adjacentPos.y );

			// Calculate the cost to enter the adjacent tile.
			int costToEnterAdjacentTile = unit->GetMovementCostAcrossTerrain( adjacentType );
			//DebugPrintf( "Unit %s move into tile (%d, %d) with cost %d (TerrainType \"%s\").", ( unit->CanMoveAcrossTerrain( adjacentType ) ? "CAN" : "CANNOT" ), adjacentPos.x, adjacentPos.y, costToEnterAdjacentTile, adjacentType->GetName().GetString().c_str() );

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
		//DebugPrintf( "Tracing path through selected tiles: (%d, %d)", tileInfo->tilePos.x, tileInfo->tilePos.y );
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
	PostMessage( "On my way!", GetCurrentPlayer()->GetColor() );
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
*/


void Game::EnforceVictoryConditions()
{
	// TODO
}


Map* Game::GetMap() const
{
	return mMap;
}


Scenario* Game::GetScenario() const
{
	return mMap->GetScenario();
}


void Game::NextTurn()
{
	assertion( mStatus == STATUS_IN_PROGRESS, "Cannot advance turn for Game that is not in progress!" );

	if( mCurrentTurnIndex > -1 )
	{
		// If this isn't the first turn, end the previous turn.
		OnTurnEnd.Invoke( mCurrentTurnIndex, GetCurrentPlayer() );
	}

	// Increment the turn counter.
	++mCurrentTurnIndex;

	// Choose the next Player to take a turn.
	mCurrentPlayerIndex = ( ( mCurrentPlayerIndex + 1 ) % GetPlayerCount() );

	// Start the next turn.
	StartTurn();
}
