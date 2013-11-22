/*
 * Author      : Matthew Johnson
 * Date        : 23/Feb/2013
 * Description :
 *   
 */
 
#pragma once

#include "Vector3.h"
#include "Vector2.h"
#include <ostream>
#include <iomanip>
namespace mage
{

	//---------------------------------------
	// Vector 4
	//---------------------------------------
	template< typename T >
	class Vector4
	{
	public:
		// Components
		T x, y, z, w;

		static const Vector4 ZERO;
		static const Vector4 ONE;
		static const Vector4 UNIT_X;
		static const Vector4 UNIT_Y;
		static const Vector4 UNIT_Z;
		static const Vector4 UNIT_W;

		//---------------------------------------
		// Constructors
		//---------------------------------------

		// Initialize all components
		explicit Vector4( T init=0 )
			: x( init ), y( init ), z( init ), w( init )
		{}

		// Set components
		Vector4( T x, T y, T z, T w )
			: x( x ), y( y ), z( z ), w( w )
		{}

		// From array
		explicit Vector4( T* a )
			: x( a[0] ), y( a[1] ), z( a[2] ), w( a[3] )
		{}

		// From Vector3
		explicit Vector4( const Vector3< T >& v3, T w )
			: x( v3.x ), y( v3.y ), z( v3.z ), w( w )
		{}

		// Copy
		Vector4( const Vector4& other )
			: x( other.x ), y( other.y ), z( other.z ), w( other.w )
		{}
		//---------------------------------------


		//---------------------------------------
		// Assignment
		//---------------------------------------
		Vector4& operator=( const Vector4& other )
		{
			Set( other );
			return *this;
		}

		Vector4& operator=( const T* arr )
		{
			Set( arr );
			return *this;
		}

		Vector4& operator=( T* arr )
		{
			Set( arr );
			return *this;
		}

		inline void Set( const Vector4& other )
		{
			x = other.x;
			y = other.y;
			z = other.z;
			w = other.w;
		}

		inline void Set( T X, T Y, T Z, T W )
		{
			x = X;
			y = Y;
			z = Z;
			w = W;
		}

		inline void Set( T* arr )
		{
			x = arr[0];
			y = arr[1];
			z = arr[2];
			w = arr[3];
		}

		//---------------------------------------


		//---------------------------------------
		// Casting
		//---------------------------------------
		template< typename U >
		operator Vector4< U >() { return Vector4< U >(
			static_cast< U >( x ),
			static_cast< U >( y ),
			static_cast< U >( z ),
			static_cast< U >( w ) ); }
		//---------------------------------------

		//---------------------------------------
		// Conversion
		//---------------------------------------
		//Vector3 xyz()
		//{
		//	return Vector3( x, y, z );
		//}
		//Vector2 xy()
		//{
		//	return Vector3( x, y );
		//}
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
			int* r = new int[4];
			for ( int i = 0; i < 4; ++i ) r[i] = (*this)[i];
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
		Vector4 operator+( const Vector4& other ) const
		{
			return Vector4( x + other.x, y + other.y, z + other.z, w + other.w );
		}
		Vector4& operator+=( const Vector4& other )
		{
			x += other.x;
			y += other.y;
			z += other.z;
			w += other.w;
			return *this;
		}
		//---------------------------------------
	

		//---------------------------------------
		// Subtraction
		//---------------------------------------
		Vector4 operator-( const Vector4& other ) const
		{
			return Vector4( x - other.x, y - other.y, z - other.z, w - other.w );
		}
		Vector4& operator-=( const Vector4& other )
		{
			x -= other.x;
			y -= other.y;
			z -= other.z;
			w -= other.w;
			return *this;
		}
		//---------------------------------------


		//---------------------------------------
		// Negation
		//---------------------------------------
		Vector4 operator-() const
		{
			return Vector4( -x, -y, -z, -w );
		}
		Vector4& Negate()
		{
			x = -x;
			y = -y;
			z = -z;
			w = -w;
			return *this;
		}
		//---------------------------------------


		//---------------------------------------
		// Dot product
		//---------------------------------------
		T operator*( const Vector4& other ) const
		{
			return x * other.x + y * other.y + z * other.z + w * other.w;
		}
		T Dot( const Vector4& other ) const
		{
			return *this * other;
		}
		static T Dot( const Vector4& A, const Vector4& B )
		{
			return A * B;
		}
		//---------------------------------------


		//---------------------------------------
		// Scalar multiplication
		//---------------------------------------
		Vector4 operator*( const T S ) const
		{
			return Vector4( S * x, S * y, S * z, S * w );
		}
		Vector4& operator*=( const T S )
		{
			x *= S;
			y *= S;
			z *= S;
			w *= S;
			return *this;
		}
		//---------------------------------------

		//---------------------------------------
		// Scalar division
		//---------------------------------------
		Vector4 operator/( const T S ) const
		{
			T invScalar = 1 / S;
			return Vector4( invScalar * x, invScalar * y, invScalar * z, invScalar * w );
		}
		Vector4& operator/=( const T S )
		{
			T invScalar = 1 / S;
			return invScalar * *this;
		}
		//---------------------------------------


		//---------------------------------------
		// Length of vector
		//---------------------------------------
		T Length() const
		{
			return sqrt( LengthSqr() );
		}
		T LengthSqr() const
		{
			return x*x + y*y + z*z + w*w;
		}
		//---------------------------------------


		//---------------------------------------
		// Normalize this vector
		//---------------------------------------
		Vector4& Normalize()
		{
			T len = Length();
			//assertion( len == 0, "Vector4::Normalize() : Division by zero!\n" );
			if ( len )
			{
				T invLen = static_cast< T >( 1.0 ) / len;
				x *= invLen;
				y *= invLen;
				z *= invLen;
				w *= invLen;
			}
			return *this;
		}
		//---------------------------------------

		
		//---------------------------------------
		// Utility functions
		//---------------------------------------
		bool operator==( const Vector4& other )
		{
			return x == other.x &&
				   y == other.y &&
				   z == other.z &&
				   w == other.w;
		}
		//---------------------------------------
	};

	//---------------------------------------
	// Allow scalar to be used as LHS
	template< typename T, typename U >
	Vector4< T > operator*( U S, const Vector4< T >& vector )
	{
		return vector * static_cast< T >( S );
	}
	//---------------------------------------

	//---------------------------------------
	// Debugging output
	template< typename T >
	std::ostream& operator<<( std::ostream& out, const Vector4< T >& vector )
	{
		return out << vector.x << ", " << vector.y << ", " << vector.z << ", " << vector.w;
	}
	//---------------------------------------

	//---------------------------------------
	// Define Vector types
	typedef Vector4< float    > Vec4f;
	typedef Vector4< double   > Vec4d;
	typedef Vector4< int      > Vec4i;
	//---------------------------------------

}