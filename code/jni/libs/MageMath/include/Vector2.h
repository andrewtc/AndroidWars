/*
 * Author      : Matthew Johnson
 * Date        : 15/Oct/2012
 * Description :
 *   Feature rich Vector class.
 */

#pragma once

#include <ostream>
 

namespace mage
{

	//---------------------------------------
	// Vector 2
	//---------------------------------------
	template< typename T >
	class Vector2
	{
	public:
		// Components
		T x, y;

		static const Vector2 ZERO;
		static const Vector2 ONE;
		static const Vector2 UNIT_X;
		static const Vector2 UNIT_Y;

		//---------------------------------------
		// Constructors
		//---------------------------------------

		// Initialize all components
		explicit Vector2( T init=0 )
			: x( init ), y( init )
		{}

		// Set components
		Vector2( T x, T y )
			: x( x ), y( y )
		{}

		// Copy
		Vector2( const Vector2& other )
			: x( other.x ), y( other.y )
		{}
		//---------------------------------------


		//---------------------------------------
		// Assignment
		//---------------------------------------
		Vector2& operator=( const Vector2& other )
		{
			Set( other );
			return *this;
		}

		inline void Set( const Vector2& other )
		{
			x = other.x;
			y = other.y;
		}

		inline void Set( T X, T Y )
		{
			x = X;
			y = Y;
		}

		//---------------------------------------


		//---------------------------------------
		// Casting
		//---------------------------------------
		template< typename U >
		operator Vector2< U >() const { return Vector2< U >(
			static_cast< U >( x ),
			static_cast< U >( y ) ); }
		//---------------------------------------


		//---------------------------------------
		// Accessors
		//---------------------------------------
		const T* GetAsArray() const
		{
			return &x;
		}

		T* GetAsArray()
		{
			int* r = new int[2];
			for ( int i = 0; i < 2; ++i ) r[i] = (*this)[i];
			return r;
		}

		T& operator[]( int i )
		{
			return *( &x + i );
		}

		const T& operator[]( int i ) const
		{
			return *( &x + i );
		}
		//---------------------------------------


		//---------------------------------------
		// Addition
		//---------------------------------------
		Vector2 operator+( const Vector2& other ) const
		{
			return Vector2( x + other.x, y + other.y );
		}
		Vector2& operator+=( const Vector2& other )
		{
			x += other.x;
			y += other.y;
			return *this;
		}
		//---------------------------------------
	

		//---------------------------------------
		// Subtraction
		//---------------------------------------
		Vector2 operator-( const Vector2& other ) const
		{
			return Vector2( x - other.x, y - other.y );
		}
		Vector2& operator-=( const Vector2& other )
		{
			x -= other.x;
			y -= other.y;
			return *this;
		}
		//---------------------------------------


		//---------------------------------------
		// Negation
		//---------------------------------------
		Vector2 operator-() const
		{
			return Vector2( -x, -y );
		}
		Vector2& Negate()
		{
			x = -x;
			y = -y;
			return *this;
		}
		//---------------------------------------


		//---------------------------------------
		// Dot product
		//---------------------------------------
		T operator*( const Vector2& other ) const
		{
			return x * other.x + y * other.y;
		}
		T Dot( const Vector2& other ) const
		{
			return *this * other;
		}
		static T Dot( const Vector2& A, const Vector2& B )
		{
			return A * B;
		}
		//---------------------------------------


		//---------------------------------------
		// Cross product
		//---------------------------------------
		T Cross( const Vector2& other ) const
		{
			return Cross( *this, other );
		}
		static T Cross( const Vector2& A, const Vector2& B )
		{
			return A.x * B.y - A.y * B.x;
		}
		static Vector2 Cross( T A, const Vector2& B )
		{
			return Vector2( -A * B.y, A * B.x );
		}
		static Vector2 Cross( const Vector2& A, T B )
		{
			return Vector2( B * A.y, -B * A.x );
		}
		//---------------------------------------


		//---------------------------------------
		// Scalar multiplication
		//---------------------------------------
		Vector2 operator*( const T S ) const
		{
			return Vector2( S * x, S * y );
		}
		Vector2& operator*=( const T S )
		{
			x *= S;
			y *= S;
			return *this;
		}
		Vector2 operator/( const T S ) const
		{
			return Vector2( x / S, y / S );
		}
		Vector2& operator/=( const T S )
		{
			x /= S;
			y /= S;
			return *this;
		}
		//---------------------------------------


