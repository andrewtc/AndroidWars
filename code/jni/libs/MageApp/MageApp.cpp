#include "MageApp.h"

#include <EGL/egl.h>
#include <GLES/gl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

namespace mage
{

	//---------------------------------------
	struct Engine
	{
		android_app* app;
		int32 width, height;
		float volume;

		EGLDisplay display;
		EGLSurface surface;
		EGLContext context;
	};
	static Engine gEngine;

	static bool gIsRunning;
	static bool gIsPaused;
	static bool gNeedTextureReload = false;
	static Clock* gMasterClock;

	// Touch stuff
	static const float POINTER_INITIAL_MOTION_TOLERANCE = 5.0f;
	static const float POINTER_MOTION_TOLERANCE = 0.5f;
	static const int INVALID_POINTER_ID = -1;
	static int gActivePointerID = INVALID_POINTER_ID;
	static bool gPointerMoved = false;
	static std::map< int, Pointer > gPointersByID;

	static UpdateFn gUpdateFn = DefaultUpdateFn;
	static RenderFn gRenderFn = DefaultRenderFn;
	static OnSaveStateRestoredFn gOnSaveStateRestoredFn = DefaultOnSaveStateRestoredFn;
	static OnSaveStateFn gOnSaveStateFn = DefaultOnSaveStateFn;
	static OnDestroyFn gOnDestroyFn = DefaultOnDestroyFn;
	static OnScreenSizeChangedFn gOnScreenSizeChanged = DefaultOnScreenSizeChangedFn;
	static OnWindowShownFn gOnWindowShownFn = DefaultOnWindowShownFn;
	static OnPointerDownFn gOnPointerDown = 0;
	static OnPointerUpFn gOnPointerUp = 0;
	static OnPointerMotionFn gOnPointerMotionFn = 0;
	static OnFocusLostFn gOnFocusLostFn = 0;
	static OnFocusGainedFn gOnFocusGainedFn = 0;
	static OnVolumeChangedFn gOnVolumeChangedFn = 0;

	static Vec2f getPointerPositionFromEvent( const AInputEvent* event, size_t pointerIndex );
	static int getPointerIDFromEvent( const AInputEvent* event, size_t pointerIndex );
	static void processPointerDown( const AInputEvent* event, size_t pointerIndex );
	static void updatePointers( const AInputEvent* event );
	static void resetPointers();
	static Pointer& getPointerByID( int pointerID );
	static void processPointerUp( const AInputEvent* event, size_t pointerIndex );

	static void handleAppCmd( struct android_app* app, int32_t cmd );
	static int32_t handleInputEvent( struct android_app* app, AInputEvent* event );

	static void OnDraw();

	static int initGL();
	static void shutdownGL();
	static void printGLString( const char *name, GLenum s );
	//---------------------------------------


