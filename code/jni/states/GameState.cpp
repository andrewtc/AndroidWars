#include "androidwars.h"

using namespace mage;


GameState::GameState() :
	mManager( nullptr )
{ }


GameState::~GameState()
{
	// Make sure the exit code was run before deleting the state.
	assertion( !IsInitialized(), "Cannot delete GameState because it is still initialized!" );
}


void GameState::Enter( GameStateManager* manager, const Dictionary& parameters )
{
	assertion( !IsInitialized(), "Cannot enter GameState because it has already been initialized!" );

	// Keep track of the manager for this state.
	mManager = manager;
	assertion( mManager, "Cannot enter state because no GameStateManager was provided!" );

	// Initialize the state.
	OnEnter( parameters );
}


void GameState::Update( float elapsedTime )
{
	// Run the state update code.
	OnUpdate( elapsedTime );
}


void GameState::Draw()
{
	// Run the state draw code.
	OnDraw();
}


void GameState::Exit()
{
	assertion( IsInitialized(), "Cannot exit GameState because it is not currently initialized!" );

	// Run the exit code.
	OnExit();

	// Unregister the manager.
	mManager = nullptr;
}


GameStateManager* GameState::GetManager() const
{
	// Return the manager for this state.
	return mManager;
}


bool GameState::IsInitialized() const
{
	// Return whether the state has been entered.
	return ( mManager != nullptr );
}
