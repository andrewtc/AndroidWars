#pragma once


namespace mage
{
	/**
	 * Represents all stats for a single user-controlled Player in the game.
	 */
	class Player
	{
	public:
		Game* GetGame() const;
		Faction* GetFaction() const;

	protected:
		Player( Game* game, Faction* faction );
		~Player();

		Game* mGame;
		Faction* mFaction;

		friend class Game;
	};
}
