#include "androidwars.h"

using namespace mage;

const float World::TILE_WORLD_SCALE = 64.0f;
const float World::INVERSE_TILE_WORLD_SCALE = ( 1.0f / TILE_WORLD_SCALE );

const float World::MAP_BORDER_SCALE = ( TILE_WORLD_SCALE * 5.0f );


World::World() :
	mMap( nullptr ),
	mCamera( gWindowWidth, gWindowHeight )
{ }


World::~World()
{
	if( IsInitialized() )
	{
		Destroy();
	}
}


void World::Init( Map* map )
{
	assertion( !IsInitialized(), "Cannot initialize World that has already been initialized!" );

	// Set the Map.
	mMap = map;
	assertion( mMap, "Cannot create World without a valid Map!" );

	// Resize to fill the whole tile buffer (for initialization).
	// TODO: Allow this without resizing.
	mTileSprites.ForEachTileInMaxArea( [this]( const TileSpritesGrid::Iterator& tileSprite )
	{
		// Initialize all TileSprites.
		tileSprite->Init( this, tileSprite.GetTilePos() );
	});

	// Create initial tile sprites.
	MapResized( Vec2s::ZERO, mMap->GetSize() );

	// Listen for when the Map is resized.
	mMap->OnResize.AddCallback( this, &World::MapResized );

	// Listen for when the Map is changed.
	mMap->OnTileChanged.AddCallback( this, &World::TileChanged );
}


void World::Destroy()
{
	assertion( !IsInitialized(), "Cannot destroy World that is not initialized!" );

	// Remove the resize callback.
	mMap->OnResize.RemoveCallback( this, &World::MapResized );

	// Reset the Map reference.
	mMap = nullptr;
}


void World::Update( float elapsedTime )
{
	mTileSprites.ForEachTile( [ this, elapsedTime ]( const TileSpritesGrid::Iterator& tileSprite )
	{
		// Update the TileSprite that represents this Tile.
		tileSprite->Update( elapsedTime );
	});
}


void World::Draw()
{
	mTileSprites.ForEachTile( [ this ]( const TileSpritesGrid::Iterator& tileSprite )
	{
		// Draw the TileSprite that represents this Tile.
		tileSprite->Draw();
	});
}


Map* World::GetMap() const
{
	return mMap;
}


RectF World::GetMapBounds() const
{
	return RectF( 0.0f, 0.0f, mMap->GetWidth() * TILE_WORLD_SCALE, mMap->GetHeight() * TILE_WORLD_SCALE );
}


RectF World::GetCameraBounds() const
{
	RectF cameraBounds = GetMapBounds();

	cameraBounds.Left -= MAP_BORDER_SCALE;
	cameraBounds.Top -= MAP_BORDER_SCALE;
	cameraBounds.Right += MAP_BORDER_SCALE;
	cameraBounds.Bottom += MAP_BORDER_SCALE;

	return cameraBounds;
}


Camera* World::GetCamera()
{
	return &mCamera;
}


const Camera* World::GetCamera() const
{
	return &mCamera;
}


void World::CenterCamera()
{
	// Set the position of the Camera to the center of the World.
	RectF bounds = GetCameraBounds();
	mCamera.LookAt( Vec2f( bounds.CenterX(), bounds.CenterY() ) );
}



Vec2f World::WorldToScreenCoords( const Vec2f& worldCoords ) const
{
	return WorldToScreenCoords( worldCoords.x, worldCoords.y );
}


Vec2f World::WorldToScreenCoords( float worldX, float worldY ) const
{
	return Vec2f( worldX, worldY ) - mCamera.GetPosition();
}


Vec2f World::ScreenToWorldCoords( const Vec2f& screenCoords ) const
{
	return ScreenToWorldCoords( screenCoords.x, screenCoords.y );
}


Vec2f World::ScreenToWorldCoords( float screenX, float screenY ) const
{
	return Vec2f( screenX, screenY ) + mCamera.GetPosition();
}


Vec2s World::WorldToTileCoords( const Vec2f& worldCoords ) const
{
	return WorldToTileCoords( worldCoords.x, worldCoords.y );
}


Vec2s World::WorldToTileCoords( float worldX, float worldY ) const
{
	return Vec2s( worldX * INVERSE_TILE_WORLD_SCALE, worldY * INVERSE_TILE_WORLD_SCALE );
}


Vec2f World::TileToWorldCoords( const Vec2s& tileCoords ) const
{
	return TileToWorldCoords( tileCoords.x, tileCoords.y );
}


Vec2f World::TileToWorldCoords( short tileX, short tileY ) const
{
	return Vec2f( tileX * TILE_WORLD_SCALE, tileY * TILE_WORLD_SCALE );
}


bool World::IsInitialized() const
{
	return ( mMap != nullptr );
}


void World::MapResized( const Vec2s& oldSize, const Vec2s& newSize )
{
	DebugPrintf( "Resized map from (%d,%d) to (%d,%d).", oldSize.x, oldSize.y, newSize.x, newSize.y );

	// Determine the area of TileSprites affected by the resize.
	short refreshWidth  = std::max( oldSize.x, newSize.x );
	short refreshHeight = std::max( oldSize.y, newSize.y );
	RectS area( 0, 0, refreshWidth, refreshHeight );

	mTileSprites.ForEachTileInArea( area, []( const TileSpritesGrid::Iterator& tileSprite )
	{
		// Update the sprites of all TileSprites in the refresh area.
		tileSprite->UpdateSprite();
	});

	// Resize the TileSprites grid.
	mTileSprites.Resize( newSize );
}


void World::TileChanged( const Map::Iterator& tile )
{
	// Update the TileSprite for this tile.
	TileSpritesGrid::Iterator tileSprite = mTileSprites.GetTile( tile.GetTilePos() );
	tileSprite->UpdateSprite();

	tileSprite.ForEachAdjacent( []( const TileSpritesGrid::Iterator& adjacent )
	{
		// Update adjacent TileSprites.
		adjacent->UpdateSprite();
	});
}
