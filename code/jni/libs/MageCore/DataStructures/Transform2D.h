/*
 * Author      : Matthew Johnson
 * Date        : 13/Oct/2013
 * Description :
 *   
 */
 
#pragma once

namespace mage
{

	class Transform2D
	{
	public:
		Transform2D();
		~Transform2D();

		void LoadIdentity();
		
		void SetRotation( float angleRad );
		void SetRotation( const Matrix3f& rMatrix );
		void SetTranslation( const Vec2f& trans );
		void SetScale( const Vec2f& scale );
		void SetScaleUniform( float scale );
		void SetMatrix( const Matrix3f& mat );

		float GetRotation() const					{ return mAngleRad; }
		const Vec2f& GetTranslation() const			{ return mTranslate; }
		const Vec2f& GetScale() const				{ return mScale; }
		float GetScaleUniform() const				{ return mScale[0]; }
		const Matrix3f& GetMatrix() const			{ return mMatrix; }

		Transform2D operator* ( const Transform2D& tranasform ) const;

	private:
		void UpdateMatrix();

		Matrix3f mMatrix;

		Matrix3f mRMatrix;
		float mAngleRad;
		Vec2f mTranslate;
		Vec2f mScale;
		bool mIsIdentitiy;
		bool mIsRotation;
		bool mIsUniformScale;
	};

}