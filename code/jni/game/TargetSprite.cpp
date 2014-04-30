#include "androidwars.h"

using namespace mage;


const char* const TargetSprite::ANIMATION_SET_NAME = "Target";
const HashString TargetSprite::DEFAULT_ANIMATION_NAME = "Idle";


TargetSprite::TargetSprite() :
	mIsInitialized( false ),
	mSprite( nullptr )
{ }


TargetSprite::~TargetSprite()
{
	if( IsInitialized() )
	{
		Destroy();
	}
}


void TargetSprite::Init()
{
	assertion( !IsInitialized(), "Cannot initialize TargetSprite that has already been initialized!" );
	mIsInitialized = true;

	// Load the arrow animation.
	std::string animationSetPath = Scenario::FormatAnimationPath( ANIMATION_SET_NAME );
	std::string animationSetName = Scenario::FormatAnimationName( ANIMATION_SET_NAME );
	SpriteManager::LoadSpriteAnimations( animationSetPath.c_str() );

	// Create the segment and add it to the list of segments.
	mSprite = SpriteManager::CreateSprite( animationSetName, Vec2f::ZERO, DEFAULT_ANIMATION_NAME );
}


void TargetSprite::Destroy()
{
	assertion( IsInitialized(), "Cannot destroy TargetSprite that is not initialized!" );

	// Destroy the Sprite.
	SpriteManager::DestroySprite( mSprite );

	mIsInitialized = false;
}


bool TargetSprite::IsInitialized() const
{
	return mIsInitialized;
}


void TargetSprite::Update( float elapsedTime )
{
	assertion( IsInitialized(), "Cannot update TargetSprite that is not initialized!" );

	// Update the Sprite animation.
	mSprite->OnUpdate( elapsedTime );
}


void TargetSprite::Draw( const Camera& camera )
{
	assertion( IsInitialized(), "Cannot draw TargetSprite that is not initialized!" );

	// Draw the Sprite.
	mSprite->OnDraw( camera );
}


void TargetSprite::SetPosition( const Vec2f& position )
{
	mSprite->Position = position;
}


Vec2f TargetSprite::GetPosition() const
{
	return mSprite->Position;
}
