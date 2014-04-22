#include "androidwars.h"

using namespace mage;


const char* const Map::MAPS_FOLDER_PATH = "map";
const char* const Map::MAP_FILE_EXTENSION = "maps/";
const char* const Map::TERRAIN_TYPE_PALETTE_CHARS = "123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";


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
	DebugPrintf( "Saving Map state..." );

	// TODO: Add the ID.
	//rapidjson::Value gameIDValue;
	//gameIDValue.SetString( mGameID.c_str(), result.GetAllocator() );
	//result.AddMember( "id", gameIDValue, result.GetAllocator() );

	// Serialize the scenario name.
	rapidjson::Value scenarioName;
	scenarioName.SetString( mScenario->GetName().GetCString() );
	object.AddMember( "scenarioName", scenarioName, document.GetAllocator() );

	// Serialize the width and height of the Map.
	rapidjson::Value width, height;
	width.SetInt( GetWidth() );
	height.SetInt( GetHeight() );
	object.AddMember( "width", width, document.GetAllocator() );
	object.AddMember( "height", height, document.GetAllocator() );

	// Store a palette of all TerrainTypes in use by this Map.
	std::map< TerrainType*, size_t > terrainTypesPalette;
	size_t nextPaletteIndex = 0;
	
	std::string mapData;

	ForEachTile( [ &mapData, &terrainTypesPalette, &nextPaletteIndex ]( ConstIterator tile )
	{
		// Get the TerrainType for this tile.
		TerrainType* terrainType = tile->GetTerrainType();

		// Look up the TerrainType mapping (if it exists).
		auto it = terrainTypesPalette.find( terrainType );
		size_t currentIndex = 0;

		if( it == terrainTypesPalette.end() )
		{
			// If the TerrainType mapping does not already exist, map the TerrainType to a unique character that will be used when serializing the Map data.
			assertion( nextPaletteIndex < sizeof( TERRAIN_TYPE_PALETTE_CHARS ), "There are too many TerrainTypes to represent in the TerrainTypes palette!" );
			terrainTypesPalette[ terrainType ] = nextPaletteIndex;
			currentIndex = nextPaletteIndex;
			++nextPaletteIndex;
		}
		else
		{
			// Otherwise, look up the index for this TerrainType.
			currentIndex = it->second;
		}

		// Print out a character for this Tile.
		mapData.push_back( TERRAIN_TYPE_PALETTE_CHARS[ currentIndex ] );
	});

	// Create a palette of all TerrainTypes in use by this Map.
	rapidjson::Value terrainTypes;
	terrainTypes.SetObject();
	std::string paletteChar;

	for( auto it = terrainTypesPalette.begin(); it != terrainTypesPalette.end(); ++it )
	{
		// Serialize all mappings.
		rapidjson::Value mapping;
		mapping.SetString( it->first->GetName().GetCString() );
		paletteChar = TERRAIN_TYPE_PALETTE_CHARS[ it->second ];
		terrainTypes.AddMember( paletteChar.c_str(), mapping, document.GetAllocator() );
	}

	object.AddMember( "terrainTypes", terrainTypes, document.GetAllocator() );

	// Serialize the Map data.
	rapidjson::Value tiles;
	tiles.SetString( mapData.c_str() );
	object.AddMember( "tiles", tiles, document.GetAllocator() );

	// Start an array for Units.
	rapidjson::Value unitsArray;
	unitsArray.SetArray();

	ForEachUnit( [ &document, &object, &unitsArray ]( Unit* unit )
	{
		// Serialize each Unit.
		rapidjson::Value unitJSON;
		unitJSON.SetObject();

		// Have the Unit serialize its values to JSON.
		unit->SaveToJSON( document, unitJSON );

		// Add each Unit's JSON to the array.
		unitsArray.PushBack( unitJSON, document.GetAllocator() );
	});

	// Add it to the result.
	object.AddMember( "units", unitsArray, document.GetAllocator() );
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
	// TODO
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
	// Fire the change callback.
	OnTileChanged.Invoke( tile );
}


void Map::UnitMoved( Unit* unit, const Path& path )
{
	// TODO
}


void Map::UnitDied( Unit* unit )
{
	// TODO
}
