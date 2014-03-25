#include "androidwars.h"

using namespace mage;


Tile::Tile() :
	mTerrainType( nullptr ),
	mOwner( nullptr )
{ }


Tile::~Tile() { }


void Tile::SetTerrainType( TerrainType* terrainType )
{
	mTerrainType = terrainType;

	if( !IsCapturable() )
	{
		// If the tile should not have an owner, clear it.
		ClearOwner();
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


void Tile::SetOwner( Player* player )
{
	if( !player || IsCapturable() )
	{
		// Only set the owner if the tile can be captured or the
		// owner is being cleared.
		mOwner = player;
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


Vec2s Map::GetAdjacentTilePos( short x, short y, CardinalDirection direction )
{
	Vec2s result( x, y );

	switch( direction )
	{
	case CARDINAL_DIRECTION_EAST:
		result.x += 1;
		break;

	case CARDINAL_DIRECTION_WEST:
		result.x -= 1;
		break;

	case CARDINAL_DIRECTION_SOUTH:
		result.y += 1;
		break;

	case CARDINAL_DIRECTION_NORTH:
		result.y -= 1;
		break;
	}

	return result;
}


Vec2s Map::GetAdjacentTilePos( const Vec2s& tilePos, CardinalDirection direction )
{
	return GetAdjacentTilePos( tilePos.x, tilePos.y, direction );
}


CardinalDirection Map::GetOppositeDirection( CardinalDirection direction )
{
	CardinalDirection result = CARDINAL_DIRECTION_NONE;

	switch( direction )
	{
	case CARDINAL_DIRECTION_EAST:
		result = CARDINAL_DIRECTION_WEST;
		break;

	case CARDINAL_DIRECTION_WEST:
		result = CARDINAL_DIRECTION_EAST;
		break;

	case CARDINAL_DIRECTION_SOUTH:
		result = CARDINAL_DIRECTION_NORTH;
		break;

	case CARDINAL_DIRECTION_NORTH:
		result = CARDINAL_DIRECTION_SOUTH;
		break;
	}

	return result;
}


Map::Map() :
	mIsInitialized( false ),
	mScenario( nullptr ),
	mSize( 0, 0 )
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
	assertion( IsValid(), "Cannot initialize Map with invalid size (%d,%d)!", mSize.x, mSize.y );
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


void Map::Resize( const Vec2s& size )
{
	Resize( size.x, size.y );
}


void Map::Resize( short x, short y )
{
	assertion( IsValidSize( x, y ), "Cannot resize Map to invalid size (%d,%d)!", x, y );

	//

	// Set the size of the map.
	mSize.Set( x, y );
}


short Map::GetWidth() const
{
	return mSize.x;
}


short Map::GetHeight() const
{
	return mSize.y;
}


Vec2s Map::GetSize() const
{
	return mSize;
}


bool Map::IsValidSize( const Vec2s& size )
{
	return ( size.x > 0 && size.y > 0 && size.x <= MAX_SIZE && size.y <= MAX_SIZE );
}


bool Map::IsValidSize( short x, short y )
{
	return ( x > 0 && y > 0 && x <= MAX_SIZE && y <= MAX_SIZE );
}


bool Map::IsValid() const
{
	return IsValidSize( mSize );
}


void Map::SetTile( const Vec2s& tilePos, const Tile& tile )
{
	Iterator iterator = GetTile( tilePos );
	( *iterator ) = tile;
}


void Map::SetTile( short x, short y, const Tile& tile )
{
	Iterator iterator = GetTile( x, y );
	( *iterator ) = tile;
}


Map::Iterator Map::GetTile( const Vec2s& tilePos )
{
	return Iterator( this, tilePos );
}


Map::Iterator Map::GetTile( short x, short y )
{
	return Iterator( this, x, y );
}


Map::ConstIterator Map::GetTile( const Vec2s& tilePos ) const
{
	return ConstIterator( this, tilePos );
}


Map::ConstIterator Map::GetTile( short x, short y ) const
{
	return ConstIterator( this, x, y );
}


bool Map::IsValidTilePos( const Vec2s& tilePos ) const
{
	return IsValidTilePos( tilePos.x, tilePos.y );
}


bool Map::IsValidTilePos( short x, short y ) const
{
	return ( x >= 0 && x < mSize.x && y >= 0 && y < mSize.y );
}


void Map::ForEachTile( ForEachTileCallback callback )
{
	RectS area( 0, 0, mSize.x, mSize.y );
	ForEachTileInArea( area, callback );
}


void Map::ForEachTile( ForEachConstTileCallback callback ) const
{
	RectS area( 0, 0, mSize.x, mSize.y );
	ForEachTileInArea( area, callback );
}


void Map::ForEachTileInArea( const RectS& area, ForEachTileCallback callback )
{
	assertion( area.IsValid(), "Cannot run Tile callback on invalid area (%d,%d,%d,%d)!", area.Left, area.Top, area.Right, area.Bottom );
	assertion( callback.IsValid(), "Cannot run Tile callback on area (%d,%d,%d,%d) because the callback is invalid!", area.Left, area.Top, area.Right, area.Bottom );

	for( short y = area.Top; y < area.Bottom; ++y )
	{
		for( short x = area.Left; x < area.Right; ++x )
		{
			// Fire the callback for each Tile.
			callback.Invoke( Iterator( this, x, y ) );
		}
	}
}


void Map::ForEachTileInArea( const RectS& area, ForEachConstTileCallback callback ) const
{
	assertion( area.IsValid(), "Cannot run Tile callback on invalid area (%d,%d,%d,%d)!", area.Left, area.Top, area.Right, area.Bottom );
	assertion( callback.IsValid(), "Cannot run Tile callback on area (%d,%d,%d,%d) because the callback is invalid!", area.Left, area.Top, area.Right, area.Bottom );

	for( short y = area.Top; y < area.Bottom; ++y )
	{
		for( short x = area.Left; x < area.Right; ++x )
		{
			// Fire the callback for each Tile.
			callback.Invoke( ConstIterator( this, x, y ) );
		}
	}
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


void Map::Fill( const Tile& tile )
{
	RectS area( 0, 0, mSize.x, mSize.y );
	Fill( tile, area );
}


void Map::Fill( const Tile& tile, const RectS& area )
{
	ForEachTileInArea( area, [ this, &tile ]( const Iterator& it )
	{
		// Fill each Tile in the area.
		( *it ) = tile;
	});
}


void Map::SetTileChangedCallback( const TileChangedCallback& callback )
{
	mTileChangedCallback = callback;
}


void Map::ClearTileChangedCallback()
{
	SetTileChangedCallback( TileChangedCallback() );
}


Map::TileChangedCallback Map::GetTileChangedCallback() const
{
	return mTileChangedCallback;
}


bool Map::HasTileChangedCallback() const
{
	return mTileChangedCallback.IsValid();
}


Scenario* Map::GetScenario() const
{
	return mScenario;
}


size_t Map::GetTileIndex( const Vec2s& tilePos ) const
{
	assertion( IsValidTilePos( tilePos ), "Cannot get tile index for invalid tile position (%d,%d)!", tilePos.x, tilePos.y );
	return ( ( tilePos.y << MAX_SIZE_EXPONENT ) + tilePos.x );
}
