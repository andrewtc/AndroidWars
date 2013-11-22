#include "Frustum.h"
#include "MathUtil.h"

using namespace mage;

//---------------------------------------
Frustum::Frustum()
{}
//---------------------------------------
Frustum::~Frustum()
{}
//---------------------------------------
void Frustum::SetFrustum( float angle, float ratio, float zNear, float zFar )
{
	mAngle = angle;
	mRatio = ratio;
	mNearDist = zNear;
	mFarDist = zFar;

	const float tanAngle = 2.0f * std::tan( Mathf::DegreesToRadians( 0.5f * mAngle ) );
	mNearH = mNearDist * tanAngle;
	mNearW = mNearH * mRatio;
	mFarH = mFarDist * tanAngle;
	mFarW = mFarH * mRatio;
}
//---------------------------------------
void Frustum::SetCamera( const Vec3f& p, const Vec3f& d, const Vec3f& u )
{
	Vec3f nc, fc, x, y, z;

	z = p - d;
	z.Normalize();

	x = u.Cross( z );
	x.Normalize();

	y = z.Cross( x );

	nc = p - z * mNearDist;
	fc = p - z * mFarDist;

	mNearPoints[0] = nc + y * mNearH - x * mNearW;
	mNearPoints[1] = nc + y * mNearH + x * mNearW;
	mNearPoints[2] = nc - y * mNearH - x * mNearW;
	mNearPoints[3] = nc - y * mNearH + x * mNearW;

	mFarPoints[0] = fc + y * mFarH - x * mFarW;
	mFarPoints[1] = fc + y * mFarH + x * mFarW;
	mFarPoints[2] = fc - y * mFarH - x * mFarW;
	mFarPoints[3] = fc - y * mFarH + x * mFarW;

	mPlanes[ FP_TOP    ] = Planef( mNearPoints[1], mNearPoints[0], mFarPoints[0] );
	mPlanes[ FP_BOTTOM ] = Planef( mNearPoints[2], mNearPoints[3], mFarPoints[3] );
	mPlanes[ FP_LEFT   ] = Planef( mNearPoints[0], mNearPoints[2], mFarPoints[2] );
	mPlanes[ FP_RIGHT  ] = Planef( mNearPoints[3], mNearPoints[1], mFarPoints[3] );
	mPlanes[ FP_NEAR   ] = Planef( mNearPoints[0], mNearPoints[1], mNearPoints[3] );
	mPlanes[ FP_FAR    ] = Planef( mFarPoints[1], mFarPoints[0], mFarPoints[2] );
}
//---------------------------------------
const Planef& Frustum::GetPlane( int i ) const
{
	return mPlanes[i];
}
//---------------------------------------