#include "androidwars.h"

using namespace mage;


const char* const Map::MAPS_FOLDER_PATH = "map";
const char* const Map::MAP_FILE_EXTENSION = "maps/";
const char* const Map::TERRAIN_TYPE_CHARS = "123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
const size_t Map::TERRAIN_TYPE_CHAR_COUNT = strlen( Map::TERRAIN_TYPE_CHARS );


Tile::Tile() :
	mTerrainType( nullptr ),
	mBestTotalCostToEnter( 0 ),
	mLastClosedSearchIndex( -1 ),
	mLastOpenedSearchIndex( -1 ),
	mOwner( nullptr ),
	mUnit( nullptr )
{ }


Tile::Tile( const Tile& other ) :
	mTerrainType( other.mTerrainType ),
	mOwner( other.mOwner ),
	OnChanged() // Don't copy event bindings.
{ }


Tile::~Tile() { }


void Tile::operator=( const Tile& other )
{
	// Set all properties from the other Tile.
	SetTerrainType( other.mTerrainType );
	SetOwner( other.mOwner );
}


void Tile::SetTerrainType( TerrainType* terrainType )
{
	// Save the old value.
	TerrainType* oldTerrainType = mTerrainType;

	// Set the TerrainType.
	mTerrainType = terrainType;

	if( !IsCapturable() )
	{
		// If the tile should not have an owner, clear it.
		ClearOwner();
	}

	if( mTerrainType != oldTerrainType )
	{
		// If the TerrainType changed, fire the changed callback.
		OnChanged.Invoke();
	}
}


void Tile::ClearTerrainType()
{
	SetTerrainType( nullptr );
}


TerrainType* Tile::GetTerrainType() const
{
	return mTerrainType;
}


bool Tile::HasTerrainType() const
{
	return ( mTerrainType != nullptr );
}


void Tile::SetOwner( Faction* owner )
{
	// Save the old value.
	Faction* oldOwner = mOwner;

	if( !owner || IsCapturable() )
	{
		// Only set the owner if the tile can be captured or the
		// owner is being cleared.
		mOwner = owner;
	}

	if( mOwner != oldOwner )
	{
		// If the owner changed, fire the changed callback.
		OnChanged.Invoke();
	}
}


void Tile::ClearOwner()
{
	SetOwner( nullptr );
}


Faction* Tile::GetOwner() const
{
	return mOwner;
}


bool Tile::HasOwner() const
{
	return ( mOwner != nullptr );
}


void Tile::SetUnit( Unit* unit )
{
	mUnit = unit;
}


void Tile::ClearUnit()
{
	SetUnit( nullptr );
}


Unit* Tile::GetUnit() const
{
	return mUnit;
}


bool Tile::IsEmpty() const
{
	return ( mUnit == nullptr );
}


bool Tile::IsOccupied() const
{
	return ( mUnit != nullptr );
}


bool Tile::IsCapturable() const
{
	return ( mTerrainType && mTerrainType->IsCapturable() );
}


void Tile::Open( int searchIndex )
{
	mLastOpenedSearchIndex = searchIndex;
}


bool Tile::IsOpen( int searchIndex ) const
{
	return ( mLastOpenedSearchIndex == searchIndex );
}


void Tile::Close( int searchIndex )
{
	mLastClosedSearchIndex = searchIndex;
}


bool Tile::IsClosed( int searchIndex ) const
{
	return ( mLastClosedSearchIndex == searchIndex );
}


void Tile::SetPreviousTileDirection( PrimaryDirection direction )
{
	mPreviousTileDirection = direction;
}


PrimaryDirection Tile::GetPreviousTileDirection() const
{
	return mPreviousTileDirection;
}


void Tile::SetBestTotalCostToEnter( int totalCostToEnter )
{
	mBestTotalCostToEnter = totalCostToEnter;
}


int Tile::GetBestTotalCostToEnter() const
{
	return mBestTotalCostToEnter;
}


