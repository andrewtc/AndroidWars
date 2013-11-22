/*
 * Author      : Matthew Johnson
 * Date        : 10/Apr/2013
 * Description :
 *   
 */
 
#pragma once

#include <RNG.h>

namespace mage
{

	template< typename T >
	class Range
	{
	public:
		Range();
		Range( T min, T max );
		~Range();

		// Inclusive: true iff value in [Min, Max]
		inline bool IsValueInRange( const T& value ) const;

		// Get random from range, inclusive
		inline T GetRandomFromRange() const;

		T Min;
		T Max;
	};

	//---------------------------------------
	// Imp
	//---------------------------------------
	template< typename T >
	Range< T >::Range() { /* NOT INIT */ }
	//---------------------------------------
	template< typename T >
	Range< T >::Range( T min, T max )
		: Min( min )
		, Max( max )
	{}
	//---------------------------------------
	template< typename T >
	Range< T >::~Range()
	{}
	//---------------------------------------
	template< typename T >
	bool Range< T >::IsValueInRange( const T& value ) const
	{
		return ( value >= Min && value <= Max );
	}
	//---------------------------------------
	template< typename T >
	inline T Range< T >::GetRandomFromRange() const
	{
		return RNG::RandomInRange( Min, Max );
	}
	//---------------------------------------

	//---------------------------------------
	// Types
	//---------------------------------------
	typedef Range< int    > IntRange;
	typedef Range< float  > FloatRange;
	typedef Range< double > DoubleRange;
	//---------------------------------------
}