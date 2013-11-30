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
		enum Status
		{
			STATUS_NOT_STARTED,
			STATUS_IN_PROGRESS,
			STATUS_GAME_OVER
		};

		static const char* MAPS_FOLDER_PATH;
		static const char* MAP_FILE_EXTENSION;

		static const int MIN_PLAYERS = 2;
		static const int MAX_PLAYERS = 4;

		static Game* Create( int numPlayers, const std::string& mapName );
		static std::string FormatMapPath( const std::string& mapName );

		Game();
		~Game();

		void SetMapName( const std::string& mapName );

		void Start();
		bool IsNotStarted() const;
		bool IsInProgress() const;
		bool IsGameOver() const;

		void SetCamera( Camera* camera );
		Camera* GetCamera() const;

		Vec2f TileToWorldCoords( const Vec2i& tileCoords ) const;

		void OnUpdate( float dt );
		void OnDraw();

		void AddPlayer( Player* player );
		bool HasPlayer( Player* player ) const;
		int GetNumPlayers() const;

		Unit* SpawnUnit( UnitType* unitType, int tileX, int tileY );

	protected:
		typedef std::vector< Player* > Players;

		static MapObject* SpawnObjectFromXml( const XmlReader::XmlReaderIterator& xmlIterator );

		int mNextPlayerIndex;
		Camera* mCamera;
		Status mStatus;
		Players mPlayers;
		std::string mMapName;
		TileMap mMap;
	};


	inline void Game::SetMapName( const std::string& mapName )
	{
		mMapName = mapName;
	}


	inline bool Game::IsNotStarted() const
	{
		return ( mStatus == STATUS_NOT_STARTED );
	}


	inline bool Game::IsInProgress() const
	{
		return ( mStatus == STATUS_IN_PROGRESS );
	}


	inline bool Game::IsGameOver() const
	{
		return ( mStatus == STATUS_GAME_OVER );
	}


	inline void Game::SetCamera( Camera* camera )
	{
		mCamera = camera;
	}


	inline Camera* Game::GetCamera() const
	{
		return mCamera;
	}


	inline Vec2f Game::TileToWorldCoords( const Vec2i& tileCoords ) const
	{
		return mMap.TileToWorld( tileCoords );
		// TODO: Make this dynamic.
		//return Vec2f( tileCoords.x * 64.0f, tileCoords.y * 96.0f );
	}


	inline bool Game::HasPlayer( Player* player ) const
	{
		bool result = false;

		for( Players::const_iterator it = mPlayers.begin(); it != mPlayers.end(); ++it )
		{
			if ( *it == player )
			{
				result = true;
				break;
			}
		}

		return result;
	}


	inline int Game::GetNumPlayers() const
	{
		return mPlayers.size();
	}
}