		//---------------------------------------
		// Length of vector
		//---------------------------------------
		T Length() const
		{
			return static_cast< T >( std::sqrt( LengthSqr() ) );
		}
		inline T LengthSqr() const
		{
			return x*x + y*y;
		}
		T GetEuclidianDistanceTo( const Vector2& other ) const
		{
			T dx = other.x - x;
			T dy = other.y - y;
			return static_cast< T >( std::sqrt( dx*dx + dy*dy ) );
		}
		T GetManhattanDistanceTo( const Vector2& other ) const
		{
			T dx = other.x - x;
			T dy = other.y - y;
			return static_cast< T >( std::abs( dx ) + abs( dy ) );
		}
		T GetChebyshevDistanceTo( const Vector2& other ) const
		{
			T dx = other.x - x;
			T dy = other.y - y;
			return static_cast< T >( Math< T >::Max( std::abs( dx ), abs( dy ) ) );
		}
		//---------------------------------------


		//---------------------------------------
		// Normalize this vector
		//---------------------------------------
		Vector2& Normalize()
		{
			T len = Length();
			if ( len > Mathf::EPSILON )
			{
				x /= len;
				y /= len;
			}
			return *this;
		}
		//---------------------------------------


		//---------------------------------------
		// Comparison
		//---------------------------------------
		bool operator==( const Vector2& other ) const
		{
			return x == other.x && y == other.y;
		}
		bool operator!=( const Vector2& other ) const
		{
			return !(*this == other);
		}
		bool operator< (const Vector2& other ) const
		{
			return x < other.x && y < other.y;
		}
		bool operator> (const Vector2& other ) const
		{
			return x > other.x && y > other.y;
		}
		//---------------------------------------


		//---------------------------------------
		// Angle (in radians)
		//---------------------------------------
		float Angle() const
		{
			return atan2f( y, x );
		}
		void Rotate( T angleRad )
		{
			T c = std::cos( angleRad );
			T s = std::sin( angleRad );

			T _x = x * c - y * s;
			T _y = x * s + y * c;

			x = _x;
			y = _y;
		}
		//---------------------------------------


		//---------------------------------------
		// Interpolation
		//---------------------------------------
		static Vector2 CatmullRom( const Vector2& p0, const Vector2& p1, const Vector2& p2, const Vector2& p3, float t )
		{
			Vector2 v;
			const float t2 = t * t;
			const float t3 = t2 * t;

			v.x = 0.5f * ( ( 2.0f * p1.x ) + ( -p0.x + p2.x ) * t +
				( 2.0f * p0.x - 5.0f * p1.x + 4.0f * p2.x - p3.x ) * t2 +
				(-p0.x + 3.0f * p1.x - 3.0f * p2.x + p3.x ) * t3 );

			v.y = 0.5f * ( ( 2.0f * p1.y ) + ( -p0.y + p2.y ) * t +
				( 2.0f * p0.y - 5.0f * p1.y + 4.0f * p2.y - p3.y ) * t2 +
				(-p0.y + 3.0f * p1.y - 3.0f * p2.y + p3.y ) * t3 );

			return v;
		}
		//---------------------------------------


		//---------------------------------------
		// Distance
		//---------------------------------------
		static T DistanceSqr(const Vector2& A, const Vector2& B )
		{
			Vector2 c = A - B;
			return Dot( c, c );
		}
		//---------------------------------------
	};

	//---------------------------------------
	// Allow scalar to be used as LHS
	template< typename T >
	Vector2< T > operator*( float S, const Vector2< T >& vector )
	{
		return vector * static_cast< T >( S );
	}
	//---------------------------------------

	//---------------------------------------
	// Debugging output
	template< typename T >
	std::ostream& operator<<( std::ostream& out, const Vector2< T >& vector )
	{
		return out << vector.x << ", " << vector.y;
	}
	//---------------------------------------

	//---------------------------------------
	// Define Vector types
	typedef Vector2< float    > Vec2f;
	typedef Vector2< double   > Vec2d;
	typedef Vector2< int      > Vec2i;
	typedef Vector2< short    > Vec2s;
	typedef Vector2< unsigned > Vec2u;
	typedef Vector2< long     > Vec2l;
	//---------------------------------------

}
