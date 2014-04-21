/*
 * Author      : Matthew Johnson
 * Date        : 3/Jan/2014
 * Description :
 *   
 */
 
#pragma once

namespace mage
{

	class RenderContext
	{
	protected:
		RenderContext()
			: mInitialized( false )
			, mIsValid( false )
            , mScreenWidth( 0 )
            , mScreenHeight( 0 )
		{}
		virtual ~RenderContext() {}

	public:
		virtual bool Init( void** hWindow ) = 0;
		virtual int SwapBuffers() = 0;
		virtual bool Invalidate() = 0;
		virtual void Suspend() = 0;
		virtual int Resume( void** hWindow ) = 0;

		int32 GetScreenWidth() { return mScreenWidth; }
		int32 GetScreenHeight() { return mScreenHeight; }
		int32 GetBufferColorSize() { return mColorSize; }
		int32 GetBufferDepthSize() { return mDepthSize; }
		float GetDriverVersion() { return mVersion; }
		bool Initialized() const { return mInitialized; }
		bool IsValid() const { return mIsValid; }

	protected:
		// Window info
		int32 mScreenWidth;
		int32 mScreenHeight;
		int32 mColorSize;
		int32 mDepthSize;

		// Flags
		bool mInitialized;
		bool mIsValid;

		// Version
		float mVersion;

	};

}