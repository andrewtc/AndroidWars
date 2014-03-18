/*
 * Author      : Matthew Johnson
 * Date        : 15/Oct/2013
 * Description :
 *   
 */
 
#pragma once

namespace mage
{

	class Camera
		: public Object
	{
		DECLARE_RTTI;
	public:
		Camera( float viewWidth, float viewHeight );
		Camera( float viewWidth, float viewHeight, const std::string& name );
		virtual ~Camera();

		void SetPosition( const Vec2f& pos );
		void SetViewport( float width, float height );
		void LookAt( const Vec2f& pos );
		void LookAtClamp( Vec2f& pos );
		// Limit the movement of the camera to these bounds
		void SetWorldBounds( const RectF& worldBounds );
		// Test a rect against this viewport
		bool RectInViewport( RectF rect ) const;

		const Vec2f& GetPosition() const			{ return mPosition; }
		float GetViewWidth() const					{ return mViewWidth; }
		float GetViewHeight() const					{ return mViewHeight; }

	private:
		Vec2f mPosition;
		float mZoom;
		RectF mViewportRect;
		RectF mWorldBounds;
		RectF mLookAtBounds;
		float mViewWidth, mViewHeight;
	};

}
