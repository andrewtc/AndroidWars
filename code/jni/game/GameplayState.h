#pragma once

namespace mage
{
	/**
	 * Encapsulates all gameplay functionality.
	 */
	class GameplayState : public GameState
	{
	public:
		GameplayState();
		virtual ~GameplayState();

		Game* GetGame() const;

	private:
		virtual void OnEnter( const Dictionary& parameters );
		virtual void OnUpdate( float elapsedTime );
		virtual void OnDraw();
		virtual void OnExit();

		virtual void OnScreenSizeChanged( int32 w, int32 h );
		virtual bool OnPointerDown( const Pointer& pointer );
		virtual bool OnPointerUp( const Pointer& pointer );
		virtual bool OnPointerMotion( const Pointer& activePointer, const PointersByID& pointersByID );

		bool mWasMotion;
		Game* mGame;
		Camera* mCamera;
		Vec2f mCameraTarget;
		Vec2f mCameraVelocity;
	};
}
