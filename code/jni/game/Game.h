#pragma once


namespace mage
{
	class Player;
	class Unit;

	/**
	 * Represents a tile direction (i.e. north, south, east, or west).
	 */
	enum CardinalDirection
	{
		CARDINAL_DIRECTION_NONE,
		CARDINAL_DIRECTION_EAST,
		CARDINAL_DIRECTION_NORTH,
		CARDINAL_DIRECTION_WEST,
		CARDINAL_DIRECTION_SOUTH
	};

	static const CardinalDirection FIRST_VALID_DIRECTION = CARDINAL_DIRECTION_EAST;
	static const CardinalDirection LAST_VALID_DIRECTION = CARDINAL_DIRECTION_SOUTH;
	static const int NUM_DIRECTIONS = ( LAST_VALID_DIRECTION - FIRST_VALID_DIRECTION + 1 );

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

		void OnUpdate( float dt );
		void OnDraw();

		void AddPlayer( Player* player );
		bool HasPlayer( Player* player ) const;
		int GetNumPlayers() const;

		Unit* SpawnUnit( UnitType* unitType, int tileX, int tileY );
		void SelectUnit( Unit* unit );

		void OnTouchEvent( float x, float y );

		TileMap* GetMap();
		const TileMap* GetMap() const;
		MapTile& GetTile( int x, int y );
		const MapTile& GetTile( int x, int y ) const;
		MapTile& GetTile( const Vec2i& tilePos );
		const MapTile& GetTile( const Vec2i& tilePos ) const;
		TerrainType* GetTerrainTypeOfTile( int x, int y );
		TerrainType* GetTerrainTypeOfTile( const Vec2i& tilePos );
		bool TileIsReachable( const Vec2i& tilePos ) const;
		int GetIndexOfTile( const Vec2i& tilePos ) const;
		static Vec2i GetAdjacentTilePos( int x, int y, CardinalDirection direction );
		static Vec2i GetAdjacentTilePos( const Vec2i& tilePos, CardinalDirection direction );
		static CardinalDirection GetOppositeDirection( CardinalDirection direction );

		// Events
		EventFunc( ConfirmMoveEvent );
		EventFunc( CancelMoveEvent );

	protected:
		struct TileInfo
		{
			Vec2i tilePos;
			int bestTotalCostToEnter;
			CardinalDirection previousTileDirection;
		};

		typedef std::vector< Player* > Players;

		const static unsigned int TERRAIN_LAYER_INDEX = 1;

		static MapObject* SpawnObjectFromXml( const XmlReader::XmlReaderIterator& xmlIterator );

		TileInfo* GetReachableTileInfo( const Vec2i& tilePos );
		const TileInfo* GetReachableTileInfo( const Vec2i& tilePos ) const;
		void SelectReachableTilesForUnit( Unit* unit, const Vec2i& tilePos, int costToEnter, CardinalDirection previousTileDirection, int movementRange );
		void FindBestPathToTile( const Vec2i& tilePos, Path& result ) const;
		void MoveUnitToTile( Unit* unit, const Vec2i& tilePos );

		void ShowMoveDialog() const { mMoveDialog->Show(); }
		// True if game input should be blocked
		bool WidgetIsOpen() const;
		// Hide all the game dialogs
		void HideAllDialogs();

		int mNextPlayerIndex;
		Camera* mCamera;
		Status mStatus;
		Players mPlayers;
		std::string mMapName;
		TileMap mMap;
		Unit* mSelectedUnit;
		Path mSelectedPath;
		std::map< int, TileInfo > mReachableTiles;
		Widget* mMoveDialog;
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


	inline TileMap* Game::GetMap()
	{
		return &mMap;
	}


	inline const TileMap* Game::GetMap() const
	{
		return &mMap;
	}


	inline MapTile& Game::GetTile( int x, int y )
	{
		return mMap.GetTile( x, y, TERRAIN_LAYER_INDEX );
	}


	inline const MapTile& Game::GetTile( int x, int y ) const
	{
		return mMap.GetTile( x, y, TERRAIN_LAYER_INDEX );
	}


	inline MapTile& Game::GetTile( const Vec2i& tilePos )
	{
		return GetTile( tilePos.x, tilePos.y );
	}


	inline const MapTile& Game::GetTile( const Vec2i& tilePos ) const
	{
		return GetTile( tilePos.x, tilePos.y );
	}


	inline TerrainType* Game::GetTerrainTypeOfTile( const Vec2i& tilePos )
	{
		return GetTerrainTypeOfTile( tilePos.x, tilePos.y );
	}


	inline bool Game::TileIsReachable( const Vec2i& tilePos ) const
	{
		return ( GetReachableTileInfo( tilePos ) != nullptr );
	}


	inline int Game::GetIndexOfTile( const Vec2i& tilePos ) const
	{
		return ( tilePos.x + ( tilePos.y * mMap.GetTileWidth() ) );
	}


	inline Vec2i Game::GetAdjacentTilePos( int x, int y, CardinalDirection direction )
	{
		Vec2i result( x, y );

		switch( direction )
		{
		case CARDINAL_DIRECTION_EAST:
			result.x += 1;
			break;

		case CARDINAL_DIRECTION_WEST:
			result.x -= 1;
			break;

		case CARDINAL_DIRECTION_SOUTH:
			result.y += 1;
			break;

		case CARDINAL_DIRECTION_NORTH:
			result.y -= 1;
			break;
		}

		return result;
	}


	inline Vec2i Game::GetAdjacentTilePos( const Vec2i& tilePos, CardinalDirection direction )
	{
		return GetAdjacentTilePos( tilePos.x, tilePos.y, direction );
	}


	inline CardinalDirection Game::GetOppositeDirection( CardinalDirection direction )
	{
		CardinalDirection result = CARDINAL_DIRECTION_NONE;

		switch( direction )
		{
		case CARDINAL_DIRECTION_EAST:
			result = CARDINAL_DIRECTION_WEST;
			break;

		case CARDINAL_DIRECTION_WEST:
			result = CARDINAL_DIRECTION_EAST;
			break;

		case CARDINAL_DIRECTION_SOUTH:
			result = CARDINAL_DIRECTION_NORTH;
			break;

		case CARDINAL_DIRECTION_NORTH:
			result = CARDINAL_DIRECTION_SOUTH;
			break;
		}

		return result;
	}


	inline Game::TileInfo* Game::GetReachableTileInfo( const Vec2i& tilePos )
	{
		TileInfo* tileInfo = nullptr;

		// Find the existing tile info (if any).
		auto it = mReachableTiles.find( GetIndexOfTile( tilePos ) );

		if( it != mReachableTiles.end() )
		{
			// If an existing tile info was found, return it.
			tileInfo = &( it->second );
		}

		return tileInfo;
	}


	inline const Game::TileInfo* Game::GetReachableTileInfo( const Vec2i& tilePos ) const
	{
		const TileInfo* tileInfo = nullptr;

		// Find the existing tile info (if any).
		auto it = mReachableTiles.find( GetIndexOfTile( tilePos ) );

		if( it != mReachableTiles.end() )
		{
			// If an existing tile info was found, return it.
			tileInfo = &( it->second );
		}

		return tileInfo;
	}
}
