/*
 * Author      : Matthew Johnson
 * Date        : 30/Jan/2013
 * Description :
 *   
 */
 
#pragma once

#include "Plane.h"

namespace mage
{

	class Frustum
	{
	public:
		Frustum();
		~Frustum();

		void SetFrustum( float angle, float ratio, float zNear, float zFar );
		void SetCamera( const Vec3f& p, const Vec3f& d, const Vec3f& u );

		const Planef& GetPlane( int i ) const;

//	private:
		// Frustum planes
		enum FrustumPlane
		{
			FP_TOP,
			FP_BOTTOM,
			FP_LEFT,
			FP_RIGHT,
			FP_NEAR,
			FP_FAR
		};
		Planef mPlanes[6];

		// top left, top right, bottom left, bottom right
		// 0		1			2			3
		Vec3f mNearPoints[4];
		Vec3f mFarPoints[4];

		float mNearDist, mFarDist;
		float mRatio, mAngle;
		float mNearW, mNearH;
		float mFarW, mFarH;
	};

}