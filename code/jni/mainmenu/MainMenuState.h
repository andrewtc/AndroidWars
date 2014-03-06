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
		static const Vec2f BACKGROUND_SCROLL_VELOCITY;

		MainMenuState();
		virtual ~MainMenuState();

		LogInInputState* GetLogInState() const;
		DashboardInputState* GetDashboardState() const;

		Widget* GetWidget() const;

	private:
		virtual void OnEnter( const Dictionary& parameters );
		virtual void OnUpdate( float elapsedTime );
		virtual void OnDraw();
		virtual void OnExit();

		LogInInputState* mLogInState;
		DashboardInputState* mDashboardState;
		Widget* mWidget;
	};


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

		void OnLogInButtonPressed();

		Widget* GetLoginScreen() const;

		ProgressInputState* mProgressDialog;

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

		Widget* GetDashboardScreen() const;

	private:
		DashboardInputState( GameState* owner );
		virtual ~DashboardInputState();

		void OnLogOutButtonPressed();
		void OnRefreshButtonPressed();
		void OnNewGameButtonPressed();

		void RefreshGamesList( const std::vector< OnlineGameListData >& currentGameList );

		ListLayout* GetGamesList() const;

		friend class GameState;
	};
}
