/*
 * Author      : Matthew Johnson
 * Date        : 30/Jan/2013
 * Description :
 *   Utility Class for testing for and finding collisions.
 *   Each primitive shape should have the following functions:
 *     bool Test( ShapeA, ShapeB )
 *     CollisionInfo Find( float time, ShapeA, velocityA, ShapeB, velocityB )
 *   Variation on these may be added for certain shapes.
 *
 *   Test return bool for if a collision occures.
 *   Find return a CollisionInfo structure detailing the collision.
 *     CollisionInfo:
 *       bool Collision
 *       float ContactTime
 *       Vec3f ContactPoint
 */
 
#pragma once

#include "Geometry.h"

namespace mage
{

	class Intersection
	{
	public:

		struct CollisionInfo
		{
			CollisionInfo()
				: Collision( false )
				, ContactTime( 0.0f )
				, ContactPoint( Vec3f::ZERO )
			{}

			bool Collision;
			float ContactTime;
			Vec3f ContactPoint;
		};

		// Sphere v Frustum
		static int Test( const Spheref& sphere, const Frustum& frustum );

		// Sphere v Sphere
		static bool Test( const Spheref& A, const Spheref& B );
		static CollisionInfo Find( float t, const Spheref& A, const Vec3f& velA, const Spheref& B, const Vec3f& velB );

		// Sphere v Plane
		static bool Test( const Spheref& sphere, const Planef& plane );

		// Line/Ray v Shpere
		static bool Test( const Ray3f& ray, const Spheref& sphere );

		// Line/Ray v Plane
		static bool Test( const Ray3f& ray, const Planef& plane );
		static CollisionInfo Find( const Ray3f& ray, const Planef& plane );

		// Box3 v Box3
		static bool Test( const Box3f& A, const Box3f& B );
		// TODO Find intersection

	private:
		Intersection();
		~Intersection();
	};

}