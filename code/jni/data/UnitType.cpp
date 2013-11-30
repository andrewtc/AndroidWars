#include "androidwars.h"

using namespace mage;


const char* const UnitType::SPRITE_DIRECTORY = "sprites";
const char* const UnitType::ANIMATION_FILE_EXTENSION = "anim";


std::string UnitType::FormatAnimationPath( const std::string& animationName )
{
	// Construct a path to the animation and return the result.
	std::stringstream formatter;
	formatter << SPRITE_DIRECTORY << "/" << animationName << "." << ANIMATION_FILE_EXTENSION;
	return formatter.str();
}


UnitType::UnitType( const HashString& name )
	: Record( name )
{ }


UnitType::~UnitType() { }


void UnitType::LoadAnimation()
{
	// Load the animation.
	SpriteManager::LoadSpriteAnimations( mAnimationSetName.GetString().c_str() );
}
