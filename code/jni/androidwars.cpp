#include <MageApp.h>
#include "ui/Widget.h"

#include "androidwars.h"

using namespace mage;

// Global
int32 gWindowWidth;
int32 gWindowHeight;

Vec2f gCameraTarget;
Vec2f gCameraVelocity;

bool gInit = false;
bool gWasMotion = false;

Camera* gCamera = nullptr;
BitmapFont* gFont = nullptr;

Game* gGame = nullptr;
Database* gDatabase = nullptr;
GameStateManager* gGameStateManager = nullptr;
SoundManager* gSoundManager = nullptr;
OnlineGameClient* gOnlineGameClient = nullptr;

// Test
Widget* gWidget = nullptr;
Widget* gTestWidget = nullptr;
SoundClip* gJumpSoundFx = nullptr;
SoundClip* gDieSoundFx = nullptr;


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
EventFunc( NextTurnEvent )
{
	// Make widget if does not exist
	//if ( !gWidget )
	//{
	//	gWidget = Widget::LoadWidget( "ui/test.xml" );
	//}
	// Show the widget
	//gWidget->Show();

	// TEST: End the current turn.
	gGame->PostMessage( "Next Turn" );
	gGame->NextTurn();

	// Play a sfx - this is done through data now
//	gSoundManager->PlaySound( gJumpSoundFx );
}

EventFunc( RestartGameEvent )
{
	if ( gGame )
	{
		delete gGame;
	}
	gGame = Game::Create( 2, "Cobra Cove" );
	gGame->SetCamera( gCamera );
	gGame->Start();
}

EventFunc( ExitGameEvent )
{
	ExitApp();
}

void RegisterEventFuncs()
{
	RegisterEventFunc( NextTurnEvent );
	RegisterEventFunc( RestartGameEvent );
	RegisterEventFunc( ExitGameEvent );
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

	// Poll for online request responses.
	gOnlineGameClient->update();

	// TODO: Move this to a game state.
	if ( gGame )
		gGame->OnUpdate( dt );

	// Camera test
	gCameraTarget += gCameraVelocity;
	gCameraVelocity *= 0.9f;
	if ( gCamera )
		gCamera->LookAtClamp( gCameraTarget );

	// Update the current GameState.
	gGameStateManager->Update( dt );
}

void OnScreenSizeChanged( int32 w, int32 h )
{
	gWindowWidth = w;
	gWindowHeight = h;

	gCameraTarget.x = w / 2.0f;
	gCameraTarget.y = h / 2.0f;

	gGameStateManager->OnScreenSizeChanged( w, h );

	DebugPrintf( "Window size w=%d h=%d\n", gWindowWidth, gWindowHeight );
}


void OnHelloWorldSuccess( long requestID, int statusCode, const std::string& response )
{
	DebugPrintf( "The online game server says: \"%s\"", response.c_str() );
}


void OnHelloWorldComplete( long requestID, int statusCode, const std::string& response )
{
	DebugPrintf( "RESPONSE %d: Status Code %d : \"%s\"", requestID, statusCode, response.c_str() );
}


void OnWindowShown()
{
	// The window is shown
	// Now is ok to load graphics

	if ( !gInit )
	{
		// TODO: Move this into a GameState class.
		// Load Widget definitions
		Widget::LoadDefinitions( "ui/definitions.xml" );

		gTestWidget = Widget::LoadWidget( "ui/next_turn.xml" );

		if( !gCamera )
		{
			// Create a camera
			gCamera = new Camera( gWindowWidth, gWindowHeight );
		//	gFont = new BitmapFont( "fonts/font.fnt" );
		}

		if( !gDatabase )
		{
			// Create the global Database.
			DebugPrintf( "Creating database..." );
			gDatabase = new Database();

			// Load all data into the Database.
			DebugPrintf( "Loading game data..." );
			gDatabase->LoadGameData();
		}

		if( !gGame )
		{
			// Create a new Game and start it.
			gGame = Game::Create( 2, "Cobra Cove" );
			gGame->SetCamera( gCamera );
			gGame->Start();
		}

		// Test cloud function.
		gOnlineGameClient->callCloudFunction( "hello", "{\"name\": \"Andrew\"}", &OnHelloWorldSuccess, nullptr, &OnHelloWorldComplete );
	}

	gInit = true;
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
	gWasMotion = false;

	gGameStateManager->OnPointerDown( x, y, which );
}

void OnPointerUp( float x, float y, size_t which )
{
	if ( !Widget::ProcessOnPointerUp( x, y ) )
	{
		if ( gGame && !gWasMotion )
			gGame->OnTouchEvent( x, y );
	}
	gWasMotion = false;

	gGameStateManager->OnPointerUp( x, y, which );
}

void OnPointerMotion( float x, float y, float dx, float dy, size_t which )
{
//	DebugPrintf( "Motion (%3.f %.3f) d(%3.f %.3f) ", x, y, dx, dy );
	if ( !Widget::ProcessOnPointerDown( x, y ) )
	{
		gCameraVelocity.Set( dx, dy );
	}
	Vec2f d( dx, dy );
//	DebugPrintf( "motion delta=%f", d.LengthSqr() );
	if ( d.LengthSqr() > 5 )
	{
		gWasMotion = true;
	}

	gGameStateManager->OnPointerMotion( x, y, dx, dy, which );
}

void OnFocusLost()
{
	if ( gSoundManager )
		gSoundManager->Stop();
}

void OnFocusGained()
{
	/*if ( gSoundManager )
		gSoundManager->Start();*/
}

void OnVolumeChanged( float volume )
{
	/*if ( gSoundManager )
		gSoundManager->SetVolume( volume );*/
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

	// Create OnlineGameClient.
	gOnlineGameClient = new OnlineGameClient();
	gOnlineGameClient->init( app );

	// Create the GameStateManager and create the first state.
	gGameStateManager = new GameStateManager();
	gGameStateManager->ChangeState< MainMenuState >();

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
