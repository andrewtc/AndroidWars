#pragma once

#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"
#include "MathUtil.h"

#include <memory.h>
//#include <stdarg.h>

namespace mage
{
	template< class TReal >
	class Matrix4
	{
	public:

		// Special Matricies
		static const Matrix4 ZERO;
		static const Matrix4 IDENTITY;

		Matrix4( bool zero = false ) { if ( zero ) Zero(); else Identity(); }
		//Matrix4( TReal* arr ) { memcpy( mat, arr, 16 * sizeof( TReal ) ); }
		Matrix4( const TReal* arr )
		{
			for ( int i = 0; i < 16; ++i )
				mat[i] = arr[i];
		}
		
		Matrix4( TReal a, TReal b, TReal c, TReal d,
				 TReal e, TReal f, TReal g, TReal h,
				 TReal i, TReal j, TReal k, TReal l,
				 TReal m, TReal n, TReal o, TReal p )
		{
			mat[0] = a; mat[1] = b; mat[2] = c; mat[3] = d;
			mat[4] = e; mat[5] = f; mat[6] = g; mat[7] = h;
			mat[8] = i; mat[9] = j; mat[10]= k; mat[11]= l;
			mat[12]= m; mat[13]= n; mat[14]= o; mat[15]= p;
		}

		Matrix4( const Quaternion< TReal >& quat )
		{
			RotationFromQuat( quat );
		}

		~Matrix4() {}

		void RotationFromQuat( const Quaternion< TReal >& quat )
		{
			TReal X2 = quat.x * quat.x;
			TReal Y2 = quat.y * quat.y;
			TReal Z2 = quat.z * quat.z;

			TReal XY = quat.x * quat.y;
			TReal ZW = quat.z * quat.w;
			TReal XZ = quat.x * quat.z;
			TReal YW = quat.y * quat.w;
			TReal YZ = quat.y * quat.z;
			TReal XW = quat.x * quat.w;

			mat[0] = 1 - 2 * Y2 - 2 * Z2;
			mat[1] = 2 * XY - 2 * ZW;
			mat[2] = 2 * XZ + 2 * YW;
			mat[3] = 0;

			mat[4] = 2 * XY + 2 * ZW;
			mat[5] = 1 - 2 * X2 - 2 * Z2;
			mat[6] = 2 * YZ - 2 * XW;
			mat[7] = 0;

			mat[8 ] = 2 * XZ - 2 * YW;
			mat[9 ] = 2 * YZ + 2 * XW;
			mat[10] = 1 - 2 * X2 - 2 * Y2;
			mat[11] = 0;

			mat[12] = 0;
			mat[13] = 0;
			mat[14] = 0;
			mat[15] = 1;

			//Transpose();
		}

		const TReal* GetArray() const { return mat; }

		TReal& operator[]( int i )
		{
			return mat[i];
		}

		const TReal& operator[]( int i ) const
		{
			return mat[i];
		}

		Matrix4& operator=( const Matrix4& other )
		{
			memcpy( mat, other.mat, 16 * sizeof( TReal ) );
		
			return *this;
		}

		bool operator==( const Matrix4& other )
		{
			for ( int i = 0; i < 16; ++i )
			{
				if ( mat[i] != other.mat[i] )
				{
					return false;
				}
			}
			return true;
		}
		bool operator==( const TReal* other )
		{
			for ( int i = 0; i < 16; ++i )
			{
				if ( mat[i] != other[i] )
				{
					return false;
				}
			}
			return true;
		}

		Matrix4& Set( const Matrix4& other )
		{
			memcpy( mat, other.mat, 16 * sizeof( TReal ) );
			return *this;
		}

		Matrix4& Set( const TReal* arr )
		{
			memcpy( mat, arr, 16 * sizeof( TReal ) );
			return *this;
		}

		Matrix4 operator-( const Matrix4& other ) const
		{
			Matrix4 m;
			for ( int i = 0; i < 16; ++i )
				m.mat[i] = mat[i] - other.mat[i];
			return m;
		}

		Matrix4& operator-=( const Matrix4& other )
		{
			for ( int i = 0; i < 16; ++i ) mat[i] -= other.mat[i];
			return *this;
		}

