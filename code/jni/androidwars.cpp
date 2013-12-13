#include <MageApp.h>
#include "ui/Widget.h"

#include "androidwars.h"

using namespace mage;

// Global
int32 gWindowWidth;
int32 gWindowHeight;

Vec2f gCameraTarget;
Vec2f gCameraVelocity;

Camera* gCamera;
BitmapFont* gFont;

Game* gGame;
Database* gDatabase;
SoundManager* gSoundManager;

// Test
Widget* gWidget;
Widget* gTestWidget;
SoundClip* gJumpSoundFx;
SoundClip* gDieSoundFx;


// Closes the test widget
EventFunc( TestWidgetButtonEvent )
{
	DebugPrintf( "The Test Button Was Pressed" );
	// Widget events always have a pointer to themselves in the params as Widget*
	// Use the RTTI to see what kind of widget it is
	Widget* w = params.Get( "Widget", (Widget*)0 );
	// Hide the entire widget set
	w->GetRootWidget()->Hide();
	// You could also destroy the widget but this will
	// cause the widget to be constructed from disk
	// when we need to show it again
	//Widget::DestroyWidget( gWidget );

	// Play a sfx - this is done through data now
//	gSoundManager->PlaySound( gDieSoundFx );
}

// Show the test widget
EventFunc( ShowTestWidgetEvent )
{
	// Make widget if does not exist
	//if ( !gWidget )
	//{
	//	gWidget = Widget::LoadWidget( "ui/test.xml" );
	//}
	// Show the widget
	//gWidget->Show();

	// TEST: End the current turn.
	gGame->NextTurn();

	// Play a sfx - this is done through data now
//	gSoundManager->PlaySound( gJumpSoundFx );
}

void RegisterEventFuncs()
{
	RegisterEventFunc( TestWidgetButtonEvent );
	RegisterEventFunc( ShowTestWidgetEvent );
}

void OnDraw()
{
	static int sTestCount = 0;
	ClearScreen();

	// Draw the game.
	if ( gGame )
		gGame->OnDraw();
	// Draw the UI
	Widget::DrawAllWidgets( *gCamera );

	// Camera debug
//	DrawRect( gCameraTarget.x - 5, gCameraTarget.y - 5, 10, 10, Color::PINK );
	FlushRenderer();
}

void OnUpdate( float dt )
{
	// Update the game here...
	Widget::UpdateAllWidgets( dt );

	if ( gGame )
		gGame->OnUpdate( dt );

	// Camera test
	gCameraTarget += gCameraVelocity;
	gCameraVelocity *= 0.9f;
	if ( gCamera )
		gCamera->LookAtClamp( gCameraTarget );
}

void OnScreenSizeChanged( int32 w, int32 h )
{
	gWindowWidth = w;
	gWindowHeight = h;

	gCameraTarget.x = w / 2.0f;
	gCameraTarget.y = h / 2.0f;
}

void OnWindowShown()
{
	// The window is shown
	// Now is ok to load graphics

	// Load Widget definitions
	Widget::LoadDefinitions( "ui/definitions.xml" );

	gTestWidget = Widget::LoadWidget( "ui/show_test.xml" );

	// Create a camera
	gCamera = new Camera( gWindowWidth, gWindowHeight );
//	gFont = new BitmapFont( "fonts/font.fnt" );

	// Create the global Database.
	DebugPrintf( "Creating database..." );
	gDatabase = new Database();

	// Load all data into the Database.
	DebugPrintf( "Loading game data..." );
	gDatabase->LoadGameData();

	// Create a new Game and start it.
	gGame = Game::Create( 2, "Cobra Cove" );
	gGame->SetCamera( gCamera );
	gGame->Start();
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
	Widget::ProcessOnPointerDown( x, y );
}

void OnPointerUp( float x, float y, size_t which )
{
	if ( !Widget::ProcessOnPointerUp( x, y ) )
	{
		if ( gGame )
			gGame->OnTouchEvent( x, y );
	}
}

void OnPointerMotion( float x, float y, float dx, float dy, size_t which )
{
//	DebugPrintf( "Motion (%3.f %.3f) d(%3.f %.3f) ", x, y, dx, dy );
	if ( !Widget::ProcessOnPointerDown( x, y ) )
		gCameraVelocity.Set( dx, dy );
}

void OnFocusLost()
{
	if ( gSoundManager )
		gSoundManager->Stop();
}

void OnFocusGained()
{
	if ( gSoundManager )
		gSoundManager->Start();
}

void OnVolumeChanged( float volume )
{
	if ( gSoundManager )
		gSoundManager->SetVolume( volume );
}

void main()
{
	// Initialize the application
	MageAppInit( "Test" );
	// Do user initialization here

	// Sounds can either be .wav or .pcm files, but they must be saved as mono with a sampling rate of 44,100 Hz.
	gSoundManager = new SoundManager( app );
//	gJumpSoundFx = gSoundManager->LoadSoundClip( "sfx/super_mario_jump.wav", "jump.sfx" );
//	gDieSoundFx = gSoundManager->LoadSoundClip( "sfx/super_mario_die.wav", "die.sfx" );
	gSoundManager->Start();
	// Starts background music.
//	gSoundManager->PlayMusic( "music/super_mario_overworld.mp3" );

	RegisterRenderFn( OnDraw );
	RegisterUpdateFn( OnUpdate );
	RegisterOnWindowShownFn( OnWindowShown );
	RegisterOnScreenSizeChangedFn( OnScreenSizeChanged );
	RegisterOnSaveStateFn( OnSaveState );
	RegisterOnSaveStateRestoredFn( OnSaveStateRestore );
	RegisterOnPointerUpFn( OnPointerUp );
	RegisterOnPointerDownFn( OnPointerDown );
	RegisterOnPointerMotionFn( OnPointerMotion );
	RegisterOnFocusLostFn( OnFocusLost );
	RegisterOnFocusGainedFn( OnFocusGained );
	RegisterOnVolumeChangedFn( OnVolumeChanged );

	RegisterEventFuncs();

	// Run the application
	Run();

	gSoundManager->Stop();
}
