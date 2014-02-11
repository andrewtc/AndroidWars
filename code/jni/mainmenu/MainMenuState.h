#pragma once

namespace mage
{
	class LogInInputState;
	class DashboardInputState;


	/**
	 * Encapsulates all main menu functionality.
	 */
	class MainMenuState : public GameState
	{
	public:
		MainMenuState();
		virtual ~MainMenuState();

		LogInInputState* GetLogInState() const;
		DashboardInputState* GetDashboardState() const;

	private:
		virtual void OnEnter( const Dictionary& parameters );
		virtual void OnUpdate( float elapsedTime );
		virtual void OnDraw();
		virtual void OnExit();

		LogInInputState* mLogInState;
		DashboardInputState* mDashboardState;
	};


	inline LogInInputState* MainMenuState::GetLogInState() const
	{
		return mLogInState;
	}


	inline DashboardInputState* MainMenuState::GetDashboardState() const
	{
		return mDashboardState;
	}


	/**
	 * Input state that allows user to log in.
	 */
	class LogInInputState : public DerivedInputState< MainMenuState >
	{
	protected:
		void OnEnter( const Dictionary& parameters );
		void OnExit();

	private:
		LogInInputState( GameState* owner );
		virtual ~LogInInputState();

		void OnLogInButtonPressed( Dictionary& params );

		Widget* mWidget;

		friend class GameState;
	};


	/**
	 * Input state that allows user to select options from the user dashboard.
	 */
	class DashboardInputState : public DerivedInputState< MainMenuState >
	{
	protected:
		void OnEnter( const Dictionary& parameters );
		void OnExit();

	private:
		DashboardInputState( GameState* owner );
		virtual ~DashboardInputState();

		void OnLogOutButtonPressed( Dictionary& params );

		Widget* mWidget;

		friend class GameState;
	};
}
