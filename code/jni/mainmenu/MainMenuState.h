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


	inline LogInInputState* MainMenuState::GetLogInState() const
	{
		return mLogInState;
	}


	inline DashboardInputState* MainMenuState::GetDashboardState() const
	{
		return mDashboardState;
	}


	inline Widget* MainMenuState::GetWidget() const
	{
		return mWidget;
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

		void OnLogInButtonPressed( float x, float y );

		Widget* GetLoginScreen() const;

		friend class GameState;
	};


	inline Widget* LogInInputState::GetLoginScreen() const
	{
		Widget* loginScreen = nullptr;

		// Get the login widget.
		Widget* mainMenu = GetOwnerDerived()->GetWidget();

		if( mainMenu )
		{
			loginScreen = mainMenu->GetChildByName( "loginScreen" );
		}

		return loginScreen;
	}


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
		void OnRefreshButtonPressed( Dictionary& params );
		void OnNewGameButtonPressed( Dictionary& params );

		friend class GameState;
	};
}
