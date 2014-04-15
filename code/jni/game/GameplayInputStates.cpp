#include "androidwars.h"

using namespace mage;


SelectUnitInputState::SelectUnitInputState( GameState* owner ) :
	DerivedInputState( owner )
{ }


SelectUnitInputState::~SelectUnitInputState() { }


void SelectUnitInputState::OnEnter( const Dictionary& parameters )
{
	GameplayState* owner = GetOwnerDerived();

	// Show the tile palette.
	//owner->GetTilePalette()->Show();
}


void SelectUnitInputState::OnExit()
{
	GameplayState* owner = GetOwnerDerived();

	// Hide the tile palette.
	//owner->GetTilePalette()->Hide();
}


bool SelectUnitInputState::OnPointerDown( const Pointer& pointer )
{
	return false; //InputState::OnPointerDown( pointer );
}


bool SelectUnitInputState::OnPointerUp( const Pointer& pointer )
{
	bool wasHandled = false; //InputState::OnPointerUp( pointer );
	return wasHandled;
}


bool SelectUnitInputState::OnPointerMotion( const Pointer& activePointer, const PointersByID& pointersByID )
{
	bool wasHandled = false; //InputState::OnPointerMotion( activePointer, pointersByID );
	return wasHandled;
}
