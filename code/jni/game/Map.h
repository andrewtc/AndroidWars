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
			ConstIterator GetAdjacent( CardinalDirection direction ) const;

			const Tile* operator->() const;

			bool IsValid() const;

		private:
			ConstIterator( const Map* map );
			ConstIterator( const Map* map, const Vec2s& tilePos );
			ConstIterator( const Map* map, short x, short y );
			~ConstIterator();

			const Map* mMap;
			short mX, mY;
		};

		class Iterator
		{
		public:
			Iterator GetAdjacent( CardinalDirection direction ) const;

			Tile* operator->() const;

			bool IsValid() const;

		private:
			Iterator( Map* map );
			Iterator( const Map* map, const Vec2s& tilePos );
			Iterator( Map* map, short x, short y );
			~Iterator();

			Map* mMap;
			short mX, mY;
		};

		static const short MAX_SIZE_EXPONENT = 10;
		static const short MAX_SIZE = ( 1 << MAX_SIZE_EXPONENT );

		typedef Callback< void, short, short > TileChangedCallback;

		Map( Database* database );
		~Map();

		void SaveToJSON( rapidjson::Document& document, rapidjson::Value& object );

		void LoadFromFile( const std::string& filePath );
		void LoadFromJSON( const rapidjson::Value& object );

		Iterator GetTile( const Vec2s& tilePos );
		Iterator GetTile( short x, short y );
		ConstIterator GetTile( const Vec2s& tilePos ) const;
		ConstIterator GetTile( short x, short y ) const;

		void SetTileChangedCallback( const TileChangedCallback& callback );
		void ClearTileChangedCallback();
		TileChangedCallback GetTileChangedCallback() const;
		bool HasTileChangedCallback() const;

	private:
		static const size_t MAX_TILES = ( (size_t) MAX_SIZE * (size_t) MAX_SIZE );

		bool IsValidTilePos( short x, short y ) const;
		size_t GetTileIndex( short x, short y ) const;

		short mWidth, mHeight;
		TileChangedCallback mTileChangedCallback;
		Database* mDatabase;
		Tile mTiles[ MAX_TILES ];
	};
}
