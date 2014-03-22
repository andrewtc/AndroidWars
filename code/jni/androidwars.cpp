#include <MageApp.h>

#include "androidwars.h"

using namespace mage;

// Global
int32 gWindowWidth;
int32 gWindowHeight;

bool gInit = false;

BitmapFont* gFont = nullptr;

GameStateManager* gGameStateManager = nullptr;
WidgetManager* gWidgetManager = nullptr;
SoundManager* gSoundManager = nullptr;
OnlineGameClient* gOnlineGameClient = nullptr;


void OnDraw()
{
	static int sTestCount = 0;
	ClearScreen();

	if( gGameStateManager )
	{
		// Draw the game.
		gGameStateManager->Draw();
	}

	// Camera debug
//	DrawRect( gCameraTarget.x - 5, gCameraTarget.y - 5, 10, 10, Color::PINK );
	FlushRenderer();
}

void OnUpdate( float dt )
{
	// Poll for online request responses.
	gOnlineGameClient->Update();

	if( gGameStateManager )
	{
		// Update the current GameState.
		gGameStateManager->Update( dt );
	}
}

void OnScreenSizeChanged( int32 w, int32 h )
{
	gWindowWidth = w;
	gWindowHeight = h;

	if( gGameStateManager )
	{
		gGameStateManager->OnScreenSizeChanged( w, h );
	}

	DebugPrintf( "Window size w=%d h=%d\n", gWindowWidth, gWindowHeight );
}


void OnWindowShown()
{
	// The window is shown
	// Now is ok to load graphics

	if ( !gInit )
	{
		// Create the WidgetManager.
		gWidgetManager = new WidgetManager();
		gWidgetManager->Init();

		// Create the GameStateManager and create the first state.
		DebugPrintf( "Creating GameStateManager..." );
		gGameStateManager = new GameStateManager();

		// TODO: Start in the Main Menu state.
		//gGameStateManager->ChangeState< MainMenuState >();
		gGameStateManager->ChangeState< EditorState >();
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


void OnPointerDown( const Pointer& pointer )
{
	DebugPrintf( "Pointer %d: Down at (%.3f,%.3f).", pointer.id, pointer.position.x, pointer.position.y );

	if( gGameStateManager )
	{
		// TODO: Update this.
		gGameStateManager->OnPointerDown( pointer.position.x, pointer.position.y, pointer.id );
	}
}

void OnPointerUp( const Pointer& pointer )
{
	DebugPrintf( "Pointer %d: Up at (%.3f,%.3f).", pointer.id, pointer.position.x, pointer.position.y );

	if( gGameStateManager )
	{
		// TODO: Update this.
		gGameStateManager->OnPointerUp( pointer.position.x, pointer.position.y, pointer.id );
	}
}


void OnPointerMotion()
{
	const std::map< int, Pointer >& pointersByID = GetPointers();

	for( auto it = pointersByID.begin(); it != pointersByID.end(); ++it )
	{
		const Pointer& pointer = it->second;

		if( pointer.isMoving )
		{
			DebugPrintf( "Pointer %d: Motion from (%.3f,%.3f) to (%.3f,%.3f).", pointer.id, pointer.lastPosition.x, pointer.lastPosition.y, pointer.position.x, pointer.position.y );

			if( gGameStateManager )
			{
				// TODO: Update this.
				Vec2f displacement = ( pointer.position - pointer.lastPosition );
				gGameStateManager->OnPointerMotion( pointer.position.x, pointer.position.y, displacement.x, displacement.y, pointer.id );
			}
		}
	}
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

	// Initialize the JNI wrapper.
	assertion( app, "Cannot create OnlineGameClient without a reference to the Java app instance!" );
	JNI::Init( app->activity->vm );

	// Sounds can either be .wav or .pcm files, but they must be saved as mono with a sampling rate of 44,100 Hz.
	gSoundManager = new SoundManager( app );
//	gJumpSoundFx = gSoundManager->LoadSoundClip( "sfx/super_mario_jump.wav", "jump.sfx" );
//	gDieSoundFx = gSoundManager->LoadSoundClip( "sfx/super_mario_die.wav", "die.sfx" );
	gSoundManager->Start();
	// Starts background music.
//	gSoundManager->PlayMusic( "music/super_mario_overworld.mp3" );

	// Create OnlineGameClient.
	DebugPrintf( "Creating OnlineGameClient..." );
	gOnlineGameClient = new OnlineGameClient();
	gOnlineGameClient->Init( app );

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

	//RegisterEventFuncs();

	// Run the application
	Run();

	if( gGameStateManager )
	{
		gGameStateManager->DestroyActiveState();
		delete gGameStateManager;
	}

	if( gWidgetManager )
	{
		gWidgetManager->Destroy();
		delete gWidgetManager;
	}

	gSoundManager->Stop();
}
