#pragma once

namespace mage
{
	/**
	 * Manages all functionality for one area within the game.
	 */
	class GameState
	{
	public:
		GameState();
		virtual ~GameState();

		void Enter( GameStateManager* manager, const Dictionary& parameters );
		void Update( float elapsedTime );
		void Exit();

		virtual void OnScreenSizeChanged( int32 w, int32 h ) { };
		virtual void OnPointerDown( float x, float y, size_t which ) { };
		virtual void OnPointerUp( float x, float y, size_t which ) { };
		virtual void OnPointerMotion( float x, float y, float dx, float dy, size_t which ) { };

		GameStateManager* GetManager() const;
		bool IsInitialized() const;

	protected:
		virtual void OnEnter( const Dictionary& parameters ) = 0;
		virtual void OnUpdate( float elapsedTime ) = 0;
		virtual void OnExit() = 0;

	private:
		GameStateManager* mManager;
	};
}
