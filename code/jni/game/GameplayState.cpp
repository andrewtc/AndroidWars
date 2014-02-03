#include "androidwars.h"

using namespace mage;


GameplayState::GameplayState() :
	GameState()
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
}


void GameplayState::OnUpdate( float elapsedTime )
{

}


void GameplayState::OnExit()
{
	DebugPrintf( "GameplayState exited!" );
}
