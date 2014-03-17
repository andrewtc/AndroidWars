#include "androidwars.h"

using namespace mage;

const float World::TILE_WORLD_SCALE = 64.0f;


World::World( Map* map ) :
	mMap( map )
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


void World::Draw( const Camera& camera )
{
	mMap->ForEachTile( [ this, &camera ]( const Map::Iterator& tile )
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
				tileSprite->OnDraw( camera );
			}
		}

		// Debug draw.
		DrawRectOutline( worldPos.x, worldPos.y, TILE_WORLD_SCALE, TILE_WORLD_SCALE, Color::WHITE, 1.0f );
	});
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
