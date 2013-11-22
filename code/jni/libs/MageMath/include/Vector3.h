/*
 * Author      : Matthew Johnson
 * Date        : 15/Oct/2012
 * Description :
 *   Feature rich Vector class.
 */
#pragma once

#include <ostream>
#include <iomanip>
//#include "Quaternion.h"
#include "Vector2.h"

namespace mage
{
	//---------------------------------------
	// Vector 3
	//---------------------------------------
	template< typename T >
	class Vector3
	{
	public:
		// Components
		T x, y, z;

		static const Vector3 ZERO;
		static const Vector3 ONE;
		static const Vector3 UNIT_X;
		static const Vector3 UNIT_Y;
		static const Vector3 UNIT_Z;

		//---------------------------------------
		// Constructors
		//---------------------------------------

		// Initialize all components
		Vector3()
			: x( 0 ), y( 0 ), z( 0 )
		{}

		Vector3( const Vector2< T >& v, T z )
			: x( v.x ), y( v.y ), z( z )
		{}

		// Set components
		Vector3( T x, T y, T z )
			: x( x ), y( y ), z( z )
		{}
		//---------------------------------------


		//---------------------------------------
		// Assignment
		//---------------------------------------
		Vector3& operator=( const Vector3& other )
		{
			Set( other );
			return *this;
		}

		inline void Set( const Vector3& other )
		{
			x = other.x;
			y = other.y;
			z = other.z;
		}

		inline void Set( T X, T Y, T Z )
		{
			x = X;
			y = Y;
			z = Z;
		}

		inline void Set( T* arr )
		{
			x = arr[0];
			y = arr[1];
			z = arr[2];
		}

		//---------------------------------------


		//---------------------------------------
		// Casting
		//---------------------------------------
		template< typename U >
		operator Vector3< U >() { return Vector3< U >(
			static_cast< U >( x ),
			static_cast< U >( y ),
			static_cast< U >( z ) ); }
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
			int* r = new int[3];
			for ( int i = 0; i < 3; ++i ) r[i] = (*this)[i];
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
		Vector3 operator+( const Vector3& other ) const
		{
			return Vector3( x + other.x, y + other.y, z + other.z );
		}
		Vector3& operator+=( const Vector3& other )
		{
			x += other.x;
			y += other.y;
			z += other.z;
			return *this;
		}
		//---------------------------------------
	

		//---------------------------------------
		// Subtraction
		//---------------------------------------
		Vector3 operator-( const Vector3& other ) const
		{
			return Vector3( x - other.x, y - other.y, z - other.z );
		}
		Vector3& operator-=( const Vector3& other )
		{
			x -= other.x;
			y -= other.y;
			z -= other.z;
			return *this;
		}
		//---------------------------------------


		//---------------------------------------
		// Negation
		//---------------------------------------
		Vector3 operator-() const
		{
			return Vector3( -x, -y, -z );
		}
		Vector3& Negate()
		{
			x = -x;
			y = -y;
			z = -z;
			return *this;
		}
		//---------------------------------------


		//---------------------------------------
		// Dot product
		//---------------------------------------
		T operator*( const Vector3& other ) const
		{
			return x * other.x + y * other.y + z * other.z;
		}
		T Dot( const Vector3& other ) const
		{
			return *this * other;
		}
		static T Dot( const Vector3& A, const Vector3& B )
		{
			return A * B;
		}
		//---------------------------------------


		//---------------------------------------
		// Scalar multiplication
		//---------------------------------------
		Vector3 operator*( const T S ) const
		{
			return Vector3( S * x, S * y, S * z );
		}
		Vector3& operator*=( const T S )
		{
			x *= S;
			y *= S;
			z *= S;
			return *this;
		}
		//---------------------------------------

		//---------------------------------------
		// Scalar division
		//---------------------------------------
		Vector3 operator/( const T S ) const
		{
			T invScalar = 1 / S;
			return Vector3( invScalar * x, invScalar * y, invScalar * z );
		}
		Vector3& operator/=( const T S )
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
			return x*x + y*y + z*z;
		}
		//---------------------------------------


		//---------------------------------------
		// Normalize this vector
		//---------------------------------------
		Vector3& Normalize()
		{
			T len = Length();
			//assertion( len == 0, "Vector3::Normalize() : Division by zero!\n" );
			if ( len )
			{
				x /= len;
				y /= len;
				z /= len;
			}
			return *this;
		}
		//---------------------------------------

	
		//---------------------------------------
		// Axis angle rotation
		//---------------------------------------
		/*void Rotate( const Vector3& axis, T angleRad )
		{
			Quaternion< T > rotationQ( axis, angleRad );
			Quaternion< T > conjuateQ = rotationQ.Conjugate();
			Quaternion< T > W = rotationQ * (*this) * conjuateQ;

			x = W.x;
			y = W.y;
			z = W.z;
		}*/
		//---------------------------------------


		//---------------------------------------
		// Cross Product
		//---------------------------------------
		Vector3 Cross( const Vector3& other ) const
		{
			return Vector3::Cross( *this, other );
		}
		static Vector3 Cross( const Vector3& v1, const Vector3& v2 )
		{
			return Vector3( v1.y * v2.z - v1.z * v2.y,
							v1.z * v2.x - v1.x * v2.z,
							v1.x * v2.y - v1.y * v2.x );
		}
		//---------------------------------------


		//---------------------------------------
		// Utility functions
		//---------------------------------------
		template< typename op_t >
		static Vector3 ComponentWiseOp( const Vector3& A, const Vector3& B, op_t fn )
		{
			return
			(
				fn( A.x, B.x ),
				fn( A.y, B.y ),
				fn( A.z, B.z )
			);
		}

		static Vector3 Min( const Vector3& A, const Vector3& B )
		{
			//return ComponentWiseOp( A, B, std::min );

			return Vector3
			(
				std::min( A.x, B.x ),
				std::min( A.y, B.y ),
				std::min( A.z, B.z )
			);
		}

		static Vector3 Max( const Vector3& A, const Vector3& B )
		{
			//return ComponentWiseOp( A, B, std::max );

			return Vector3
			(
				std::max( A.x, B.x ),
				std::max( A.y, B.y ),
				std::max( A.z, B.z )
			);
		}

		bool operator==( const Vector3& other ) const
		{
			if ( x == other.x &&
				y == other.y &&
				z == other.z ) return true;
			return false;
		}
		//---------------------------------------
	};

	//---------------------------------------
	// Allow scalar to be used as LHS
	template< typename T, typename U >
	Vector3< T > operator*( U S, const Vector3< T >& vector )
	{
		return vector * static_cast< T >( S );
	}
	//---------------------------------------

	//---------------------------------------
	// Debugging output
	template< typename T >
	std::ostream& operator<<( std::ostream& out, const Vector3< T >& vector )
	{
		//char buffer[256];
		//sprintf( buffer, "[ %0.3f, %0.3f, %0.3f ]", vector.x, vector.y, vector.z );
		//return out << buffer;
		return out << vector.x << ", " << vector.y << ", " << vector.z;
	}
	//---------------------------------------

	//---------------------------------------
	// Define Vector types
	typedef Vector3< float    > Vec3f;
	typedef Vector3< double   > Vec3d;
	typedef Vector3< int      > Vec3i;
	typedef Vector3< unsigned > Vec3u;
	typedef Vector3< long     > Vec3l;
	//---------------------------------------

}