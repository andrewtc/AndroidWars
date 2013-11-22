/*
 * Author      : Matthew Johnson
 * Date        : 15/Apr/2013
 * Description :
 *   
 */
 
#pragma once

namespace mage
{

	class RNG
	{
	public:
		~RNG();

		// [0, 1]
		static double RandomUnit()
		{
			return static_cast< double >( Rand() ) / static_cast< double >( RandMax );
		}

		// [-1, 1]
		static double RandomUniform()
		{
			return static_cast< double >( 2 * RandomUnit() - 1 );
		}

		// [min, max]
		template< typename TReal >
		static TReal RandomInRange( TReal min, TReal max )
		{
			return static_cast< TReal >( min + ( max - min ) * RandomUnit() );
		}

		static void SetRandomSeed( unsigned long seed )
		{
			Seed = seed;
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

	protected:
		RNG();

	private:
		static unsigned long Seed;
	};

}