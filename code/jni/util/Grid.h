#pragma once

namespace mage
{
#define MAGE_GRID_TEMPLATE \
	template< typename TileType, size_t MaxSizePowerOfTwo >

#define MAGE_GRID \
	Grid< TileType, MaxSizePowerOfTwo >

#define MAGE_GRID_BASIC_ITERATOR_TEMPLATE \
	MAGE_GRID_TEMPLATE \
	template< class IteratorType, class GridType, class DataType >

#define MAGE_GRID_BASIC_ITERATOR \
	MAGE_GRID::BasicIterator< IteratorType, GridType, DataType >

#define MAGE_GRID_ITERATOR_BASE \
	BasicIterator< Iterator, MAGE_GRID, TileType >

#define MAGE_GRID_CONST_ITERATOR_BASE \
	BasicIterator< ConstIterator, const MAGE_GRID, const TileType >


	/**
	 * Data structure for storing information in a grid.
	 */
	template< typename TileType, size_t MaxSizePowerOfTwo >
	class Grid
	{
	private:
		template< class IteratorType, class GridType, class DataType >
		class BasicIterator
		{
		protected:
			BasicIterator();
			BasicIterator( GridType* grid, const Vec2s& tilePos = Vec2s( -1, -1 ) );
			~BasicIterator();

		public:
			typedef Delegate< void, const IteratorType& > ForEachAdjacentCallback;

			IteratorType GetAdjacent( PrimaryDirection direction ) const;
			void ForEachAdjacent( ForEachAdjacentCallback callback ) const;

			DataType& operator*() const;
			DataType* operator->() const;

			bool operator==( const IteratorType& other ) const;
			bool operator!=( const IteratorType& other ) const;
			bool operator>( const IteratorType& other ) const;
			bool operator>=( const IteratorType& other ) const;
			bool operator<( const IteratorType& other ) const;
			bool operator<=( const IteratorType& other ) const;

			void operator+=( const Vec2s& tileOffset );
			void operator-=( const Vec2s& tileOffset );

			IteratorType operator+( const Vec2s& tileOffset ) const;
			IteratorType operator-( const Vec2s& tileOffset ) const;

			GridType* GetGrid() const;
			Vec2s GetPosition() const;
			short GetX() const;
			short GetY() const;
			size_t GetIndex() const;
			bool IsValid() const;

		private:
			GridType* mGrid;
			Vec2s mTilePos;
		};

	public:
		class Iterator : public MAGE_GRID_ITERATOR_BASE
		{
		public:
			Iterator() : MAGE_GRID_ITERATOR_BASE() { }
			Iterator( MAGE_GRID* grid ) : MAGE_GRID_ITERATOR_BASE( grid ) { }
			Iterator( MAGE_GRID* grid, const Vec2s& tilePos ) : MAGE_GRID_ITERATOR_BASE( grid, tilePos ) { }
			Iterator( MAGE_GRID* grid, short x, short y ) : MAGE_GRID_ITERATOR_BASE( grid, Vec2s( x, y ) ) { }
		};

		class ConstIterator : public MAGE_GRID_CONST_ITERATOR_BASE
		{
		public:
			ConstIterator() : MAGE_GRID_CONST_ITERATOR_BASE() { }
			ConstIterator( const MAGE_GRID* grid ) : MAGE_GRID_CONST_ITERATOR_BASE( grid ) { }
			ConstIterator( const MAGE_GRID* grid, const Vec2s& tilePos ) : MAGE_GRID_CONST_ITERATOR_BASE( grid, tilePos ) { }
			ConstIterator( const MAGE_GRID* grid, short x, short y ) : MAGE_GRID_CONST_ITERATOR_BASE( grid, Vec2s( x, y ) ) { }
			ConstIterator( const Iterator& iterator ) : MAGE_GRID_CONST_ITERATOR_BASE( iterator.GetGrid(), iterator.GetPosition() ) { }
		};

		static const size_t MAX_SIZE_POWER_OF_TWO = MaxSizePowerOfTwo;
		static const short MAX_SIZE = ( 1 << MAX_SIZE_POWER_OF_TWO );
		static const size_t MAX_TILES = ( (size_t) MAX_SIZE * (size_t) MAX_SIZE );

