/*
 * Author      : Matthew Johnson
 * Date        : 15/Nov/2013
 * Description :
 *
 */

#pragma once

#include <android_native_app_glue.h>

#include <MageMath.h>
#include <MageCore.h>
#include <MageRenderer.h>

#define main() android_main( struct android_app* app )
#define MageAppInit( title ) InitApp( title, app );

namespace mage
{
	struct Pointer
	{
		Pointer();

		bool IsActivePointer() const;
		Vec2f GetDisplacement() const;

		bool hasMoved;
		bool isMoving;
		int id;
		Vec2f startPosition;
		Vec2f lastPosition;
		Vec2f position;
	};

	typedef std::map< int, Pointer > PointersByID;

	const PointersByID& GetPointers();
	const Pointer& GetActivePointer();
	size_t GetPointerCount();

	typedef void(*UpdateFn)( float );
	typedef void(*RenderFn)( void );
	typedef void(*OnSaveStateRestoredFn)( const void* );
	typedef size_t(*OnSaveStateFn)( void* );
	typedef void(*OnDestroyFn)( void );
	typedef void(*OnScreenSizeChangedFn)( int32 w, int32 h );
	typedef void(*OnWindowShownFn)( void );
	typedef void(*OnPointerDownFn)( const Pointer& pointer );
	typedef void(*OnPointerUpFn)( const Pointer& pointer );
	typedef void(*OnPointerMotionFn)( const Pointer& activePointer, const PointersByID& pointersByID );
	typedef void(*OnFocusLostFn)( void );
	typedef void(*OnFocusGainedFn)( void );
	typedef void(*OnVolumeChangedFn)( float volume );


	// Initialize the app - do this before calling Run(). If a savestate exists, you will get that callback before this function returns.
	bool InitApp( const char* title, struct android_app* app );
	// Run the app
	void Run();
	// Exit the app
	// Note: this is a request and will happen as soon as the destroy cmd is processed
	void ExitApp();

	void DefaultUpdateFn( float dt );
	void DefaultRenderFn();
	void DefaultOnSaveStateRestoredFn( const void* data );
	size_t DefaultOnSaveStateFn( void* );
	// This function is called when application destruction is requested. The rendering context is gone at this point.
	void DefaultOnDestroyFn();
	void DefaultOnScreenSizeChangedFn( int32 w, int32 h );
	void DefaultOnWindowShownFn();

	// UpdateFn is passed a fixed timestep
	void RegisterUpdateFn( UpdateFn fn );
	// RenderFn is call after UpdateFn
	void RegisterRenderFn( RenderFn fn );
	// When the application state is restored OnSaveStateRestoredFn is called with the saved data
	void RegisterOnSaveStateRestoredFn( OnSaveStateRestoredFn fn );
	// When the application state needs to be saved OnSaveStateFn is called. OnSaveStateFn must return the size of the saved state.
	void RegisterOnSaveStateFn( OnSaveStateFn fn );
	// OnDestroyFn is called when the application is exiting
	void RegisterOnDestroyFn( OnDestroyFn fn );
	void RegisterOnScreenSizeChangedFn( OnScreenSizeChangedFn fn );
	void RegisterOnWindowShownFn( OnWindowShownFn fn );
	void RegisterOnPointerDownFn( OnPointerDownFn fn );
	void RegisterOnPointerUpFn( OnPointerUpFn fn );
	void RegisterOnPointerMotionFn( OnPointerMotionFn fn );
	void RegisterOnFocusLostFn( OnFocusLostFn fn );
	void RegisterOnFocusGainedFn( OnFocusGainedFn fn );
	void RegisterOnVolumeChangedFn( OnVolumeChangedFn fn );
}
