/*
 * Author      : Matthew Johnson
 * Date        : 27/Dec/2012
 * Description :
 *   A quaternion is of the form q = w + x*i + y*j + z*k
 */
 
#pragma once

#include "Matrix3.h"

namespace mage
{

	template< typename TReal >
	class Quaternion
	{
	public:
		// Components
		TReal w, x, y, z;

		// Constructors
		Quaternion();
		Quaternion( TReal w, TReal x, TReal y, TReal z );
		Quaternion( const Quaternion& q );
		Quaternion( const Matrix3< TReal >& rot );
		Quaternion( const Vector3< TReal >& axis, TReal angleRad );

		// Assignment
		inline Quaternion& operator=( const Quaternion& q );

		// Accessors [w, x, y, z]
		inline TReal  operator[]( int i ) const;
		inline TReal& operator[]( int i );

		// Arithmetic operations
		inline Quaternion operator+( const Quaternion& q ) const;
		inline Quaternion operator-( const Quaternion& q ) const;
		inline Quaternion operator*( const Quaternion& q ) const;
		inline Quaternion operator*( TReal scalar ) const;
		inline Quaternion operator/( TReal scalar ) const;
		inline Quaternion operator-() const;
		friend Quaternion< TReal > operator*( TReal scalar, const Quaternion< TReal >& q ) { return q * scalar; }
		//friend Quaternion< TReal > operator*( const Quaternion< TReal >& q, const Vector3< TReal >& v );

		// Arithmetic modifications
		inline Quaternion& operator+=( const Quaternion& q );
		inline Quaternion& operator-=( const Quaternion& q );
		inline Quaternion& operator*=( TReal scalar );
		inline Quaternion& operator/=( TReal scalar );

		// Quaternion functions
		inline TReal Length() const;
		inline TReal SquareLength() const;
		inline TReal Dot( const Quaternion& q ) const;
		inline TReal Normalize( TReal epsilon = Math< TReal >::ZERO_TOLERANCE );
		Quaternion Inverse() const;
		Quaternion Conjugate() const;
		Quaternion Exp() const;
		Quaternion Log() const;

		// Conversion to/from matrix/quaternion/axis angle functions
		void ToRotationMatrix( Matrix3< TReal >& rot ) const;
		void FromRotationMatrix( const Matrix3< TReal >& rot );
		void ToAxisAngle( Vector3< TReal >& axis, TReal& angleRad ) const;
		void FromAxisAngle( const Vector3< TReal >& axis, TReal angleRad );

		// Rotation of a vector by a quaternion
		Vector3< TReal > Rotate( const Vector3< TReal >& v ) const;

		// Spherical linear interpolation
		Quaternion& Slerp( TReal t, const Quaternion& p, const Quaternion& q );

		// Spherical quadratic interpolation
		Quaternion& Squad( TReal t, const Quaternion& q0, const Quaternion& a0,
			const Quaternion& a1, const Quaternion& q1 );
		Quaternion& Intermediate( const Quaternion& q0, const Quaternion& q1 );


		// Special Quaternions
		static const Quaternion ZERO;
		static const Quaternion IDENTITY;

	};

	//---------------------------------------
	// Implementation
	//---------------------------------------

	//---------------------------------------
	// Macros
	//---------------------------------------
#	define Q_TMPL template< typename TReal >
#	define Q Quaternion< TReal >
	//---------------------------------------

	//---------------------------------------
	// Constructors
	//---------------------------------------
	Q_TMPL Q::Quaternion()
	{}
	//---------------------------------------
	Q_TMPL Q::Quaternion( TReal w, TReal x, TReal y, TReal z )
		: w( w )
		, x( x )
		, y( y )
		, z( z )
	{}
	//---------------------------------------
	Q_TMPL Q::Quaternion( const Quaternion& q )
		: w( q.w )
		, x( q.x )
		, y( q.y )
		, z( q.z )
	{}
	//---------------------------------------
	Q_TMPL Q::Quaternion( const Matrix3< TReal >& rot )
	{
		FromRotationMatrix( rot );
	}
	//---------------------------------------
	Q_TMPL Q::Quaternion( const Vector3< TReal >& axis, TReal angleRad )
	{
		FromAxisAngle( axis, angleRad );
	}
	//---------------------------------------

