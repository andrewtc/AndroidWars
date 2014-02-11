#include "androidwars.h"

using namespace mage;


InputState::InputState( GameState* owner ) :
	mOwner( owner ), mIsInitialized( false )
{
	assertion( mOwner, "Cannot create InputState without a GameState owner!" );
}


InputState::~InputState() { }


void InputState::Enter( const Dictionary& parameters )
{
	assertion( !IsInitialized(), "Cannot enter InputState that has already been initialized!" );

	// Initialize the state.
	mIsInitialized = true;

	// Let the InputState initialize itself.
	OnEnter( parameters );
}


void InputState::Update( float elapsedTime )
{
	assertion( IsInitialized(), "Cannot update InputState that has not been initialized!" );

	// Update the InputState.
	OnUpdate( elapsedTime );
}


void InputState::Draw()
{
	assertion( IsInitialized(), "Cannot draw InputState that has not been initialized!" );

	// Draw the InputState.
	OnDraw();
}


void InputState::Exit()
{
	assertion( IsInitialized(), "Cannot exit InputState that has not been initialized!" );

	// Let the InputState clean itself up.
	OnExit();

	// Uninitialize the state.
	mIsInitialized = false;
}
