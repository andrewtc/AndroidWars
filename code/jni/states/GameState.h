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

		virtual void OnScreenSizeChanged( int32 width, int32 height );
		virtual bool OnPointerDown( const Pointer& pointer );
		virtual bool OnPointerUp( const Pointer& pointer );
		virtual bool OnPointerMotion( const Pointer& activePointer, const PointersByID& pointersByID );

		template< class InputStateSubclass, typename... Parameters >
		InputStateSubclass* CreateState( Parameters... );
		void PushState( InputState* inputState, const Dictionary& parameters = Dictionary() );
		void ChangeState( InputState* inputState, const Dictionary& parameters = Dictionary() );
		void PopState();
		void PopAllStates();
		void CancelStateChange();
		void DestroyState( InputState* inputState );

		GameStateManager* GetManager() const;
		bool IsInitialized() const;
		InputState* GetActiveState() const;
		bool HasActiveState() const;
		bool HasPendingStateChange() const;

	protected:
		virtual void OnEnter( const Dictionary& parameters ) = 0;
		virtual void OnUpdate( float elapsedTime );
		virtual void OnDraw();
		virtual void OnExit() = 0;

	private:
		bool mHasPendingStateChange;
		bool mIsPoppingAllStates;
		GameStateManager* mManager;
		InputState* mPendingState;
		Dictionary mPendingStateParameters;
		Camera mDefaultCamera;
		std::vector< InputState* > mInputStates;
	};


	template< class InputStateSubclass, typename... Parameters >
	InputStateSubclass* GameState::CreateState( Parameters... parameters )
	{
		// Create a new InputState with the supplied parameters.
		InputStateSubclass* result = new InputStateSubclass( this, parameters... );
		return result;
	}


	inline InputState* GameState::GetActiveState() const
	{
		InputState* activeState = nullptr;

		if( mInputStates.size() > 0 )
		{
			activeState = mInputStates.back();
		}

		return activeState;
	}


	inline bool GameState::HasActiveState() const
	{
		return ( GetActiveState() != nullptr );
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
