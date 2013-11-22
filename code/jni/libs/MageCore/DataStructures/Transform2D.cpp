#include "CoreLib.h"

using namespace mage;

//---------------------------------------
Transform2D::Transform2D()
	: mMatrix( true )
	, mRMatrix( true )
	, mScale( 1.0f, 1.0f )
	, mIsIdentitiy( true )
	, mIsRotation( true )
	, mIsUniformScale( true )
{}
//---------------------------------------
Transform2D::~Transform2D()
{}
//---------------------------------------
void Transform2D::LoadIdentity()
{
	mMatrix = Matrix3f::IDENTITY;
	mTranslate.Set( 0, 0 );
	mScale.Set( 1.0f, 1.0f );
	mIsIdentitiy = true;
	mIsRotation = true;
	mIsUniformScale = true;
	UpdateMatrix();
}
//---------------------------------------
void Transform2D::SetRotation( float angleRad )
{
	mRMatrix = Matrix3f::MakeRotation( angleRad );
	mIsIdentitiy = false;
	mIsRotation = true;
	UpdateMatrix();
}
//---------------------------------------
void Transform2D::SetRotation( const Matrix3f& rMatrix )
{
	mRMatrix = rMatrix;
	mIsIdentitiy = false;
	mIsRotation = true;
	UpdateMatrix();
}
//---------------------------------------
void Transform2D::SetTranslation( const Vec2f& trans )
{
	mTranslate = trans;
	mIsIdentitiy = false;
	UpdateMatrix();
}
//---------------------------------------
void Transform2D::SetScale( const Vec2f& scale )
{
	mScale = scale;
	mIsIdentitiy = false;
	mIsUniformScale = false;
	UpdateMatrix();
}
//---------------------------------------
void Transform2D::SetScaleUniform( float scale )
{
	mScale.Set( scale, scale );
	mIsIdentitiy = false;
	mIsUniformScale = true;
	UpdateMatrix();
}
//---------------------------------------
void Transform2D::SetMatrix( const Matrix3f& mat )
{
	mRMatrix = mat;
	mIsIdentitiy = false;
	mIsRotation = false;
	mIsUniformScale = false;
	UpdateMatrix();
}
//---------------------------------------
Transform2D Transform2D::operator* ( const Transform2D& tranasform ) const
{
	if ( mIsIdentitiy )
		return tranasform;

	if ( tranasform.mIsIdentitiy )
		return *this;

	Transform2D result;

	if ( mIsRotation && tranasform.mIsRotation )
	{
		if ( mIsUniformScale )
		{
			result.SetRotation( mRMatrix * tranasform.mRMatrix );
			result.SetTranslation( GetScaleUniform() * ( mRMatrix * tranasform.mTranslate ) + mTranslate );

			if ( tranasform.mIsUniformScale )
			{
				result.SetScaleUniform( GetScaleUniform() * tranasform.GetScaleUniform() );
			}
			else
			{
				result.SetScale( GetScaleUniform() * tranasform.GetScale() );
			}

			return result;
		}
	}

	Matrix3f MA = mRMatrix;
	Matrix3f MB = tranasform.mRMatrix;

	if ( mIsRotation )
	{
		Matrix3f diag( Vec3f( mScale.x, mScale.y, 1.0f ) );
		MA = mRMatrix * diag;
	}

	if ( tranasform.mIsRotation )
	{
		Matrix3f diag( Vec3f( tranasform.mScale.x, tranasform.mScale.y, 1.0f ) );
		MB = tranasform.mRMatrix * diag;
	}

	result.SetMatrix( MA * MB );
	result.SetTranslation( MA * tranasform.mTranslate + mTranslate );
	return result;
}
//---------------------------------------
void Transform2D::UpdateMatrix()
{
	if ( mIsRotation )
	{
		Matrix3f diag( Vec3f( mScale.x, mScale.y, 1.0f ) );
		mMatrix = mRMatrix * diag;
	}
	else
	{
		mMatrix = mRMatrix;
	}

	Vec3f& r1 = mMatrix[0];
	Vec3f& r2 = mMatrix[1];

	r1[2] = mTranslate[0];
	r2[2] = mTranslate[1];
}
//---------------------------------------