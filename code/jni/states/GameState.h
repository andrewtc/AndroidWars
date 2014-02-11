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
		void Draw();
		void Exit();

		virtual void OnScreenSizeChanged( int32 w, int32 h ) { };
		virtual void OnPointerDown( float x, float y, size_t which ) { };
		virtual void OnPointerUp( float x, float y, size_t which ) { };
		virtual void OnPointerMotion( float x, float y, float dx, float dy, size_t which ) { };

		template< class InputStateSubclass, typename... Parameters >
		InputStateSubclass* CreateState( Parameters... );
		void ChangeState( InputState* inputState, const Dictionary& parameters = Dictionary() );
		void CancelStateChange();
		void DestroyState( InputState* inputState );

		GameStateManager* GetManager() const;
		bool IsInitialized() const;
		bool HasActiveState() const;
		bool HasPendingStateChange() const;

	protected:
		virtual void OnEnter( const Dictionary& parameters ) = 0;
		virtual void OnUpdate( float elapsedTime ) = 0;
		virtual void OnDraw() = 0;
		virtual void OnExit() = 0;

	private:
		GameStateManager* mManager;
		InputState* mActiveState;
		InputState* mPendingState;
		Dictionary mPendingStateParameters;
		bool mHasPendingStateChange;
	};


	template< class InputStateSubclass, typename... Parameters >
	InputStateSubclass* GameState::CreateState( Parameters... parameters )
	{
		// Create a new InputState with the supplied parameters.
		InputStateSubclass* result = new InputStateSubclass( this, parameters... );
		return result;
	}


	inline bool GameState::HasActiveState() const
	{
		return ( mActiveState != nullptr );
	}


	inline bool GameState::HasPendingStateChange() const
	{
		return mHasPendingStateChange;
	}


	inline GameStateManager* GameState::GetManager() const
	{
		// Return the manager for this state.
		return mManager;
	}


	inline bool GameState::IsInitialized() const
	{
		// Return whether the state has been entered.
		return ( mManager != nullptr );
	}
}
