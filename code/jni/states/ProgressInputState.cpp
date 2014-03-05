#include "androidwars.h"

using namespace mage;


ProgressInputState::ProgressInputState( GameState* owner ) :
	DialogInputState( owner )
{ }


ProgressInputState::~ProgressInputState() { }


void ProgressInputState::OnEnter( const Dictionary& parameters )
{
	DialogInputState::OnEnter( parameters );
}


void ProgressInputState::OnExit()
{
	DialogInputState::OnExit();
}
