/*
 * Author      : Matthew Johnson
 * Date        : 26/Dec/2012
 * Description :
 *   
 */
 
#pragma once

#include "Vector3.h"

namespace mage
{

	template< typename TReal >
	class Matrix3
	{
	public:
		static const Matrix3 ZERO;
		static const Matrix3 IDENTITY;


		//---------------------------------------
		// Constructors
		//---------------------------------------
		Matrix3( bool identity=false )
		{
			if ( identity )
			{
				rows[0] = Vector3< TReal >( 1, 0, 0 );
				rows[1] = Vector3< TReal >( 0, 1, 0 );
				rows[2] = Vector3< TReal >( 0, 0, 1 );
			}
			else
			{
				rows[0] = Vector3< TReal >( 0, 0, 0 );
				rows[1] = Vector3< TReal >( 0, 0, 0 );
				rows[2] = Vector3< TReal >( 0, 0, 0 );
			}
		}
		//---------------------------------------
		/*Matrix3( TReal m00, TReal m01, TReal m02,
				 TReal m10, TReal m11, TReal m12,
				 TReal m20, TReal m21, TReal m22 )
			: m00( m00 ), m01( m01 ), m02( m02 ),
			  m10( m10 ), m11( m11 ), m12( m12 ),
			  m20( m20 ), m21( m21 ), m22( m22 )
		{}*/
		//---------------------------------------
		// From row vectors
		Matrix3( const Vector3< TReal >& r1,
				 const Vector3< TReal >& r2,
				 const Vector3< TReal >& r3 )
		{
			rows[0] = r1;
			rows[1] = r2;
			rows[2] = r3;
		}
		//---------------------------------------
		// Diagonal
		Matrix3( TReal m00, TReal m11, TReal m22 )
		{
			rows[0] = Vector3< TReal >( m00, 0, 0 );
			rows[1] = Vector3< TReal >( 0, m11, 0 );
			rows[2] = Vector3< TReal >( 0, 0, m22 );
		}
		Matrix3( const Vector3< TReal >& diag )
		{
			rows[0] = Vector3< TReal >( diag[0], 0, 0 );
			rows[1] = Vector3< TReal >( 0, diag[1], 0 );
			rows[2] = Vector3< TReal >( 0, 0, diag[2] );
		}
		//---------------------------------------
		// Rotation
		static Matrix3 MakeRotation( TReal angleRad )
		{
			TReal c = std::cos( angleRad );
			TReal s = std::sin( angleRad );

			return Matrix3(
				Vector3< TReal >( c,-s, 0 ),
				Vector3< TReal >( s, c, 0 ),
				Vector3< TReal >( 0, 0, 1 )
				);
		}
		//---------------------------------------
		// Translation
		static Matrix3 MakeTranslation( const Vector2< TReal >& v )
		{
			return Matrix3(
				Vector3< TReal >( 1, 0, v.x ),
				Vector3< TReal >( 0, 1, v.y ),
				Vector3< TReal >( 0, 0, 1  )
				);
		}
		static Matrix3 MakeTranslation( TReal tx, TReal ty )
		{
			return Matrix3(
				Vector3< TReal >( 1, 0, tx ),
				Vector3< TReal >( 0, 1, ty ),
				Vector3< TReal >( 0, 0, 1  )
				);
		}
		//---------------------------------------
		// Scale
		static Matrix3 MakeScale( TReal sx, TReal sy )
		{
			return Matrix3( sx, sx, 1 );
		}
		//---------------------------------------



		//---------------------------------------
		// Special
		//---------------------------------------
		Matrix3 GetTranspose() const
		{
			return Matrix3(
				Vec3f( rows[0][0], rows[1][0], rows[2][0] ),
				Vec3f( rows[0][1], rows[1][1], rows[2][1] ),
				Vec3f( rows[0][2], rows[1][2], rows[2][2] ) );
		}
		//---------------------------------------
		TReal Determinant() const
		{
			TReal d = rows[0][0] * ( rows[1][1] * rows[2][2] - rows[1][2] * rows[2][1] )
				    - rows[0][1] * ( rows[1][0] * rows[2][2] - rows[1][2] * rows[2][0] )
					+ rows[0][2] * ( rows[1][0] * rows[2][1] - rows[1][1] * rows[2][0] );
			return d;
		}
		//---------------------------------------
		Matrix3 Adjoint() const
		{
			Matrix3 a;

			a[0][0] = rows[1][1] * rows[2][2] - rows[1][2] * rows[2][1];
			a[1][0] = rows[1][2] * rows[2][0] - rows[1][0] * rows[2][2];
			a[2][0] = rows[1][0] * rows[2][1] - rows[1][1] * rows[2][1];

			a[0][1] = rows[0][2] * rows[2][1] - rows[0][1] * rows[2][2];
			a[1][1] = rows[0][0] * rows[2][2] - rows[0][2] * rows[2][0];
			a[2][1] = rows[0][1] * rows[2][0] - rows[0][0] * rows[2][1];

			a[0][2] = rows[0][1] * rows[1][2] - rows[0][2] * rows[1][1];
			a[1][2] = rows[0][2] * rows[1][0] - rows[0][0] * rows[1][2];
			a[2][2] = rows[0][0] * rows[1][1] - rows[0][1] * rows[1][0];

			return a;
		}
		//---------------------------------------
		Matrix3 Inverse() const
		{
			Matrix3 inverse;
			TReal d = Determinant();
			TReal invD;

			if ( std::fabs( d ) > 0 )
			{
				invD = (TReal) ( 1.0 / d );
				inverse = Adjoint();
				inverse = inverse * invD;
			}
			
			return inverse;
		}
		//---------------------------------------



