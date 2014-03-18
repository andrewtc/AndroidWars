#pragma once

namespace mage
{
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


	class Tile
	{
	public:
		Tile();
		~Tile();

		void SetTerrainType( TerrainType* terrainType );
		void ClearTerrainType();
		TerrainType* GetTerrainType() const;
		bool HasTerrainType() const;

		void SetOwner( Player* player );
		void ClearOwner();
		Player* GetOwner() const;
		bool HasOwner() const;

		bool IsCapturable() const;

	private:
		TerrainType* mTerrainType;
		Player* mOwner;
	};


	/**
	 * Holds information about each tile on the map and allows tile data to be manipulated.
	 */
	class Map
	{
	public:
		class ConstIterator
		{
		public:
			ConstIterator( const Map* map );
			ConstIterator( const Map* map, const Vec2s& tilePos );
			ConstIterator( const Map* map, short x, short y );
			~ConstIterator();

			ConstIterator GetAdjacent( CardinalDirection direction ) const;

			const Tile& operator*() const;
			const Tile* operator->() const;

			Vec2s GetTilePos() const;
			bool IsValid() const;

		private:
			const Map* mMap;
			Vec2s mTilePos;
		};

		class Iterator
		{
		public:
			Iterator( Map* map );
			Iterator( Map* map, const Vec2s& tilePos );
			Iterator( Map* map, short x, short y );
			~Iterator();

			Iterator GetAdjacent( CardinalDirection direction ) const;

			Tile& operator*() const;
			Tile* operator->() const;

			Vec2s GetTilePos() const;
			bool IsValid() const;

		private:
			Map* mMap;
			Vec2s mTilePos;
		};

		static const short MAX_SIZE_EXPONENT = 10;
		static const short MAX_SIZE = ( 1 << MAX_SIZE_EXPONENT );
		static const size_t MAX_TILES = ( (size_t) MAX_SIZE * (size_t) MAX_SIZE );

		typedef Callback< void, short, short > TileChangedCallback;
		typedef Callback< void, const Iterator& > ForEachTileCallback;
		typedef Callback< void, const ConstIterator& > ForEachConstTileCallback;
		typedef Callback< void, Unit* > ForEachUnitCallback;
		typedef Callback< void, const Unit* > ForEachConstUnitCallback;

		static Vec2s GetAdjacentTilePos( short x, short y, CardinalDirection direction );
		static Vec2s GetAdjacentTilePos( const Vec2s& tilePos, CardinalDirection direction );
		static CardinalDirection GetOppositeDirection( CardinalDirection direction );

		Map();
		~Map();

		void Init( Scenario* scenario );
		void Destroy();

		void SaveToJSON( rapidjson::Document& document, rapidjson::Value& object );

		void LoadFromFile( const std::string& filePath );
		void LoadFromJSON( const rapidjson::Value& object );

		void Resize( const Vec2s& size );
		void Resize( short x, short y );
		short GetWidth() const;
		short GetHeight() const;
		Vec2s GetSize() const;
		static bool IsValidSize( const Vec2s& size );
		static bool IsValidSize( short x, short y );
		bool IsValid() const;

		void SetTile( const Vec2s& tilePos, const Tile& tile );
		void SetTile( short x, short y, const Tile& tile );
		Iterator GetTile( const Vec2s& tilePos );
		Iterator GetTile( short x, short y );
		ConstIterator GetTile( const Vec2s& tilePos ) const;
		ConstIterator GetTile( short x, short y ) const;
		bool IsValidTilePos( const Vec2s& tilePos ) const;
		bool IsValidTilePos( short x, short y ) const;

		void ForEachTile( ForEachTileCallback callback );
		void ForEachTile( ForEachConstTileCallback callback ) const;
		void ForEachTileInArea( const RectS& area, ForEachTileCallback callback );
		void ForEachTileInArea( const RectS& area, ForEachConstTileCallback ) const;

		Unit* SpawnUnit();
		void DestroyUnit( Unit* unit );

		void ForEachUnit( ForEachUnitCallback callback );
		void ForEachUnit( ForEachConstUnitCallback callback ) const;

		void Fill( const Tile& tile );
		void Fill( const Tile& tile, const RectS& area );

		void SetTileChangedCallback( const TileChangedCallback& callback );
		void ClearTileChangedCallback();
		TileChangedCallback GetTileChangedCallback() const;
		bool HasTileChangedCallback() const;

		Scenario* GetScenario() const;

	private:
		size_t GetTileIndex( const Vec2s& tilePos ) const;

		bool mIsInitialized;
		Scenario* mScenario;
		TileChangedCallback mTileChangedCallback;
		Vec2s mSize;
		std::vector< Unit* > mUnits;
		Tile mTiles[ MAX_TILES ];
	};
}
