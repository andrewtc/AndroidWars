/*
 * Author      : Matthew Johnson
 * Date        : 28/Jan/2013
 * Description :
 *   
 */
 
#pragma once

#include "Vector3.h"

namespace mage
{

	template< typename TReal >
	class AABox3
	{
	public:
		AABox3();
		AABox3( TReal xmin, TReal xmax, TReal ymin, TReal ymax,
			TReal zmin, TReal zmax );
		~AABox3();

		// Computer the center and extent of the edges
		void GetCenterExtents( Vector3< TReal >& center, Vector3< TReal >& extent );

		// Intersection testing
		bool Intersects( const AABox3& AABox3 ) const;

		Vector3< TReal > Min, Max;
	};

	//---------------------------------------
	// Implementation
	//---------------------------------------
	template< typename TReal >
	AABox3< TReal >::AABox3()
	{}
	//---------------------------------------
	template< typename TReal >
	AABox3< TReal >::AABox3( TReal xmin, TReal xmax, TReal ymin, TReal ymax,
		TReal zmin, TReal zmax )
	{
		Min.x = xmin;
		Max.x = xmax;
		Min.y = ymin;
		Max.y = ymax;
		Min.z = zmin;
		Max.z = zmax;
	}
	//---------------------------------------
	template< typename TReal >
	AABox3< TReal >::~AABox3()
	{}
	//---------------------------------------
	template< typename TReal >
	void AABox3< TReal >::GetCenterExtents( Vector3< TReal >& center, Vector3< TReal >& extent )
	{
		center.x = ( (TReal) 0.5 ) * ( Max.x + Min.x );
		center.y = ( (TReal) 0.5 ) * ( Max.y + Min.y );
		center.z = ( (TReal) 0.5 ) * ( Max.z + Min.z );
		extent.x = ( (TReal) 0.5 ) * ( Max.x - Min.x );
		extent.y = ( (TReal) 0.5 ) * ( Max.y - Min.y );
		extent.z = ( (TReal) 0.5 ) * ( Max.z - Min.z );
	}
	//---------------------------------------
	template< typename TReal >
	bool AABox3< TReal >::Intersects( const AABox3& AABox3 ) const
	{
		for ( int i = 0; i < 3; ++i )
		{
			if ( Max[i] < AABox3.Min[i] || Min[i] > AABox3.Max[i] ) return false;
		}
		return true;
	}
	//---------------------------------------

	typedef AABox3< float  > AABox3f;
	typedef AABox3< double > AABox3d;

}