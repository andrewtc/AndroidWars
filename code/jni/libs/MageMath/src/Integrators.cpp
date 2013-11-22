#include "Integrators.h"

using namespace mage;

//---------------------------------------
template< typename TReal >
TReal Integrators< TReal >::RombergIntegral( int order, TReal a, TReal b, Function func,
	void* userData )
{
	TReal** rom;

	// Allocate memory
	rom = new TReal*[2];
	rom[0] = new TReal[order*2];
	for ( int i = 1; i < 2; ++i )
	{
		rom[i] = &rom[0][order*i];
	}

	TReal h = b - a;

	rom[0][0] = ((TReal) 0.5) * h * ( func( a, userData ) + func( b, userData ) );
	for ( int i0 = 2, p0 = 1; i0 <= order; ++i0, p0 *= 2, h *= (TReal) 0.5 )
	{
		// Trapazoid approx.
		TReal sum = 0;
		for ( int i1 = 1; i1 <= p0; ++i1 )
		{
			sum += func( a + h * ( i1 - ((TReal) 0.5) ), userData );
		}

		// Richardson extrapolation
		rom[1][0] = ((TReal) 0.5) * ( rom[0][0] + h * sum );
		for ( int i2 = 1, p1 = 4; i2 < i0; ++i2, p1 *= 4 )
		{
			rom[1][i2] = ( p1 * rom[1][i2-1] - rom[0][i2-1] ) / ( p1 - 1 );
		}

		for ( int i1 = 0; i1 < i0; ++i1 )
		{
			rom[0][i1] = rom[1][i1];
		}
	}

	TReal result = rom[0][ order - 1 ];
	delete[] rom[0];
	delete[] rom;
	return result;
}
//---------------------------------------

//---------------------------------------
// Explicit instantiation
//---------------------------------------
template class Integrators< float  >;
template class Integrators< double >;
//---------------------------------------