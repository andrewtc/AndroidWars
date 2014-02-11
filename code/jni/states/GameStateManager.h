#pragma once


namespace mage
{
	/**
	 * Manages and updates all registered GameStates.
	 */
	class GameStateManager
	{
	public:
		GameStateManager();
		~GameStateManager();

		template< class GameStateSubclass >
		void ChangeState( const Dictionary& parameters = Dictionary() );

		void CancelStateChange();
		void DestroyActiveState();

		void Update( float elapsedTime );
		void Draw();

		void OnScreenSizeChanged( int32 w, int32 h );
		void OnPointerDown( float x, float y, size_t which );
		void OnPointerUp( float x, float y, size_t which );
		void OnPointerMotion( float x, float y, float dx, float dy, size_t which );

		GameState* GetActiveState() const;
		bool HasActiveState() const;
		bool HasPendingStateChange() const;

	private:
		void ExitActiveState();

		GameState* mActiveState;
		GameState* mPendingState;
		Dictionary mPendingStateParameters;
	};


	template< class GameStateSubclass >
	void GameStateManager::ChangeState( const Dictionary& parameters )
	{
		// If we were about to transition to some other state, cancel the state change.
		CancelStateChange();

		// Instantiate the new GameState instance to switch in.
		mPendingState = new GameStateSubclass();

		// Copy the parameters to be passed to the new GameState instance.
		mPendingStateParameters = parameters;
	}
}
