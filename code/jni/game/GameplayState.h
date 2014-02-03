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

	private:
		virtual void OnEnter( const Dictionary& parameters );
		virtual void OnUpdate( float elapsedTime );
		virtual void OnExit();
	};
}