	//---------------------------------------
	// Assignment
	//---------------------------------------
	Q_TMPL inline Q& Q::operator=( const Quaternion& q )
	{
		w = q.w;
		x = q.x;
		y = q.y;
		z = q.z;
		return *this;
	}
	//---------------------------------------

	//---------------------------------------
	// Accessors [w, x, y, z]
	//---------------------------------------
	Q_TMPL inline TReal  Q::operator[]( int i ) const
	{
		return *( &w + i );
	}
	//---------------------------------------
	Q_TMPL inline TReal& Q::operator[]( int i )
	{
		return *( &w + i );
	}
	//---------------------------------------

	//---------------------------------------
	// Arithmetic operations
	//---------------------------------------
	Q_TMPL inline Q Q::operator+( const Quaternion& q ) const
	{
		Quaternion result;
		for ( int i = 0; i < 4; ++i )
		{
			result[i] = (*this)[i] + q[i];
		}
		return result;
	}
	//---------------------------------------
	Q_TMPL inline Q Q::operator-( const Quaternion& q ) const
	{
		Quaternion result;
		for ( int i = 0; i < 4; ++i )
		{
			result[i] = (*this)[i] - q[i];
		}
		return result;
	}
	//---------------------------------------
	Q_TMPL inline Q Q::operator*( const Quaternion& q ) const
	{
		Quaternion result;
		
		result.w =
			w * q.w -
			x * q.x -
			y * q.y -
			z * q.z;

		result.x =
			w * q.x +
			x * q.w +
			y * q.z -
			z * q.y;

		result.y =
			w * q.y +
			y * q.w +
			z * q.x -
			x * q.z;

		result.z =
			w * q.z -
			z * q.w -
			x * q.y -
			y * q.x;

		return result;
	}
	//---------------------------------------
	Q_TMPL inline Q Q::operator*( TReal scalar ) const
	{
		Quaternion result;
		for ( int i = 0; i < 4; ++i )
		{
			result[i] = scalar * (*this)[i];
		}
		return result;
	}
	//---------------------------------------
	Q_TMPL inline Q Q::operator/( TReal scalar ) const
	{
		Quaternion result;
		TReal invScalar = 1 / scalar;
		for ( int i = 0; i < 4; ++i )
		{
			result[i] = invScalar * (*this)[i];
		}
		return result;
	}
	//---------------------------------------
	Q_TMPL inline Q Q::operator-() const
	{
		Quaternion result;
		for ( int i = 0; i < 4; ++i )
		{
			result[i] = -(*this)[i];
		}
		return result;
	}
	//---------------------------------------
	//Q_TMPL Quaternion< TReal > Q::operator*( TReal scalar, const Quaternion< TReal >& q )
	//{
	//	return q * scalar;
	//}
	//---------------------------------------
	template< typename TReal >
	Quaternion< TReal > operator*( const Quaternion< TReal >& q, const Vector3< TReal >& v )
	{
		return Quaternion< TReal >
		(
			-( q.x * v.x ) - ( q.y * v.y ) - ( q.z * v.z ),
			 ( q.w * v.x ) + ( q.y * v.z ) - ( q.z * v.y ),
			 ( q.w * v.y ) + ( q.z * v.x ) - ( q.x * v.z ),
			 ( q.w * v.z ) + ( q.x * v.y ) - ( q.y * v.x )
		);
	}
	//---------------------------------------

	//---------------------------------------
	// Arithmetic modifications
	//---------------------------------------
	Q_TMPL inline Q& Q::operator+=( const Quaternion& q )
	{
		for ( int i = 0; i < 4; ++i )
		{
			(*this)[i] += q[i];
		}
		return *this;
	}
	//---------------------------------------
	Q_TMPL inline Q& Q::operator-=( const Quaternion& q )
	{
		for ( int i = 0; i < 4; ++i )
		{
			(*this)[i] -= q[i];
		}
		return *this;
	}
	//---------------------------------------
	Q_TMPL inline Q& Q::operator*=( TReal scalar )
	{
		for ( int i = 0; i < 4; ++i )
		{
			(*this)[i] *= scalar;
		}
		return *this;
	}
	//---------------------------------------
	Q_TMPL inline Q& Q::operator/=( TReal scalar )
	{
		TReal invScalar = 1 / scalar;
		for ( int i = 0; i < 4; ++i )
		{
			(*this)[i] *= invScalar;
		}
		return *this;
	}
	//---------------------------------------

