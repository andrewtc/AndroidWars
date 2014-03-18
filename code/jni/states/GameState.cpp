#include "androidwars.h"

using namespace mage;


GameState::GameState() :
	mManager( nullptr ),
	mHasPendingStateChange( false ),
	mIsPoppingAllStates( false ),
	mDefaultCamera( gWindowWidth, gWindowHeight )
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
	// Get the new active state.
	InputState* activeState = GetActiveState();

	if( HasPendingStateChange() )
	{
		if( mIsPoppingAllStates || ( mPendingState && mPendingState->IsInitialized() ) )
		{
			for( ; activeState && activeState != mPendingState; activeState = GetActiveState() )
			{
				// If clearing all states or the state is already on the stack, exit and pop
				// all states until we reach the desired state.
				activeState->Exit();
				mInputStates.pop_back();
			}
		}

		if( mPendingState && mPendingState != activeState )
		{
			// If there is a state to enter, enter and push the new state.
			mPendingState->Enter( mPendingStateParameters );
			mInputStates.push_back( mPendingState );
			activeState = mPendingState;
		}

		// Clear the state change.
		mPendingState = nullptr;
		mHasPendingStateChange = false;
		mIsPoppingAllStates = false;
	}

	// Run the state update code.
	OnUpdate( elapsedTime );

	if( activeState )
	{
		// Let the current InputState update itself.
		activeState->Update( elapsedTime );
	}
}


void GameState::OnUpdate( float elapsedTime )
{
	// Update the WidgetManager.
	gWidgetManager->Update( elapsedTime );
}


void GameState::Draw()
{
	// Get the new active state.
	InputState* activeState = GetActiveState();

	// Run the state draw code.
	OnDraw();

	if( activeState )
	{
		// Let the current InputState draw itself.
		activeState->Draw();
	}
}


void GameState::OnDraw()
{
	// By default, draw all Widgets.
	gWidgetManager->Draw( mDefaultCamera );
}


void GameState::Exit()
{
	assertion( IsInitialized(), "Cannot exit GameState because it is not currently initialized!" );

	for( InputState* activeState = GetActiveState(); activeState != nullptr; activeState = GetActiveState() )
	{
		// Exit and pop all states.
		activeState->Exit();
		mInputStates.pop_back();
	}

	// Run the exit code.
	OnExit();

	// Unregister the manager.
	mManager = nullptr;
}


void GameState::OnScreenSizeChanged( int32 width, int32 height )
{
	// TODO
};


bool GameState::OnPointerDown( float x, float y, size_t which )
{
	bool wasHandled = gWidgetManager->PointerDown( x, y, which );

	if( !wasHandled )
	{
		InputState* activeState = GetActiveState();

		if( activeState )
		{
			// Let the current InputState handle the event.
			wasHandled = activeState->OnPointerDown( x, y, which );
		}
	}

	return wasHandled;
};


bool GameState::OnPointerUp( float x, float y, size_t which )
{
	bool wasHandled = gWidgetManager->PointerUp( x, y, which );

	if( !wasHandled )
	{
		InputState* activeState = GetActiveState();

		if( activeState )
		{
			// Let the current InputState handle the event.
			wasHandled = activeState->OnPointerUp( x, y, which );
		}
	}

	return wasHandled;
};


bool GameState::OnPointerMotion( float x, float y, float dx, float dy, size_t which )
{
	// Let the Widget manager handle the event first.
	bool wasHandled = gWidgetManager->PointerMotion( x, y, dx, dy, which );

	if( !wasHandled )
	{
		InputState* activeState = GetActiveState();

		if( activeState )
		{
			// Let the current InputState handle the event.
			wasHandled = activeState->OnPointerMotion( x, y, dx, dy, which );
		}
	}

	return wasHandled;
};


void GameState::PushState( InputState* inputState, const Dictionary& parameters )
{
	assertion( inputState, "Cannot push null state!" );
	assertion( inputState->GetOwner() == this, "Cannot push InputState that was not created by the current GameState!" );
	assertion( !inputState->IsInitialized(), "Cannot push InputState that is already initialized!" );

	// Cancel any pending state change.
	CancelStateChange();

	// Keep track of the new InputState as the pending state.
	mPendingState = inputState;
	mPendingStateParameters = parameters;
	mHasPendingStateChange = true;
}


void GameState::ChangeState( InputState* inputState, const Dictionary& parameters )
{
	assertion( inputState, "Cannot change to null state!" );
	assertion( inputState->GetOwner() == this, "Cannot change to InputState that was not created by the current GameState!" );

	// Cancel any pending state change.
	CancelStateChange();

	// Keep track of the new InputState as the pending state.
	mPendingState = inputState;
	mPendingStateParameters = parameters;
	mHasPendingStateChange = true;

	// If the state is not yet initalized, pop all states.
	mIsPoppingAllStates = !( inputState->IsInitialized() );
}


void GameState::PopState()
{
	assertion( mInputStates.size() > 0, "Cannot pop InputState because no state is currently active for this GameState!" );

	// Cancel any pending state change.
	CancelStateChange();

	if( mInputStates.size() > 1 )
	{
		// Schedule a change to the previous InputState.
		InputState* previousState = *( mInputStates.rbegin() + 1 );
		ChangeState( previousState );
	}
	else
	{
		// Pop all states.
		PopAllStates();
	}
}


void GameState::PopAllStates()
{
	// Cancel any pending state change.
	CancelStateChange();

	// Clear all states.
	mHasPendingStateChange = true;
	mIsPoppingAllStates = true;
}


void GameState::CancelStateChange()
{
	// Clear the pending state.
	mPendingState = nullptr;
	mHasPendingStateChange = false;
}


void GameState::DestroyState( InputState* inputState )
{
	assertion( inputState, "Cannot destroy null InputState!" );
	assertion( inputState->GetOwner() == this, "Cannot destroy InputState that was not created by the current GameState!" );
	assertion( !inputState->IsInitialized(), "Cannot destroy InputState that is currently initialized!" );

	// Destroy the InputState.
	delete inputState;
}