std::string Map::FormatMapPath( const std::string& mapName )
{
	std::stringstream formatter;
	formatter << MAPS_FOLDER_PATH << "/" << mapName << "." << MAP_FILE_EXTENSION;
	return formatter.str();
}


Map::Map() :
	mIsInitialized( false ),
	mScenario( nullptr ),
	mNextSearchIndex( 0 )
{ }


Map::~Map() { }


void Map::Init( Scenario* scenario )
{
	assertion( !mIsInitialized, "Cannot initialize Map that has already been initialized!" );

	// Initialize the Map.
	mIsInitialized = true;

	// Set the Scenario.
	assertion( scenario, "Cannot initialize Map without a valid Scenario!" );
	mScenario = scenario;

	// Add a UnitAbility for each ability supported by the Scenario.
	// TODO: Make this come from the Scenario itself.
	RegisterUnitAbility< UnitAttackAbility >();
	RegisterUnitAbility< UnitWaitAbility >();

	// Make sure the size of the map is valid.
	assertion( IsValid(), "Cannot initialize Map with invalid size (%d,%d)!", GetWidth(), GetHeight() );

	// Get the default TerrainType for the Scenario.
	TerrainType* defaultTerrainType = mScenario->GetDefaultTerrainType();

	ForEachTileInMaxArea( [ this, defaultTerrainType ]( const Iterator& tile )
	{
		// Initialize all tiles to the default TerrainType.
		tile->SetTerrainType( defaultTerrainType );

		tile->OnChanged.AddCallback( [ this, tile ]()
		{
			// When the Tile changes, notify the Map that the Tile has changed.
			TileChanged( tile );
		});
	});
}


void Map::Destroy()
{
	assertion( mIsInitialized, "Cannot destroy Map that has not been initialized!" );

	// Clear the scenario.
	mScenario = nullptr;

	// Destroy the Map.
	mIsInitialized = false;
}


