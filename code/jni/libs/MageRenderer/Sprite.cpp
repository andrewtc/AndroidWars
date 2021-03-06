#include "RendererLib.h"

using namespace mage;

//---------------------------------------
// Sprite Definition
//---------------------------------------
SpriteDefinition::SpriteDefinition()
	: SpriteSheet( 0 )
	, RootComponent( 0 )
{}
//---------------------------------------
SpriteDefinition::~SpriteDefinition()
{}
//---------------------------------------
// Private recursive helper for GetComponentByName()
SpriteComponent* _GetComponentByName( SpriteComponent* sprComp, const std::string& name )
{
	if ( sprComp )
	{
		// Search our children
		auto found = sprComp->ChildComponents.find( name );
		if ( found != sprComp->ChildComponents.end() )
		{
			return sprComp;
		}
		// Search our children's children
		else
		{
			for ( auto itr = sprComp->ChildComponents.begin();
				itr != sprComp->ChildComponents.end(); ++itr )
			{
				SpriteComponent* foundInChildren = _GetComponentByName( itr->second, name );
				// Found in children
				if ( foundInChildren )
				{
					return foundInChildren;
				}
			}
		}
	}

	// Not found
	return nullptr;
}
//---------------------------------------
SpriteComponent* SpriteDefinition::GetComponentByName( const std::string& name )
{
	return _GetComponentByName( RootComponent, name );
}
//---------------------------------------
SpriteComponent* SpriteDefinition::GetComponentFromPath( const std::string& path )
{
	std::vector< std::string > pathTokens;
	StringUtil::Tokenize( path, pathTokens, "/" );

	SpriteComponent* sprComp = RootComponent;
	for ( std::vector< std::string >::const_iterator itr = pathTokens.begin(); itr != pathTokens.end(); ++itr )
	{
		SpriteComponent* child = sprComp->ChildComponents[ *itr ];
		if ( child )
			sprComp = child;
		else
			break;
	}

	return sprComp;
}
//---------------------------------------


//---------------------------------------
// SpriteAnimationFrame
//---------------------------------------
void SpriteAnimationFrame::AddComponent( SpriteAnimationComponent* component )
{
	assertion( component, "Cannot add null SpriteAnimationComponent to SpriteAnimationFrame!" );

	// Store each component in Z order.
	int z = component->FrameZOrder;
	ComponentsByZIndex[ z ].push_back( component );

	// Get the clipping rect for the component.
	auto it = component->SprComponent->SpriteClips.find( component->SprComponentClipName );
	assertion( it != component->SprComponent->SpriteClips.end(), "No clipping rect found for SpriteComponent!" );
	const RectI& componentRect = it->second;

	// Expand the clipping rect for this frame to contain the component rect, if necessary.
	if( componentRect.Left < ClippingRect.Left )
	{
		ClippingRect.Left = componentRect.Left;
	}

	if( componentRect.Top < ClippingRect.Top )
	{
		ClippingRect.Top = componentRect.Top;
	}

	if( componentRect.Right > ClippingRect.Right )
	{
		ClippingRect.Right = componentRect.Right;
	}

	if( componentRect.Bottom > ClippingRect.Bottom )
	{
		ClippingRect.Bottom = componentRect.Bottom;
	}

	DebugPrintf( "ADDED COMPONENT (z = %d, clipping rect = {%d,%d,%d,%d})", z, ClippingRect.Left, ClippingRect.Top, ClippingRect.Right, ClippingRect.Bottom );
}
//---------------------------------------