		Matrix4 operator+( const Matrix4& other ) const
		{
			Matrix4 m;
			for ( int i = 0; i < 16; ++i )
				m.mat[i] = mat[i] + other.mat[i];
			return m;
		}

		Matrix4& operator+=( const Matrix4& other )
		{
			for ( int i = 0; i < 16; ++i )
				mat[i] += other.mat[i];
			return *this;
		}

		Matrix4 operator*( const Matrix4& other ) const
		{
			Matrix4 m;
			m.mat[0] = mat[0]*other.mat[0] + mat[1]*other.mat[4] + mat[2]*other.mat[8] + mat[3]*other.mat[12];
			m.mat[1] = mat[0]*other.mat[1] + mat[1]*other.mat[5] + mat[2]*other.mat[9] + mat[3]*other.mat[13];
			m.mat[2] = mat[0]*other.mat[2] + mat[1]*other.mat[6] + mat[2]*other.mat[10] + mat[3]*other.mat[14];
			m.mat[3] = mat[0]*other.mat[3] + mat[1]*other.mat[7] + mat[2]*other.mat[11] + mat[3]*other.mat[15];

			m.mat[4] = mat[4]*other.mat[0] + mat[5]*other.mat[4] + mat[6]*other.mat[8] + mat[7]*other.mat[12];
			m.mat[5] = mat[4]*other.mat[1] + mat[5]*other.mat[5] + mat[6]*other.mat[9] + mat[7]*other.mat[13];
			m.mat[6] = mat[4]*other.mat[2] + mat[5]*other.mat[6] + mat[6]*other.mat[10] + mat[7]*other.mat[14];
			m.mat[7] = mat[4]*other.mat[3] + mat[5]*other.mat[7] + mat[6]*other.mat[11] + mat[7]*other.mat[15];

			m.mat[8] = mat[8]*other.mat[0] + mat[9]*other.mat[4] + mat[10]*other.mat[8] + mat[11]*other.mat[12];
			m.mat[9] = mat[8]*other.mat[1] + mat[9]*other.mat[5] + mat[10]*other.mat[9] + mat[11]*other.mat[13];
			m.mat[10] = mat[8]*other.mat[2] + mat[9]*other.mat[6] + mat[10]*other.mat[10] + mat[11]*other.mat[14];
			m.mat[11] = mat[8]*other.mat[3] + mat[9]*other.mat[7] + mat[10]*other.mat[11] + mat[11]*other.mat[15];

			m.mat[12] = mat[12]*other.mat[0] + mat[13]*other.mat[4] + mat[14]*other.mat[8] + mat[15]*other.mat[12];
			m.mat[13] = mat[12]*other.mat[1] + mat[13]*other.mat[5] + mat[14]*other.mat[9] + mat[15]*other.mat[13];
			m.mat[14] = mat[12]*other.mat[2] + mat[13]*other.mat[6] + mat[14]*other.mat[10] + mat[15]*other.mat[14];
			m.mat[15] = mat[12]*other.mat[3] + mat[13]*other.mat[7] + mat[14]*other.mat[11] + mat[15]*other.mat[15];
		
			return m;
		}

		Vector3< TReal > operator*( const Vector3< TReal >& v ) const
		{
			return Vector3< TReal >
			(
				mat[0] * v.x + mat[1] * v.y + mat[2] * v.z + mat[12],
				mat[4] * v.x + mat[5] * v.y + mat[6] * v.z + mat[13],
				mat[8] * v.x + mat[9] * v.y + mat[10] * v.z + mat[14]
			);
		}

		Vector4< TReal > operator*( const Vector4< TReal >& v ) const
		{
			return Vector4< TReal >
			(
				mat[0] * v.x + mat[1] * v.y + mat[2] * v.z + mat[3] * v.w,
				mat[4] * v.x + mat[5] * v.y + mat[6] * v.z + mat[7] * v.w,
				mat[8] * v.x + mat[9] * v.y + mat[10] * v.z + mat[11] * v.w,
				mat[12] * v.x + mat[13] * v.y + mat[14] * v.z + mat[15] * v.w
			);
		}

		Matrix4& operator*=( const Matrix4& other )
		{
			return *this = *this * other;
		}