void Map::SaveToJSON( rapidjson::Document& document, rapidjson::Value& object )
{
	/*
	DebugPrintf( "Saving Map state..." );

	// TODO: Add the ID.
	//rapidjson::Value gameIDValue;
	//gameIDValue.SetString( mGameID.c_str(), result.GetAllocator() );
	//result.AddMember( "id", gameIDValue, result.GetAllocator() );

	// Serialize the scenario name.
	DebugPrintf( "Saving Map info..." );
	rapidjson::Value scenarioName;
	scenarioName.SetString( mScenario->GetName().GetCString() );
	object.AddMember( "scenarioName", scenarioName, document.GetAllocator() );

	// Serialize the width and height of the Map.
	short width = GetWidth(), height = GetHeight();
	rapidjson::Value widthValue, heightValue;
	widthValue.SetInt( width );
	heightValue.SetInt( height );
	object.AddMember( "width", widthValue, document.GetAllocator() );
	object.AddMember( "height", heightValue, document.GetAllocator() );

	// Store a palette of all TerrainTypes in use by this Map.
	std::vector< TerrainType* > terrainTypeMappings;
	size_t nextPaletteIndex = 0;
	
	std::string mapData;

	DebugPrintf( "Saving Tiles..." );
	for( size_t y = 0; y < height; ++y )
	{
		for( size_t x = 0; x < width; ++x )
		{
			//DebugPrintf( "Saving tile (%d,%d)...", x, y );
			Iterator tile = GetTile( x, y );

			// Get the TerrainType for this tile.
			TerrainType* terrainType = tile->GetTerrainType();
			int currentIndex = -1;

			for( int i = 0; i < terrainTypeMappings.size(); ++i )
			{
				if( terrainTypeMappings[ i ] == terrainType )
				{
					// Look up the TerrainType mapping (if it exists).
					currentIndex = i;
					break;
				}
			}

			if( currentIndex == -1 )
			{
				DebugPrintf( "Tile (%d,%d)", x, y );
				// If the TerrainType mapping does not already exist, map the TerrainType to a unique character that will be used when serializing the Map data.
				assertion( nextPaletteIndex < TERRAIN_TYPE_CHAR_COUNT, "There are too many TerrainTypes to represent in the TerrainTypes palette!" );
				DebugPrintf( "Assertion passed!" );
				terrainTypeMappings.push_back( terrainType );
				DebugPrintf( "Added tile." );
				currentIndex = ( terrainTypeMappings.size() - 1 );
				++nextPaletteIndex;
			}

			// Print out a character for this Tile.
			//mapData.push_back( TERRAIN_TYPE_CHARS[ currentIndex ] );
		}
	}

	// Create a palette of all TerrainTypes in use by this Map.
	rapidjson::Value terrainTypes;
	terrainTypes.SetObject();
	std::string paletteChar;

	DebugPrintf( "Saving TerrainTypes..." );
	for( int i = 0; i < terrainTypeMappings.size(); ++i )
	{
		TerrainType* terrainType = terrainTypeMappings[ i ];

		// Serialize all mappings.
		rapidjson::Value mapping;
		mapping.SetString( terrainType->GetName().GetCString() );
		paletteChar = TERRAIN_TYPE_CHARS[ i ];
		terrainTypes.AddMember( paletteChar.c_str(), mapping, document.GetAllocator() );
	}

	object.AddMember( "terrainTypes", terrainTypes, document.GetAllocator() );

	// Serialize the Map data.
	rapidjson::Value tiles;
	tiles.SetString( mapData.c_str() );
	object.AddMember( "tiles", tiles, document.GetAllocator() );

	// Start an array for Factions.
	rapidjson::Value factionsArray;
	factionsArray.SetArray();

	std::map< Faction*, int > factionMappings;
	int nextFactionID = 1;

	DebugPrintf( "Saving Factions..." );
	for( auto it = mFactions.begin(); it != mFactions.end(); ++it )
	{
		Faction* faction = *it;

		// Save a unique ID for this Faction.
		factionMappings[ faction ] = nextFactionID;

		// Serialize each Faction.
		rapidjson::Value factionJSON;
		factionJSON.SetObject();
		faction->SaveToJSON( document, factionJSON );
		factionsArray.PushBack( factionJSON, document.GetAllocator() );

		++nextFactionID;
	}

	// Start an array for Units.
	rapidjson::Value unitsArray;
	unitsArray.SetArray();

	DebugPrintf( "Saving Units..." );
	for( auto it = mUnits.begin(); it != mUnits.end(); ++it )
	{
		Unit* unit = *it;

		// Serialize each Unit.
		rapidjson::Value unitJSON;
		unitJSON.SetObject();
		unit->SaveToJSON( document, unitJSON );

		// Get the Faction owner for this Unit.
		Faction* owner = unit->GetOwner();
		int ownerID = 0;

		if( owner )
		{
			// Get the Faction ID for the owner of this Unit.
			assertion( factionMappings.find( owner ) != factionMappings.end(), "Tried to save Unit to JSON with invalid Faction ID!" );
			ownerID = factionMappings[ owner ];
		}

		// Save the owner Faction ID to JSON.
		rapidjson::Value ownerValue;
		ownerValue.SetInt( ownerID );
		unitJSON.AddMember( "owner", ownerValue, document.GetAllocator() );

		// Save the Unit JSON.
		unitsArray.PushBack( unitJSON, document.GetAllocator() );
	}

	// Add it to the result.
	object.AddMember( "units", unitsArray, document.GetAllocator() );

	DebugPrintf( "Save complete!" );
	*/
}


void Map::LoadFromFile( const std::string& filePath )
{
	// TODO
}


