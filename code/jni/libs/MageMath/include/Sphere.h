/*
 * Author      : Matthew Johnson
 * Date        : 28/Jan/2013
 * Description :
 *   3D Sphere. It is represented by abs(x-c) = r.
 */
 
#pragma once

#include "Vector3.h"

namespace mage
{

	template< typename TReal >
	class Sphere
	{
	public:
		Sphere() {}
		Sphere( const Vector3< TReal >& center, TReal radius )
			: Center( center )
			, Radius( radius )
		{}
		~Sphere() {}

		Vector3< TReal > Center;
		TReal Radius;
	};

	typedef Sphere< float  > Spheref;
	typedef Sphere< double > Sphered;
}