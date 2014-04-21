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
		typedef std::vector< std::pair< Player*, Faction* > > FactionControllerMappings;

		static const size_t MIN_CONTROLLED_FACTION_COUNT = 1;

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

		Player* CreatePlayer();
		const Players& GetPlayers() const;
		size_t GetPlayerCount() const;
		void DestroyPlayer( Player* player );
		void DestroyAllPlayers();

		void GivePlayerControlOfFaction( Player* player, Faction* faction );
		void ReleaseControlOfFaction( Faction* faction );
		Player* GetControllerOfFaction( Faction* faction ) const;
		bool PlayerControlsFaction( Player* player, Faction* faction ) const;
		bool FactionHasController( Faction* faction ) const;
		size_t GetControlledFactionCount() const;

		Player* GetControllerByTurnOrder( int turnOrder ) const;
		Faction* GetFactionByTurnOrder( int turnOrder ) const;
		int GetTurnOrderOfFaction( Faction* faction ) const;
		int GetTurnOrderOfCurrentFaction() const;
		Faction* GetCurrentFaction() const;

		void SetLocalPlayer( Player* player );
		Player* GetLocalPlayer() const;
		bool HasLocalPlayer() const;

		void NextTurn();
		int GetTurnNumber() const;
		Event< int, Faction* > OnTurnStart;
		Event< int, Faction* > OnTurnEnd;

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
		int mCurrentFactionIndex;
		Map* mMap;
		Camera* mCamera;
		Player* mLocalPlayer;
		Status mStatus;
		Players mPlayers;
		FactionControllerMappings mFactionControllerMappings;

		friend class Unit;
	};
}
