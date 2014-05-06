#include "androidwars.h"

using namespace mage;


const char* const UnitSprite::DEFAULT_ANIMATION_NAME = "Idle";


UnitSprite::UnitSprite( MapView* mapView, Unit* unit ) :
	mMapView( mapView ), mUnit( unit ), mIsInitialized( false ), mIsVisible( true ), mSprite( nullptr )
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

	// Listen for changes to the Unit.
	mUnit->OnOwnerChanged.AddCallback( this, &UnitSprite::OnUnitOwnerChanged );
	mUnit->OnTileChanged.AddCallback( this, &UnitSprite::OnUnitTileChanged );
	mUnit->OnTakeDamage.AddCallback( this, &UnitSprite::OnUnitTakeDamage );
	mUnit->OnHealthChanged.AddCallback( this, &UnitSprite::OnUnitHealthChanged );
	mUnit->OnDestroyed.AddCallback( this, &UnitSprite::OnUnitDestroyed );
	mUnit->OnActivate.AddCallback( this, &UnitSprite::OnUnitActivate );
	mUnit->OnDeactivate.AddCallback( this, &UnitSprite::OnUnitDeactivate );

	// Determine the position of the Unit in the MapView.
	Vec2f worldPos = mMapView->TileToWorldCoords( mUnit->GetTilePos() );

	// Create the Sprite.
	UnitType* unitType = mUnit->GetUnitType();
	mSprite = SpriteManager::CreateSprite( unitType->GetAnimationSetName(), mUnit->GetTilePos() );

	// Set the initial position.
	UpdateTilePosition();

	// Call initial callbacks to update unit properties.
	OnUnitOwnerChanged( mUnit->GetOwner(), nullptr );
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
	if( mIsVisible )
	{
		// If the UnitSprite is visible, draw the internal Sprite.
		mSprite->OnDraw( camera );

		if( mUnit->IsDamaged() )
		{
			// Draw health.
			BitmapFont* font = mMapView->GetDefaultFont();
			Vec2f textPos = ( GetPosition() - camera.GetPosition() );
			float height = ( mSprite->GetClippingRectForCurrentAnimation().Height() * 0.5f );
			DrawTextFormat( textPos.x, textPos.y + height - font->GetLineHeight(), font, "%d", mUnit->GetHealth() );
		}
	}
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


MapView* UnitSprite::GetMapView() const
{
	return mMapView;
}


void UnitSprite::OnOwnerColorChanged( const Color& color )
{
	// Update the color of the Sprite.
	UpdateColor();
}


void UnitSprite::OnUnitOwnerChanged( Faction* owner, Faction* formerOwner )
{
	// Update the color of the sprite.
	UpdateColor();

	if( formerOwner )
	{
		// Unbind callbacks from the old Faction.
		formerOwner->OnColorChanged.RemoveCallback( this, &UnitSprite::OnOwnerColorChanged );
	}

	// Bind callbacks on the new Faction.
	owner->OnColorChanged.AddCallback( this, &UnitSprite::OnOwnerColorChanged );
}


void UnitSprite::OnUnitTileChanged( const Map::Iterator& tile )
{
	UpdateTilePosition();
}


void UnitSprite::OnUnitTakeDamage( int health, Unit* instigator )
{
	// TODO: Show damage animation.
}


void UnitSprite::OnUnitHealthChanged( int health )
{
	// TODO
}


void UnitSprite::OnUnitDestroyed()
{
	// Tell the MapView to destroy this UnitSprite.
	mMapView->DestroyUnitSprite( this );
}


void UnitSprite::OnUnitActivate()
{
	//DebugPrintf( "Activating Unit!" );

	// Update the color of the UnitSprite.
	UpdateColor();
}


void UnitSprite::OnUnitDeactivate()
{
	//DebugPrintf( "Deactivating Unit!" );

	// Update the color of the UnitSprite.
	UpdateColor();
}


void UnitSprite::UpdateColor()
{
	Color color;

	if( mUnit->IsActive() )
	{
		// Set the color of the Unit to the owning Faction's color.
		mSprite->DrawColor = mUnit->GetOwner()->GetColor();
	}
	else
	{
		// Adjust color for inactive state.
		mSprite->DrawColor = mUnit->GetOwner()->GetInactiveColor();
	}
}


void UnitSprite::UpdateTilePosition()
{
	// Get the current Tile for the Unit.
	Map::Iterator tile = mUnit->GetTile();

	// Hide or show the UnitSprite depending on whether its Unit is still on the game board.
	mIsVisible = tile.IsValid();

	if( mIsVisible )
	{
		// If the UnitSprite is still visible, update its position.
		SetPosition( mMapView->TileToWorldCoords( mUnit->GetTilePos() ) );
	}
}
