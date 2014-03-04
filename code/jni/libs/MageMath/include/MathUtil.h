#pragma once

#include <cstdlib>
#include <cfloat>
#include <cmath>

namespace mage
{
	template< typename TReal >
	class Math
	{
	public:
		//---------------------------------------
		// Constants
		static const TReal EPSILON;
		static const TReal MAX_REAL;
		static const TReal ZERO_TOLERANCE;

		static const TReal PI;
		static const TReal TWO_PI;
		static const TReal HALF_PI;
		static const TReal INV_PI;

		static const TReal DEG_TO_RAD;
		static const TReal RAD_TO_DEG;
		//---------------------------------------

		//---------------------------------------
		// Conversion
		static TReal DegreesToRadians( TReal degrees )
		{
			return degrees * DEG_TO_RAD;
		}

		static TReal RadiansToDegrees( TReal radians )
		{
			return radians * RAD_TO_DEG;
		}
		//---------------------------------------

		//---------------------------------------
		// Random
		/*
		static void SetRandomSeed( unsigned long seed )
		{
			Seed = seed;
			//srand( seed );
		}
		static unsigned long GetSeed()
		{
			return Seed;
		}
		static int Rand()
		{
			Seed = Seed * 1103515245 + 12345;
			return (unsigned int) ( Seed / 65536 ) % 32768;
		}
		static const unsigned long RandMax = 32767;
	private:
		static unsigned long Seed;
	public:

		// [0, 1]
		static double RandomUnit()
		{
			return static_cast< double >( Rand() ) / static_cast< double >( RandMax );
		}

		// [-1, 1]
		static TReal Random()
		{
			return static_cast< TReal >( 2 * RandomUnit() - 1 );
		}

		// [min, max]
		static TReal RandomInRange( TReal min, TReal max )
		{
			return static_cast< TReal >( min + ( max - min ) * RandomUnit() );
		}*/
		//---------------------------------------

		//---------------------------------------
		// Interpolation
		template< typename TRealIn, typename TRealOut >
		static TRealOut lerp( TRealIn value, TRealIn inMin, TRealIn inMax, TRealOut outMin, TRealOut outMax )
		{
			TRealIn  spanIn  = inMax  - inMin;
			TRealOut spanOut = outMax - outMin;

			TRealOut scaled = static_cast< TRealOut > ( ( value - inMin ) / spanIn );

			return outMin + ( scaled * spanOut );
		}
		//template< typename TReal >
		static TReal lerp( TReal a, TReal b, float t )
		{
			return a + t * ( b - a );
		}
		// Round
		static TReal Round( TReal n )
		{
			return (TReal) ( std::floor( n + 0.5 ) );
		}
		// Symmetric Round
		static TReal RoundSymmertic( TReal n )
		{
			return (TReal) ( ( n > 0.0 ) ? std::floor( n + 0.5 ) : std::ceil( n - 0.5 ) );
		}
		//---------------------------------------

		//---------------------------------------
		// Constraints
		static TReal Clamp( TReal value, TReal min, TReal max )
		{
			return value < min ? min : value > max ? max : value;
		}
		static void ClampToRange( TReal& value, TReal min, TReal max )
		{
			value = value < min ? min : value > max ? max : value;
		}
		static TReal Max( TReal a, TReal b )
		{
			return a > b ? a : b;
		}
		static TReal Min( TReal a, TReal b )
		{
			return a < b ? a : b;
		}
		static TReal Wrap( TReal value, TReal min, TReal max )
		{
			//assertion( min <= max );
			TReal range = ( max - min );

			while( value >= max )
				value -= range;
			while( value < min )
				value += range;

			return value;
		}
		static void WrapToRange( TReal& value, TReal min, TReal max )
		{
			//assertion( min <= max );
			TReal range = ( max - min );

			while( value >= max )
				value -= range;
			while( value < min )
				value += range;
		}
		//---------------------------------------

		//---------------------------------------
		// Util
		static int Sign( int v )
		{
			return v >= 0 ? 1 : -1;
		}
		static bool IsPowerOfTwo( int n )
		{
			return n && !( n & ( n - 1 ) );
		}
		//---------------------------------------

	};

	//---------------------------------------
	// Define types
	typedef Math< float  > Mathf;
	typedef Math< double > Mathd;
	typedef Math< int    > Mathi;
	//---------------------------------------
}
