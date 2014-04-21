#include "androidwars.h"

using namespace mage;


GameplayState::GameplayState() :
	GameState(),
	mIsNetworkGame( false ),
	mSelectUnitInputState( nullptr ),
	mMoveUnitInputState( nullptr ),
	mSelectActionInputState( nullptr ),
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

	// Create Factions.
	Faction* redFaction = mMap.CreateFaction();
	redFaction->SetControllable( true );
	redFaction->SetColor( Color::RED );

	Faction* blueFaction = mMap.CreateFaction();
	blueFaction->SetControllable( true );
	blueFaction->SetColor( Color::BLUE );

	// Create two test Units.
	// TODO: Remove this.
	UnitType* testUnitType = mScenario.UnitTypes.FindByName( "Tank" );
	Unit* testUnit1 = mMap.CreateUnit( testUnitType, redFaction, 5, 5 );
	Unit* testUnit2 = mMap.CreateUnit( testUnitType, blueFaction, 10, 5, 9 );

	// Create a local Player controlling both Factions.
	Player* localPlayer = mGame.CreatePlayer();
	mGame.SetLocalPlayer( localPlayer );

	mGame.GivePlayerControlOfFaction( localPlayer, redFaction );
	mGame.GivePlayerControlOfFaction( localPlayer, blueFaction );

	// Initialize the Game.
	mGame.Init( &mMap );

	// Set the default font for the MapView.
	mMapView.SetDefaultFont( gWidgetManager->GetFontByName( "default_s.fnt" ) );

	// Initialize the MapView.
	mMapView.Init( &mMap );

	// Create input states.
	mSelectUnitInputState = CreateState< SelectUnitInputState >();
	mMoveUnitInputState = CreateState< MoveUnitInputState >();
	mSelectActionInputState = CreateState< SelectActionInputState >();

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

	// Update all Widgets.
	GameState::OnUpdate( elapsedTime );
}


void GameplayState::OnDraw()
{
	if( mMapView.IsInitialized() )
	{
		// Draw the MapView.
		mMapView.Draw();
	}

	// Draw all Widgets.
	GameState::OnDraw();
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


SelectUnitInputState* GameplayState::GetSelectUnitInputState() const
{
	return mSelectUnitInputState;
}


MoveUnitInputState* GameplayState::GetMoveUnitInputState() const
{
	return mMoveUnitInputState;
}


SelectActionInputState* GameplayState::GetSelectActionInputState() const
{
	return mSelectActionInputState;
}