		//---------------------------------------
		// Access
		//---------------------------------------
		Vector3< TReal > operator[]( int index ) const
		{
			return rows[ index ];
		}
		//---------------------------------------
		Vector3< TReal >& operator[]( int index )
		{
			return rows[ index ];
		}
		//---------------------------------------
		Vector3< TReal > GetColumn( int index ) const
		{
			return Vector3< TReal >(
				rows[0][ index ],
				rows[1][ index ],
				rows[2][ index ]
				);
		}
		//---------------------------------------



		//---------------------------------------
		// Operators
		//---------------------------------------
		Matrix3& operator=( const Matrix3& rhs )
		{
			rows[0] = rhs.rows[0];
			rows[1] = rhs.rows[1];
			rows[2] = rhs.rows[2];
			return *this;
		}
		//---------------------------------------
		bool operator==( const Matrix3& rhs ) const
		{
			return rows[0] == rhs.rows[0] &&
				   rows[1] == rhs.rows[1] &&
				   rows[2] == rhs.rows[2];
		}
		//---------------------------------------
		bool operator!=( const Matrix3& rhs ) const
		{
			return !( *this == rhs );
		}
		//---------------------------------------
		Matrix3 operator+( const Matrix3& rhs ) const
		{
			return Matrix3(
				rows[0] + rhs.rows[0],
				rows[1] + rhs.rows[1],
				rows[2] + rhs.rows[2]
				);
		}
		//---------------------------------------
		Matrix3 operator-( const Matrix3& rhs ) const
		{
			return Matrix3(
				rows[0] - rhs.rows[0],
				rows[1] - rhs.rows[1],
				rows[2] - rhs.rows[2]
			);
		}
		//---------------------------------------
		Matrix3 operator*( TReal scale ) const
		{
			return Matrix3(
				scale * rows[0],
				scale * rows[1],
				scale * rows[2]
			);
		}
		//---------------------------------------
		Matrix3 operator/( TReal scale ) const
		{
			const float invScale = 1.0f / scale;
			return *this * (TReal) invScale;
		}
		//---------------------------------------
		Matrix3 operator*( const Matrix3& rhs ) const
		{
			return Matrix3(
				Vector3< TReal >( rows[0] * rhs.GetColumn( 0 ), rows[0] * rhs.GetColumn( 1 ), rows[0] * rhs.GetColumn( 2 ) ),
				Vector3< TReal >( rows[1] * rhs.GetColumn( 0 ), rows[1] * rhs.GetColumn( 1 ), rows[1] * rhs.GetColumn( 2 ) ),
				Vector3< TReal >( rows[2] * rhs.GetColumn( 0 ), rows[2] * rhs.GetColumn( 1 ), rows[2] * rhs.GetColumn( 2 ) )
				);
		}
		//---------------------------------------
		Vector3< TReal > operator*( const Vector3< TReal >& vec ) const
		{
			return Vector3< TReal >(
				vec * rows[0],
				vec * rows[1],
				vec * rows[2]
				);
		}
		//---------------------------------------
		Vector2< TReal > operator*( const Vector2< TReal >& vec ) const
		{
			const Vec2f a( rows[0][0], rows[0][1] );
			const Vec2f b( rows[1][0], rows[1][1] );
			return Vector2< TReal >(
				vec * a + rows[0][2],
				vec * b + rows[1][2]
			);
		}
		//---------------------------------------
		template< typename T >
		friend Vector3< T > operator*( const Vector3< T >& vec, const Matrix3& mat );
		//---------------------------------------
		template< typename T >
		friend Matrix3< T > operator*( const Vector2< T >& vec, const Matrix3& mat );
		//---------------------------------------
		template< typename T >
		friend Matrix3< T > operator*( T scale, const Matrix3& mat );
		//---------------------------------------

		
		/*union
		{
			struct  
			{
				TReal m00, m01, m02;
				TReal m10, m11, m12;
				TReal m20, m21, m22;
			};
			Vector3< TReal > rows[3];
		};*/
		Vector3< TReal > rows[3];
	};

	//---------------------------------------
	template< typename T >
	Vector3< T > operator*( const Vector3< T >& vec, const Matrix3< T >& mat )
	{
		return mat.GetTranspose() * vec;
	}
	//---------------------------------------
	template< typename T, typename U >
	Matrix3< T > operator*( const Vector2< U >& vec, const Matrix3< T >& mat )
	{
		return mat * Vec3f( vec, 1.0f );
	}
	//---------------------------------------
	template< typename T, typename U >
	Matrix3< U > operator*( T scale, const Matrix3< U >& mat )
	{
		return mat * scale;
	}
	//---------------------------------------


	//---------------------------------------
	typedef Matrix3< float  > Matrix3f;
	typedef Matrix3< double > Matrix3d;
}