void Map::LoadFromJSON( const rapidjson::Value& object )
{
	// Destroy all Units.
	// TODO: Don't do this.
	DestroyAllUnits();

	// Get the Units tag.
	const rapidjson::Value& unitsArray = object[ "units" ];
	assertion( unitsArray.IsArray(), "Could not load game state from JSON because no \"units\" list was found!" );

	Scenario* scenario = GetScenario();

	for( auto it = unitsArray.Begin(); it != unitsArray.End(); ++it )
	{
		const rapidjson::Value& object = ( *it );
		assertion( object.IsObject(), "Could not load Unit from JSON because the JSON provided was not an object!" );

		// Get all properties.
		HashString unitTypeName = GetJSONStringValue( object, "unitType", "" );
		int ownerIndex = GetJSONIntValue( object, "owner", -1 );
		int tileX = GetJSONIntValue( object, "x", -1 );
		int tileY = GetJSONIntValue( object, "y", -1 );

		assertion( GetTile( tileX, tileY ).IsValid(), "Loaded invalid tile position (%d,%d) from JSON!", tileX, tileY );

		// Get references.
		UnitType* unitType = scenario->UnitTypes.FindByName( unitTypeName );
		assertion( unitType, "Could not load invalid UnitType (\"%s\") from JSON!", unitTypeName.GetCString() );

		Faction* faction = GetFactionByIndex( ownerIndex );
		assertion( faction, "Could not load Unit with invalid Faction index (%d) from JSON!", ownerIndex );

		// Spawn the Unit.
		Unit* unit = CreateUnit( unitType, faction, tileX, tileY );

		// Load each Unit from the array.
		unit->LoadFromJSON( *it );
	}
}


void Map::FillWithDefaultTerrainType()
{
	// Get the default TerrainType for this Scenario.
	TerrainType* defaultTerrainType = mScenario->GetDefaultTerrainType();
	assertion( defaultTerrainType != nullptr, "No default TerrainType found for this Scenario!" );

	// Fill the whole Map with the default TerrainType.
	Tile tile;
	tile.SetTerrainType( defaultTerrainType );
	FillMaxArea( tile );
}


Faction* Map::CreateFaction()
{
	// Create a new Faction.
	Faction* faction = new Faction( this );

	// Add the Faction to the list of Factions.
	mFactions.push_back( faction );

	return faction;
}


Faction* Map::GetFactionByIndex( size_t index ) const
{
	Faction* faction = nullptr;

	if( index >= 0 && index < mFactions.size() )
	{
		faction = mFactions[ index ];
	}

	return faction;
}


const Map::Factions& Map::GetFactions() const
{
	return mFactions;
}


size_t Map::GetFactionCount() const
{
	return mFactions.size();
}


void Map::DestroyFaction( Faction* faction )
{
	assertion( faction->GetMap() == this, "Cannot destroy Faction created by a different Map!" );

	auto it = mFactions.begin();

	for( ; it != mFactions.end(); ++it )
	{
		if( *it == faction )
		{
			// Find the Faction in the list of Factions.
			break;
		}
	}

	// Remove the Faction from the list of Factions.
	assertion( it != mFactions.end(), "Cannot destroy Faction because it was not found in the Map Faction list!" );
	mFactions.erase( it );

	// Destroy the Faction.
	delete faction;
}


bool Map::AreFriends( const Faction* first, const Faction* second ) const
{
	bool result = false;

	if( first && second )
	{
		// TODO: Support alliances.
		result = ( first == second );
	}

	return result;
}


bool Map::AreEnemies( const Faction* first, const Faction* second ) const
{
	return !AreFriends( first, second );
}


Unit* Map::CreateUnit( UnitType* unitType, Faction* owner, short tileX, short tileY, int health, int ammo, int supplies )
{
	return CreateUnit( unitType, owner, Vec2s( tileX, tileY ) );
}


