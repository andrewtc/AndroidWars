/*
 * Author      : Matthew Johnson
 * Date        : 13/Jul/2013
 * Description :
 *   
 */
 
#pragma once

#include <limits>

namespace mage
{



	template<
		typename T >
	struct Comparable1
	{
		friend bool operator> ( const T& a, const T& b ) { return b < a; }
		friend bool operator<=( const T& a, const T& b ) { return !( b < a ); }
		friend bool operator>=( const T& a, const T& b ) { return !( a < b ); }
	};

	template<
		typename T,
		typename U >
	struct Comparable2
	{
		friend bool operator<=( const T& a, const U& b ) { return !( a > b ); }
		friend bool operator>=( const T& a, const U& b ) { return !( a < b ); }
		friend bool operator<=( const U& a, const T& b ) { return !( b < a ); }
		friend bool operator>=( const U& a, const T& b ) { return !( b > a ); }
		friend bool operator> ( const U& a, const T& b ) { return b < a; }
		friend bool operator< ( const U& a, const T& b ) { return b > a; }
	};

	template<
		typename T >
	struct EqualityComparable1
	{
		friend bool operator!=( const T& a, const T& b ) { return !( a == b ); }
	};

	template<
		typename T,
		typename U >
	struct EqualityComparable2
	{
		friend bool operator==( const U& a, const T& b ) { return b == a; }
		friend bool operator!=( const U& a, const T& b ) { return !( b == a ); }
		friend bool operator!=( const T& a, const U& b ) { return !( a == b ); }
	};

	template<
		typename T >
	struct Incrementable
	{
		friend T operator++( T& a, int )
		{
			T _r( a );
			++a;
			return _r;
		}
	};

	template<
		typename T >
	struct Decrementable
	{
		friend T operator--( T& a, int )
		{
			T _r( a );
			--a;
			return _r;
		}
	};

