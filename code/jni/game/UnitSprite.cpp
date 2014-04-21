#include "androidwars.h"

using namespace mage;


UnitSprite::UnitSprite( MapView* mapView, Unit* unit ) :
	mMapView( mapView ), mUnit( unit ), mIsInitialized( false ), mSprite( nullptr )
{
	assertion( mMapView, "Cannot create UnitSprite without a valid MapView!" );
	assertion( mUnit, "Cannot create UnitSprite without a valid Unit!" );
}


UnitSprite::~UnitSprite()
{
	if( IsInitialized() )
	{
		Destroy();
	}
}


void UnitSprite::Init()
{
	assertion( !IsInitialized(), "Cannot initialize UnitSprite that has already been initialized!" );
	mIsInitialized = true;

	// Listen for changes to the Unit's position.
	mUnit->OnTeleport.AddCallback( this, &UnitSprite::OnUnitTeleport );
	mUnit->OnMove.AddCallback( this, &UnitSprite::OnUnitMove );

	// Determine the position of the Unit in the MapView.
	Vec2f worldPos = mMapView->TileToWorldCoords( mUnit->GetTilePos() );

	// Create the Sprite.
	UnitType* unitType = mUnit->GetUnitType();
	mSprite = SpriteManager::CreateSprite( unitType->GetAnimationSetName(), mUnit->GetTilePos() );

	// Move the Sprite to the proper location on the Map.
	SetPosition( mMapView->TileToWorldCoords( mUnit->GetTilePos() ) );
}


void UnitSprite::Destroy()
{
	assertion( IsInitialized(), "Cannot destroy UnitSprite that has not been initialized!" );
	mIsInitialized = false;

	// Destroy the sprite.
	SpriteManager::DestroySprite( mSprite );
}


bool UnitSprite::IsInitialized() const
{
	return mIsInitialized;
}


void UnitSprite::Update( float elapsedTime )
{
	if( mIsMoving )
	{
		// Update the movement timer.
		mMoveAnimationTimer += elapsedTime;

		if( mMoveAnimationTimer >= 1.0f )
		{
			// If the move animation is finished, set the position of the sprite to the destination.
			mSprite->Position = mMapView->TileToWorldCoords( mMovementPath.GetDestination() );

			// End the animation.
			mIsMoving = false;
		}
		else
		{
			// Otherwise, update the position of the sprite.
			Vec2f tilePosition = mMovementPath.Interpolate( mMoveAnimationTimer );
			mSprite->Position = ( tilePosition * MapView::TILE_WORLD_SCALE );
		}
	}

	// Update the Sprite.
	mSprite->OnUpdate( elapsedTime );
}


void UnitSprite::Draw( const Camera& camera )
{
	// Draw the Sprite at the location of the Unit.
	mSprite->OnDraw( camera );

	// Draw health.
	BitmapFont* font = mMapView->GetDefaultFont();
	Vec2f textPos = ( GetPosition() - camera.GetPosition() );
	float height = ( mSprite->GetClippingRectForCurrentAnimation().Height() * 0.5f );
	DrawTextFormat( textPos.x, textPos.y + height - font->GetLineHeight(), font, "%d", mUnit->GetHealth() );
}


void UnitSprite::SetPosition( const Vec2f& position )
{
	mSprite->Position = position;
}


Vec2f UnitSprite::GetPosition() const
{
	return mSprite->Position;
}


Unit* UnitSprite::GetUnit() const
{
	return mUnit;
}


Sprite* UnitSprite::GetSprite() const
{
	return mSprite;
}


RectF UnitSprite::GetWorldBounds() const
{
	// Get the Sprite bounds.
	RectF bounds = mSprite->GetClippingRectForCurrentAnimation();

	// Offset the bounds by the current Sprite position.
	Vec2f position = mSprite->Position;

	bounds.Left   += position.x;
	bounds.Top    += position.y;
	bounds.Right  += position.x;
	bounds.Bottom += position.y;

	return bounds;
}


void UnitSprite::OnUnitTeleport( const Map::Iterator& tile )
{
	// Update the sprite position.
	SetPosition( mMapView->TileToWorldCoords( mUnit->GetTilePos() ) );
}


void UnitSprite::OnUnitMove( const Path& path )
{
	// Start the move animation.
	DebugPrintf( "Starting UnitSprite move animation." );
	mIsMoving = true;
	mMoveAnimationTimer = 0.0f;
	mMovementPath = path;
}
