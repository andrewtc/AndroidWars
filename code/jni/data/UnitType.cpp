#include "androidwars.h"

using namespace mage;

const char* const UnitType::SPRITE_DIRECTORY = "sprites";
const char* const UnitType::ANIMATION_FILE_EXTENSION = "anim";
const int UnitType::DEFAULT_MAX_SUPPLIES;


std::string UnitType::FormatAnimationPath( const std::string& animationName )
{
	// Construct a path to the animation and return the result.
	std::stringstream formatter;
	formatter << SPRITE_DIRECTORY << "/" << animationName << "." << ANIMATION_FILE_EXTENSION;
	return formatter.str();
}


std::string UnitType::FormatAnimationName( const std::string& animationName )
{
	// Construct a name for the animation and return the result.
	std::stringstream formatter;
	formatter << animationName << "." << ANIMATION_FILE_EXTENSION;
	return formatter.str();
}


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
	// Look up the MovementType from the Database.
	return GetDatabase()->MovementTypes.FindByName( mMovementTypeName );
}