	template<
		typename T >
	struct Steppable
		: Incrementable< T >
		, Decrementable< T >
	{};

#define binary_commutative_operator( name, op )										\
	template< typename T, typename U >												\
	struct name##2																	\
	{																				\
		friend T operator op( T lhs, const U& rhs ) { return lhs op##= rhs; }		\
		friend T operator op( const U& lhs, T rhs ) { return rhs op##= lhs; }		\
	};																				\
	template< typename T >															\
	struct name##1																	\
	{																				\
		friend T operator op( T lhs, const T& rhs ) { return lhs op##= rhs; }		\
	};																				\

#define binary_non_commutative_operator( name, op )									\
	template< typename T, typename U >												\
	struct name##2																	\
	{																				\
		friend T operator op( T lhs, const U& rhs ) { return lhs op##= rhs; }		\
	};																				\
	template< typename T, typename U >												\
	struct name##2##left															\
	{																				\
		friend T operator op( const U& lhs, const T& rhs )							\
			{ return T( lhs ) op##= rhs; }											\
	};																				\
	template< typename T >															\
	struct name##1																	\
	{																				\
		friend T operator op( T lhs, const T& rhs ) { return lhs op##= rhs; }		\
	};																				\

#define binary_operator( name, op )													\
	template< typename T, typename U >												\
	struct name##2																	\
	{																				\
		friend T operator op( T lhs, const U& rhs ) { return lhs op##= rhs; }		\
	};																				\
	template< typename T >															\
	struct name##1																	\
	{																				\
		friend T operator op( T lhs, const T& rhs ) { return lhs op##= rhs; }		\
	};																				\

	binary_commutative_operator( Multipliable, * )
	binary_commutative_operator( Addable, + )
	binary_commutative_operator( XOrable, ^ )
	binary_commutative_operator( Andable, & )
	binary_commutative_operator( Orable, | )
	binary_non_commutative_operator( Subtractable, - )
	binary_non_commutative_operator( Dividable, / )
	binary_operator( RightShiftable, >> )
	binary_operator( LeftShiftable, << )

#undef binary_commutative_operator
#undef binary_non_commutative_operator
#undef binary_operator

	template< typename T, typename U >
	struct Bitwiseable2
		: Andable2< T, U >
		, XOrable2< T, U >
		, Orable2< T, U >
	{};

	template< typename T, typename U = T >
	struct Bitwiseable
		: Bitwiseable2< T, U >
	{};
	template < typename T >
	struct Bitwiseable< T, T >
		: Andable1< T >
		, XOrable1< T >
		, Orable1< T >
	{};

	template< typename T, typename U >
	struct Shiftable2
		: LeftShiftable2< T, U >
		, RightShiftable2< T, U >
	{};

	template< typename T, typename U = int >
	struct Shiftable
		: Shiftable2< T, U >
	{};

	template< typename T, typename U >
	struct ComparableOperators2
		: Comparable2< T, U >
		, EqualityComparable2< T, U >
	{};

	template< typename T, typename U = T >
	struct ComparableOperators
		: ComparableOperators2< T, U >
	{};
	template < typename T >
	struct ComparableOperators< T, T >
		: Comparable1< T >
		, EqualityComparable1< T >
	{};

	template< typename T, typename U >
	struct Operators2
		: Comparable2< T, U >
		, EqualityComparable2< T, U >
		, Multipliable2< T, U >
		, Dividable2< T, U >
		, Addable2< T, U >
		, Subtractable2< T, U >
	{};

	template< typename T, typename U = T >
	struct Operators
		: Operators2< T, U >
	{};

	template< typename T >
	struct Operators< T, T >
		: Comparable1< T >
		, EqualityComparable1< T >
		, Multipliable1< T >
		, Dividable1< T >
		, Addable1< T >
		, Subtractable1< T >
		, Steppable< T >
	{};


	template<
		// Base type must be an integer type
		typename base_type,
		// integer part bits
		unsigned char ibits,
		// fraction part bits
		unsigned char fbits = std::numeric_limits< base_type >::digits - ibits >
	class FixedPoint
		: Comparable1< FixedPoint< base_type, ibits, fbits > >
		, EqualityComparable1< FixedPoint< base_type, ibits, fbits > >
		, Steppable< FixedPoint< base_type, ibits, fbits > >
		, Addable1< FixedPoint< base_type, ibits, fbits > >
		, Subtractable1< FixedPoint< base_type, ibits, fbits > >
		, Multipliable1< FixedPoint< base_type, ibits, fbits > >
		, Dividable1< FixedPoint< base_type, ibits, fbits > >
		, Shiftable< FixedPoint< base_type, ibits, fbits > >
	{
		// Allow other template types private access
		friend class FixedPoint;

		// Give access to numeric_limits
		friend class std::numeric_limits< FixedPoint >;

		// Type Promotion used in multiplication and division
		struct UnspecifiedTypePromotion {};
		template<
			typename T,
			typename U=void >
		struct TypePromotion
		{
			typedef UnspecifiedTypePromotion type;
		};

		template< typename U > struct TypePromotion< char, U > { typedef short type; };
		template< typename U > struct TypePromotion< unsigned char, U > { typedef unsigned short type; };
		template< typename U > struct TypePromotion< short, U > { typedef int type; };
		template< typename U > struct TypePromotion< unsigned short, U > { typedef unsigned int type; };
		template< typename U > struct TypePromotion< int, U > { typedef long long type; };
		template< typename U > struct TypePromotion< unsigned int, U > { typedef unsigned long long type; };

#define PromoteType( value )								\
	static_cast< typename FixedPoint::template				\
	TypePromotion< base_type >::type >( value )

		enum FixedInternal { _INTERNAL_ };

		// Private direct constructor
		FixedPoint( FixedInternal, base_type value )
			: mValue( value )
		{}

	public:
		static const unsigned char NUM_IBITS = ibits;
		static const unsigned char NUM_FBITS = fbits;

		// Smallest step we can represent
		static double Step() { return 1.0 / ( 1 << fbits ); }

		// Default constructor (initializes to zero)
		FixedPoint()
			: mValue( 0 )
		{}

		// Conversion constructor (integer types only)
		// Shifts the value into the integer part
		template< typename other_int_type >
		FixedPoint( other_int_type value )
			: mValue( (base_type) value << fbits )
		{}
		// Floating point constructors
		FixedPoint( float value )
			: mValue( (base_type) ( value / Step() ) )
		{}
		FixedPoint( double value )
			: mValue( (base_type) ( value / Step() ) )
		{}

		// Copy constructors
		FixedPoint( const FixedPoint& fp )
			: mValue( fp.mValue )
		{}
		// Copy from different fixed point position
		template<
			unsigned char other_ibits,
			unsigned char other_fbits >
		FixedPoint( const FixedPoint< base_type, other_ibits, other_fbits >& fp )
			: mValue( fp.mValue )
		{
			// Make adjustments for differing precision

			// Gain integer precision
			if ( ibits > other_ibits )
				mValue >>= (ibits - other_ibits);
			// Lose integer precision
			else if ( other_ibits > ibits )
				mValue <<= (other_ibits - ibits);
		}

		// Assignment
		FixedPoint& operator=( const FixedPoint& fp )
		{
			mValue = fp.mValue;
			return *this;
		}
		template<
			unsigned char other_ibits,
			unsigned char other_fbits >
		FixedPoint& operator=( const FixedPoint< base_type, other_ibits, other_fbits >& fp )
		{
			// Use copy constructor for conversion
			FixedPoint tmp( fp );
			mValue = tmp.mValue;
			return *this;
		}

		// Compare
		bool operator< ( const FixedPoint& fp ) const { return mValue < fp.mValue; }

		// Equal
		bool operator==( const FixedPoint& fp ) const { return mValue == fp.mValue; }

		//template< typename T >
		//friend bool operator==( const T& a, const FixedPoint& b ) { return b == FixedPoint( a ); }
		//template< typename T >
		//friend bool operator==( const FixedPoint& b, const T& a ) { return b == FixedPoint( a ); }

		template< typename T >
		bool operator==( const T& other ) const { return *this == FixedPoint( other ); }

#define equality_ops( type )																\
	friend bool operator==( const type x, const FixedPoint& fp  ) { return fp == x; }		\
	friend bool operator!=( const type x, const FixedPoint& fp  ) { return !( fp == x ); }	\
	friend bool operator!=( const FixedPoint& fp, const type x  ) { return !( fp == x ); }	\
		
		// Workaround due to issue with templates
		equality_ops( float )
		equality_ops( double )
		equality_ops( char )
		equality_ops( unsigned char )
		equality_ops( short )
		equality_ops( unsigned short )
		equality_ops( int )
		equality_ops( unsigned int )

		// Not
		bool operator !() const { return mValue == 0; }

		// Negation (undefined for unsigned types)
		FixedPoint operator-() const
		{
			FixedPoint _ret;
			_ret.mValue = -mValue;
			return _ret;
		}
		FixedPoint operator+() const { return *this; }

		// Increment/Decrement
		FixedPoint& operator++()
		{
			mValue += ( 1 << fbits );
			return *this;
		}
		FixedPoint& operator--()
		{
			mValue -= ( 1 << fbits );
			return *this;
		}

		// Add/Sub
		FixedPoint& operator+=( const FixedPoint& fp )
		{
			mValue += fp.mValue;
			return *this;
		}
		//FixedPoint operator+ ( const FixedPoint& fp )
		//{
		//	return FixedPoint( _INTERNAL_, mValue + fp.mValue );
		//}
		FixedPoint& operator-=( const FixedPoint& fp )
		{
			mValue -= fp.mValue;
			return *this;
		}

		// Bit stuff
		FixedPoint& operator&=( const FixedPoint& fp )
		{
			mValue &= fp.mValue;
			return *this;
		}
		FixedPoint& operator|=( const FixedPoint& fp )
		{
			mValue |= fp.mValue;
			return *this;
		}
		FixedPoint& operator^=( const FixedPoint& fp )
		{
			mValue ^= fp.mValue;
			return *this;
		}
		FixedPoint operator~()
		{
			FixedPoint _ret( ~(int)(*this) );
			//_ret.mValue = ~mValue;
			return _ret;
		}
		//FixedPoint& operator~=( const FixedPoint& fp )
		//{
		//	mValue ~= fp.mValue;
		//	return *this;
		//}

		template< typename T >
		friend T operator &( FixedPoint lhs, const T& rhs ) { return lhs &= rhs; }
		template< typename T >
		friend T operator &( const T& lhs, FixedPoint rhs ) { return rhs &= lhs; }

		template< typename T >
		friend T operator |( FixedPoint lhs, const T& rhs ) { return lhs |= rhs; }
		template< typename T >
		friend T operator |( const T& lhs, FixedPoint rhs ) { return rhs |= lhs; }

		template< typename T >
		friend T operator ^( FixedPoint lhs, const T& rhs ) { return lhs ^= rhs; }
		template< typename T >
		friend T operator ^( const T& lhs, FixedPoint rhs ) { return rhs ^= lhs; }

		//template< typename T >
		//friend T operator ~( FixedPoint lhs, const T& rhs ) { return lhs ~= rhs; }
		//template< typename T >
		//friend T operator ~( const T& lhs, FixedPoint rhs ) { return rhs ~= lhs; }

		// Multiply/Divide
		FixedPoint& operator*=( const FixedPoint& fp )
		{
			// Promote value before multiplication to avoid overflow
			// Then shift down the fraction part
			mValue = ( PromoteType( mValue ) * fp.mValue ) >> fbits;
			return *this;
		}
		FixedPoint& operator/=( const FixedPoint& fp )
		{
			// Similar to multiplication, but we shift back on promoted type
			mValue = ( PromoteType( mValue ) << fbits ) / fp.mValue;
			return *this;
		}

		// Shift
		FixedPoint& operator>>=( size_t shift )
		{
			mValue >>= shift;
			return *this;
		}
		FixedPoint& operator<<=( size_t shift )
		{
			mValue <<= shift;
			return *this;
		}

		// Conversion (for integer types)
		template< typename conversion_type >
		operator conversion_type() const
		{
			// Simply drop off the faction part
			return (conversion_type) ( mValue >> fbits );
		}
		//operator char() const { return (char) ( mValue >> fbits ); }
		//operator unsigned char() const { return (unsigned char) ( mValue >> fbits ); }
		//operator short() const { return (short) ( mValue >> fbits ); }
		//operator unsigned short() const { return (unsigned short) ( mValue >> fbits ); }
		//operator int() const { return (int) ( mValue >> fbits ); }
		//operator unsigned int() const { return (unsigned int) ( mValue >> fbits ); }
		// Conversion specialization (for float types)
		operator float() const
		{
			return (float) ( mValue * Step() );
		}
		operator double() const
		{
			return (double) ( mValue * Step() );
		}
		// Conversion specialization to bool
		operator bool() const
		{
			return (bool) mValue;
		}

		template<
			typename base_type,
			unsigned char ibits,
			unsigned char fbits >
		friend FixedPoint< base_type, ibits, fbits > sin( const FixedPoint< base_type, ibits, fbits >& angle )
		{
			//float x = (float) angle;
			//x = std::fmod( x + Mathf::PI, Mathf::TWO_PI ) - Mathf::PI;
			//static const float B = 4.0f / Mathf::PI;
			//static const float C = -4.0f / ( Mathf::PI * Mathf::PI );
			//float y = B * x + C * x * std::abs( x );
			//static const float P = 0.255f;
			//return FixedPoint< base_type, ibits, fbits >( P * ( y * std::abs( y ) - y ) + y );
			// shits fast as hell yo
			return std::sin( (float) angle );
		}

		template<
			typename base_type,
			unsigned char ibits,
			unsigned char fbits >
			friend FixedPoint< base_type, ibits, fbits > cos( const FixedPoint< base_type, ibits, fbits >& angle )
		{
			// Not even gonna try after failing at sin
			return std::cos( (float) angle );
		}

	private:
		base_type mValue;

#undef PromoteType
	};


	//---------------------------------------
	// Streaming support
	//
	// The FixedPoint is converted to a double
	// before being streamed out. It is also
	// streamed in as a double.
	//---------------------------------------
	template<
		typename stream_type,
		typename base_type,
		unsigned char ibits,
		unsigned char fbits >
	stream_type& operator>>( stream_type& s, const FixedPoint< base_type, ibits, fbits >& value )
	{
		double _value = 0;
		s >> _value;
		if ( s )
			value = _value;
		return s;
	}
	//---------------------------------------
	template<
		typename stream_type,
		typename base_type,
		unsigned char ibits,
		unsigned char fbits >
		stream_type& operator<<( stream_type& s, const FixedPoint< base_type, ibits, fbits >& value )
	{
		double _value = value;
		s << _value;
		return s;
	}
	//---------------------------------------
	/*
	template<
		typename base_type,
		unsigned char ibits,
		unsigned char fbits >
	FixedPoint< base_type, ibits, fbits > sin( const FixedPoint< base_type, ibits, fbits >& angle )
	{
		
		float x = (float) angle;
		x = std::fmod( x + Mathf::PI, Mathf::TWO_PI ) - Mathf::PI;
		const float B = 4.0f / Mathf::PI;
		const float C = -4.0f / ( Mathf::PI * Mathf::PI );
		float y = B * x + C * x * std::abs( x );
		const float P = 0.255f;
		return FixedPoint< base_type, ibits, fbits >( P * ( y * std::abs( y ) - y ) + y );

		//double angleSqr = angle * angle;
#if 0
		FixedPoint< base_type, ibits, fbits > s = 7.61e-03;
		s *= angleSqr;
		s -= 1.6605e-01;
		s *= angleSqr;
		s += 1.0;
		s *= angle;
#endif

#if 0
		FixedPoint< base_type, ibits, fbits > s = -2.39e-08;
		s *= angleSqr;
		s += 2.7526e-06;
		s *= angleSqr;
		s -= 1.98409e-04;
		s *= angleSqr;
		s += 8.3333315e-03;
		s *= angleSqr;
		s -= 1.666666664e-01;
		s *= angleSqr;
		s += 1.0;
		s *= angle;
		return s;
#endif
	}*/

}


//---------------------------------------
// Numeric limits specialization for FixedPoint type
//---------------------------------------
namespace std
{
	template<
		typename base_type,
		unsigned char ibits,
		unsigned char fbits >
	class numeric_limits< mage::FixedPoint< base_type, ibits, fbits > >
	{
	public:
		typedef mage::FixedPoint< base_type, ibits, fbits > fp_type;

		// No denormalized values
		static const float_denorm_style has_denorm = denorm_absent;

		// No denormalized values
		static const bool has_denorm_loss = false;

		// We don't have a representation for infinity
		static const bool has_infinity = false;

		// We don't have a representation for NaN
		static const bool has_quiet_NaN = false;
		static const bool has_signaling_NaN = false;

		// Finite set of values
		static const bool is_bounded = true;

		// Close enough
		static const bool is_exact = true;

		// No
		static const bool is_iec559 = false;

		// Not an int
		static const bool is_integer = false;

		// No
		static const bool is_modulo = false;

		// Test if type is signed, we just ask the base_type
		static const bool is_signed =
			std::numeric_limits< typename base_type >::is_signed;

		// We are obviously specialized
		static const bool is_specialized = true;

		// No
		static const bool tinyness_before = false;

		// No
		static const bool traps = false;

		// Rounding style
		static const float_round_style round_style = round_toward_zero;

		// Number of bits
		static const int digits = ibits;

		// Number of digits
		static const int digits10 = digits;

		static const int max_exponent = 0;
		static const int max_exponent10 = 0;
		static const int min_exponent = 0;
		static const int min_exponent10 = 0;
		static const int radix = 0;

		//---------------------------------------
		static fp_type (min)()
		{
			fp_type _min;
			_min.mValue = (numeric_limits< typename base_type >::min)();
			return _min;
		}
		//---------------------------------------
		static fp_type (max)()
		{
			fp_type _max;
			_max.mValue = (numeric_limits< typename base_type >::max)();
			return _max;
		}
		//---------------------------------------
		static fp_type round_error()
		{
			return (fp_type)(0.5);
		}
		//---------------------------------------
		static fp_type denorm_min()
		{
			return (fp_type)(0);
		}
		//---------------------------------------
		static fp_type infinity()
		{
			return (fp_type)(0);
		}
		//---------------------------------------
		static fp_type quiet_NaN()
		{
			return (fp_type)(0);
		}
		//---------------------------------------
		static fp_type signaling_NaN()
		{
			return (fp_type)(0);
		}
		//---------------------------------------
	};
}