		typedef Delegate< void, const Vec2s&, const Vec2s& > OnResizeCallback;
		typedef Delegate< void, const Iterator& > ForEachTileCallback;
		typedef Delegate< void, const ConstIterator& > ForEachConstTileCallback;

		static Vec2s GetAdjacentTilePos( short x, short y, PrimaryDirection direction );
		static Vec2s GetAdjacentTilePos( const Vec2s& tilePos, PrimaryDirection direction );

		Grid();
		~Grid();

		static RectS GetMaxBounds();
		void Resize( const Vec2s& size );
		void Resize( short x, short y );
		short GetWidth() const;
		short GetHeight() const;
		Vec2s GetSize() const;
		static bool IsValidSize( const Vec2s& size );
		static bool IsValidSize( short x, short y );
		bool IsValid() const;

		void SetTile( const Vec2s& tilePos, const TileType& tile );
		void SetTile( short x, short y, const TileType& tile );
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
		void ForEachTileInMaxArea( ForEachTileCallback callback );
		void ForEachTileInMaxArea( ForEachConstTileCallback ) const;

		void Fill( const TileType& tile );
		void Fill( const TileType& tile, const RectS& area );
		void FillMaxArea( const TileType& tile );

		static size_t GetTileIndex( const Vec2s& tilePos );
		static size_t GetTileIndex( short x, short y );

	private:
		Vec2s mSize;

	public:
		Event< const Vec2f&, const Vec2f& > OnResize;

	private:
		std::vector< Unit* > mUnits;
		TileType mTiles[ MAX_TILES ];
	};


	MAGE_GRID_BASIC_ITERATOR_TEMPLATE
	MAGE_GRID_BASIC_ITERATOR::BasicIterator() :
		mGrid( nullptr ), mTilePos( -1, -1 )
	{ }


	MAGE_GRID_BASIC_ITERATOR_TEMPLATE
	MAGE_GRID_BASIC_ITERATOR::BasicIterator( GridType* grid, const Vec2s& tilePos ) :
		mGrid( grid ), mTilePos( tilePos )
	{
		assertion( mGrid, "Cannot create Grid iterator without a valid Grid reference!" );
	}


	MAGE_GRID_BASIC_ITERATOR_TEMPLATE
	MAGE_GRID_BASIC_ITERATOR::~BasicIterator() { }


	MAGE_GRID_BASIC_ITERATOR_TEMPLATE
	IteratorType MAGE_GRID_BASIC_ITERATOR::GetAdjacent( PrimaryDirection direction ) const
	{
		return IteratorType( mGrid, GetAdjacentTilePos( mTilePos, direction ) );
	}


	MAGE_GRID_BASIC_ITERATOR_TEMPLATE
	void MAGE_GRID_BASIC_ITERATOR::ForEachAdjacent( ForEachAdjacentCallback callback ) const
	{
		for( size_t i = 0; i < PRIMARY_DIRECTION_COUNT; ++i )
		{
			// For each PrimaryDirection, get the adjacent tile.
			PrimaryDirection direction = PRIMARY_DIRECTIONS[ i ];
			IteratorType adjacent = GetAdjacent( direction );

			if( adjacent.IsValid() )
			{
				// If the adjacent tile is valid, call the callback.
				callback.Invoke( adjacent );
			}
		}
	}


	MAGE_GRID_BASIC_ITERATOR_TEMPLATE
	DataType& MAGE_GRID_BASIC_ITERATOR::operator*() const
	{
		assertion( mGrid, "Cannot get data for Iterator without a valid Grid reference!" );
		return mGrid->mTiles[ mGrid->GetTileIndex( mTilePos ) ];
	}


	MAGE_GRID_BASIC_ITERATOR_TEMPLATE
	DataType* MAGE_GRID_BASIC_ITERATOR::operator->() const
	{
		return &( operator*() );
	}


	MAGE_GRID_BASIC_ITERATOR_TEMPLATE
	bool MAGE_GRID_BASIC_ITERATOR::operator==( const IteratorType& other ) const
	{
		return ( mGrid == other.mGrid ) && ( mTilePos == other.mTilePos );
	}


