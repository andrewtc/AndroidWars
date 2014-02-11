#include "androidwars.h"

using namespace mage;


MainMenuState::MainMenuState() :
	GameState(),
	mLogInState( nullptr ),
	mDashboardState( nullptr )
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

}


void MainMenuState::OnDraw()
{

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
	DerivedInputState( owner ), mWidget( nullptr )
{
	// Register callbacks.
	RegisterObjectEventFunc( LogInInputState, OnLogInButtonPressed );

	// Create the log in widget.
	mWidget = Widget::LoadWidget( "ui/main_menu.xml" );
	mWidget->Hide();
}


LogInInputState::~LogInInputState()
{
	// Destroy the widget.
	Widget::DestroyWidget( mWidget );
	mWidget = nullptr;

	// Unregister callbacks.
	UnregisterObjectEventFunc( LogInInputState, OnLogInButtonPressed );
}


void LogInInputState::OnEnter( const Dictionary& parameters )
{
	// Show the widget.
	mWidget->Show();
}


void LogInInputState::OnExit()
{
	// Hide the widget.
	mWidget->Hide();
}


ObjectEventFunc( LogInInputState, OnLogInButtonPressed )
{
	DebugPrintf( "Log in button pressed!" );

	// Get the username and password values from the login box.
	Button* usernameField = (Button*) mWidget->GetChildByName( "usernameField" );
	Button* passwordField = (Button*) mWidget->GetChildByName( "passwordField" );

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
}


// ========== DashboardInputState ==========

DashboardInputState::DashboardInputState( GameState* owner ) :
	DerivedInputState( owner )
{
	// Register callbacks.
	RegisterObjectEventFunc( DashboardInputState, OnLogOutButtonPressed );

	// Create the log in widget.
	mWidget = Widget::LoadWidget( "ui/dashboard.xml" );
	mWidget->Hide();
}


DashboardInputState::~DashboardInputState()
{
	// Destroy the widget.
	Widget::DestroyWidget( mWidget );
	mWidget = nullptr;

	// Unregister callbacks.
	UnregisterObjectEventFunc( DashboardInputState, OnLogOutButtonPressed );
}


void DashboardInputState::OnEnter( const Dictionary& parameters )
{
	// Show the widget.
	mWidget->Show();
}


void DashboardInputState::OnExit()
{
	// Hide the widget.
	mWidget->Hide();
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

