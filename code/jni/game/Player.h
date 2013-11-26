#pragma once


namespace mage
{
	/**
	 * Represents all stats for a single user-controlled Player in the game.
	 */
	class Player
	{
	public:
		Player();
		~Player();

		int GetIndex() const;
		int GetFunds() const;

	protected:
		int mIndex;
		int mFunds;
		Game* mGame;

		friend class Game;
	};


	inline int Player::GetIndex() const
	{
		return mIndex;
	}


	inline int Player::GetFunds() const
	{
		return mFunds;
	}
}