	MAGE_GRID_BASIC_ITERATOR_TEMPLATE
	bool MAGE_GRID_BASIC_ITERATOR::operator!=( const IteratorType& other ) const
	{
		return ( mGrid != other.mGrid ) || ( mTilePos != other.mTilePos );
	}


	MAGE_GRID_BASIC_ITERATOR_TEMPLATE
	bool MAGE_GRID_BASIC_ITERATOR::operator>( const IteratorType& other ) const
	{
		return ( mGrid > other.mGrid ) || ( ( mGrid == other.mGrid ) && ( mTilePos > other.mTilePos ) );
	}


	MAGE_GRID_BASIC_ITERATOR_TEMPLATE
	bool MAGE_GRID_BASIC_ITERATOR::operator>=( const IteratorType& other ) const
	{
		return ( mGrid > other.mGrid ) || ( ( mGrid == other.mGrid ) && ( mTilePos >= other.mTilePos ) );
	}


	MAGE_GRID_BASIC_ITERATOR_TEMPLATE
	bool MAGE_GRID_BASIC_ITERATOR::operator<( const IteratorType& other ) const
	{
		return ( mGrid < other.mGrid ) || ( ( mGrid == other.mGrid ) && ( mTilePos < other.mTilePos ) );
	}


	MAGE_GRID_BASIC_ITERATOR_TEMPLATE
	bool MAGE_GRID_BASIC_ITERATOR::operator<=( const IteratorType& other ) const
	{
		return ( mGrid < other.mGrid ) || ( ( mGrid == other.mGrid ) && ( mTilePos <= other.mTilePos ) );
	}


	MAGE_GRID_BASIC_ITERATOR_TEMPLATE
	void MAGE_GRID_BASIC_ITERATOR::operator+=( const Vec2s& tileOffset )
	{
		mTilePos += tileOffset;
	}


	MAGE_GRID_BASIC_ITERATOR_TEMPLATE
	void MAGE_GRID_BASIC_ITERATOR::operator-=( const Vec2s& tileOffset )
	{
		mTilePos -= tileOffset;
	}


	MAGE_GRID_BASIC_ITERATOR_TEMPLATE
	IteratorType MAGE_GRID_BASIC_ITERATOR::operator+( const Vec2s& tileOffset ) const
	{
		return IteratorType( mGrid, mTilePos + tileOffset );
	}


	MAGE_GRID_BASIC_ITERATOR_TEMPLATE
	IteratorType MAGE_GRID_BASIC_ITERATOR::operator-( const Vec2s& tileOffset ) const
	{
		return IteratorType( mGrid, mTilePos - tileOffset );
	}


	MAGE_GRID_BASIC_ITERATOR_TEMPLATE
	GridType* MAGE_GRID_BASIC_ITERATOR::GetGrid() const
	{
		return mGrid;
	}


	MAGE_GRID_BASIC_ITERATOR_TEMPLATE
	Vec2s MAGE_GRID_BASIC_ITERATOR::GetPosition() const
	{
		return mTilePos;
	}


	MAGE_GRID_BASIC_ITERATOR_TEMPLATE
	short MAGE_GRID_BASIC_ITERATOR::GetX() const
	{
		return mTilePos.x;
	}


	MAGE_GRID_BASIC_ITERATOR_TEMPLATE
	short MAGE_GRID_BASIC_ITERATOR::GetY() const
	{
		return mTilePos.y;
	}


	MAGE_GRID_BASIC_ITERATOR_TEMPLATE
	size_t MAGE_GRID_BASIC_ITERATOR::GetIndex() const
	{
		return MAGE_GRID::GetTileIndex( mTilePos );
	}


	MAGE_GRID_BASIC_ITERATOR_TEMPLATE
	bool MAGE_GRID_BASIC_ITERATOR::IsValid() const
	{
		return ( mGrid && mGrid->IsValidTilePos( mTilePos ) );
	}


	MAGE_GRID_TEMPLATE
	Vec2s MAGE_GRID::GetAdjacentTilePos( short x, short y, PrimaryDirection direction )
	{
		return GetAdjacentTilePos( Vec2s( x, y ), direction );
	}


	MAGE_GRID_TEMPLATE
	Vec2s MAGE_GRID::GetAdjacentTilePos( const Vec2s& tilePos, PrimaryDirection direction )
	{
		return ( tilePos + direction.GetOffset() );
	}


