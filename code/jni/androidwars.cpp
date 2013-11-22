#include <MageApp.h>

using namespace mage;

// Global
int32 gWindowWidth;
int32 gWindowHeight;

RectF gRect( 50, 50, 200, 200 );
Color gColor = Color::RED;

void OnDraw()
{
	ClearScreen();

	DrawRect( 50, 50, 150, 150, gColor);

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


void OnPointerDown( float x, float y, size_t which )
{
	DebugPrintf( "Touch at %f %f\n", x, y );
	if ( gRect.Contains( x, y ) )
		gColor = Color::GREEN;
	else
		gColor = Color::RED;

}

void OnPointerUp( float x, float y, size_t which )
{

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
	RegisterOnPointerUpFn( OnPointerUp );
	RegisterOnPointerDownFn( OnPointerDown );
	// Run the application
	Run();
}
