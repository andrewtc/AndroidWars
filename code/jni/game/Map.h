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


#define MAGE_MAP_BASIC_ITERATOR_TEMPLATE \
	template< class IteratorType, class MapType, class TileType >

#define MAGE_MAP_BASIC_ITERATOR \
	Map::BasicIterator< IteratorType, MapType, TileType >


	/**
	 * Holds information about each tile on the map and allows tile data to be manipulated.
	 */
	class Map
	{
	private:
		MAGE_MAP_BASIC_ITERATOR_TEMPLATE
		class BasicIterator
		{
		public:
			BasicIterator( MapType* map, const Vec2s& tilePos = Vec2s( -1, -1 ) );
			~BasicIterator();

			IteratorType GetAdjacent( CardinalDirection direction ) const;

			TileType& operator*() const;
			TileType* operator->() const;

			void operator+=( const Vec2s& tileOffset );
			void operator-=( const Vec2s& tileOffset );

			IteratorType operator+( const Vec2s& tileOffset ) const;
			IteratorType operator-( const Vec2s& tileOffset ) const;

			Vec2s GetTilePos() const;
			bool IsValid() const;

		private:
			MapType* mMap;
			Vec2s mTilePos;
		};

	public:
		class ConstIterator : public BasicIterator< ConstIterator, const Map, const Tile >
		{
		public:
			ConstIterator( const Map* map ) : BasicIterator( map ) { };
			ConstIterator( const Map* map, const Vec2s& tilePos ) : BasicIterator( map, tilePos ) { };
			ConstIterator( const Map* map, short x, short y ) : BasicIterator( map, Vec2s( x, y ) ) { };
		};

		class Iterator : public BasicIterator< Iterator, Map, Tile >
		{
		public:
			Iterator( Map* map ) : BasicIterator( map ) { };
			Iterator( Map* map, const Vec2s& tilePos ) : BasicIterator( map, tilePos ) { };
			Iterator( Map* map, short x, short y ) : BasicIterator( map, Vec2s( x, y ) ) { };
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


	MAGE_MAP_BASIC_ITERATOR_TEMPLATE
	MAGE_MAP_BASIC_ITERATOR::BasicIterator( MapType* map, const Vec2s& tilePos ) :
		mMap( map ), mTilePos( tilePos )
	{
		assertion( mMap, "Cannot create Map iterator without a Map reference!" );
	}


	MAGE_MAP_BASIC_ITERATOR_TEMPLATE
	MAGE_MAP_BASIC_ITERATOR::~BasicIterator() { }


	MAGE_MAP_BASIC_ITERATOR_TEMPLATE
	IteratorType MAGE_MAP_BASIC_ITERATOR::GetAdjacent( CardinalDirection direction ) const
	{
		return IteratorType( mMap, GetAdjacentTilePos( mTilePos, direction ) );
	}


	MAGE_MAP_BASIC_ITERATOR_TEMPLATE
	TileType& MAGE_MAP_BASIC_ITERATOR::operator*() const
	{
		return mMap->mTiles[ mMap->GetTileIndex( mTilePos ) ];
	}


	MAGE_MAP_BASIC_ITERATOR_TEMPLATE
	TileType* MAGE_MAP_BASIC_ITERATOR::operator->() const
	{
		return &( operator*() );
	}


	MAGE_MAP_BASIC_ITERATOR_TEMPLATE
	void MAGE_MAP_BASIC_ITERATOR::operator+=( const Vec2s& tileOffset )
	{
		mTilePos += tileOffset;
	}


	MAGE_MAP_BASIC_ITERATOR_TEMPLATE
	void MAGE_MAP_BASIC_ITERATOR::operator-=( const Vec2s& tileOffset )
	{
		mTilePos -= tileOffset;
	}


	MAGE_MAP_BASIC_ITERATOR_TEMPLATE
	IteratorType MAGE_MAP_BASIC_ITERATOR::operator+( const Vec2s& tileOffset ) const
	{
		return IteratorType( mMap, mTilePos + tileOffset );
	}


	MAGE_MAP_BASIC_ITERATOR_TEMPLATE
	IteratorType MAGE_MAP_BASIC_ITERATOR::operator-( const Vec2s& tileOffset ) const
	{
		return IteratorType( mMap, mTilePos - tileOffset );
	}


	MAGE_MAP_BASIC_ITERATOR_TEMPLATE
	Vec2s MAGE_MAP_BASIC_ITERATOR::GetTilePos() const
	{
		return mTilePos;
	}


	MAGE_MAP_BASIC_ITERATOR_TEMPLATE
	bool MAGE_MAP_BASIC_ITERATOR::IsValid() const
	{
		return ( mMap->IsValidTilePos( mTilePos ) && ( *this )->HasTerrainType() );
	}
}
