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


Map::ConstIterator::ConstIterator( const Map* map ) :
	mMap( map ), mX( -1 ), mY( -1 )
{
	assertion( mMap, "Cannot create Map::ConstIterator without a Map reference!" );
}


Map::ConstIterator::ConstIterator( const Map* map, const Vec2s& tilePos ) :
	mMap( map ), mX( tilePos.x ), mY( tilePos.y )
{
	assertion( mMap, "Cannot create Map::ConstIterator without a Map reference!" );
}


Map::ConstIterator::ConstIterator( const Map* map, short x, short y ) :
	mMap( map ), mX( x ), mY( y )
{
	assertion( mMap, "Cannot create Map::ConstIterator without a Map reference!" );
}


Map::ConstIterator::~ConstIterator() { }


Map::ConstIterator Map::ConstIterator::GetAdjacent( CardinalDirection direction ) const
{
	short x = mX, y = mY;

	switch( direction )
	{
	case CARDINAL_DIRECTION_EAST:
		++x;
		break;

	case CARDINAL_DIRECTION_NORTH:
		++y;
		break;

	case CARDINAL_DIRECTION_WEST:
		--x;
		break;

	case CARDINAL_DIRECTION_SOUTH:
		--y;
		break;
	}

	return ConstIterator( mMap, x, y );
}


const Tile* Map::ConstIterator::operator->() const
{
	return mMap->mTiles[ mMap->GetTileIndex( mX, mY ) ];
}


bool Map::ConstIterator::IsValid() const
{
	return mMap->IsValidTilePos( mX, mY );
}


Map::Iterator::Iterator( Map* map ) :
	mMap( map ), mX( -1 ), mY( -1 )
{
	assertion( mMap, "Cannot create Map::Iterator without a Map reference!" );
}


Map::Iterator::Iterator( Map* map, short x, short y ) :
	mMap( map ), mX( x ), mY( y )
{
	assertion( mMap, "Cannot create Map::Iterator without a Map reference!" );
}


Map::Iterator::Iterator( const Map* map, const Vec2s& tilePos ) :
	mMap( map ), mX( tilePos.x ), mY( tilePos.y )
{
	assertion( mMap, "Cannot create Map::ConstIterator without a Map reference!" );
}


Map::Iterator::~Iterator() { }


Map::Iterator Map::Iterator::GetAdjacent( CardinalDirection direction ) const
{
	short x = mX, y = mY;

	switch( direction )
	{
	case CARDINAL_DIRECTION_EAST:
		++x;
		break;

	case CARDINAL_DIRECTION_NORTH:
		++y;
		break;

	case CARDINAL_DIRECTION_WEST:
		--x;
		break;

	case CARDINAL_DIRECTION_SOUTH:
		--y;
		break;
	}

	return ConstIterator( mMap, x, y );
}


Tile* Map::Iterator::operator->() const
{
	return mMap->mTiles[ mMap->GetTileIndex( mX, mY ) ];
}


bool Map::Iterator::IsValid() const
{
	return mMap->IsValidTilePos( mX, mY );
}


Map::Map( Database* database ) :
	mDatabase( database )
{ }


Map::~Map() { }


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


bool Map::IsValidTilePos( short x, short y ) const
{
	return ( x >= 0 && x < mWidth && y >= 0 && y < mHeight );
}


size_t Map::GetTileIndex( short x, short y ) const
{
	assertion( IsValidTilePos( x, y ), "Cannot get tile index for invalid tile position (%d,%d)!", x, y );
	return ( ( y << MAX_SIZE_EXPONENT ) + x );
}