Unit* Map::CreateUnit( UnitType* unitType, Faction* owner, const Vec2s& tilePos, int health, int ammo, int supplies )
{
	Unit* unit = nullptr;

	// Get the Tile where the Unit will be placed.
	Iterator tile = GetTile( tilePos );

	if( tile.IsValid() )
	{
		if( tile->IsEmpty() )
		{
			// Create a new Unit.
			unit = new Unit();

			// Load Unit properties.
			unit->SetUnitType( unitType );
			unit->SetOwner( owner );

			// Set the health and ammo for the Unit.
			if( health >= 0 )
			{
				unit->SetHealth( health );
			}
			else
			{
				unit->ResetHealth();
			}

			if( ammo >= 0 )
			{
				unit->SetAmmo( ammo );
			}
			else
			{
				unit->ResetAmmo();
			}

			if( supplies >= 0 )
			{
				unit->SetSupplies( supplies );
			}
			else
			{
				unit->ResetSupplies();
			}

			// Initialize the Unit.
			unit->Init( this, tile );

			// Place the Unit into the Tile.
			tile->SetUnit( unit );

			// Add the Unit to the list of Units.
			mUnits.push_back( unit );

			if( mIsInitialized )
			{
				// Call the Unit created callback.
				OnUnitCreated.Invoke( unit );
			}
		}
		else
		{
			WarnFail( "Cannot create Unit at Tile (%d,%d) because the Tile is occupied by another Unit!", tilePos.x, tilePos.y );
		}
	}
	else
	{
		WarnFail( "Cannot create Unit at invalid Tile (%d,%d)!", tilePos.x, tilePos.y );
	}

	return unit;
}


void Map::ForEachUnit( ForEachUnitCallback callback )
{
	for( auto it = mUnits.begin(); it != mUnits.end(); ++it )
	{
		// Call the function for each Unit.
		callback.Invoke( *it );
	}
}


void Map::ForEachUnit( ForEachConstUnitCallback callback ) const
{
	for( auto it = mUnits.begin(); it != mUnits.end(); ++it )
	{
		// Call the function for each Unit.
		callback.Invoke( *it );
	}
}


UnitAbility* Map::GetUnitAbilityByName( const HashString& name ) const
{
	UnitAbility* result = nullptr;

	// Look up the ability by name.
	auto it = mUnitAbilitiesByName.find( name );

	if( it != mUnitAbilitiesByName.end() )
	{
		result = it->second;
	}

	return result;
}


void Map::DetermineAvailableActions( const Unit* unit, const Path& movementPath, Actions& result )
{
	// Clear the list of results.
	result.clear();

	for( auto it = mUnitAbilitiesByName.begin(); it != mUnitAbilitiesByName.end(); ++it )
	{
		// Ask each UnitAbility for a list of possible actions after the Unit moves along the path.
		UnitAbility* ability = it->second;
		ability->DetermineAvailableActions( unit, movementPath, result );
	}
}


void Map::PerformAction( Action& action )
{
	// Find the UnitAbility to activate for the specified Action.
	auto it = mUnitAbilitiesByName.find( action.Type );

	if( it != mUnitAbilitiesByName.end() )
	{
		// If a UnitAbility was found that can process the Action, activate the ability.
		UnitAbility* ability = it->second;
		ability->ProcessAction( action );
	}
	else
	{
		WarnFail( "Cannot perform Action \"%s\" because no UnitAbility was found that can process the Action!", action.Type.GetCString() );
	}
}