	MAGE_GRID_TEMPLATE
	MAGE_GRID::Grid() :
		mSize( 0, 0 )
	{ }


	MAGE_GRID_TEMPLATE
	MAGE_GRID::~Grid() { }


	MAGE_GRID_TEMPLATE
	RectS MAGE_GRID::GetMaxBounds()
	{
		return RectS( 0, 0, MAX_SIZE, MAX_SIZE );
	}


	MAGE_GRID_TEMPLATE
	void MAGE_GRID::Resize( const Vec2s& size )
	{
		Resize( size.x, size.y );
	}


	MAGE_GRID_TEMPLATE
	void MAGE_GRID::Resize( short x, short y )
	{
		assertion( IsValidSize( x, y ), "Cannot resize Map to invalid size (%d,%d)!", x, y );

		// Set the size of the map.
		Vec2s oldSize( mSize );
		mSize.Set( x, y );

		// Fire the resized event.
		OnResize.Invoke( oldSize, mSize );
	}


	MAGE_GRID_TEMPLATE
	short MAGE_GRID::GetWidth() const
	{
		return mSize.x;
	}


	MAGE_GRID_TEMPLATE
	short MAGE_GRID::GetHeight() const
	{
		return mSize.y;
	}


	MAGE_GRID_TEMPLATE
	Vec2s MAGE_GRID::GetSize() const
	{
		return mSize;
	}


	MAGE_GRID_TEMPLATE
	bool MAGE_GRID::IsValidSize( const Vec2s& size )
	{
		return ( size.x > 0 && size.y > 0 && size.x <= MAX_SIZE && size.y <= MAX_SIZE );
	}


	MAGE_GRID_TEMPLATE
	bool MAGE_GRID::IsValidSize( short x, short y )
	{
		return ( x > 0 && y > 0 && x <= MAX_SIZE && y <= MAX_SIZE );
	}


	MAGE_GRID_TEMPLATE
	bool MAGE_GRID::IsValid() const
	{
		return IsValidSize( mSize );
	}


	MAGE_GRID_TEMPLATE
	void MAGE_GRID::SetTile( const Vec2s& tilePos, const TileType& tile )
	{
		Iterator iterator = GetTile( tilePos );
		( *iterator ) = tile;
	}


	MAGE_GRID_TEMPLATE
	void MAGE_GRID::SetTile( short x, short y, const TileType& tile )
	{
		Iterator iterator = GetTile( x, y );
		( *iterator ) = tile;
	}


	MAGE_GRID_TEMPLATE
	typename MAGE_GRID::Iterator MAGE_GRID::GetTile( const Vec2s& tilePos )
	{
		return Iterator( this, tilePos );
	}


	MAGE_GRID_TEMPLATE
	typename MAGE_GRID::Iterator MAGE_GRID::GetTile( short x, short y )
	{
		return Iterator( this, x, y );
	}


	MAGE_GRID_TEMPLATE
	typename MAGE_GRID::ConstIterator MAGE_GRID::GetTile( const Vec2s& tilePos ) const
	{
		return ConstIterator( this, tilePos );
	}


	MAGE_GRID_TEMPLATE
	typename MAGE_GRID::ConstIterator MAGE_GRID::GetTile( short x, short y ) const
	{
		return ConstIterator( this, x, y );
	}


	MAGE_GRID_TEMPLATE
	bool MAGE_GRID::IsValidTilePos( const Vec2s& tilePos ) const
	{
		return IsValidTilePos( tilePos.x, tilePos.y );
	}


	MAGE_GRID_TEMPLATE
	bool MAGE_GRID::IsValidTilePos( short x, short y ) const
	{
		return ( x >= 0 && x < mSize.x && y >= 0 && y < mSize.y );
	}


	MAGE_GRID_TEMPLATE
	void MAGE_GRID::ForEachTile( ForEachTileCallback callback )
	{
		RectS area( 0, 0, mSize.x, mSize.y );
		ForEachTileInArea( area, callback );
	}


	MAGE_GRID_TEMPLATE
	void MAGE_GRID::ForEachTile( ForEachConstTileCallback callback ) const
	{
		RectS area( 0, 0, mSize.x, mSize.y );
		ForEachTileInArea( area, callback );
	}


