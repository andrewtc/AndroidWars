/*
 * Author      : Matthew Johnson
 * Date        : 28/Jan/2013
 * Description :
 *   A Line is defined by P+t*D where
 *    P = origin
 *    D = direction (normalized)
 *    t = time (>=0)
 */
 
#pragma once

#include "Vector3.h"
#include "Vector2.h"

namespace mage
{

	//---------------------------------------
	// 3D Line
	//---------------------------------------
	template< typename TReal >
	class Line3
	{
	public:
		Line3() {}
		Line3( const Vector3< TReal >& origin, const Vector3< TReal >& direction )
			: Origin( origin )
			, Direction( direction )
		{}
		~Line3() {}

		Vector3< TReal > Origin, Direction;
	};


	//---------------------------------------
	// 2D Line
	//---------------------------------------
	template< typename TReal >
	class Line2
	{
	public:
		Line2() {}
		Line2( const Vector2< TReal >& origin, const Vector2< TReal >& direction )
			: Origin( origin )
			, Direction( direction )
		{}
		~Line2() {}

		Vector3< TReal > Origin, Direction;
	};


	typedef Line3< float  > Line3f;
	typedef Line3< double > Line3d;

	typedef Line2< float  > Line2f;
	typedef Line2< double > Line2d;

	// Line and Ray are the same thing, typedef it to make intents for functions clearer
	typedef Line3< float  > Ray3f;
	typedef Line3< double > Ray3d;

	typedef Line2< float  > Ray2f;
	typedef Line2< double > Ray2d;
}