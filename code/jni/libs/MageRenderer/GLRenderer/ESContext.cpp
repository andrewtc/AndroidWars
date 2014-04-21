#include "RendererLib.h"

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include "gl3stub.h"

namespace mage
{
	struct ESWindow
	{
		ANativeWindow* window;
		EGLDisplay display;
		EGLSurface surface;
		EGLContext context;
		EGLConfig config;
	};
}

using namespace mage;

//---------------------------------------
ESContext* ESContext::GetContext()
{
	static ESContext context;
	return &context;
}
//---------------------------------------
ESContext::ESContext()
	: mES3Supported( false )
	, mEGLInitialized( false )
{
	mWindow = new ESWindow;

	mWindow->window = 0;
	mWindow->display = EGL_NO_DISPLAY;
	mWindow->surface = EGL_NO_SURFACE;
	mWindow->context = EGL_NO_CONTEXT;
	mWindow->config = 0;
}
//---------------------------------------
ESContext::~ESContext()
{
	Destroy();
	delete mWindow;
}
//---------------------------------------
bool ESContext::Init( void** hWindow )
{
	if ( !mInitialized )
	{
		// Initialize EGL
		mWindow->window = (ANativeWindow*) *hWindow;

		InitEGLSurface();
		InitEGLContext();
		InitGLES();

		// Wait until everything else is initialized since some
		// vendors do not have the surface size ready when the surface is created
		QueryScreenSize();

		mInitialized = true;
	}
	return true;
}
//---------------------------------------
int ESContext::SwapBuffers()
{
	bool b = eglSwapBuffers( mWindow->display, mWindow->surface );
	if ( !b )
	{
		EGLint err = eglGetError();
		if ( err == EGL_BAD_SURFACE )
		{
			// Recreate surface
			InitEGLSurface();
			return EGL_SUCCESS; // Still consider glContext is valid
		}
		else if ( err == EGL_CONTEXT_LOST || err == EGL_BAD_CONTEXT )
		{
			// Context has been lost!!
			mIsValid = false;
			Destroy();
			InitEGLContext();
		}
		return err;
	}
	return EGL_SUCCESS;
}
//---------------------------------------
bool ESContext::Invalidate()
{
	Destroy();

	mInitialized = false;
	return true;
}
//---------------------------------------
void ESContext::Suspend()
{
	if ( mWindow->surface != EGL_NO_SURFACE )
	{
		eglDestroySurface( mWindow->display, mWindow->surface );
		mWindow->surface = EGL_NO_SURFACE;
	}
}
//---------------------------------------
int ESContext::Resume( void** hWindow )
{
	if ( mInitialized == false )
	{
		Init( hWindow );
		return EGL_SUCCESS;
	}

	int32 original_widhth = mScreenWidth;
	int32 original_height = mScreenHeight;

	// Create surface
	mWindow->window = (ANativeWindow*) *hWindow;
	mWindow->surface = eglCreateWindowSurface( mWindow->display, mWindow->config, mWindow->window, NULL );
	QueryScreenSize();


	if ( eglMakeCurrent( mWindow->display, mWindow->surface, mWindow->surface, mWindow->context ) == EGL_TRUE )
	{
		if ( mScreenWidth != original_widhth || mScreenHeight != original_height )
		{
			// Screen resized
			DebugPrintf( "ESContext: Screen resized [%d,%d] -> [%d,%d]\n"
				, original_widhth, original_height
				, mScreenWidth, mScreenHeight );

			// Send resize event
			Dictionary params;
			params.Set( "width", mScreenWidth );
			params.Set( "height", mScreenHeight );
			EventManager::FireEvent( "WindowResized", params );
		}

		return EGL_SUCCESS;
	}

	EGLint err = eglGetError();
	WarnFail( "Unable to eglMakeCurrent %d", err );

	if ( err == EGL_CONTEXT_LOST )
	{
		// Recreate context
		DebugPrintf( "Re-creating egl context" );
		InitEGLContext();
	}
	else
	{
		// Recreate surface
		Destroy();
		InitEGLSurface();
		InitEGLContext();
	}

	return err;
}
//---------------------------------------
void ESContext::InitGLES()
{
	if ( !mEGLInitialized )
	{
		mVersion = 2.0f;

		// Try to initialize a ES 3.0 context
		const char* verStr = (const char*) glGetString( GL_VERSION );
		if ( strstr( verStr, "OpenGL ES 3." ) && gl3stubInit() )
		{
			mES3Supported = true;
			mVersion = 3.0f;
		}
		mEGLInitialized = true;
	}
}
//---------------------------------------
void ESContext::Destroy()
{
	if ( mWindow->display != EGL_NO_DISPLAY )
	{
		eglMakeCurrent( mWindow->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
		if ( mWindow->context != EGL_NO_CONTEXT )
		{
			eglDestroyContext( mWindow->display, mWindow->context );
		}

		if ( mWindow->surface != EGL_NO_SURFACE )
		{
			eglDestroySurface( mWindow->display, mWindow->surface );
		}
		eglTerminate( mWindow->display );
	}

	mWindow->display = EGL_NO_DISPLAY;
	mWindow->context = EGL_NO_CONTEXT;
	mWindow->surface = EGL_NO_SURFACE;
	mIsValid = false;
}
//---------------------------------------
bool ESContext::InitEGLSurface()
{
	mWindow->display = eglGetDisplay( EGL_DEFAULT_DISPLAY );
    eglInitialize( mWindow->display, 0, 0 );

    /*
     * Here specify the attributes of the desired configuration.
     * Below, we select an EGLConfig with at least 8 bits per color
     * component compatible with on-screen windows
     */
    const EGLint attribs[] =
	{
		EGL_RENDERABLE_TYPE,
        EGL_OPENGL_ES2_BIT, //Request opengl ES2.0
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_BLUE_SIZE, 8, EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8, EGL_DEPTH_SIZE, 24, EGL_NONE
	};
    mColorSize = 8;
    mDepthSize = 24;

    EGLint num_configs;
    eglChooseConfig( mWindow->display, attribs, &mWindow->config, 1, &num_configs );

    if ( !num_configs )
    {
		WarnInfo( "Falling back to 16bit depth buffer.\n" );

        //Fall back to 16bit depth buffer
        const EGLint attribs[] =
		{
			EGL_RENDERABLE_TYPE,
            EGL_OPENGL_ES2_BIT, //Request opengl ES2.0
			EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_BLUE_SIZE, 8, EGL_GREEN_SIZE, 8,
			EGL_RED_SIZE, 8, EGL_DEPTH_SIZE, 16, EGL_NONE
		};
        eglChooseConfig( mWindow->display, attribs, &mWindow->config, 1, &num_configs );
        mDepthSize = 16;
    }

    if ( !num_configs )
    {
        WarnFail( "Unable to retrieve EGL config.\n" );
        return false;
    }

    mWindow->surface = eglCreateWindowSurface( mWindow->display, mWindow->config, mWindow->window, NULL );
    QueryScreenSize();

    /* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
	 * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
	 * As soon as we picked a EGLConfig, we can safely reconfigure the
	 * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
	EGLint format;
	eglGetConfigAttrib( mWindow->display, mWindow->config, EGL_NATIVE_VISUAL_ID, &format );
	ANativeWindow_setBuffersGeometry( mWindow->window, 0, 0, format );

    return true;
}
//---------------------------------------
bool ESContext::InitEGLContext()
{
	const EGLint context_attribs[] =
	{
		EGL_CONTEXT_CLIENT_VERSION, 2, //Request opengl ES2.0
		EGL_NONE
	};
	mWindow->context = eglCreateContext( mWindow->display, mWindow->config, NULL, context_attribs );

	if( eglMakeCurrent( mWindow->display, mWindow->surface, mWindow->surface, mWindow->context ) == EGL_FALSE )
	{
		WarnFail( "Unable to eglMakeCurrent.\n" );
		return false;
	}

	mIsValid = true;
	return true;
}
//---------------------------------------
void ESContext::QueryScreenSize()
{
	eglQuerySurface( mWindow->display, mWindow->surface, EGL_WIDTH, &mScreenWidth );
	eglQuerySurface( mWindow->display, mWindow->surface, EGL_HEIGHT, &mScreenHeight );

	DebugPrintf( "ESContext: WindowSize [%d,%d]", mScreenWidth, mScreenHeight );
}
//---------------------------------------
