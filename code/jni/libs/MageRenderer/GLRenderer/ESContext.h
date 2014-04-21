/*
 * Author      : Matthew Johnson
 * Date        : 3/Jan/2014
 * Description :
 *   OpenGL ES context. Will try to initialize as ES 3.0 if available. Falls back to ES 2.0 context.
 */
 
#pragma once

namespace mage
{

	class ESContext
		: public GLContext
	{
	public:
		ESContext();
		virtual ~ESContext();

		virtual bool Init( void** hWindow );
		virtual int SwapBuffers();
		virtual bool Invalidate();
		virtual void Suspend();
		virtual int Resume( void** hWindow );

		static ESContext* GetContext();
	protected:
		void InitGLES();
		void Destroy();
		bool InitEGLSurface();
		bool InitEGLContext();
		void QueryScreenSize();

		bool mES3Supported;
		bool mEGLInitialized;

		struct ESWindow* mWindow;	
	};

}
