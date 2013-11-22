/*
 * Author      : Matthew Johnson
 * Date        : 29/Sep/2013
 * Description :
 *   
 */
 
#pragma once

 #include "Vector2.h"
 
namespace mage
{

	template< typename TReal >
	class Matrix2
	{
	public:
		//---------------------------------------
		static const Matrix2 ZERO;
		static const Matrix2 IDENTITY;
		//---------------------------------------


		//---------------------------------------
		// Various ways to access matrix data
		union
		{
			struct  
			{
				TReal m00, m01;
				TReal m10, m11;
			};
			TReal m[2][2];
			TReal v[4];
		};
		//---------------------------------------


		//---------------------------------------
		Matrix2()
			: m00( 0 )
			, m01( 0 )
			, m10( 0 )
			, m11( 0 )
		{}
		Matrix2( TReal m00, TReal m01, TReal m10, TReal m11 )
			: m00( m00 )
			, m01( m01 )
			, m10( m10 )
			, m11( m11 )
		{}
		Matrix2( TReal rotationRad )
		{
			SetRotation( rotationRad );
		}
		//---------------------------------------


		//---------------------------------------
		void SetRotation( TReal rotationRad )
		{
			TReal c = std::cos( rotationRad );
			TReal s = std::sin( rotationRad );

			m00 = c; m01 = -s;
			m10 = s; m11 =  c;
		}
		//---------------------------------------


		//---------------------------------------
		Matrix2 GetTranspose() const
		{
			return Matrix2( m00, m10, m01, m11 );
		}
		//---------------------------------------


		//---------------------------------------
		Vector2< TReal > operator* ( const Vector2< TReal >& rhs ) const
		{
			return Vector2< TReal >( m00 * rhs.x + m01 * rhs.y, m10 * rhs.x + m11 * rhs.y );
		}
		//---------------------------------------


		//---------------------------------------
		Matrix2 operator* ( const Matrix2& rhs ) const
		{
			return Matrix2(
				m00 * rhs.m00 + m01 * rhs.m10,
				m00 * rhs.m01 + m01 * rhs.m11,
				m10 * rhs.m00 + m11 * rhs.m10,
				m10 * rhs.m01 + m11 * rhs.m11
			);
		}
		//---------------------------------------
	};

	//---------------------------------------
	// Define Matrix types
	typedef Matrix2< float  > Matrix2f;
	typedef Matrix2< double > Matrix2d;
	//---------------------------------------
}