	//---------------------------------------
	bool InitApp( const char* title, struct android_app* app )
	{
		// Make sure glue isn't stripped.
		app_dummy();

		DebugPrintf( "Initializing Application\n" );

		memset( &gEngine, 0, sizeof(gEngine) );
		gEngine.app = app;
		app->userData = &gEngine;
		app->onAppCmd = handleAppCmd;
		app->onInputEvent = handleInputEvent;
		gEngine.volume = 0.1f;

		DebugPrintf( "Registering asset manager\n" );

		InitializeAssetManager( app->activity->assetManager );

		DebugPrintf( "Creating clock\n" );

		gMasterClock = &Clock::Initialize();

		if( app->savedState != NULL )
		{
			// We are starting with a previous saved state; restore from it.
			DebugPrintf( "Restoring previous savestate\n" );
			gOnSaveStateRestoredFn( app->savedState );
		}

		DebugPrintf( "Init complete\n" );

		return true;
	}
	//---------------------------------------
	void Run()
	{
		gIsRunning = true;
		const float dt = 1.0f / 60.0f;

		while( gIsRunning )
		{
			// Read all pending events.
			int ident;
			int events;
			struct android_poll_source* source;

			while( (ident = ALooper_pollAll( 0, NULL, &events, (void**)&source ) ) >= 0 )
			{
				// Process this event.
				if( source != NULL )
					source->process( gEngine.app, source );

				// Check if we are exiting.
				if( gEngine.app->destroyRequested != 0 )
				{
					DebugPrintf( "Shuting Down\n" );
					shutdownGL();
					gOnDestroyFn();
					return;
				}
			}

			// Reset the velocity of each Pointer.
			resetPointers();

			// Only do stuff if not paused
			if ( !gIsPaused )
			{
				gMasterClock->AdvanceTime( dt );
				gUpdateFn( dt );
				OnDraw();
			}
		}
	}
	//---------------------------------------
	void ExitApp()
	{
		ANativeActivity_finish( gEngine.app->activity );
	}
	//---------------------------------------
	// Default Callbacks
	//---------------------------------------
	void DefaultUpdateFn( float dt )
	{}
	//---------------------------------------
	void DefaultRenderFn()
	{
		ClearScreen();
	}
	//---------------------------------------
	void DefaultOnSaveStateRestoredFn( const void* data )
	{}
	//---------------------------------------
	size_t DefaultOnSaveStateFn( void* data )
	{
		return 0;
	}
	//---------------------------------------
	void DefaultOnDestroyFn()
	{}
	//---------------------------------------
	void DefaultOnScreenSizeChangedFn( int32 w, int32 h )
	{}
	//---------------------------------------
	void DefaultOnWindowShownFn()
	{}
	//---------------------------------------
	// Callback registers
	//---------------------------------------
	void RegisterUpdateFn( UpdateFn fn )
	{
		gUpdateFn = fn;
	}
	//---------------------------------------
	void RegisterRenderFn( RenderFn fn )
	{
		gRenderFn = fn;
	}
	//---------------------------------------
	void RegisterOnSaveStateRestoredFn( OnSaveStateRestoredFn fn )
	{
		gOnSaveStateRestoredFn = fn;
	}
	//---------------------------------------
	void RegisterOnSaveStateFn( OnSaveStateFn fn )
	{
		gOnSaveStateFn = fn;
	}
	//---------------------------------------
	void RegisterOnDestroyFn( OnDestroyFn fn )
	{
		gOnDestroyFn = fn;
	}
	//---------------------------------------
	void RegisterOnScreenSizeChangedFn( OnScreenSizeChangedFn fn )
	{
		gOnScreenSizeChanged = fn;
	}
	//---------------------------------------
	void RegisterOnWindowShownFn( OnWindowShownFn fn )
	{
		gOnWindowShownFn = fn;
	}
	//---------------------------------------
	void RegisterOnPointerDownFn( OnPointerDownFn fn )
	{
		gOnPointerDown = fn;
	}
	//---------------------------------------
	void RegisterOnPointerUpFn( OnPointerUpFn fn )
	{
		gOnPointerUp = fn;
	}
	//---------------------------------------
	void RegisterOnPointerMotionFn( OnPointerMotionFn fn )
	{
		gOnPointerMotionFn = fn;
	}
	//---------------------------------------
	void RegisterOnFocusLostFn( OnFocusLostFn fn )
	{
		gOnFocusLostFn = fn;
	}
	//---------------------------------------
	void RegisterOnFocusGainedFn( OnFocusGainedFn fn )
	{
		gOnFocusGainedFn = fn;
	}
	//---------------------------------------
	void RegisterOnVolumeChangedFn( OnVolumeChangedFn fn )
	{
		gOnVolumeChangedFn = fn;
	}
	//---------------------------------------
	// Input handling
	//---------------------------------------
	Pointer::Pointer() :
		id( INVALID_POINTER_ID ),
		hasMoved( false ),
		isMoving( false )
	{ }
	//---------------------------------------
	bool Pointer::IsActivePointer() const
	{
		return ( id == gActivePointerID );
	}
	//---------------------------------------
	const std::map< int, Pointer >& GetPointers()
	{
		return gPointersByID;
	}
	//---------------------------------------
	static Vec2f getPointerPositionFromEvent( const AInputEvent* event, size_t pointerIndex )
	{
		return Vec2f( AMotionEvent_getX( event, pointerIndex ),
					  AMotionEvent_getY( event, pointerIndex ) );
	}
	//---------------------------------------
	static int getPointerIDFromEvent( const AInputEvent* event, size_t pointerIndex )
	{
		return AMotionEvent_getPointerId( event, pointerIndex );
	}
	//---------------------------------------
	static void processPointerDown( const AInputEvent* event, size_t pointerIndex )
	{
		// Keep track of the new pointer.
		int pointerID = getPointerIDFromEvent( event, pointerIndex );
		Pointer& pointer = gPointersByID[ pointerID ];

		// Determine the initial Pointer position from the event.
		Vec2f position = getPointerPositionFromEvent( event, pointerIndex );

		pointer.id = pointerID;
		pointer.startPosition = position;
		pointer.lastPosition = position;
		pointer.position = position;

		if( gActivePointerID == INVALID_POINTER_ID )
		{
			// If this is the first pointer down, make this the primary pointer.
			gActivePointerID = pointerID;
		}

		if( gOnPointerDown )
		{
			// Let the app handle the pointer down event.
			gOnPointerDown( pointer );
		}
	}
	//---------------------------------------
	static void updatePointers( const AInputEvent* event )
	{
		// Get the number of pointers.
		size_t pointerCount = AMotionEvent_getPointerCount( event );

		for( size_t pointerIndex = 0; pointerIndex < pointerCount; ++pointerIndex )
		{
			// Fetch each Pointer.
			int pointerID = getPointerIDFromEvent( event, pointerIndex );
			Pointer& pointer = getPointerByID( pointerID );

			// Determine how far the Pointer moved.
			pointer.position = getPointerPositionFromEvent( event, pointerIndex );

			if( !pointer.hasMoved )
			{
				// If the pointer has moved from its original location, mark that it has moved from its original position.
				Vec2f initialDisplacement = ( pointer.position - pointer.startPosition );
				pointer.hasMoved = ( initialDisplacement.LengthSqr() >= POINTER_INITIAL_MOTION_TOLERANCE );
			}

			if( pointer.hasMoved )
			{
				// Get the actual distance the Pointer travelled this frame.
				Vec2f displacement = ( pointer.position - pointer.lastPosition );

				if( displacement.LengthSqr() >= POINTER_MOTION_TOLERANCE )
				{
					// If the Pointer moved, mark that it moved.
					gPointerMoved = true;
					pointer.isMoving = true;
				}
			}
		}

		if( gPointerMoved && gOnPointerMotionFn )
		{
			// Let the app handle the move event.
			gOnPointerMotionFn();
		}
	}
	//---------------------------------------
	static void resetPointers()
	{
		for( auto it = gPointersByID.begin(); it != gPointersByID.end(); ++it )
		{
			// Update the last position of the Pointer.
			Pointer& pointer = it->second;
			pointer.lastPosition = pointer.position;
		}
	}
	//---------------------------------------
	static Pointer& getPointerByID( int pointerID )
	{
		auto it = gPointersByID.find( pointerID );
		assertion( it != gPointersByID.end(), "Could not find Pointer data for invalid ID %d!", pointerID );
		return it->second;
	}
	//---------------------------------------
	static void processPointerUp( const AInputEvent* event, size_t pointerIndex )
	{
		// Get the Pointer ID from its index.
		int pointerID = getPointerIDFromEvent( event, pointerIndex );

		// Look up the Pointer by its ID.
		auto it = gPointersByID.find( pointerID );
		assertion( it != gPointersByID.end(), "Could not remove Pointer data for invalid ID %d!", pointerID );
		Pointer& pointer = it->second;

		if( pointer.IsActivePointer() )
		{
			// If the Pointer being removed is the active one, choose a new active Pointer.
			gActivePointerID = INVALID_POINTER_ID;

			for( auto it = gPointersByID.begin(); it != gPointersByID.end(); ++it )
			{
				if( it->first != pointer.id )
				{
					// Choose the first available Pointer as the new active Pointer.
					gActivePointerID = it->first;
					break;
				}
			}
		}

		if( gOnPointerUp )
		{
			// Let the app handle the pointer up event.
			gOnPointerUp( pointer );
		}

		// Remove the Pointer.
		gPointersByID.erase( it );
	}
	//---------------------------------------
	void handleAppCmd( struct android_app* app, int32_t cmd )
	{
		Engine* engine = (Engine*) app->userData;

		switch( cmd )
		{
			case APP_CMD_SAVE_STATE:
			{
				DebugPrintf( "Saving state\n" );
				// The system has asked us to save our current state.  Do so.
				engine->app->savedStateSize = gOnSaveStateFn( engine->app->savedState );
			}
			break;

			case APP_CMD_INIT_WINDOW:
			{
				// The window is being shown, get it ready.
				if( engine->app->window != NULL )
				{
					DebugPrintf( "Showing window\n" );

					// Init graphics
					initGL();
					// OnWindowShownFn requires GL is init first
					gOnWindowShownFn();
					if ( gOnVolumeChangedFn )
						gOnVolumeChangedFn( gEngine.volume );
				}
			}
			break;

			case APP_CMD_TERM_WINDOW:
			{
				// The window is being hidden or closed, clean it up.
				DebugPrintf( "Hiding window\n" );
				shutdownGL();
			}
			break;

			case APP_CMD_GAINED_FOCUS:
			{
				DebugPrintf( "Focus gained...\n" );
				if ( gNeedTextureReload )
					Texture2D::ReloadAllTextures();
				if ( gOnFocusGainedFn )
					gOnFocusGainedFn();
			}
			break;

			case APP_CMD_LOST_FOCUS:
			{
				DebugPrintf( "Focus lost...\n" );
				if ( gOnFocusLostFn )
					gOnFocusLostFn();
				gNeedTextureReload = true;
				shutdownGL();
			}
			break;

			case APP_CMD_PAUSE:
			{
				DebugPrintf( "Pausing Activity...\n" );
				gIsPaused = true;
			}
			break;

			case APP_CMD_RESUME:
			{
				DebugPrintf( "Resuming Activity...\n" );
				gIsPaused = false;
			}
			break;
		}
	}
	//---------------------------------------
	int32_t handleInputEvent( struct android_app* app, AInputEvent* pEvent )
	{
	//    LOGI("AMotionEvent_getAction=%d", AMotionEvent_getAction(pEvent));
	//    LOGI("AMotionEvent_getFlags=%d", AMotionEvent_getFlags(pEvent));
	//    LOGI("AMotionEvent_getMetaState=%d", AMotionEvent_getMetaState(pEvent));
	//    LOGI("AMotionEvent_getEdgeFlags=%d", AMotionEvent_getEdgeFlags(pEvent));
	//    LOGI("AMotionEvent_getDownTime=%lld", AMotionEvent_getDownTime(pEvent));
	//    LOGI("AMotionEvent_getEventTime=%lld", AMotionEvent_getEventTime(pEvent));
	//    LOGI("AMotionEvent_getXOffset=%f", AMotionEvent_getXOffset(pEvent));
	//    LOGI("AMotionEvent_getYOffset=%f", AMotionEvent_getYOffset(pEvent));
	//    LOGI("AMotionEvent_getXPrecision=%f", AMotionEvent_getXPrecision(pEvent));
	//    LOGI("AMotionEvent_getYPrecision=%f", AMotionEvent_getYPrecision(pEvent));
	//    LOGI("AMotionEvent_getPointerCount=%d", AMotionEvent_getPointerCount(pEvent));
	//    LOGI("AMotionEvent_getRawX=%f", AMotionEvent_getRawX(pEvent, 0));
	//    LOGI("AMotionEvent_getRawY=%f", AMotionEvent_getRawY(pEvent, 0));
	//    LOGI("AMotionEvent_getX=%f", AMotionEvent_getX(pEvent, 0));
	//    LOGI("AMotionEvent_getY=%f", AMotionEvent_getY(pEvent, 0));
	//    LOGI("AMotionEvent_getPressure=%f", AMotionEvent_getPressure(pEvent, 0));
	//    LOGI("AMotionEvent_getSize=%f", AMotionEvent_getSize(pEvent, 0));
	//    LOGI("AMotionEvent_getOrientation=%f", AMotionEvent_getOrientation(pEvent, 0));
	//    LOGI("AMotionEvent_getTouchMajor=%f", AMotionEvent_getTouchMajor(pEvent, 0));
	//    LOGI("AMotionEvent_getTouchMinor=%f", AMotionEvent_getTouchMinor(pEvent, 0));

		Engine* engine = (Engine*) app->userData;

		int sourceId = AInputEvent_getSource( pEvent );
		bool wasHandled = false;

		if ( sourceId == AINPUT_SOURCE_TOUCHPAD )
		{
			// Gamepad or something
			wasHandled = true;
		}
		else if ( sourceId == AINPUT_SOURCE_TOUCHSCREEN )
		{
			// Touch event
			if( AInputEvent_getType( pEvent ) == AINPUT_EVENT_TYPE_MOTION )
			{
				const int action = AMotionEvent_getAction( pEvent );

				// Get the pointer index.
				const size_t pointerIndex = ( action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK ) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;

				wasHandled = true;

				switch ( action & AMOTION_EVENT_ACTION_MASK )
				{
					case AMOTION_EVENT_ACTION_DOWN:
						processPointerDown( pEvent, pointerIndex );
						break;

					case AMOTION_EVENT_ACTION_POINTER_DOWN:
						processPointerDown( pEvent, pointerIndex );
						break;

					case AMOTION_EVENT_ACTION_UP:
						processPointerUp( pEvent, pointerIndex );
						break;

					case AMOTION_EVENT_ACTION_POINTER_UP:
						processPointerUp( pEvent, pointerIndex );
						break;

					case AMOTION_EVENT_ACTION_MOVE:
						updatePointers( pEvent );
						break;

					case AMOTION_EVENT_ACTION_CANCEL:
						// TODO
						break;

					default:
						// Don't handle the event.
						wasHandled = false;
						break;
				}
			}


			/*if( AInputEvent_getType( pEvent ) == AINPUT_EVENT_TYPE_MOTION )
			{
				size_t pointerCount = AMotionEvent_getPointerCount( pEvent );

				for( size_t i = 0; i < pointerCount; ++i )
				{
					//DebugPrintf( "Received motion event from pointer %zu: (%.1f, %.1f)",
					//      i, AMotionEvent_getX( pEvent, i ), AMotionEvent_getY( pEvent, i ) );
				}
				return 1;
			}
			else if( AInputEvent_getType( pEvent ) == AINPUT_EVENT_TYPE_KEY )
			{
				DebugPrintf( "Received key event: %d", AKeyEvent_getKeyCode( pEvent ) );

				return 1;
			}*/
		}
		else
		{
			// Keypress
			if( AInputEvent_getType( pEvent ) == AINPUT_EVENT_TYPE_KEY )
			{
				int32_t code = AKeyEvent_getKeyCode( pEvent );
				DebugPrintf( "Received key event: %d", code );

				bool volumeChanged = false;

				if ( code == 25 )
				{
					engine->volume -= 0.05f;
					Mathf::ClampToRange( gEngine.volume, 0.0f, 1.0f );
					volumeChanged = true;
				}
				if ( code == 24 )
				{
					engine->volume += 0.05f;
					Mathf::ClampToRange( gEngine.volume, 0.0f, 1.0f );
					volumeChanged = true;
				}

				if ( volumeChanged )
				{
					DebugPrintf( "Volume changed: %.3f\n", engine->volume );
					if ( gOnVolumeChangedFn )
						gOnVolumeChangedFn( gEngine.volume );
				}

				wasHandled = false;
			}
		}

		// System event
		return wasHandled;
	}
	//---------------------------------------
	void OnDraw()
	{
		if( gEngine.display == NULL )
		{
			// No display.
			return;
		}

		gRenderFn();

		eglSwapBuffers( gEngine.display, gEngine.surface );
	}
	//---------------------------------------
	// OpenGL Initialization
	int initGL()
	{
		// initialize OpenGL ES and EGL

		//
		// Here specify the attributes of the desired configuration.
		// Below, we select an EGLConfig with at least 8 bits per color
		// that supports OpenGL ES 2.0.
		//

		const EGLint attribs[] =
		{
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
			EGL_BLUE_SIZE, 8,
			EGL_GREEN_SIZE, 8,
			EGL_RED_SIZE, 8,
			EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
			EGL_NONE
		};

		EGLint w, h, dummy, format;
		EGLint numConfigs;
		EGLConfig config;
		EGLSurface surface;
		EGLContext context;

		EGLDisplay display = eglGetDisplay( EGL_DEFAULT_DISPLAY );

		eglInitialize( display, 0, 0 );

		//
		// Here, the application chooses the configuration it desires. In this
		// sample, we have a very simplified selection process, where we pick
		// the first EGLConfig that matches our criteria.
		//

		eglChooseConfig( display, attribs, &config, 1, &numConfigs );

		//
		// EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
		// guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
		// As soon as we picked a EGLConfig, we can safely reconfigure the
		// ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID.
		//

		eglGetConfigAttrib( display, config, EGL_NATIVE_VISUAL_ID, &format );

		ANativeWindow_setBuffersGeometry( gEngine.app->window, 0, 0, format );

		surface = eglCreateWindowSurface( display, config, gEngine.app->window, NULL );

		const EGLint attrib_list [] =
		{
			EGL_CONTEXT_CLIENT_VERSION, 2,
			EGL_NONE
		};

		context = eglCreateContext( display, config, NULL, attrib_list );

		if( eglMakeCurrent( display, surface, surface, context ) == EGL_FALSE )
		{
			WarnCrit( "Unable to eglMakeCurrent" );
			return -1;
		}

		eglQuerySurface( display, surface, EGL_WIDTH, &w );
		eglQuerySurface( display, surface, EGL_HEIGHT, &h );

		gEngine.display = display;
		gEngine.context = context;
		gEngine.surface = surface;
		gEngine.width = w;
		gEngine.height = h;

		// Initialize GL state.
		//glEnable( GL_DEPTH_TEST );
		//glEnable( GL_CULL_FACE );
		glFrontFace( GL_CCW );

		printGLString( "Version", GL_VERSION );
		printGLString( "Vendor", GL_VENDOR );
		printGLString( "Renderer", GL_RENDERER );
		//printGLString( "Extensions", GL_EXTENSIONS );

		if ( !CreateRenderer() )
		{
			FatalError( "Failed to create Renderer\n" );
			return -1;
		}

		SetViewport( 0, 0, w, h );
		SetOrthoView( 0, w, h, 0, 0, 1 );

		gOnScreenSizeChanged( w, h );

		return 0;
	}
	//---------------------------------------
	// OpenGL shutdown
	void shutdownGL()
	{
		if( gEngine.display != EGL_NO_DISPLAY )
		{
			eglMakeCurrent( gEngine.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );

			if( gEngine.context != EGL_NO_CONTEXT )
				eglDestroyContext( gEngine.display, gEngine.context );

			if( gEngine.surface != EGL_NO_SURFACE )
				eglDestroySurface( gEngine.display, gEngine.surface );

			eglTerminate( gEngine.display );
		}

		gEngine.display = EGL_NO_DISPLAY;
		gEngine.context = EGL_NO_CONTEXT;
		gEngine.surface = EGL_NO_SURFACE;
	}
	//---------------------------------------
	void printGLString( const char *name, GLenum s )
	{
		const char *v = (const char *) glGetString( s );
		DebugPrintf( "GL %s = %s\n", name, v );
	}
	//---------------------------------------


}
