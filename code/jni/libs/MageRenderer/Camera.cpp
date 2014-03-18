#include "RendererLib.h"

using namespace mage;

MAGE_IMPLEMENT_RTTI( Object, Camera );

//---------------------------------------
Camera::Camera( float viewWidth, float viewHeight )
	: Object( "Camera" )
	, mViewWidth( viewWidth )
	, mViewHeight( viewHeight )
{
	SetViewport( viewWidth, viewHeight );
	mWorldBounds.Set( -1, -1, 0, 0 );
	mLookAtBounds.Set( 0, 0, 0, 0 );
	LookAt( Vec2f::ZERO );
}
//---------------------------------------
Camera::Camera( float viewWidth, float viewHeight, const std::string& name )
	: Object( name )
	, mViewWidth( viewWidth )
	, mViewHeight( viewHeight )
{
	SetViewport( viewWidth, viewHeight );
	mWorldBounds.Set( -1, -1, 0, 0 );
	mLookAtBounds.Set( 0, 0, 0, 0 );
	LookAt( Vec2f::ZERO );
}
//---------------------------------------
Camera::~Camera()
{}
//---------------------------------------
void Camera::SetPosition( const Vec2f& pos )
{
	mPosition = pos;
	mViewportRect.Left = pos.x;
	mViewportRect.Top = pos.y;
}
//---------------------------------------
void Camera::SetViewport( float width, float height )
{
	mViewportRect.Set( 0, 0, width, height );
}
//---------------------------------------
void Camera::LookAt( const Vec2f& pos )
{
	DebugPrintf( "Look-at position: (%.3f,%.3f)", pos.x, pos.y );

	// Find the top left position of the Camera.
	Vec2f topLeft( pos.x - ( mViewWidth * 0.5f ),
				   pos.y - ( mViewHeight * 0.5f ) );
	DebugPrintf( "Look-at top left: (%.3f,%.3f)", topLeft.x, topLeft.y );

	// Clamp the top left of the Camera to the look-at bounds.
	Mathf::ClampToRange( topLeft.x, mLookAtBounds.Left, mLookAtBounds.Right );
	Mathf::ClampToRange( topLeft.y, mLookAtBounds.Top, mLookAtBounds.Bottom );
	DebugPrintf( "Clamped look-at position: (%.3f,%.3f)", topLeft.x, topLeft.y );

	// Set the new position of the Camera.
	mPosition.Set( topLeft.x, topLeft.y );
}
//---------------------------------------
void Camera::LookAtClamp( Vec2f& pos )
{
	LookAt( pos );
	pos = mPosition + Vec2f( mViewWidth * 0.5f, mViewHeight * 0.5f );
}
//---------------------------------------
void Camera::SetWorldBounds( const RectF& worldBounds )
{
	// Set the world bounds.
	mWorldBounds = worldBounds;
	assertion( mWorldBounds.IsValid(), "Cannot set invalid world bounds (%.3f, %.3f, %.3f, %.3f) on Camera!", mWorldBounds.Left, mWorldBounds.Top, mWorldBounds.Right, mWorldBounds.Bottom );
	DebugPrintf( "Set Camera world bounds to (%.3f, %.3f, %.3f, %.3f).", mWorldBounds.Left, mWorldBounds.Top, mWorldBounds.Right, mWorldBounds.Bottom );
	DebugPrintf( "World bounds size is (%.3f, %.3f).", mWorldBounds.Width(), mWorldBounds.Height() );
	DebugPrintf( "View bounds are (%.3f, %.3f, %.3f, %.3f).", mViewportRect.Left, mViewportRect.Top, mViewportRect.Right, mViewportRect.Bottom );

	// Calculate new look-at bounds.
	float halfWorldBoundsWidth = ( 0.5f * mWorldBounds.Width() );
	float halfWorldBoundsHeight = ( 0.5f * mWorldBounds.Height() );
	float halfViewWidth = ( 0.5f * mViewWidth );
	float halfViewHeight = ( 0.5f * mViewHeight );

	Vec2f worldBoundsCenter( mWorldBounds.CenterX(), mWorldBounds.CenterY() );
	DebugPrintf( "World bounds center is (%.3f, %.3f).", worldBoundsCenter.x, worldBoundsCenter.y );

	float halfLookAtBoundsWidth  = std::max( halfWorldBoundsWidth - halfViewWidth, 0.0f );
	float halfLookAtBoundsHeight = std::max( halfWorldBoundsHeight - halfViewHeight, 0.0f );

	mLookAtBounds.Left   = ( worldBoundsCenter.x - halfLookAtBoundsWidth  ) - halfViewWidth;
	mLookAtBounds.Top    = ( worldBoundsCenter.y - halfLookAtBoundsHeight ) - halfViewHeight;
	mLookAtBounds.Right  = ( worldBoundsCenter.x + halfLookAtBoundsWidth  ) - halfViewWidth;
	mLookAtBounds.Bottom = ( worldBoundsCenter.y + halfLookAtBoundsHeight ) - halfViewHeight;

	DebugPrintf( "Camera look-at bounds are now (%.3f, %.3f, %.3f, %.3f).", mLookAtBounds.Left, mLookAtBounds.Top, mLookAtBounds.Right, mLookAtBounds.Bottom );
}
//---------------------------------------
bool Camera::RectInViewport( RectF rect ) const
{
	// Put rect into camera space
	rect.Left -= mPosition.x;
	rect.Top  -= mPosition.y;
	// Test intersection
	return RectF::TestIntersection( mViewportRect, rect );
}
//---------------------------------------