void Map::FindReachableTiles( const Unit* unit, TileSet& result )
{
	int searchIndex = ReserveSearchIndex();

	// Clear the list of results.
	result.clear();

	// Clear the open list.
	mOpenList.clear();

	// Get the Unit's current Tile and type.
	Iterator originTile = unit->GetTile();
	UnitType* unitType = unit->GetUnitType();
	MovementType* movementType = unit->GetMovementType();

	// Get the starting movement range of the Unit.
	int movementRange = unit->GetMovementRange();

	// Add the origin tile to the open list.
	originTile->Open( searchIndex );
	originTile->SetPreviousTileDirection( PrimaryDirection::NONE );
	originTile->SetBestTotalCostToEnter( 0 );
	mOpenList.insert( 0, originTile );

	while( !mOpenList.isEmpty() )
	{
		// Pop the first element off the open list.
		Map::Iterator tile = mOpenList.popMinElement();

		// Close the tile and add it to the result.
		tile->Close( searchIndex );
		result.insert( tile );

		for( int i = 0; i < CARDINAL_DIRECTION_COUNT; ++i )
		{
			// Determine the direction to search.
			PrimaryDirection direction = CARDINAL_DIRECTIONS[ i ];

			// If the adjacent tile isn't in the previous direction, get the adjacent tile.
			Iterator adjacent = tile.GetAdjacent( direction );

			if( adjacent.IsValid() && !adjacent->IsClosed( searchIndex ) )
			{
				if( unit->CanEnterTile( adjacent ) )
				{
					// If the adjacent tile is valid and isn't already closed, get the TerrainType of the adjacent tile.
					TerrainType* adjacentTerrainType = adjacent->GetTerrainType();

					// If the adjacent tile is passable, find the total cost of entering the tile.
					int costToEnterAdjacent = movementType->GetMovementCostAcrossTerrain( adjacentTerrainType );
					int adjacentTotalCost = ( tile->GetBestTotalCostToEnter() + costToEnterAdjacent );

					if( adjacentTotalCost <= movementRange )
					{
						if( !adjacent->IsOpen( searchIndex ) )
						{
							// If the tile info isn't already on the open list, add it.
							adjacent->SetPreviousTileDirection( direction.GetOppositeDirection() );
							adjacent->SetBestTotalCostToEnter( adjacentTotalCost );
							mOpenList.insert( adjacentTotalCost, adjacent );
						}
						else if( adjacentTotalCost < adjacent->GetBestTotalCostToEnter() )
						{
							// If the node is already on the open list but has a larger total cost,
							// update the value.
							adjacent->SetPreviousTileDirection( direction.GetOppositeDirection() );
							adjacent->SetBestTotalCostToEnter( adjacentTotalCost );
							mOpenList.update( adjacentTotalCost, adjacent );
						}
					}
				}
			}
		}
	}
}


void Map::ForEachReachableTile( const Unit* unit, ForEachReachableTileCallback callback )
{
	assertion( unit, "Cannot find reachable tiles for null Unit!" );
	assertion( callback.IsValid(), "Cannot call invalid callback on reachable tiles!" );

	// Find all reachable tiles for the Unit.
	TileSet reachableTiles;
	FindReachableTiles( unit, reachableTiles );

	for( auto it = reachableTiles.begin(); it != reachableTiles.end(); ++it )
	{
		// Invoke the callback on all reachable tiles.
		callback.Invoke( *it, unit );
	}
}


