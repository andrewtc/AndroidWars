#include "androidwars.h"

using namespace mage;


HashString TileSprite::ChooseTileVariation( const Map::ConstIterator& tile )
{
	// By default, use the default animation.
	HashString result = "Idle";

	if( tile.IsValid() )
	{
		//DebugPrintf( "Determining Variation for tile (%d,%d)...", tile.GetX(), tile.GetY() );

		// Get the TerrainType for the Tile.
		TerrainType* terrainType = tile->GetTerrainType();

		// Get the variations for the TerrainType.
		const TerrainType::Variations& variations = terrainType->GetVariations();

		for( auto it = variations.rbegin(); it != variations.rend(); ++it )
		{
			// Find the last variation that matches the tile.
			const Variation* variation = *it;
			//DebugPrintf( "Testing variation \"%s\"...", variation->GetAnimationName().GetCString() );

			if( TileMatchesVariation( tile, variation ) )
			{
				// Get the name of the animation to use for the tile.
				result = variation->GetAnimationName();
				break;
			}
		}
	}

	//DebugPrintf( "Chose variation \"%s\"", result.GetCString() );
	return result;
}


bool TileSprite::TileMatchesVariation( const Map::ConstIterator& tile, const Variation* variation )
{
	bool isVariationMatch = false;

	// Get the list of Conditions for this Variation.
	const Variation::Conditions& conditions = variation->GetConditions();

	for( auto it = conditions.begin(); it != conditions.end(); ++it )
	{
		// Get the list of Expressions in the condition.
		Variation::Condition* condition = *it;
		const Variation::Condition::ExpressionsByDirection& expressions = condition->GetExpressionsByDirection();
		bool isConditionMatch = true;
		//DebugPrintf( "Testing condition %d (%d expressions)...", it - conditions.begin(), expressions.size() );

		for( auto it2 = expressions.begin(); it2 != expressions.end(); ++it2 )
		{
			// Test each Expression in the Condition to see if this Tile is a match.
			PrimaryDirection direction = it2->first;

			// Determine what type to test.
			Map::ConstIterator adjacent = tile.GetAdjacent( direction );
			HashString expressionToTest = ( adjacent.IsValid() && adjacent->HasTerrainType() ? adjacent->GetTerrainType()->GetName() : "None" );
			//DebugPrintf( "Testing whether %s expression matches the tile (%d,%d) with type \"%s\"...", direction.GetName().GetCString(), adjacent.GetX(), adjacent.GetY(), expressionToTest.GetCString() );

			if( !condition->HasExpression( direction, expressionToTest ) )
			{
				// If the expression does not match, the condition doesn't match.
				//DebugPrintf( "Expression does NOT match!" );
				isConditionMatch = false;
				break;
			}
		}

		if( isConditionMatch )
		{
			isVariationMatch = true;
			break;
		}
	}

	return isVariationMatch;
}


TileSprite::TileSprite() :
	mMapView( nullptr ),
	mSprite( nullptr )
{ }


TileSprite::~TileSprite()
{
	DestroySprite();
}


void TileSprite::Init( MapView* mapView, const Vec2s& tilePos )
{
	mMapView = mapView;
	assertion( mMapView, "Cannot initialize TileSprite without valid World!" );

	// Initialize the Tile pointer.
	mTile = mapView->GetMap()->GetTile( tilePos );

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
		mSprite->OnDraw( *mMapView->GetCamera() );
	}
}


Map::Iterator TileSprite::GetTile() const
{
	return mTile;
}


Vec2s TileSprite::GetTilePos() const
{
	return mTile.GetPosition();
}


short TileSprite::GetTileX() const
{
	return mTile.GetX();
}


short TileSprite::GetTileY() const
{
	return mTile.GetY();
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
			Vec2s tilePos = mTile.GetPosition();
			Vec2f worldPos = mMapView->TileToWorldCoords( tilePos );
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
