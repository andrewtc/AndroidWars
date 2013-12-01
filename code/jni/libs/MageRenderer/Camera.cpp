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
	LookAt( Vec2f::ZERO );
}
//---------------------------------------
Camera::~Camera()
{}
//---------------------------------------
void Camera::SetPositin( const Vec2f& pos )
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
	mPosition.Set( pos.x - mViewportRect.Right * 0.5f, pos.y - mViewportRect.Bottom * 0.5f );
	
	// Clamp in X
	if ( mWorldBounds.Width() <= mViewportRect.Right )
	{
		mPosition.x = mWorldBounds.Left;
	}
	else
	{
		Mathf::ClampToRange( mPosition.x,
				mWorldBounds.Left,
				mWorldBounds.Right - mViewportRect.Right );
	}

	// Clamp in Y
	if ( mWorldBounds.Height() <= mViewportRect.Bottom )
	{
		mPosition.y = mWorldBounds.Top;
	}
	else
	{
		Mathf::ClampToRange( mPosition.y,
			mWorldBounds.Top,
			mWorldBounds.Bottom - mViewportRect.Bottom );
	}
}
//---------------------------------------
void Camera::LookAtClamp( Vec2f& pos )
{
	LookAt( pos );
	pos = mPosition + Vec2f( mViewportRect.Right * 0.5f, mViewportRect.Bottom * 0.5f );
}
//---------------------------------------
void Camera::SetWorldBounds( const RectF& worldBounds )
{
	mWorldBounds = worldBounds;
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
