#pragma once


namespace mage
{
	class Player;
	class Unit;

	/**
	 * Represents a single play session of AndroidWars.
	 */
	class Game
	{
	public:
		typedef std::vector< Player* > Players;

		static const size_t MIN_PLAYER_COUNT = 2;
		static const size_t MAX_PLAYER_COUNT = 4;

		enum Status
		{
			STATUS_NOT_STARTED,
			STATUS_IN_PROGRESS,
			STATUS_GAME_OVER
		};

		Game();
		~Game();

		void Init( Map* map );
		bool IsInitialized() const;
		Status GetStatus() const;
		bool IsNotStarted() const;
		bool IsInProgress() const;
		bool IsGameOver() const;
		void Destroy();

		void LoadState( const rapidjson::Document& state );
		void SaveState( rapidjson::Document& result );

		Player* CreatePlayer( Faction* faction );
		Player* GetPlayerByIndex( int index ) const;
		int GetCurrentPlayerIndex() const;
		Player* GetCurrentPlayer() const;
		const Players& GetPlayers() const;
		size_t GetPlayerCount() const;
		void DestroyPlayer( Player* player );
		void DestroyAllPlayers();

		void NextTurn();
		int GetTurnNumber() const;
		Event< int, Player* > OnTurnStart;
		Event< int, Player* > OnTurnEnd;

		Map* GetMap() const;
		Scenario* GetScenario() const;

	private:
		void StartTurn();
		void EndTurn();
		void EnforceVictoryConditions();
		void GameOver();

		void RemoveUnit( Unit* unit );
		void RemoveAllUnits();
		void DestroyRemovedUnits();

		int mCurrentTurnIndex;
		int mCurrentPlayerIndex;
		Map* mMap;
		Camera* mCamera;
		Status mStatus;
		Players mPlayers;

		friend class Unit;
	};
}
