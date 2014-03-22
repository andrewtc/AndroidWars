#include "androidwars.h"

using namespace mage;

const float World::TILE_WORLD_SCALE = 64.0f;
const float World::INVERSE_TILE_WORLD_SCALE = ( 1.0f / TILE_WORLD_SCALE );

const float World::MAP_BORDER_SCALE = ( TILE_WORLD_SCALE * 5.0f );


World::World( Map* map ) :
	mMap( map ),
	mCamera( gWindowWidth, gWindowHeight )
{

}


World::~World()
{

}


void World::Update( float elapsedTime )
{
	for( auto it = mTileSpritesByName.begin(); it != mTileSpritesByName.end(); ++it )
	{
		// Update all sprites.
		it->second->OnUpdate( elapsedTime );
	}
}


void World::Draw()
{
	mMap->ForEachTile( [ this ]( const Map::Iterator& tile )
	{
		// Get the tile position.
		Vec2s tilePos = tile.GetTilePos();

		// Calculate the world position of the tile.
		Vec2f worldPos = ( tilePos * TILE_WORLD_SCALE );

		// Draw the tile.
		TerrainType* terrainType = tile->GetTerrainType();

		if( terrainType )
		{
			// Look up the Sprite for this TerrainType (if it exists).
			Sprite* tileSprite = CreateOrGetSprite( terrainType->GetAnimationSetName() );

			if( tileSprite )
			{
				// Draw the Sprite for the tile.
				tileSprite->Position = worldPos;
				tileSprite->OnDraw( mCamera );
			}
		}
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


Sprite* World::CreateOrGetSprite( const HashString& animationSetName )
{
	Sprite* result = nullptr;

	// Look up an existing sprite for the animation set (if any).
	auto it = mTileSpritesByName.find( animationSetName );

	if( it == mTileSpritesByName.end() )
	{
		// If no existing Sprite was found, create one.
		result = SpriteManager::CreateSprite( animationSetName, Vec2f::ZERO, "Idle" );

		// Store the Sprite by name.
		mTileSpritesByName[ animationSetName ] = result;
	}
	else
	{
		// Otherwise, use the existing Sprite.
		result = it->second;
	}

	return result;
}