//---------------------------------------
// Sprite
//---------------------------------------
Sprite::Sprite( SpriteAnimationSet& animation, const HashString& initialAnimName )
	: mAnimationSet( animation )
	, DrawColor( Color::WHITE)
	, RelativeToCamera( true )
	, FixedSize( false )
	, Scale( Vec2f::ONE )
{
	HashMap< SpriteAnimation >::const_iterator anim = animation.Animations.find( initialAnimName );
	if ( anim != animation.Animations.end() )
	{
		mSprAnimInfo.CurrentAnimationName = anim->first;
	}
	else
	{
		WarnInfo( "Sprite() initial animation not found: %s\n", initialAnimName.GetString().c_str() );
		if ( !mAnimationSet.Animations.empty() )
		{
			mSprAnimInfo.CurrentAnimationName = mAnimationSet.Animations.begin()->first;
		}
	}
	mSprAnimInfo.CurrentAnimationFrame = 0;
	mSprAnimInfo.CurrentAnimationFrameTime = 0.0f;
}
//---------------------------------------
Sprite::~Sprite()
{}
//---------------------------------------
void Sprite::OnUpdate( float dt )
{
	// Advance animation time
	mSprAnimInfo.CurrentAnimationFrameTime += dt;

	// Check if animation exists
	const SpriteAnimation* anim = mAnimationSet.FindAnimation( mSprAnimInfo.CurrentAnimationName );
	if ( anim )
	{
		const SpriteAnimation& sprAnim = *anim;
		const SpriteAnimationFrame& sprFrame = sprAnim.Frames[ mSprAnimInfo.CurrentAnimationFrame ];

		// Check if frame needs to be increased
		if ( mSprAnimInfo.CurrentAnimationFrameTime >= sprFrame.Delay * dt )
		{
			++mSprAnimInfo.CurrentAnimationFrame;
			mSprAnimInfo.CurrentAnimationFrameTime = 0.0f;
		}

		// Loop frame
		if ( mSprAnimInfo.CurrentAnimationFrame >= sprAnim.FrameCount )
		{
			if ( sprAnim.LoopType == SpriteAnimation::ANIM_LOOP )
			{
				// Loop back to first frame
				mSprAnimInfo.CurrentAnimationFrame = 0;
			}
			else if ( sprAnim.LoopType == SpriteAnimation::ANIM_ONCE )
			{
				// Hold on last frame
				mSprAnimInfo.CurrentAnimationFrame = sprAnim.FrameCount - 1;
			}

			// Fire events for animation completion, if any
			HashMap< HashString >::const_iterator completionEvent = mOnCompletionEvents.find( mSprAnimInfo.CurrentAnimationName );
			if ( completionEvent != mOnCompletionEvents.end() )
			{
				Dictionary params;
				params.Set( "Sprite", this );
				params.Set( "EventName", completionEvent->second );
				EventManager::FireEvent( completionEvent->second, params );
			}
		}
	}
}
//---------------------------------------
void Sprite::OnDraw( const Camera& camera ) const
{
	const SpriteAnimation* anim = mAnimationSet.FindAnimation( mSprAnimInfo.CurrentAnimationName );
	if ( anim )
	{
		// Get the list of components sorted by z-index.
		const SpriteAnimationFrame::SprAnimComponentsByZIndex& animComponentsByZIndex =
			anim->Frames[ mSprAnimInfo.CurrentAnimationFrame ].ComponentsByZIndex;

		for( auto z = animComponentsByZIndex.rbegin(); z != animComponentsByZIndex.rend(); ++z )
		{
			// For each z-index, get the components to draw.
			const SpriteAnimationFrame::SprAnimComponents& animComponentsToDraw = z->second;

			for( auto it = animComponentsToDraw.begin(); it != animComponentsToDraw.end(); ++it )
			{
				// Get the current component to draw.
				const SpriteAnimationComponent* animComponent = *it;

				const RectI& clip = animComponent->SprComponent->SpriteClips[ animComponent->SprComponentClipName ];
				Texture2D* texture = mAnimationSet.MyDefinition->SpriteSheet;

				float px = Position.x + animComponent->FrameOffsetX * 2;
				float py = Position.y + animComponent->FrameOffsetY * 2;
				float w = clip.Width();
				float h = clip.Height();

				if ( FixedSize )
				{
					w = Size.x;
					h = Size.y;
				}

				w *= Scale.x;
				h *= Scale.y;

				RectF worldClip( px, py, px + clip.Width(), py + clip.Height() );

				if ( RelativeToCamera )
				{
					if ( camera.RectInViewport( worldClip ) )
					{
						DrawRect( texture,
							px - camera.GetPosition().x,
							py - camera.GetPosition().y,
							w,
							h,
							DrawColor,
							(int) clip.Left, (int) clip.Top, (int) clip.Width(), (int) clip.Height() );
					}
				}
				else
				{
					DrawRect( texture,
						px,
						py,
						w,
						h,
						DrawColor,
						(int) clip.Left, (int) clip.Top, (int) clip.Width(), (int) clip.Height() );
				}
			}
		}
	}
}
//---------------------------------------
void Sprite::PlayAnimation( const HashString& animName, int frame, bool restart )
{
	// Don't do anything if already in given state
	if ( restart || mSprAnimInfo.CurrentAnimationName != animName )
	{
		mSprAnimInfo.CurrentAnimationName = animName;
		mSprAnimInfo.CurrentAnimationFrame = frame;
		mSprAnimInfo.CurrentAnimationFrameTime = 0.0f;
	}
}
//---------------------------------------
void Sprite::RegisterOnCompletionEvent( const HashString& animName, const HashString& eventName )
{
	mOnCompletionEvents[ animName ] = eventName;
}
//---------------------------------------
void Sprite::UnregisterOnCompletionEvent( const HashString& animName )
{
	HashMap< HashString >::iterator evnt = mOnCompletionEvents.find( animName );
	if ( evnt != mOnCompletionEvents.end() )
	{
		mOnCompletionEvents.erase( evnt );
	}
}
//---------------------------------------
RectI Sprite::GetClippingRectForCurrentAnimation() const
{
	RectI clip;
	const SpriteAnimation* anim = mAnimationSet.FindAnimation( mSprAnimInfo.CurrentAnimationName );
	if ( anim )
	{
		// Return the clipping rect for the current animation frame.
		const SpriteAnimationFrame& frame = anim->Frames[ mSprAnimInfo.CurrentAnimationFrame ];
		clip = frame.ClippingRect;
	}
	return clip;
}
//---------------------------------------
const HashString& Sprite::GetCurrentAnimationName() const
{
	return mSprAnimInfo.CurrentAnimationName;
}
//---------------------------------------
bool Sprite::IsCurrentAnimationFinished() const
{
	// Check if animation exists
	const SpriteAnimation* anim = mAnimationSet.FindAnimation( mSprAnimInfo.CurrentAnimationName );
	if ( anim )
	{
		return mSprAnimInfo.CurrentAnimationFrame == anim->FrameCount - 1;
	}
	return false;
}
//---------------------------------------
