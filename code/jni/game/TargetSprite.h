#pragma once

namespace mage
{
	class UnitSprite;


	class TargetSprite
	{
	public:
		static const char* const ANIMATION_SET_NAME;
		static const HashString DEFAULT_ANIMATION_NAME;

		TargetSprite();
		~TargetSprite();

		void Init();
		void Destroy();
		bool IsInitialized() const;

		void Update( float elapsedTime );
		void Draw( const Camera& camera );

		void SetPosition( const Vec2f& position );
		Vec2f GetPosition() const;

	protected:
		bool mIsInitialized;
		Sprite* mSprite;
	};
}
