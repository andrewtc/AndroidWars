/*
 * Author      : Matthew Johnson
 * Date        : 28/Jan/2013
 * Description :
 *   A Plane is represented by N dot X = c where
 *    N = normal vector
 *    X = point on the plane
 *    c = plane constant
 */
 
#pragma once

#include "Vector3.h"

namespace mage
{

	template< typename TReal >
	class Plane
	{
	public:
		Plane();
		// Plane from normal and constant
		Plane( const Vector3< TReal >& normal, TReal c );
		// Plane from normal and point
		Plane( const Vector3< TReal >& normal, const Vector3< TReal >& p );
		// Plane from 3 points
		Plane( const Vector3< TReal >& p0, const Vector3< TReal >& p1,
			const Vector3< TReal >& p2 );
		~Plane();

		// Get signed distance between point and plane
		// < 0 => on positive side
		// > 0 => on negative side
		// = 0 => on plane
		TReal DistanceTo( const Vector3< TReal >& p ) const;

		// Similar to DistanceTo but just returns -1, 0, 1
		int Side( const Vector3< TReal >& p ) const;

		Vector3< TReal > Normal;
		TReal Constant;
	};

	//---------------------------------------
	// Implementation
	//---------------------------------------
	template< typename TReal >
	Plane< TReal >::Plane()
	{}
	//---------------------------------------
	template< typename TReal >
	Plane< TReal >::Plane( const Vector3< TReal >& normal, TReal c )
		: Normal( normal )
		, Constant( c )
	{}
	//---------------------------------------
	template< typename TReal >
	Plane< TReal >::Plane( const Vector3< TReal >& normal, const Vector3< TReal >& p )
		: Normal( normal )
	{
		Constant = normal.Dot( p );
	}
	//---------------------------------------
	template< typename TReal >
	Plane< TReal >::Plane( const Vector3< TReal >& p0, const Vector3< TReal >& p1,
			const Vector3< TReal >& p2 )
	{
		Vector3< TReal > edgeA = p1 - p0;
		Vector3< TReal > edgeB = p2 - p0;
		Normal = Vector3< TReal >::Cross( edgeA, edgeB );
		Normal.Normalize();
		Constant = Normal.Dot( p0 );
	}
	//---------------------------------------
	template< typename TReal >
	Plane< TReal >::~Plane()
	{}
	//---------------------------------------
	template< typename TReal >
	TReal Plane< TReal >::DistanceTo( const Vector3< TReal >& p ) const
	{
		return Normal.Dot( p ) - Constant;
	}
	//---------------------------------------
	template< typename TReal >
	int Plane< TReal >::Side( const Vector3< TReal >& p ) const
	{
		TReal dist = DistanceTo( p );
		if      ( dist < 0 ) return -1;
		else if ( dist > 0 ) return  1;
		else                 return  0;
	}
	//---------------------------------------

	typedef Plane< float  > Planef;
	typedef Plane< double > Planed;
}