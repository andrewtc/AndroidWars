#include "androidwars.h"

using namespace mage;

const int UnitType::DEFAULT_MAX_SUPPLIES;


UnitType::UnitType( const HashString& name )
	: Record( name )
{ }


UnitType::~UnitType() { }


void UnitType::LoadAnimation()
{
	// Load the animation.
	SpriteManager::LoadSpriteAnimations( mAnimationSetPath.c_str() );
}


MovementType* UnitType::GetMovementType() const
{
	// Look up the MovementType from the Scenario.
	return GetScenario()->MovementTypes.FindByName( mMovementTypeName );
}
