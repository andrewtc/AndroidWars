#include "androidwars.h"

using namespace mage;


Game::Game() :
	mStatus( STATUS_NOT_STARTED ),
	mCamera( nullptr ),
	mCurrentTurnIndex( -1 ),
	mCurrentFactionIndex( -1 )
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

	// Make sure there are at least two controllable Factions.
	size_t controlledFactionCount = GetControlledFactionCount();
	assertion( controlledFactionCount >= MIN_CONTROLLED_FACTION_COUNT, "Cannot start Game with fewer than %d controllable Faction(s)! (The number of Factions currently controlled by a Player is %d.)", MIN_CONTROLLED_FACTION_COUNT, controlledFactionCount );

	// Make sure the game hasn't been started yet.
	assertion( IsNotStarted(), "Cannot start Game that has already been started!" );

	// Start the game.
	mStatus = STATUS_IN_PROGRESS;

	// Reset the game to the first turn and set the starting Faction index.
	mCurrentTurnIndex = -1;
	mCurrentFactionIndex = -1;

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
	DebugPrintf( "Starting turn %d. It is Faction %d's turn.", mCurrentTurnIndex, mCurrentFactionIndex );

	// Get the current Faction.
	Faction* faction = GetCurrentFaction();
	assertion( faction, "Cannot start turn because current Faction index (%d) is invalid!", mCurrentFactionIndex );

	// Let the current Faction process the next turn.
	faction->OnTurnStart( mCurrentTurnIndex );
}


void Game::EndTurn()
{
	DebugPrintf( "Ending turn %d.", mCurrentTurnIndex );

	// Get the current Faction.
	Faction* faction = GetCurrentFaction();
	assertion( faction, "Cannot end turn because current Faction index (%d) is invalid!", mCurrentFactionIndex );

	// Notify the current Faction that the turn is over.
	faction->OnTurnEnd( mCurrentTurnIndex );
}


Player* Game::CreatePlayer()
{
	// Create a new Player.
	Player* player = new Player( this );

	// Add the player to the list of Players.
	mPlayers.push_back( player );

	return player;
}


const Game::Players& Game::GetPlayers() const
{
	return mPlayers;
}


size_t Game::GetPlayerCount() const
{
	return mPlayers.size();
}


void Game::DestroyPlayer( Player* player )
{
	assertion( player->GetGame() == this, "Cannot destroy Player created by another Game!" );
	delete player;
}


void Game::DestroyAllPlayers()
{
	for( auto it = mPlayers.begin(); it != mPlayers.end(); ++it )
	{
		Player* player = *it;
		delete player;
	}

	mPlayers.clear();
}


void Game::GivePlayerControlOfFaction( Player* player, Faction* faction )
{
	assertion( player, "Cannot give null Player control of Faction!" );
	assertion( faction, "Cannot give Player control of null Faction!" );
	assertion( player->GetGame() == this, "Cannot give Player created by another Game control of Faction!" );
	assertion( !IsInitialized() || faction->GetMap() == mMap, "Cannot give Player control of Faction created by another Map!" );
	assertion( faction->IsControllable(), "Cannot give Player control of uncontrollable Faction!" );

	// Force the Player currently controlling this Faction to release control of it.
	ReleaseControlOfFaction( faction );

	// Add the Faction to the list of controlled Factions.
	std::pair< Player*, Faction* > mapping( player, faction );
	mFactionControllerMappings.push_back( mapping );
}


void Game::ReleaseControlOfFaction( Faction* faction )
{
	assertion( faction, "Cannot release control of null Faction!" );
	assertion( !IsInitialized() || faction->GetMap() == mMap, "Cannot release control of Faction created by another Map!" );
	assertion( faction->IsControllable(), "Cannot release control of uncontrollable Faction!" );

	for( auto it = mFactionControllerMappings.begin(); it != mFactionControllerMappings.end(); ++it )
	{
		if( it->second == faction )
		{
			// If a controller mapping for this Faction was found, remove it.
			mFactionControllerMappings.erase( it );
			break;
		}
	}
}


Player* Game::GetControllerOfFaction( Faction* faction ) const
{
	assertion( faction, "Cannot get controller of null Faction!" );
	assertion( faction->GetMap() == mMap, "Cannot get controller of Faction created by another Map!" );

	Player* controller = nullptr;

	for( auto it = mFactionControllerMappings.begin(); it != mFactionControllerMappings.end(); ++it )
	{
		if( it->second == faction )
		{
			// If a controller mapping for this Faction was found, return the controller.
			controller = it->first;
			break;
		}
	}

	return controller;
}


bool Game::PlayerControlsFaction( Player* player, Faction* faction ) const
{
	return ( GetControllerOfFaction( faction ) == player );
}


bool Game::FactionHasController( Faction* faction ) const
{
	return ( GetControllerOfFaction( faction ) != nullptr );
}


size_t Game::GetControlledFactionCount() const
{
	return mFactionControllerMappings.size();
}


Player* Game::GetControllerByTurnOrder( int index ) const
{
	Player* result = nullptr;

	if( index >= 0 && index < (int) GetControlledFactionCount() )
	{
		result = mFactionControllerMappings[ (size_t) index ].first;
	}

	return result;
}


Faction* Game::GetFactionByTurnOrder( int index ) const
{
	Faction* result = nullptr;

	if( index >= 0 && index < (int) GetControlledFactionCount() )
	{
		result = mFactionControllerMappings[ (size_t) index ].second;
	}

	return result;
}


int Game::GetTurnOrderOfFaction( Faction* faction ) const
{
	assertion( faction, "Cannot get turn order of null Faction!" );
	assertion( faction->GetMap() == mMap, "Cannot get turn order of Faction created by another Map!" );
	assertion( faction->IsControllable(), "Cannot get turn order of uncontrollable Faction!" );

	int result = -1;

	for( int turnOrder = 0, controlledFactionCount = GetControlledFactionCount(); turnOrder < controlledFactionCount; ++turnOrder )
	{
		auto factionControllerMapping = mFactionControllerMappings[ turnOrder ];

		if( factionControllerMapping.second == faction )
		{
			// If the Faction was found in the list of mappings, return its turn order.
			result = turnOrder;
			break;
		}
	}

	return mCurrentFactionIndex;
}


int Game::GetTurnOrderOfCurrentFaction() const
{
	return mCurrentFactionIndex;
}


Faction* Game::GetCurrentFaction() const
{
	return GetFactionByTurnOrder( mCurrentFactionIndex );
}


void Game::SetLocalPlayer( Player* player )
{
	assertion( player->GetGame() == this, "Cannot set local Player because the specified Player was created by another Game!" );
	mLocalPlayer = player;
}


Player* Game::GetLocalPlayer() const
{
	return mLocalPlayer;
}


bool Game::HasLocalPlayer() const
{
	return ( mLocalPlayer != nullptr );
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
		EndTurn();
	}

	// Increment the turn counter.
	++mCurrentTurnIndex;

	// Choose the next Faction to take a turn.
	mCurrentFactionIndex = ( ( mCurrentFactionIndex + 1 ) % GetControlledFactionCount() );

	// Start the next turn.
	StartTurn();
}
