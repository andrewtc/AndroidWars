#include "androidwars.h"

using namespace mage;


GameplayState::GameplayState() :
	GameState(),
	mIsNetworkGame( false ),
	mSelectUnitInputState( nullptr ),
	mActionsDialog( nullptr )
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
	bool success = mScenario.LoadDataFromFile( "data/Data.json" );
	assertion( success, "The Scenario file \"%s\" could not be opened!" );

	// Create a new Map and paint it with default tiles.
	mMap.Init( &mScenario );
	mMap.Resize( 16, 12 );
	mMap.FillWithDefaultTerrainType();

	// Create a Faction.
	Faction* testFaction = mMap.CreateFaction();

	// Create a test Unit.
	UnitType* testUnitType = mScenario.UnitTypes.FindByName( "Infantry" );
	mMap.CreateUnit( testUnitType, testFaction, 5, 5, 10, 99 );

	// Set the default font for the MapView.
	mMapView.SetDefaultFont( gWidgetManager->GetFontByName( "default_s.fnt" ) );

	// Initialize the MapView.
	mMapView.Init( &mMap );

	// Create input states.
	mSelectUnitInputState = CreateState< SelectUnitInputState >();

	// Start by letting the player select a Unit.
	ChangeState( mSelectUnitInputState );

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
	return GameState::OnPointerDown( pointer );
}


bool GameplayState::OnPointerUp( const Pointer& pointer )
{
	return GameState::OnPointerUp( pointer );
}


bool GameplayState::OnPointerMotion( const Pointer& activePointer, const PointersByID& pointersByID )
{
	return GameState::OnPointerMotion( activePointer, pointersByID );
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