	MAGE_GRID_TEMPLATE
	void MAGE_GRID::ForEachTileInArea( const RectS& area, ForEachTileCallback callback )
	{
		assertion( area.IsValid(), "Cannot run Tile callback on invalid area (%d,%d,%d,%d)!", area.Left, area.Top, area.Right, area.Bottom );
		assertion( area.Left >= 0 && area.Top >= 0 && area.Right <= MAX_SIZE && area.Bottom <= MAX_SIZE, "Cannot run Tile callback on area (%d,%d,%d,%d) because it extends outside the max Grid bounds!", area.Left, area.Top, area.Right, area.Bottom );
		assertion( callback.IsValid(), "Cannot run Tile callback on area (%d,%d,%d,%d) because the callback is invalid!", area.Left, area.Top, area.Right, area.Bottom );

		for( short y = area.Top; y < area.Bottom; ++y )
		{
			for( short x = area.Left; x < area.Right; ++x )
			{
				// Fire the callback for each Tile.
				callback.Invoke( Iterator( this, x, y ) );
			}
		}
	}


	MAGE_GRID_TEMPLATE
	void MAGE_GRID::ForEachTileInArea( const RectS& area, ForEachConstTileCallback callback ) const
	{
		assertion( area.IsValid(), "Cannot run Tile callback on invalid area (%d,%d,%d,%d)!", area.Left, area.Top, area.Right, area.Bottom );
		assertion( area.Left >= 0 && area.Top >= 0 && area.Right <= MAX_SIZE && area.Bottom <= MAX_SIZE, "Cannot run Tile callback on area (%d,%d,%d,%d) because it extends outside the max Grid bounds!", area.Left, area.Top, area.Right, area.Bottom );
		assertion( callback.IsValid(), "Cannot run Tile callback on area (%d,%d,%d,%d) because the callback is invalid!", area.Left, area.Top, area.Right, area.Bottom );

		for( short y = area.Top; y < area.Bottom; ++y )
		{
			for( short x = area.Left; x < area.Right; ++x )
			{
				// Fire the callback for each Tile.
				callback.Invoke( ConstIterator( this, x, y ) );
			}
		}
	}


	MAGE_GRID_TEMPLATE
	void MAGE_GRID::ForEachTileInMaxArea( ForEachTileCallback callback )
	{
		// Run the callback on every tile in the max area.
		ForEachTileInArea( GetMaxBounds(), callback );
	}


	MAGE_GRID_TEMPLATE
	void MAGE_GRID::ForEachTileInMaxArea( ForEachConstTileCallback callback ) const
	{
		// Run the callback on every tile in the max area.
		ForEachTileInArea( GetMaxBounds(), callback );
	}


	MAGE_GRID_TEMPLATE
	void MAGE_GRID::Fill( const TileType& tile )
	{
		RectS area( 0, 0, mSize.x, mSize.y );
		Fill( tile, area );
	}


	MAGE_GRID_TEMPLATE
	void MAGE_GRID::Fill( const TileType& tile, const RectS& area )
	{
		ForEachTileInArea( area, [ this, &tile ]( const Iterator& it )
		{
			// Fill each Tile in the area.
			( *it ) = tile;
		});
	}


	MAGE_GRID_TEMPLATE
	void MAGE_GRID::FillMaxArea( const TileType& tile )
	{
		ForEachTileInMaxArea( [ this, &tile ]( const Iterator& it )
		{
			// Fill each Tile in the max area.
			( *it ) = tile;
		});
	}


	MAGE_GRID_TEMPLATE
	size_t MAGE_GRID::GetTileIndex( const Vec2s& tilePos )
	{
		return GetTileIndex( tilePos.x, tilePos.y );
	}


	MAGE_GRID_TEMPLATE
	size_t MAGE_GRID::GetTileIndex( short x, short y )
	{
		assertion( x >= 0 && x < MAX_SIZE && y >= 0 && y < MAX_SIZE, "Cannot get tile index for invalid tile position (%d,%d)!", x, y );
		return ( ( y << MAX_SIZE_POWER_OF_TWO ) + x );
	}
}
