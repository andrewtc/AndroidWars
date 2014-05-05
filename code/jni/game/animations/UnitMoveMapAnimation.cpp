#include "androidwars.h"

using namespace mage;


const float UnitMoveMapAnimation::DEFAULT_MOVEMENT_SPEED = 8.0f;


UnitMoveMapAnimation::UnitMoveMapAnimation( UnitSprite* unitSprite, const Path& movementPath, float movementSpeed ) :
	UnitMapAnimation( unitSprite ),
	mMovementPath( movementPath ),
	mMovementSpeed( movementSpeed ),
	mInterpolationSpeed( 0.0f )
{ }


UnitMoveMapAnimation::~UnitMoveMapAnimation() { }


void UnitMoveMapAnimation::OnStart()
{
	assertion( GetUnitSprite(), "Cannot start UnitMoveAnimation without a valid Unit!" );
	assertion( mMovementSpeed > 0.0f, "Cannot start UnitMoveAnimation with invalid movement speed %.1f!", mMovementSpeed );
	assertion( GetUnitSprite()->GetMapView() == GetMapView(), "Cannot start UnitMoveAnimation for a UnitSprite that was not created by the MapView associated with the animation!" );

	// Start the move animation.
	mPercentageComplete = 0.0f;

	if( mMovementPath.GetLength() > 0 )
	{
		// If the movement path has length, calculate the path interpolation speed.
		mInterpolationSpeed = ( mMovementSpeed / mMovementPath.GetLength() );
	}
	else
	{
		// Otherwise, finish the animation immediately.
		Finish();
	}
}


void UnitMoveMapAnimation::OnUpdate( float elapsedTime )
{
	// Update the movement timer.
	mPercentageComplete = std::min( mPercentageComplete + ( mInterpolationSpeed * elapsedTime ), 1.0f );

	// Update the position of the sprite.
	Vec2f tilePosition = mMovementPath.Interpolate( mPercentageComplete );
	GetUnitSprite()->SetPosition( tilePosition * MapView::TILE_WORLD_SCALE );

	if( mPercentageComplete >= 1.0f )
	{
		// If the Unit has reached the end of the path, finish the animation.
		Finish();
	}
}


void UnitMoveMapAnimation::OnFinish()
{
	// TODO
}