		Matrix4 operator*( const TReal scalar ) const
		{
			Matrix4 m;
			for ( int i = 0; i < 16; ++i )
				m.mat[i] = mat[i] * scalar;
			return m;
		}

		Matrix4& operator*=( const TReal scalar )
		{
			for ( int i = 0; i < 16; ++i )
				mat[i] *= scalar;
			return *this;
		}

		void Identity()
		{
			for ( int i = 0; i < 16; ++i )
				mat[i] = static_cast< TReal >( ( i % 5 ) ? 0 : 1 );
		}

		void Zero()
		{
			memset( mat, 0, sizeof( TReal ) * 16 );
		}

		Matrix4 Inverse()
		{
			TReal tmp[12];
			TReal det;
			TReal dst[16];

			Transpose();

			tmp[0] = mat[10] * mat[15];
			tmp[1] = mat[11] * mat[14];
			tmp[2] = mat[9] * mat[15];
			tmp[3] = mat[11] * mat[13];
			tmp[4] = mat[9] * mat[14];
			tmp[5] = mat[10] * mat[13];
			tmp[6] = mat[8] * mat[15];
			tmp[7] = mat[11] * mat[12];
			tmp[8] = mat[8] * mat[14];
			tmp[9] = mat[10] * mat[12];
			tmp[10] = mat[8] * mat[13];
			tmp[11] = mat[9] * mat[12];

			dst[0] = tmp[0]*mat[5] + tmp[3]*mat[6] + tmp[4]*mat[7];
			dst[0] -= tmp[1]*mat[5] + tmp[2]*mat[6] + tmp[5]*mat[7];
			dst[1] = tmp[1]*mat[4] + tmp[6]*mat[6] + tmp[9]*mat[7];
			dst[1] -= tmp[0]*mat[4] + tmp[7]*mat[6] + tmp[8]*mat[7];
			dst[2] = tmp[2]*mat[4] + tmp[7]*mat[5] + tmp[10]*mat[7];
			dst[2] -= tmp[3]*mat[4] + tmp[6]*mat[5] + tmp[11]*mat[7];
			dst[3] = tmp[5]*mat[4] + tmp[8]*mat[5] + tmp[11]*mat[6];
			dst[3] -= tmp[4]*mat[4] + tmp[9]*mat[5] + tmp[10]*mat[6];
			dst[4] = tmp[1]*mat[1] + tmp[2]*mat[2] + tmp[5]*mat[3];
			dst[4] -= tmp[0]*mat[1] + tmp[3]*mat[2] + tmp[4]*mat[3];
			dst[5] = tmp[0]*mat[0] + tmp[7]*mat[2] + tmp[8]*mat[3];
			dst[5] -= tmp[1]*mat[0] + tmp[6]*mat[2] + tmp[9]*mat[3];
			dst[6] = tmp[3]*mat[0] + tmp[6]*mat[1] + tmp[11]*mat[3];
			dst[6] -= tmp[2]*mat[0] + tmp[7]*mat[1] + tmp[10]*mat[3];
			dst[7] = tmp[4]*mat[0] + tmp[9]*mat[1] + tmp[10]*mat[2];
			dst[7] -= tmp[5]*mat[0] + tmp[8]*mat[1] + tmp[11]*mat[2];

			tmp[0] = mat[2]*mat[7];
			tmp[1] = mat[3]*mat[6];
			tmp[2] = mat[1]*mat[7];
			tmp[3] = mat[3]*mat[5];
			tmp[4] = mat[1]*mat[6];
			tmp[5] = mat[2]*mat[5];
			
			tmp[6] = mat[0]*mat[7];
			tmp[7] = mat[3]*mat[4];
			tmp[8] = mat[0]*mat[6];
			tmp[9] = mat[2]*mat[4];
			tmp[10] = mat[0]*mat[5];
			tmp[11] = mat[1]*mat[4];

			dst[8] = tmp[0]*mat[13] + tmp[3]*mat[14] + tmp[4]*mat[15];
			dst[8] -= tmp[1]*mat[13] + tmp[2]*mat[14] + tmp[5]*mat[15];
			dst[9] = tmp[1]*mat[12] + tmp[6]*mat[14] + tmp[9]*mat[15];
			dst[9] -= tmp[0]*mat[12] + tmp[7]*mat[14] + tmp[8]*mat[15];
			dst[10] = tmp[2]*mat[12] + tmp[7]*mat[13] + tmp[10]*mat[15];
			dst[10]-= tmp[3]*mat[12] + tmp[6]*mat[13] + tmp[11]*mat[15];
			dst[11] = tmp[5]*mat[12] + tmp[8]*mat[13] + tmp[11]*mat[14];
			dst[11]-= tmp[4]*mat[12] + tmp[9]*mat[13] + tmp[10]*mat[14];
			dst[12] = tmp[2]*mat[10] + tmp[5]*mat[11] + tmp[1]*mat[9];
			dst[12]-= tmp[4]*mat[11] + tmp[0]*mat[9] + tmp[3]*mat[10];
			dst[13] = tmp[8]*mat[11] + tmp[0]*mat[8] + tmp[7]*mat[10];
			dst[13]-= tmp[6]*mat[10] + tmp[9]*mat[11] + tmp[1]*mat[8];
			dst[14] = tmp[6]*mat[9] + tmp[11]*mat[11] + tmp[3]*mat[8];
			dst[14]-= tmp[10]*mat[11] + tmp[2]*mat[8] + tmp[7]*mat[9];
			dst[15] = tmp[10]*mat[10] + tmp[4]*mat[8] + tmp[9]*mat[9];
			dst[15]-= tmp[8]*mat[9] + tmp[11]*mat[10] + tmp[5]*mat[8];

			det=mat[0]*dst[0]+mat[1]*dst[1]+mat[2]*dst[2]+mat[3]*dst[3];
			if (std::fabs(det) > 0)
			{
				det = 1.0 / det;
				for ( int j = 0; j < 16; ++j )
					dst[j] *= det;

				*this = Matrix4( dst );
				return *this;
			}

			return ZERO;

			/*
			TReal a0 = mat[ 0]*mat[ 5] - mat[ 1]*mat[ 4];
			TReal a1 = mat[ 0]*mat[ 6] - mat[ 2]*mat[ 4];
			TReal a2 = mat[ 0]*mat[ 7] - mat[ 3]*mat[ 4];
			TReal a3 = mat[ 1]*mat[ 6] - mat[ 2]*mat[ 5];
			TReal a4 = mat[ 1]*mat[ 7] - mat[ 3]*mat[ 5];
			TReal a5 = mat[ 2]*mat[ 7] - mat[ 3]*mat[ 6];
			TReal b0 = mat[ 8]*mat[13] - mat[ 9]*mat[12];
			TReal b1 = mat[ 8]*mat[14] - mat[10]*mat[12];
			TReal b2 = mat[ 8]*mat[15] - mat[11]*mat[12];
			TReal b3 = mat[ 9]*mat[14] - mat[10]*mat[13];
			TReal b4 = mat[ 9]*mat[15] - mat[11]*mat[13];
			TReal b5 = mat[10]*mat[15] - mat[11]*mat[14];

			TReal det = a0*b5 - a1*b4 + a2*b3 + a3*b2 - a4*b1 + a5*b0;
			if (std::fabs(det) > 0)
			{
				Matrix4 inverse;
				inverse.mat[ 0] = + mat[ 5]*b5 - mat[ 6]*b4 + mat[ 7]*b3;
				inverse.mat[ 4] = - mat[ 4]*b5 + mat[ 6]*b2 - mat[ 7]*b1;
				inverse.mat[ 8] = + mat[ 4]*b4 - mat[ 5]*b2 + mat[ 7]*b0;
				inverse.mat[12] = - mat[ 4]*b3 + mat[ 5]*b1 - mat[ 6]*b0;
				inverse.mat[ 1] = - mat[ 1]*b5 + mat[ 2]*b4 - mat[ 3]*b3;
				inverse.mat[ 5] = + mat[ 0]*b5 - mat[ 2]*b2 + mat[ 3]*b1;
				inverse.mat[ 9] = - mat[ 0]*b4 + mat[ 1]*b2 - mat[ 3]*b0;
				inverse.mat[13] = + mat[ 0]*b3 - mat[ 1]*b1 + mat[ 2]*b0;
				inverse.mat[ 2] = + mat[13]*a5 - mat[14]*a4 + mat[15]*a3;
				inverse.mat[ 6] = - mat[12]*a5 + mat[14]*a2 - mat[15]*a1;
				inverse.mat[10] = + mat[12]*a4 - mat[13]*a2 + mat[15]*a0;
				inverse.mat[14] = - mat[12]*a3 + mat[13]*a1 - mat[14]*a0;
				inverse.mat[ 3] = - mat[ 9]*a5 + mat[10]*a4 - mat[11]*a3;
				inverse.mat[ 7] = + mat[ 8]*a5 - mat[10]*a2 + mat[11]*a1;
				inverse.mat[11] = - mat[ 8]*a4 + mat[ 9]*a2 - mat[11]*a0;
				inverse.mat[15] = + mat[ 8]*a3 - mat[ 9]*a1 + mat[10]*a0;

				TReal invDet = ((TReal)1)/det;
				inverse.mat[ 0] *= invDet;
				inverse.mat[ 1] *= invDet;
				inverse.mat[ 2] *= invDet;
				inverse.mat[ 3] *= invDet;
				inverse.mat[ 4] *= invDet;
				inverse.mat[ 5] *= invDet;
				inverse.mat[ 6] *= invDet;
				inverse.mat[ 7] *= invDet;
				inverse.mat[ 8] *= invDet;
				inverse.mat[ 9] *= invDet;
				inverse.mat[10] *= invDet;
				inverse.mat[11] *= invDet;
				inverse.mat[12] *= invDet;
				inverse.mat[13] *= invDet;
				inverse.mat[14] *= invDet;
				inverse.mat[15] *= invDet;

				return inverse;
			}

			return ZERO;
		*/
		}