	//---------------------------------------
	// Quaternion functions
	//---------------------------------------
	Q_TMPL inline TReal Q::Length() const
	{
		return std::sqrt( SquareLength() );
	}
	//---------------------------------------
	Q_TMPL inline TReal Q::SquareLength() const
	{
		return w*w + x*x + y*y + z*z;
	}
	//---------------------------------------
	Q_TMPL inline TReal Q::Dot( const Quaternion& q ) const
	{
		return w*q.w + x*q.x + y*q.y + z*q.z;
	}
	//---------------------------------------
	Q_TMPL inline TReal Q::Normalize( TReal epsilon )
	{
		TReal length = Length();

		if ( length > epsilon )
		{
			TReal invLength = 1 / length;
			w *=  invLength;
			x *=  invLength;
			y *=  invLength;
			z *=  invLength;
		}
		else
		{
			length = 0;
			w = 0;
			x = 0;
			y = 0;
			z = 0;
		}

		return length;
	}
	//---------------------------------------
	Q_TMPL Q Q::Inverse() const
	{
		Quaternion inverse;
		TReal norm = SquareLength();

		if ( norm > 0 )
		{
			TReal invNorm = 1 / norm;
			inverse.w = w * invNorm;
			for ( int i = 1; i < 4; ++i )
			{
				inverse[i] = -(*this)[i] * invNorm;
			}
		}
		else
		{
			// Return invalid result
			for ( int i = 0; i < 4; ++i )
			{
				inverse[i] = 0;
			}
		}

		return inverse;
	}
	//---------------------------------------
	Q_TMPL Q Q::Conjugate() const
	{
		return Quaternion( w, -x, -y, -z );
	}
	//---------------------------------------
	Q_TMPL Q Q::Exp() const
	{
		// TODO
		return Quaternion();
	}
	//---------------------------------------
	Q_TMPL Q Q::Log() const
	{
		// TODO
		return Quaternion();
	}
	//---------------------------------------

