#include "androidwars.h"

using namespace mage;


GameStateManager::GameStateManager() :
	mActiveState( nullptr ),
	mPendingState( nullptr )
{ }


GameStateManager::~GameStateManager()
{
	// Destroy all states (to be safe).
	DestroyActiveState();
}


void GameStateManager::Update( float elapsedTime )
{
	if( HasPendingStateChange() )
	{
		// If there is another state waiting to be switched in, exit and destroy the current state (if any).
		ExitActiveState();

		// Switch in the pending state and enter it.
		mActiveState = mPendingState;
		mPendingState = nullptr;

		// Enter the new state.
		mActiveState->Enter( this, mPendingStateParameters );
	}

	// Update the active state (if any).
	assertion( HasActiveState(), "Cannot update simulation because there is no active GameState!" );
	mActiveState->Update( elapsedTime );
}


void GameStateManager::Draw()
{
	if( HasActiveState() )
	{
		mActiveState->Draw();
	}
}


void GameStateManager::OnScreenSizeChanged( int32 w, int32 h )
{
	if( HasActiveState() )
	{
		// Pass the event to the active state (if any).
		mActiveState->OnScreenSizeChanged( w, h );
	}
}


void GameStateManager::OnPointerDown( const Pointer& pointer )
{
	if( HasActiveState() )
	{
		// Pass the event to the active state (if any).
		mActiveState->OnPointerDown( pointer );
	}
}


void GameStateManager::OnPointerUp( const Pointer& pointer )
{
	if( HasActiveState() )
	{
		// Pass the event to the active state (if any).
		mActiveState->OnPointerUp( pointer );
	}
}


void GameStateManager::OnPointerMotion( const Pointer& activePointer, const PointersByID& pointersByID )
{
	if( HasActiveState() )
	{
		// Pass the event to the active state (if any).
		mActiveState->OnPointerMotion( activePointer, pointersByID );
	}
}


void GameStateManager::CancelStateChange()
{
	if( HasPendingStateChange() )
	{
		// If we were about to change to some other state, destroy it and cancel the state change.
		delete mPendingState;
		mPendingState = nullptr;
	}
}


void GameStateManager::ExitActiveState()
{
	if( HasActiveState() )
	{
		// Exit and destroy the current state (if any).
		mActiveState->Exit();
		delete mActiveState;
		mActiveState = nullptr;
	}
}


void GameStateManager::DestroyActiveState()
{
	// Make sure the active game state has been cleaned up.
	ExitActiveState();

	// Cancel any pending state change (to be safe).
	CancelStateChange();
}


GameState* GameStateManager::GetActiveState() const
{
	return mActiveState;
}


bool GameStateManager::HasActiveState() const
{
	return ( mActiveState != nullptr );
}


bool GameStateManager::HasPendingStateChange() const
{
	return ( mPendingState != nullptr );
}
