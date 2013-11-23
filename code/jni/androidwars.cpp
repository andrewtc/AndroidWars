#include <MageApp.h>

#include "ui/Widget.h"

using namespace mage;

// Global
int32 gWindowWidth;
int32 gWindowHeight;

RectF gRect( 50, 50, 200, 200 );
Color gColor( 0xffff0000 );

Camera* gCamera;
Widget* gWidget;
BitmapFont* gFont;

EventFunc( TestWidgetButtonEvent )
{
	DebugPrintf( "The Test Button Was Pressed" );
	gColor = Color::ORANGE;
}

void OnDraw()
{
	static int sTestCount = 0;
	ClearScreen();

	DrawRect( 50, 50, 150, 150, gColor);

	gWidget->OnDraw( *gCamera );
	DrawText( 150, 20, gFont, "Hello World!\nThis is some testing text." );
	DrawTextFormat( 150, 20 + 2 * gFont->GetLineHeight(), gFont, "Counter: %d", sTestCount++ );

	FlushRenderer();
}

void OnUpdate( float dt )
{
	// Update the game here...
	if ( gWidget )
		gWidget->OnUpdate( dt );
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

	// TODO this needs to be pre-loaded someplace better...
	SpriteManager::LoadSpriteAnimations( "ui/button.anim" );
	SpriteManager::LoadSpriteAnimations( "ui/background.anim" );

	gWidget = new Widget( "ui/test.xml" );
	gCamera = new Camera( gWindowWidth, gWindowHeight );
	gFont = new BitmapFont( "fonts/font.fnt" );
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

	if ( gWidget )
		gWidget->OnClick( x, y );
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

	RegisterEventFunc( TestWidgetButtonEvent );

	// Run the application
	Run();
}
