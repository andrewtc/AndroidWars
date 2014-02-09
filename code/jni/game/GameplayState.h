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
		virtual void OnPointerDown( float x, float y, size_t which );
		virtual void OnPointerUp( float x, float y, size_t which );
		virtual void OnPointerMotion( float x, float y, float dx, float dy, size_t which );

		void OnHelloResponse( const OnlineRequestResult& result );

		bool mWasMotion;
		Game* mGame;
		Camera* mCamera;
		Vec2f mCameraTarget;
		Vec2f mCameraVelocity;
	};
}