		Matrix4& Transpose()
		{
			//in-place transpose
			std::swap(mat[1],mat[4]);
			std::swap(mat[2],mat[8]);
			std::swap(mat[3],mat[12]);
			std::swap(mat[6],mat[9]);
			std::swap(mat[7],mat[13]);
			std::swap(mat[11],mat[14]);
	
			return *this;
		}

		static Matrix4 Transpose( const Matrix4& mat )
		{
			Matrix4 m = mat;
			m.Transpose();
			return m;
		}

		static Matrix4 Inverse( const Matrix4& mat )
		{
			Matrix4 m = mat;
			m.Inverse();
			return m;
		}

		void Translate( const Vector3< TReal >& trans )
		{
		//	Identity();
			mat[12] = trans.x;
			mat[13] = trans.y;
			mat[14] = trans.z;
		}

		void Scale( const Vector3< TReal >& scale )
		{
			mat[ 0] *= scale[0];
			mat[ 5] *= scale[1];
			mat[10] *= scale[2];
		}

		// Axis needs to be normalized
		void Rotate( TReal angle, const Vector3< TReal >& axis )
		{
			TReal s = static_cast< TReal >( std::sin( Math< TReal >::DegreesToRadians( angle ) ) );
			TReal c = static_cast< TReal >( std::cos( Math< TReal >::DegreesToRadians( angle ) ) );

			TReal ux = axis.x;
			TReal uy = axis.y;
			TReal uz = axis.z;

			// Axis Angle rotation matrix: http://en.wikipedia.org/wiki/Rotation_matrix
			mat[0]  = c + ux*ux*(1-c);
			mat[1]  = uy*ux*(1-c) + uz*s;
			mat[2]  = uz*ux*(1-c) - uy*s;
			mat[3]  = 0;
    
			mat[4]  = ux*uy*(1-c) - uz*s;
			mat[5]  = c + uy*uy*(1-c);
			mat[6]  = uz*uy*(1-c) + ux*s;
			mat[7]  = 0;
    
			mat[8]  = ux*uz*(1-c) + uy*s;
			mat[9]  = uy*uz*(1-c) - ux*s;
			mat[10] = c + uz*uz*(1-c);
			mat[11] = 0;
    
			mat[12] = 0;
			mat[13] = 0;
			mat[14] = 0;
			mat[15] = 1;
		}

