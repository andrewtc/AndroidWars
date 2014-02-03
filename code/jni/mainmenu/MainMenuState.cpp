#include "androidwars.h"

using namespace mage;


MainMenuState::MainMenuState() :
	GameState()
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

	// TEST: Enter the GameplayState immediately.
	GetManager()->ChangeState< GameplayState >();
}


void MainMenuState::OnUpdate( float elapsedTime )
{

}


void MainMenuState::OnExit()
{
	DebugPrintf( "MainMenuState exited!" );
}
