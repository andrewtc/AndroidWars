#include "androidwars.h"

using namespace mage;


HashString TileSprite::ChooseTileVariation( const Map::ConstIterator& tile )
{
	HashString result = "Idle";

	if( tile.IsValid() )
	{
		TerrainType* terrainType = tile->GetTerrainType();

		if( terrainType->IsPath() )
		{
			std::stringstream formatter;

			int adjacentCount = 0;

			for( int i = 0; i < NUM_DIRECTIONS; ++i )
			{
				CardinalDirection direction = (CardinalDirection) ( FIRST_VALID_DIRECTION + i );
				Map::ConstIterator adjacent = tile.GetAdjacent( direction );

				if( adjacent.IsValid() && adjacent->GetTerrainType() == terrainType )
				{
					// Add the direction name to the variation name.
					formatter << GetDirectionName( direction );
					++adjacentCount;
				}
			}

			if( adjacentCount > 0 && adjacentCount < 4 )
			{
				// Return the formatted variation name.
				result = formatter.str();
			}
		}
	}

	return result;
}


TileSprite::TileSprite() :
	mWorld( nullptr ),
	mSprite( nullptr )
{ }


TileSprite::~TileSprite()
{
	DestroySprite();
}


void TileSprite::Init( World* world, const Vec2s& tilePos )
{
	mWorld = world;
	assertion( mWorld, "Cannot initialize TileSprite without valid World!" );

	// Initialize the Tile pointer.
	mTile = world->GetMap()->GetTile( tilePos );

	// Create initial Sprite.
	UpdateSprite();
}


void TileSprite::Update( float elapsedTime )
{
	if( mSprite )
	{
		// Update the Sprite.
		mSprite->OnUpdate( elapsedTime );
	}
}


void TileSprite::Draw()
{
	if( mSprite )
	{
		// Draw the Sprite.
		mSprite->OnDraw( *mWorld->GetCamera() );
	}
}


Map::Iterator TileSprite::GetTile() const
{
	return mTile;
}


void TileSprite::UpdateSprite()
{
	DestroySprite();

	if( mTile.IsValid() )
	{
		TerrainType* terrainType = mTile->GetTerrainType();

		if( terrainType )
		{
			// Determine the World position of the Sprite.
			Vec2s tilePos = mTile.GetTilePos();
			Vec2f worldPos = mWorld->TileToWorldCoords( tilePos );
			//DebugPrintf( "(%d,%d): {%.3f,%.3f}", tilePos.x, tilePos.y, worldPos.x, worldPos.y );

			// Determine the animation set to use and the animation to play.
			HashString animationSetName = terrainType->GetAnimationSetName();
			HashString animation = ChooseTileVariation( mTile );

			// Create a new Sprite.
			mSprite = SpriteManager::CreateSprite( animationSetName, worldPos, animation );
		}
	}
}


void TileSprite::DestroySprite()
{
	if( mSprite )
	{
		// If there was a previous Sprite, destroy it.
		SpriteManager::DestroySprite( mSprite );
	}
}