		// Perspective view
		void MakePerspective( TReal fov, TReal aspectRatio, TReal zNear, TReal zFar )
		{
			const TReal zRange = zNear - zFar;
			const TReal f = static_cast< TReal >( 1.0 ) / std::tan( Math< TReal >::DegreesToRadians( fov / static_cast< TReal >( 2.0 ) ) );
			mat[0] = f / aspectRatio;
			mat[1] = 0;
			mat[2] = 0;
			mat[3] = 0;

			mat[4] = 0;
			mat[5] = f;
			mat[6] = 0;
			mat[7] = 0;

			mat[8] = 0;
			mat[9] = 0;
			mat[10]= (zFar + zNear) / zRange;
			mat[11]= 2 * zFar * zNear / zRange;

			mat[12]= 0;
			mat[13]= 0;
			mat[14]=-1;
			mat[15]= 0;
		}

		// Orthographic view
		void MakeOrtho( TReal left, TReal right, TReal top, TReal bottom, TReal zNear, TReal zFar )
		{
			const TReal two = static_cast< TReal >( 2.0 );
			mat[0] = two / ( right - left );
			mat[1] = 0;
			mat[2] = 0;
			mat[3] = 0;

			mat[4] = 0;
			mat[5] = two / ( top - bottom );
			mat[6] = 0;
			mat[7] = 0;

			mat[8] = 0;
			mat[9] = 0;
			mat[10]= -two / ( zFar - zNear );
			mat[11]= 0;

			mat[12]= -( right + left ) / ( right - left );
			mat[13]= -( top + bottom ) / ( top - bottom );
			mat[14]= -( zFar + zNear ) / ( zFar - zNear );
			mat[15]= 1;
		}

