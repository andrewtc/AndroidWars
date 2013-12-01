/*
 * Author      : Matthew Johnson
 * Date        : 18/Nov/2013
 * Description :
 *   
 */
 
#pragma once

namespace mage
{
	//---------------------------------------
	// A component is a collection of named clipping rectangles
	struct SpriteComponent
	{
		HashString Name;
		HashMap< RectI > SpriteClips;
		HashMap< SpriteComponent* > ChildComponents;
	};
	//---------------------------------------
	// Collection of named SpriteComponents
	class SpriteDefinition
	{
	public:
		SpriteDefinition();
		~SpriteDefinition();

		std::string Name;
		Texture2D* SpriteSheet;
		SpriteComponent* RootComponent;

		SpriteComponent* GetComponentByName( const std::string& name );
		SpriteComponent* GetComponentFromPath( const std::string& path );
	};
	//---------------------------------------
	struct SpriteAnimationComponent
	{
		// @TODO rotation/flip animation

		// The component to draw
		SpriteComponent* SprComponent;
		HashString SprComponentClipName;
		// Offsets
		int FrameOffsetX;
		int FrameOffsetY;
		int FrameZOrder;		// Unused
	};
	//---------------------------------------
	// A frame of animation
	// Contains a map of components that use this animation
	struct SpriteAnimationFrame
	{
		int Index;
		int Delay;
		// @TODO We only support one component per frame right now...
		SpriteAnimationComponent* SprAnimComponent;
	};
	//---------------------------------------
	// A named animation
	// Contains a vector of animation frames
	struct SpriteAnimation
	{
		enum SpriteAnimationLoopType
		{
			ANIM_LOOP,
			ANIM_ONCE
		};

		HashString Name;
		int LoopType;
		int FrameCount;
		ArrayList< SpriteAnimationFrame > Frames;
	};
	//---------------------------------------
	// A collection of animations
	// Has the name of the SpriteDefinition this AnimationSet was created for
	// Contains a map of SpriteAnimations
	struct SpriteAnimationSet
	{
		HashString Name;
		SpriteDefinition* MyDefinition;
		HashMap< SpriteAnimation > Animations;

		const SpriteAnimation* FindAnimation( const HashString& name ) const
		{
			HashMap< SpriteAnimation >::const_iterator animItr = Animations.find( name );
			if ( animItr != Animations.end() )
			{
				return &(animItr->second);
			}
			return 0;
		}
	};
	//---------------------------------------
	// Sprite - a set of 2D animations
	class Sprite
	{
	public:
		Sprite( SpriteAnimationSet& animation, const HashString& initialAnimName );
		~Sprite();

		void OnUpdate( float dt );
		void OnDraw( const Camera& camera ) const;

		void PlayAnimation( const HashString& animName, int frame=0, bool restart=true );
		void RegisterOnCompletionEvent( const HashString& animName, const HashString& eventName );
		void UnregisterOnCompletionEvent( const HashString& animName );
		RectI GetClippingRectForCurrentAnimation() const;

		const HashString& GetCurrentAnimationName() const;
		bool IsCurrentAnimationFinished() const;

		Vec2f Position;
		Color DrawColor;
		bool RelativeToCamera;
	private:
		// Stores info on the current animation
		struct AnimationInfo
		{
			HashString CurrentAnimationName;
			int CurrentAnimationFrame;
			float CurrentAnimationFrameTime;
		};

		HashMap< HashString > mOnCompletionEvents;

		AnimationInfo mSprAnimInfo;
		const SpriteAnimationSet& mAnimationSet;

		// Not copyable
		Sprite& operator= ( const Sprite& other );
	};
	//---------------------------------------
}
