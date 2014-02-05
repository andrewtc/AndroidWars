#include "androidwars.h"

using namespace mage;


GameplayState::GameplayState() :
	GameState(),
	mGame( nullptr ),
	mCamera( nullptr ),
	mWasMotion( false )
{
	DebugPrintf( "GameplayState created!" );
}


GameplayState::~GameplayState()
{
	DebugPrintf( "GameplayState destroyed!" );
}


void GameplayState::OnEnter( const Dictionary& parameters )
{
	DebugPrintf( "GameplayState entered!" );

	// Create a camera
	mCamera = new Camera( gWindowWidth, gWindowHeight );
//	gFont = new BitmapFont( "fonts/font.fnt" );

	// Create a new Game and start it.
	mGame = Game::Create( 2, "Cobra Cove" );

	// Load all necessary game data.
	mGame->LoadDataFromFile( "data/Data.xml" );

	mGame->SetCamera( mCamera );
	mGame->Start();

	// Test the Parse cloud service.
	gOnlineGameClient->CallCloudFunction( "hello", "{ \"name\": \"Andrew\" }",
		[]( long requestID, bool isError, int statusCode, const std::string& result )
		{
			DebugPrintf( "RESPONSE %d: (%d)" );

			// Parse the JSON response.
			rapidjson::Document document;
			document.Parse< 0 >( result.c_str() );

			if( isError )
			{
				const char* error;

				if( !document.HasParseError() )
				{
					// Print the error string sent from Parse.
					error = document[ "error" ].GetString();
				}
				else
				{
					// Print the result as the error message.
					error = result.c_str();
				}

				DebugPrintf( "An error occurred: %s", error );
			}
			else
			{
				// Print the response from the server.
				DebugPrintf( "The server says: \"%s\"", document[ "result" ].GetString() );
			}
		});
}


void GameplayState::OnUpdate( float elapsedTime )
{
	if ( mGame )
	{
		// Update the game.
		mGame->OnUpdate( elapsedTime );
	}

	// Camera test
	mCameraTarget += mCameraVelocity;
	mCameraVelocity *= 0.9f;

	if ( mCamera )
		mCamera->LookAtClamp( mCameraTarget );
}


void GameplayState::OnDraw()
{
	if( mGame )
	{
		mGame->OnDraw();
	}
}


void GameplayState::OnExit()
{
	DebugPrintf( "GameplayState exited!" );

	if( mGame )
	{
		delete mGame;
	}

	if( mCamera )
	{
		delete mCamera;
	}
}


void GameplayState::OnScreenSizeChanged( int32 w, int32 h )
{
	mCameraTarget.x = w / 2.0f;
	mCameraTarget.y = h / 2.0f;
}


void GameplayState::OnPointerDown( float x, float y, size_t which )
{
	mWasMotion = false;
}


void GameplayState::OnPointerUp( float x, float y, size_t which )
{
	if( mGame && !mWasMotion )
	{
		mGame->OnTouchEvent( x, y );
	}
	mWasMotion = false;
}


void GameplayState::OnPointerMotion( float x, float y, float dx, float dy, size_t which )
{
	mCameraVelocity.Set( dx, dy );

	Vec2f d( dx, dy );

	// DebugPrintf( "motion delta=%f", d.LengthSqr() );
	if( d.LengthSqr() > 5 )
	{
		mWasMotion = true;
	}
}


Game* GameplayState::GetGame() const
{
	return mGame;
}


/*
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
}*/