void Map::FindBestPathToTile( const Unit* unit, const Vec2s& tilePos, Path& result )
{
	std::vector< PrimaryDirection > reverseDirections;
	int searchIndex = ReserveSearchIndex();

	// Clear the list of results.
	result.Clear();
	result.SetOrigin( unit->GetTilePos() );

	// Clear the open list.
	mOpenList.clear();

	// Get the Unit's current Tile and type.
	Iterator originTile = unit->GetTile();
	UnitType* unitType = unit->GetUnitType();
	MovementType* movementType = unit->GetMovementType();

	// Get the starting movement range of the Unit.
	int movementRange = unit->GetMovementRange();

	// Add the origin tile to the open list.
	originTile->Open( searchIndex );
	originTile->SetPreviousTileDirection( PrimaryDirection::NONE );
	originTile->SetBestTotalCostToEnter( 0 );
	mOpenList.insert( 0, originTile );

	while( !mOpenList.isEmpty() )
	{
		// Pop the first element off the open list.
		Map::Iterator tile = mOpenList.popMinElement();

		if( tile.GetPosition() != tilePos )
		{
			// If this isn't the goal tile, close it.
			tile->Close( searchIndex );

			for( int i = 0; i < CARDINAL_DIRECTION_COUNT; ++i )
			{
				// Determine the direction to search.
				PrimaryDirection direction = CARDINAL_DIRECTIONS[ i ];

				// If the adjacent tile isn't in the previous direction, get the adjacent tile.
				Iterator adjacent = tile.GetAdjacent( direction );

				if( adjacent.IsValid() && !adjacent->IsClosed( searchIndex ) )
				{
					if( unit->CanEnterTile( adjacent ) )
					{
						// If the adjacent tile is valid and isn't already closed, get the TerrainType of the adjacent tile.
						TerrainType* adjacentTerrainType = adjacent->GetTerrainType();

						// If the adjacent tile is passable, find the total cost of entering the tile.
						int costToEnterAdjacent = movementType->GetMovementCostAcrossTerrain( adjacentTerrainType );
						int adjacentTotalCost = ( tile->GetBestTotalCostToEnter() + costToEnterAdjacent );
						int distanceToGoal = ( originTile.GetPosition().GetManhattanDistanceTo( tilePos ) );
						int adjacentWeight = ( adjacentTotalCost + distanceToGoal );

						if( adjacentTotalCost <= movementRange )
						{
							if( !adjacent->IsOpen( searchIndex ) )
							{
								// If the tile info isn't already on the open list, add it.
								adjacent->SetPreviousTileDirection( direction.GetOppositeDirection() );
								adjacent->SetBestTotalCostToEnter( adjacentTotalCost );
								mOpenList.insert( adjacentWeight, adjacent );
							}
							else if( adjacentTotalCost < adjacent->GetBestTotalCostToEnter() )
							{
								// If the node is already on the open list but has a larger total cost,
								// update the value.
								adjacent->SetPreviousTileDirection( direction.GetOppositeDirection() );
								adjacent->SetBestTotalCostToEnter( adjacentTotalCost );
								mOpenList.update( adjacentWeight, adjacent );
							}
						}
					}
				}
			}
		}
		else
		{
			// If this is the goal tile, construct the path to the location.
			PrimaryDirection previousDirection = tile->GetPreviousTileDirection();

			while( previousDirection != PrimaryDirection::NONE )
			{
				// Construct the list of directions from the goal back to the origin tile.
				reverseDirections.push_back( previousDirection );
				tile = tile.GetAdjacent( previousDirection );
				previousDirection = tile->GetPreviousTileDirection();
			}

			// End the search.
			break;
		}
	}

	for( auto it = reverseDirections.rbegin(); it != reverseDirections.rend(); ++it )
	{
		// Construct the path by reversing the directions from the goal to the origin.
		PrimaryDirection direction = *it;
		result.AddDirection( direction.GetOppositeDirection() );
	}
}


void Map::FindTilesInRange( const Vec2s& tilePos, const IntRange& range, Tiles& result )
{
	DebugPrintf( "Finding tiles in range {%d-%d} from Tile (%d,%d)...", range.Min, range.Max, tilePos.x, tilePos.y );

	// Clear the result list.
	result.clear();

	// Clear the open list.
	mOpenList.clear();

	// Reserve the next search index.
	int searchIndex = ReserveSearchIndex();

	// Get the initial Tile.
	Iterator initialTile = GetTile( tilePos );

	if( initialTile.IsValid() )
	{
		// Flood-fill outward from the starting Tile.
		mOpenList.insert( 0, initialTile );
		initialTile->Open( searchIndex );
	}

	while( !mOpenList.isEmpty() )
	{
		// Get the next Tile and close it.
		Iterator tile = mOpenList.popMinElement();
		tile->Close( searchIndex );

		// Get the distance from the start Tile.
		short distanceFromTile = tilePos.GetManhattanDistanceTo( tile.GetPosition() );

		if( distanceFromTile >= range.Min && distanceFromTile <= range.Max )
		{
			// If the Tile is within the ranges specified, add it to the result list.
			result.push_back( tile );
			DebugPrintf( "Tile (%d,%d) is in range!", tile.GetX(), tile.GetY() );
		}

		if( distanceFromTile < range.Max )
		{
			tile.ForEachAdjacent( [ this, searchIndex, distanceFromTile ]( Iterator adjacent )
			{
				if( adjacent.IsValid() && !adjacent->IsOpen( searchIndex ) && !adjacent->IsClosed( searchIndex ) )
				{
					// If the tile distance is less than the maximum range, open all valid adjacent tiles.
					mOpenList.insert( distanceFromTile + 1, adjacent );
					adjacent->Open( searchIndex );
				}
			});
		}
	}
}