		static Matrix4 MakeLookAt( const Vector3< TReal >& target, const Vector3< TReal > up )
		{
			Vector3< TReal > N = target;
			N.Normalize();
			Vector3< TReal > U = up;
			U.Normalize();
			U = Vector3< TReal >::Cross( U , N );
			Vector3< TReal > V = Vector3< TReal >::Cross( N , U );

			return Matrix4
			(
				U.x, U.y, U.z, 0,
				V.x, V.y, V.z, 0,
				N.x, N.y, N.z, 0,
				0, 0, 0, 1
			);
		}

		static Matrix4 MakeLookAt( const Vector3< TReal >& eye, const Vector3< TReal >& center, const Vector3< TReal > up )
		{
			Vector3< TReal > Kc = center - eye;
			Kc.Normalize();

			Vector3< TReal > Ic = Vector3< TReal >::Cross( up, Kc );
			Ic.Normalize();

			Vector3< TReal > Jc = Vector3< TReal >::Cross( Kc, Ic );

			Matrix4 R(
				Ic.x, Jc.x, Kc.x, 0,
				Ic.y, Jc.y, Kc.y, 0,
				Ic.z, Jc.z, Kc.z, 0,
				0, 0, 0, 1 );

			//Matrix4 R(
			//	Ic.x, Ic.y, Ic.z, 0,
			//	Jc.x, Jc.y, Jc.z, 0,
			//	Kc.x, Kc.y, Kc.z, 0,
			//	0, 0, 0, 1 );
			//
			//R.Transpose();

			Matrix4 Tr;
			Tr.Translate( -center );

			return Tr * R;
		}

	private:
		TReal mat[16];	// tvou mat
	};

	template< typename TReal >
	inline Matrix4< TReal > operator*( TReal scalar, const Matrix4< TReal >& mat )
	{
		return mat * scalar;
	}

	template< typename TReal >
	inline Vector4< TReal > operator*( const Vector4< TReal >& vec, const Matrix4< TReal >& mat )
	{
		return Vector4< TReal >(
			vec[0] * mat[0] + vec[1] * mat[4] + vec[2] * mat[ 8] + vec[3] * mat[12],
			vec[0] * mat[1] + vec[1] * mat[5] + vec[2] * mat[ 9] + vec[3] * mat[13],
			vec[0] * mat[2] + vec[1] * mat[6] + vec[2] * mat[10] + vec[3] * mat[14],
			vec[0] * mat[3] + vec[1] * mat[7] + vec[2] * mat[11] + vec[3] * mat[15] );
	}

	//---------------------------------------
	// Define Matrix types
	typedef Matrix4< float  > Matrix4f;
	typedef Matrix4< double > Matrix4d;
	//---------------------------------------
}