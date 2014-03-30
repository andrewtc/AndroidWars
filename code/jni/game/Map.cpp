#include "androidwars.h"

using namespace mage;


Tile::Tile() :
	mTerrainType( nullptr ),
	mOwner( nullptr )
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


void Tile::SetOwner( Player* owner )
{
	// Save the old value.
	Player* oldOwner = mOwner;

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


Player* Tile::GetOwner() const
{
	return mOwner;
}


bool Tile::HasOwner() const
{
	return ( mOwner != nullptr );
}


bool Tile::IsCapturable() const
{
	return ( mTerrainType && mTerrainType->IsCapturable() );
}


Map::Map() :
	mIsInitialized( false ),
	mScenario( nullptr )
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
	// TODO
}


void Map::LoadFromFile( const std::string& filePath )
{
	// TODO
}


void Map::LoadFromJSON( const rapidjson::Value& object )
{
	// TODO
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


Scenario* Map::GetScenario() const
{
	return mScenario;
}


void Map::TileChanged( const Iterator& tile )
{
	// Fire the change callback.
	OnTileChanged.Invoke( tile );
}