	//---------------------------------------
	// Conversion to/from matrix/quaternion/axis angle functions
	//---------------------------------------
	Q_TMPL void Q::ToRotationMatrix( Matrix3< TReal >& rot ) const
	{
		rot( 0, 0 ) = 1 - ( 2*y*y + 2*z*z );
		rot( 0, 1 ) = 2*x*y - 2*w*z;
		rot( 0, 2 ) = 2*x*z + 2*w*y;
		rot( 1, 0 ) = 2*x*y + 2*w*z;
		rot( 1, 1 ) = 1 - ( 2*x*x + 2*z*z );
		rot( 1, 2 ) = 2*y*z - 2*w*x;
		rot( 2, 0 ) = 2*x*z - 2*w*y;
		rot( 2, 1 ) = 2*y*z + 2*w*x;
		rot( 2, 2 ) = 1 - ( 2*x*x + 2*y*y );
	}
	//---------------------------------------
	Q_TMPL void Q::FromRotationMatrix( const Matrix3< TReal >& rot )
	{
		// Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
		// article "Quaternion Calculus and Fast Animation".
		const int next[3] = { 1, 2, 0 };
		TReal trace = rot( 0, 0 ) + rot( 1, 1 ) + rot( 2, 2 );
		TReal root;

		if ( trace > 0 )
		{
			// |w| > 1/2, may as well choose w > 1/2
			root = std::sqrt( trace + 1 );	// 2w
			w = static_cast< TReal >( 0.5 ) * root;
			root = static_cast< TReal >( 0.5 ) / root;	// 1/(4w)
			x = ( rot( 2, 1 ) - rot( 1, 2 ) ) * root;
			y = ( rot( 0, 2 ) - rot( 2, 0 ) ) * root;
			z = ( rot( 1, 0 ) - rot( 0, 1 ) ) * root;
		}
		else
		{
			// |w| <= 1/2
			int i = 0;
			if ( rot( 1, 1 ) > rot( 0, 0 ) )
			{
				i = 1;
			}
			if ( rot( 2, 2 ) > rot( i, i ) )
			{
				i = 2;
			}
			int j = next[i];
			int k = next[j];

			root = std::sqrt( rot( i, i ) - rot( j, j ) - rot( k, k ) + 1 );
			TReal* quat[3] = { &x, &y, &z };
			*quat[i] = static_cast< TReal >( 0.5 ) * root;
			root = static_cast< TReal >( 0.5 ) / root;
			w = ( rot( k, j ) - rot( j, k ) ) * root;
			*quat[j] = ( rot( j, i ) + rot( i, j ) ) * root;
			*quat[k] = ( rot( k, i ) + rot( i, k ) ) * root;
		}
	}
	//---------------------------------------
	Q_TMPL void Q::ToAxisAngle( Vector3< TReal >& axis, TReal& angleRad ) const
	{
		// The quaternion representing the rotation is
		//   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)
		TReal sqrLength = x*x + y*y + z*z;

		if ( sqrLength > Math< TReal >::ZERO_TOLERANCE )
		{
			angleRad = 2 * std::acos( w );
			TReal invLength = 1 / std::sqrt( sqrLength );
			for ( int i = 0; i < 3; ++i )
			{
				axis[i] = (*this)[i+1] * invLength;
			}
		}
		else
		{
			// Angle is 0 (mod 2*pi), so any axis will do.
			angleRad = 0;
			axis[0] = 1;
			axis[1] = 0;
			axis[2] = 0;
		}
	}
	//---------------------------------------
	Q_TMPL void Q::FromAxisAngle( const Vector3< TReal >& axis, TReal angleRad )
	{
		// Assuming axis it unit length
		// The quaternion representing the rotation is
		//   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

		TReal halfAngle = static_cast< TReal >( 0.5 ) * angleRad;
		TReal sn = std::sin( halfAngle );
		w = std::cos( halfAngle );
		x = sn * axis.x;
		y = sn * axis.y;
		z = sn * axis.z;
	}
	//---------------------------------------
	Q_TMPL Vector3< TReal > Q::Rotate( const Vector3< TReal >& v ) const
	{
		Matrix3< TReal > rot;
		ToRotationMatrix( rot );
		return rot * v;
	}
	//---------------------------------------
	Q_TMPL Q& Q::Slerp( TReal t, const Q& p, const Q& q )
	{
		// Adapted from the gmtl project. Best slerp implementation I've been able to find.
		// Calc cos theta
		TReal cosom = p.x * q.x + p.y * q.y + p.z * q.z + p.w * q.w;

		// Adjust signs
		Q end = q;
		if ( cosom < 0.0 )
		{
			cosom = - cosom;
			end.x = -end.x;
			end.y = -end.y;
			end.z = -end.z;
			end.w = -end.w;
		}

		// Calc coeff
		TReal sclp, sclq;
		if ( ( 1.0 - cosom ) > 0.0001 )
		{
			TReal omega, sinom;
			omega = acos( cosom );
			sinom = sin( omega );
			sclp = sin( ( 1.0 - t ) * omega ) / sinom;
			sclq = sin( t * omega ) / sinom;
		}
		else
		{
			sclp = 1.0 - t;
			sclq = t;
		}

		x = sclp * p.x + sclq * end.x;
		y = sclp * p.y + sclq * end.y;
		z = sclp * p.z + sclq * end.z;
		w = sclp * p.w + sclq * end.w;

		// This implementation causes jitter in some cases.
		/*
		TReal cs = p.Dot( q );
		TReal angle = std::acos( cs );

		if ( std::fabs( angle ) >= Math< TReal >::ZERO_TOLERANCE )
		{
			TReal sn = std::sin( angle );
			TReal invSn = ( (TReal) 1 ) / sn;
			TReal tAngle = t * angle;
			TReal c0 = std::sin( angle - tAngle ) * invSn;
			TReal c1 = std::sin( tAngle ) * invSn;

			w = c0 * p.w + c1 * q.w;
			x = c0 * p.x + c1 * q.x;
			y = c0 * p.y + c1 * q.y;
			z = c0 * p.z + c1 * q.z;
		}
		else
		{
			w = p.w;
			x = p.x;
			y = p.y;
			z = p.z;
		}
		*/
		return *this;
	}
	//---------------------------------------
#	undef Q_TMPL
#	undef Q
	//---------------------------------------

	typedef Quaternion< float  > Quatf;
	typedef Quaternion< double > Quatd;
}