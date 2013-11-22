#include <MageApp.h>

using namespace mage;

// Global
int32 gWindowWidth;
int32 gWindowHeight;

void OnDraw()
{
	ClearScreen();

	DrawRect( 50, 50, 150, 150, Color::GREEN );

	FlushRenderer();
}

void OnUpdate( float dt )
{
	// Update the game here...
}

void OnScreenSizeChanged( int32 w, int32 h )
{
	gWindowWidth = w;
	gWindowHeight = h;
}

void OnWindowShown()
{
	// The window is shown
	// Now is ok to load graphics
}

size_t OnSaveState( void* state )
{
	// Save the sate here...
	return 0;
}

void OnSaveStateRestore( const void* state )
{
	// Game was loaded from a saved state...
}

void main()
{
	// Initialize the application
	MageAppInit( "Test" );
	// Do user initialization here
	RegisterRenderFn( OnDraw );
	RegisterUpdateFn( OnUpdate );
	RegisterOnWindowShownFn( OnWindowShown );
	RegisterOnScreenSizeChangedFn( OnScreenSizeChanged );
	RegisterOnSaveStateFn( OnSaveState );
	RegisterOnSaveStateRestoredFn( OnSaveStateRestore );
	// Run the application
	Run();
}
