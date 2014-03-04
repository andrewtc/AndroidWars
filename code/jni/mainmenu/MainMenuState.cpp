#include "androidwars.h"

using namespace mage;


MainMenuState::MainMenuState() :
	GameState(),
	mLogInState( nullptr ),
	mDashboardState( nullptr ),
	mWidget( nullptr )
{
	DebugPrintf( "MainMenuState created!" );
}


MainMenuState::~MainMenuState()
{
	DebugPrintf( "MainMenuState destroyed!" );
}


void MainMenuState::OnEnter( const Dictionary& parameters )
{
	DebugPrintf( "MainMenuState entered!" );

	// Create the main menu widget.
	mWidget = gWidgetManager->CreateWidgetFromTemplate( "MainMenu" );

	if( mWidget )
	{
		// Show the main menu widget.
		gWidgetManager->GetRootWidget()->AddChild( mWidget );
		mWidget->Show();
	}

	// Create all states.
	mLogInState = CreateState< LogInInputState >();
	mDashboardState = CreateState< DashboardInputState >();

	if( !gOnlineGameClient->IsAuthenticated() )
	{
		// If no user is currently logged in, show the login screen.
		ChangeState( mLogInState );
	}
	else
	{
		// Otherwise, show the user dashboard.
		ChangeState( mDashboardState );
	}
}


void MainMenuState::OnUpdate( float elapsedTime )
{
	GameState::OnUpdate( elapsedTime );
}


void MainMenuState::OnDraw()
{
	GameState::OnDraw();
}


void MainMenuState::OnExit()
{
	DebugPrintf( "MainMenuState exited!" );

	// Destroy all states.
	DestroyState( mLogInState );
	DestroyState( mDashboardState );
}


// ========== LogInInputState ==========

LogInInputState::LogInInputState( GameState* owner ) :
	DerivedInputState( owner )
{
	/*
	// Create the log in widget.
	mWidget->Hide();

	// Get Login button.
	Button* loginButton = mWidget->GetChildByName< Button >( "loginButton" );

	if( loginButton )
	{
		// Register callbacks.
		loginButton->SetOnClickDelegate( [this]( float x, float y )
		{
			OnLogInButtonPressed( x, y );
		});
	}

	// Attach the Widget to the root Widget.
	gWidgetManager->GetRootWidget()->AddChild( mWidget );
	*/
	// TODO
}


LogInInputState::~LogInInputState()
{
	/*
	// Destroy the widget.
	gWidgetManager->DestroyWidget( mWidget );
	mWidget = nullptr;

	// Get Login button.
	Button* loginButton = mWidget->GetChildByName< Button >( "loginButton" );

	if( loginButton )
	{
		// Unregister callbacks.
		loginButton->ClearOnClickDelegate();
	}
	*/
	// TODO
}


void LogInInputState::OnEnter( const Dictionary& parameters )
{
	// Show the widget.
	//mWidget->Show();
	// TODO
}


void LogInInputState::OnExit()
{
	// Hide the widget.
	//mWidget->Hide();
	// TODO
}


void LogInInputState::OnLogInButtonPressed( float x, float y )
{
	/*
	DebugPrintf( "Log in button pressed!" );

	// Get the username and password values from the login box.
	TextField* usernameField = mWidget->GetChildByName< TextField >( "usernameField" );
	TextField* passwordField = mWidget->GetChildByName< TextField >( "passwordField" );

	std::string username = usernameField->GetText();
	std::string password = passwordField->GetText();

	// Fire off the login request to the server.
	gOnlineGameClient->LogIn( username, password, [this]( bool success )
	{
		if( success )
		{
			DebugPrintf( "Login successful!" );

			// If the login was successful, go to the dashboard.
			MainMenuState* owner = GetOwnerDerived();
			owner->ChangeState( owner->GetDashboardState() );
		}
		else
		{
			// TODO: If the login failed, give the user feedback.
			DebugPrintf( "Login failed!" );
		}
	});
	*/
	// TODO
}


// ========== DashboardInputState ==========

DashboardInputState::DashboardInputState( GameState* owner ) :
	DerivedInputState( owner )
{
	/*
	// Register callbacks.
	RegisterObjectEventFunc( DashboardInputState, OnLogOutButtonPressed );
	RegisterObjectEventFunc( DashboardInputState, OnRefreshButtonPressed );
	RegisterObjectEventFunc( DashboardInputState, OnNewGameButtonPressed );

	// Create the log in widget.
	mWidget = gWidgetManager->CreateWidgetFromTemplate( "Dashboard" );
	mWidget->Hide();
	*/
}


DashboardInputState::~DashboardInputState()
{
	/*
	// Destroy the widget.
	gWidgetManager->DestroyWidget( mWidget );
	mWidget = nullptr;

	// Unregister callbacks.
	UnregisterObjectEventFunc( DashboardInputState, OnLogOutButtonPressed );
	UnregisterObjectEventFunc( DashboardInputState, OnRefreshButtonPressed );
	UnregisterObjectEventFunc( DashboardInputState, OnNewGameButtonPressed );
	*/
}


void DashboardInputState::OnEnter( const Dictionary& parameters )
{
	/*
	// Show the widget.
	mWidget->Show();
	*/
}


void DashboardInputState::OnExit()
{
	/*
	// Hide the widget.
	mWidget->Hide();
	*/
}


ObjectEventFunc( DashboardInputState, OnLogOutButtonPressed )
{
	DebugPrintf( "Log out button pressed!" );

	// Log the current user out.
	gOnlineGameClient->LogOut();

	// If the login was successful, go to the login screen.
	MainMenuState* owner = GetOwnerDerived();
	owner->ChangeState( owner->GetLogInState() );
}


ObjectEventFunc( DashboardInputState, OnRefreshButtonPressed )
{
	DebugPrintf( "Refresh button pressed!" );

	// Request the current games list.
	gOnlineGameClient->RequestCurrentGamesList( [this]( bool success, const std::vector< OnlineGameListData >& currentGameList )
	{
		for( auto it = currentGameList.begin(); it != currentGameList.end(); ++it )
		{
			DebugPrintf( "Found game: \"%s\" (id: %s)", it->name.c_str(), it->id.c_str() );
		}
	});
}


ObjectEventFunc( DashboardInputState, OnNewGameButtonPressed )
{
	DebugPrintf( "New game button pressed!" );

	// Request a new game.
	gOnlineGameClient->CallCloudFunction( "requestMatchmakingGame", "{}" );
}

