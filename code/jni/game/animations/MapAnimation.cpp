#include "androidwars.h"

using namespace mage;


MapAnimation::MapAnimation() :
	mMapView( nullptr ),
	mState( STATE_NOT_STARTED ),
	mTimer( 0.0f )
{ }


MapAnimation::~MapAnimation() { }


void MapAnimation::Start( MapView* mapView )
{
	assertion( mState == STATE_NOT_STARTED, "Cannot start MapAnimation that has already been started!" );
	assertion( mapView, "Cannot start MapAnimation without a valid MapView!" );

	DebugPrintf( "Starting MapAnimation..." );

	// Start the animation.
	mState = STATE_PLAYING;
	mMapView = mapView;
	mTimer = 0.0f;

	// Run custom animation start code.
	OnStart();
}


void MapAnimation::Finish()
{
	assertion( mState == STATE_PLAYING, "Cannot finish MapAnimation that is not currently playing!" );

	DebugPrintf( "Finished MapAnimation!" );

	// Finish the animation.
	mState = STATE_FINISHED;

	// Run custom animation finish code.
	OnFinish();

	// Fire the finished callback.
	OnFinished.Invoke();

	mMapView = nullptr;
}


void MapAnimation::Update( float elapsedTime )
{
	assertion( mState == STATE_PLAYING, "Cannot update MapAnimation that is not currently playing!" );

	// Update the animation timer.
	mTimer += elapsedTime;

	// Run custom animation update code.
	OnUpdate( elapsedTime );
}


MapAnimation::State MapAnimation::GetState() const
{
	return mState;
}


bool MapAnimation::IsNotStarted() const
{
	return ( mState == STATE_NOT_STARTED );
}


bool MapAnimation::IsPlaying() const
{
	return ( mState == STATE_PLAYING );
}


bool MapAnimation::IsFinished() const
{
	return ( mState == STATE_FINISHED );
}


float MapAnimation::GetElapsedTime() const
{
	return mTimer;
}


MapView* MapAnimation::GetMapView() const
{
	return mMapView;
}


UnitMapAnimation::UnitMapAnimation( UnitSprite* unitSprite ) :
	mUnitSprite( unitSprite )
{ }


UnitMapAnimation::~UnitMapAnimation() { }


UnitSprite* UnitMapAnimation::GetUnitSprite() const
{
	return mUnitSprite;
}
