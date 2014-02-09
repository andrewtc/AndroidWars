#pragma once

namespace mage
{
	/**
	 * Encapsulates all main menu functionality.
	 */
	class MainMenuState : public GameState
	{
	public:
		MainMenuState();
		virtual ~MainMenuState();

	private:
		virtual void OnEnter( const Dictionary& parameters );
		virtual void OnUpdate( float elapsedTime );
		virtual void OnDraw();
		virtual void OnExit();

		Widget* mWidget;
	};
}
