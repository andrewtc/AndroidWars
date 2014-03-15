#include "androidwars.h"

using namespace mage;


TerrainType::TerrainType( const HashString& name )
	: Record( name )
	, mDisplayName( "" )
	, mIsCapturable( false )
{ }


TerrainType::~TerrainType() { }


void TerrainType::LoadAnimation()
{
	// Load the animation.
	SpriteManager::LoadSpriteAnimations( mAnimationSetPath.c_str() );
}


std::string TerrainType::GetDisplayName() const
{
	std::string result = mDisplayName;

	if( result.empty() )
	{
		// If the display name is empty, return the name string.
		result = GetName().GetString();
	}

	return mDisplayName;
}


bool TerrainType::IsCapturable() const
{
	return mIsCapturable;
}