void Map::FindUnitsInRange( const Vec2s& tilePos, const IntRange& range, Units& result )
{
	// Clear the result.
	result.clear();

	// Find all tiles in range of the specified position.
	Tiles tilesInRange;
	FindTilesInRange( tilePos, range, tilesInRange );

	for( auto it = tilesInRange.begin(); it != tilesInRange.end(); ++it )
	{
		// Get the Unit at each Tile in range (if any).
		Iterator tile = *it;
		Unit* unitAtTile = tile->GetUnit();

		if( unitAtTile )
		{
			// If any selected Tiles contain a Unit, add the Unit to the list.
			result.push_back( unitAtTile );
		}
	}
}


const Map::Units& Map::GetUnits() const
{
	return mUnits;
}


size_t Map::GetUnitCount() const
{
	return mUnits.size();
}


void Map::DestroyUnit( Unit* unit )
{
	assertion( unit, "Cannot destroy null Unit!" );
	assertion( unit->IsInitialized(), "Cannot destroy Unit that has not been initialized!" );
	assertion( unit->GetMap() == this, "Cannot destroy Unit created by another Map!" );

	for( auto it = mUnits.begin(); it != mUnits.end(); ++it )
	{
		if( *it == unit )
		{
			// Remove the Unit from the list of Units.
			mUnits.erase( it );
			break;
		}
	}

	if( mIsInitialized )
	{
		// Call the Unit destroyed callback.
		OnUnitDestroyed.Invoke( unit );
	}

	// Remove the Unit from its current Tile.
	Iterator tile = unit->GetTile();
	assertion( tile->GetUnit() == unit, "Cannot remove Unit from Tile that it does not occupy!" );
	tile->ClearUnit();

	// Destroy the Unit.
	unit->Destroy();
	delete unit;
}


void Map::DestroyAllUnits()
{
	// TODO
}


Scenario* Map::GetScenario() const
{
	return mScenario;
}


int Map::ReserveSearchIndex()
{
	return mNextSearchIndex++;
}


void Map::TileChanged( const Iterator& tile )
{
	if( tile->IsOccupied() )
	{
		// Make sure the occupying Unit can still be in this Tile.
		Unit* unit = tile->GetUnit();
		TerrainType* terrainType = tile->GetTerrainType();

		if( !unit->CanMoveAcrossTerrain( terrainType ) )
		{
			// If the Unit cannot be here, destroy it.
			DestroyUnit( unit );
		}
	}

	// Fire the change callback.
	OnTileChanged.Invoke( tile );
}


void Map::UnitMoved( Unit* unit, const Path& path )
{
	// TODO
}


void Map::UnitDied( Unit* unit )
{
	// Destroy the Unit.
	DestroyUnit( unit );
}


void Map::UnregisterAllUnitAbilities()
{
	for( auto it = mUnitAbilitiesByName.begin(); it != mUnitAbilitiesByName.end(); ++it )
	{
		UnitAbility* ability = it->second;
		delete ability;
	}

	mUnitAbilitiesByName.clear();
}

