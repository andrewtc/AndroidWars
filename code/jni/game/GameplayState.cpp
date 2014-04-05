#include "androidwars.h"

using namespace mage;


GameplayState::GameplayState() :
	GameState(),
	mIsNetworkGame( false )
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

	// Get the game ID.
	std::string gameID;
	parameters.Get( "gameID", gameID );

	if( !gameID.empty(), "Game ID for GameplayState is empty!" )
	{
		// This is a network game.
		mIsNetworkGame = true;
	}

	// Load the default Scenario.
	// TODO: Allow this to change.
	mScenario.LoadDataFromFile( "data/Data.json" );

	// Create a new Map and paint it with default tiles.
	mMap.Resize( 16, 12 );
	mMap.FillWithDefaultTerrainType();
	mMap.Init( &mScenario );

	// Set the default font for the MapView.
	mMapView.SetDefaultFont( gWidgetManager->GetFontByName( "default_s.fnt" ) );

	// Initialize the MapView.
	mMapView.Init( &mMap );

	/*
	gOnlineGameClient->RequestGameData( gameID, [ this ]( bool success, OnlineGameData gameData )
	{
		if( success )
		{
			// Parse out the state.
			rapidjson::Document gameState;
			gameState.Parse< 0 >( gameData.gameState.c_str() );

			// Load the game from the state.
			mGame->Start();
			mGame->LoadState( gameState );
		}
		else
		{
			WarnFail( "Error requesting Game data!" );
		}
	});
	*/
}


void GameplayState::OnUpdate( float elapsedTime )
{
	if( mMapView.IsInitialized() )
	{
		// Update the MapView.
		mMapView.Update( elapsedTime );
	}
}


void GameplayState::OnDraw()
{
	if( mMapView.IsInitialized() )
	{
		// Draw the MapView.
		mMapView.Draw();
	}
}


void GameplayState::OnExit()
{
	DebugPrintf( "GameplayState exited!" );
}


void GameplayState::OnScreenSizeChanged( int32 w, int32 h )
{
	// TODO
}


bool GameplayState::OnPointerDown( const Pointer& pointer )
{
	// TODO
	return GameState::OnPointerDown( pointer );
}


bool GameplayState::OnPointerUp( const Pointer& pointer )
{
	// TODO
	return true;
}


bool GameplayState::OnPointerMotion( const Pointer& activePointer, const PointersByID& pointersByID )
{
	// TODO
	return true;
}


Game* GameplayState::GetGame()
{
	return &mGame;
}


const Game* GameplayState::GetGame() const
{
	return &mGame;
}


Map* GameplayState::GetMap()
{
	return &mMap;
}


const Map* GameplayState::GetMap() const
{
	return &mMap;
}


MapView* GameplayState::GetMapView()
{
	return &mMapView;
}


const MapView* GameplayState::GetMapView() const
{
	return &mMapView;
}
