#pragma once


namespace mage
{
	/**
	 * Represents all stats for a single user-controlled Player in the game.
	 */
	class Player
	{
	public:
		typedef std::set< Faction* > Factions;

		Game* GetGame() const;
		Factions GetFactions() const;

	protected:
		Player( Game* game );
		~Player();

		Game* mGame;

		friend class Game;
	};
}
