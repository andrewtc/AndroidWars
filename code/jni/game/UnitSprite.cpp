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

	// Determine the position of the Unit in the MapView.
	Vec2f worldPos = mMapView->TileToWorldCoords( mUnit->GetTilePos() );

	// Create the Sprite.
	UnitType* unitType = mUnit->GetUnitType();
	mSprite = SpriteManager::CreateSprite( unitType->GetAnimationSetName(), mUnit->GetTilePos() );
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


Vec2f UnitSprite::GetPosition() const
{
	return mSprite->Position;
}
