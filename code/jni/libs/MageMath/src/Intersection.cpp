#include "Intersection.h"
#include "MathUtil.h"
#include "Matrix3.h"

using namespace mage;

//---------------------------------------
Intersection::Intersection()
{}
//---------------------------------------
Intersection::~Intersection()
{}
//---------------------------------------
// Sphere v Frustum
//---------------------------------------
int Intersection::Test( const Spheref& sphere, const Frustum& frustum )
{
	// 1, 0, -1 : inside, intersect, outside
	int res = 1;
	for ( int i = 0; i < 6; ++i )
	{
		float dist = frustum.GetPlane( i ).DistanceTo( sphere.Center );
		if ( dist < -sphere.Radius )
		{
			return -1;
		}
		if ( dist < sphere.Radius )
		{
			res = 0;
		}
	}
	return res;
}
//---------------------------------------
// Sphere v Sphere
//---------------------------------------
bool Intersection::Test( const Spheref& A, const Spheref& B )
{
	Vec3f d = B.Center - A.Center;
	float r = A.Radius + B.Radius;
	return d.LengthSqr() <= r*r;
}
//---------------------------------------
Intersection::CollisionInfo Intersection::Find( float t, const Spheref& A, const Vec3f& velA, const Spheref& B, const Vec3f& velB )
{
	// Get the relative velocity
	Vec3f vRel = velB - velA;
	float a = vRel.LengthSqr();

	// Get the minkowski sum of the spheres
	Vec3f center = B.Center - A.Center;
	float c = center.LengthSqr();
	float r = A.Radius + B.Radius;
	float r2 = r * r;

	CollisionInfo cInfo;

	if ( a > 0 )
	{
		float b = center.Dot( vRel );
		if ( b <= 0 )
		{
			if ( -t*a <= b || t*(t*a+2*b) + c <= r2 )
			{
				float deltaC = c - r2;
				float d = b*b - a*deltaC;

				if ( d >= 0 )
				{
					// Sphere start in intersection
					if ( deltaC <= 0 )
					{
						// Contact point is midpoint
						cInfo.ContactPoint = 0.5f * ( A.Center + B.Center );
						cInfo.ContactTime = 0.0f;
					}
					else
					{
						cInfo.ContactTime = -( b + std::sqrt( d ) ) / a;

						if ( cInfo.ContactTime < 0 )
						{
							cInfo.ContactTime = 0.0f;
						}
						else if ( cInfo.ContactTime > t )
						{
							cInfo.ContactTime = t;
						}

						cInfo.ContactPoint = A.Center + cInfo.ContactTime * velA +
							( A.Radius / r ) * ( center + cInfo.ContactTime * vRel );
					}
					cInfo.Collision = true;
					return cInfo;
				}
			}
			cInfo.Collision = false;
			return cInfo;
		}
	}

	// Sphere start in intersection
	if ( c <= r2 )
	{
		// Contact point is midpoint
		cInfo.ContactPoint = 0.5f * ( A.Center + B.Center );
		cInfo.ContactTime = 0.0f;
		cInfo.Collision = true;
		return cInfo;
	}

	cInfo.Collision = false;
	return cInfo;
}
//---------------------------------------
// Sphere v Plane
//---------------------------------------
bool Intersection::Test( const Spheref& sphere, const Planef& plane )
{
	float d = plane.DistanceTo( sphere.Center );
	return std::fabs( d ) <= sphere.Radius;
}
//---------------------------------------
// Line/Ray v Shpere
//---------------------------------------
bool Intersection::Test( const Ray3f& ray, const Spheref& sphere )
{
	Vec3f d = ray.Origin - sphere.Center;
	float a = Vec3f::Dot( d, d ) - sphere.Radius * sphere.Radius;

	// Point inside the sphere
	if ( a <= 0 )
	{
		return true;
	}

	float b = ray.Direction.Dot( d );
	if ( b >= 0 )
	{
		return false;
	}

	// Real root exists if discriminant is positive
	return b * b >= a;
}
//---------------------------------------
// Line/Ray v Plane
//---------------------------------------
bool Intersection::Test( const Ray3f& ray, const Planef& plane )
{
	float d = ray.Direction.Dot( plane.Normal );
	float dist = plane.DistanceTo( ray.Origin );
	float p;
	bool intr = false;

	// Check line intersection
	if ( std::fabs( d ) > Mathf::ZERO_TOLERANCE )
	{
		// Not parrallel (intersection)
		p = -dist / d;
		intr = true;
	}
	else if ( std::fabs( dist ) <= Mathf::ZERO_TOLERANCE )
	{
		// Line is on the plane
		p = 0;
		intr = true;
	}

	// Line intersects, but need to check if point is on ray
	if ( intr )
	{
		return p >= 0;
	}
	return false;
}
//---------------------------------------
Intersection::CollisionInfo Intersection::Find( const Ray3f& ray, const Planef& plane )
{
	float d = ray.Direction.Dot( plane.Normal );
	float dist = plane.DistanceTo( ray.Origin );
	float p = 0.0f;
	bool intr = false;
	CollisionInfo info;

	// Check line intersection
	if ( std::fabs( d ) > Mathf::ZERO_TOLERANCE )
	{
		// Not parrallel (intersection)
		p = -dist / d;
		intr = true;
	}
	else if ( std::fabs( dist ) <= Mathf::ZERO_TOLERANCE )
	{
		// Line is on the plane
		p = 0;
		intr = true;
	}

	info.ContactPoint = ray.Origin + p * ray.Direction;

	// Line intersects, but need to check if point is on ray
	if ( intr )
	{
		info.Collision = p >= 0;
	}
	else
	{
		info.Collision = false;
	}

	return info;
}
//---------------------------------------
// Box3 v Box3
//---------------------------------------
/*
bool Test( const Box3f& A, const Box3f& B )
{
	// Tolerance used in cosine test
	const float tol = 1 - Mathf::ZERO_TOLERANCE;
	bool parallelAxes = false;

	const Vec3f* axesA = A.Axes;
	const Vec3f* axesB = B.Axes;
	const float* lenA = A.Lengths;
	const float* lenB = B.Lengths;

	Vec3f d = B.Center - A.Center;

	Matrix3f c;		// dot products of axes
	Matrix3f absc;	// abs( c )
	float ad[3];	// axes dotted with d
	float r, r0, r1;
	float R;	// r0 + r1

	// Axis A[0]
	for ( int i = 0; i < 3; ++i )
	{
		c[0][i] = axesA[0].Dot( axesB[i] );
		absc[0][i] = std::fabs( c[0][i] );

		if ( absc[0][i] > tol )
		{
			parallelAxes = true;
		}
	}

	ad[0] = axesA[0].Dot( d );
	r = std::fabs( ad[0] );
	r1 = lenB[0] * absc[0][0] + lenB[1] * absc[0][1] + lenB[2] * absc[0][2];
	R = lenA[0] + r1;

	if ( r > R )
	{
		return false;
	}

	// Axis A[1]
	for ( int i = 0; i < 3; ++i )
	{
		c[1][i] = axesA[1].Dot( axesB[i] );
		absc[1][i] = std::fabs( c[1][i] );
		
		if ( absc[1][i] > tol )
		{
			parallelAxes = true;
		}
	}

	ad[1] = axesA[1].Dot( d );
	r = std::fabs( ad[1] );
	r1 = lenB[0] * absc[1][0] + lenB[1] * absc[1][1] + lenB[2] * absc[1][2];
	R = lenA[1] + r1;

	if ( r > R )
	{
		return false;
	}

	// Axis A[2]
	for ( int i = 0; i < 3; ++i )
	{
		c[2][i] = axesA[2].Dot( axesB[i] );
		absc[2][i] = std::fabs( c[2][i] );

		if ( absc[2][i] > tol )
		{
			parallelAxes = true;
		}
	}

	ad[2] = axesA[2].Dot( d );
	r = std::fabs( ad[2] );
	r1 = lenB[0] * absc[2][0] + lenB[1] * absc[2][1] + lenB[2] * absc[2][2];
	R = lenA[2] + r1;

	if ( r > R )
	{
		return false;
	}

	// Axis B[0]
	r = std::fabs( axesB[0].Dot( d ) );
	r0 = lenA[0] * absc[0][0] + lenA[1] * absc[1][0] + lenA[2] * absc[2][0];
	R = r0 + lenB[0];

	if ( r > R )
	{
		return false;
	}

	// Axis B[1]
	r = std::fabs( axesB[1].Dot( d ) );
	r0 = lenA[0] * absc[0][1] + lenA[1] * absc[1][1] + lenA[2] * absc[2][1];
	R = r0 + lenB[1];

	if ( r > R )
	{
		return false;
	}

	// Axis B[2]
	r = std::fabs( axesB[2].Dot( d ) );
	r0 = lenA[0] * absc[0][2] + lenA[1] * absc[1][2] + lenA[2] * absc[2][2];
	R = r0 + lenB[2];

	if ( r > R )
	{
		return false;
	}

	// At least one pair of axes is parallel
	if ( parallelAxes )
	{
		return true;
	}

	// Axis A[0] x B[0]
	r = std::fabs( ad[2] * c[1][0] - ad[1] * c[2][0] );
	r0 = lenA[1] * absc[2][0] + lenA[2] * absc[1][0];
	r1 = lenB[1] * absc[0][2] + lenB[2] * absc[0][1];
	R = r0 + r1;

	if ( r > R )
	{
		return false;
	}

	// Axis A[0] x B[1]
	r = std::fabs( ad[2] * c[1][0] - ad[1] * c[2][0] );
	r0 = lenA[1] * absc[2][0] + lenA[2] * absc[1][0];
	r1 = lenB[1] * absc[0][2] + lenB[2] * absc[0][1];
	R = r0 + r1;

	if ( r > R )
	{
		return false;
	}

	// Axis A[0] x B[2]
	r = std::fabs( ad[2] * c[1][2] - ad[1] * c[2][2] );
	r0 = lenA[1] * absc[2][2] + lenA[2] * absc[1][2];
	r1 = lenB[0] * absc[0][1] + lenB[1] * absc[0][0];
	R = r0 + r1;

	if ( r > R )
	{
		return false;
	}

	// Axis A[1] x B[0]
	r = std::fabs( ad[0] * c[2][0] - ad[2] * c[0][0] );
	r0 = lenA[0] * absc[2][0] + lenA[2] * absc[0][0];
	r1 = lenB[1] * absc[1][2] + lenB[2] * absc[1][1];
	R = r0 + r1;

	if ( r > R )
	{
		return false;
	}

	// Axis A[1] x B[1]
	r = std::fabs( ad[0] * c[2][1] - ad[2] * c[0][1] );
	r0 = lenA[0] * absc[2][1] + lenA[2] * absc[0][1];
	r1 = lenB[2] * absc[1][2] + lenB[2] * absc[1][0];
	R = r0 + r1;

	if ( r > R )
	{
		return false;
	}

	// Axis A[1] x B[2]
	r = std::fabs( ad[0] * c[2][2] - ad[2] * c[0][2] );
	r0 = lenA[0] * absc[2][2] + lenA[2] * absc[0][2];
	r1 = lenB[0] * absc[1][1] + lenB[1] * absc[1][0];
	R = r0 + r1;

	if ( r > R )
	{
		return false;
	}

	// Axis A[2] x B[0]
	r = std::fabs( ad[1] * c[0][1] - ad[0] * c[1][1] );
	r0 = lenA[0] * absc[1][1] + lenA[1] * absc[0][1];
	r1 = lenB[0] * absc[2][2] + lenB[2] * absc[2][0];
	R = r0 + r1;

	if ( r > R )
	{
		return false;
	}

	// Axis A[2] x B[1]
	r = std::fabs( ad[1] * c[0][1] - ad[0] * c[1][1] );
	r0 = lenA[0] * absc[1][1] + lenA[1] * absc[0][1];
	r1 = lenB[0] * absc[2][2] + lenB[2] * absc[2][0];
	R = r0 + r1;

	if ( r > R )
	{
		return false;
	}

	// Axis A[2] x B[2]
	r = std::fabs( ad[1] * c[0][2] - ad[0] * c[1][2] );
	r0 = lenA[0] * absc[1][2] + lenA[1] * absc[0][2];
	r1 = lenB[0] * absc[2][1] + lenB[1] * absc[2][0];
	R= r0 + r1;

	if ( r > R )
	{
		return false;
	}

	return true;
}
//---------------------------------------
*/