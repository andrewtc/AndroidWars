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

		Game* GetGame() const;
		bool HasGame() const;
		int GetIndex() const;
		int GetFunds() const;
		Color GetPlayerColor() const;
		void GenerateFunds();
		bool HasLost() const;
		void OnGainUnit( Unit* unit );
		void OnLoseUnit( Unit* unit );
		void AddFunds( int funds ) { mFunds -= funds; if ( mFunds < 0 ) mFunds = 0; }

		int CitiesOwned;

	protected:
		int mIndex;
		int mFunds;
		int mUnits;
		Game* mGame;

		friend class Game;
	};


	inline Game* Player::GetGame() const
	{
		return mGame;
	}


	inline bool Player::HasGame() const
	{
		return ( mGame != nullptr );
	}


	inline int Player::GetIndex() const
	{
		return mIndex;
	}


	inline int Player::GetFunds() const
	{
		return mFunds;
	}
}
