#pragma once

namespace mage
{
	static const size_t PRIMARY_DIRECTION_COUNT  = 8;
	static const size_t CARDINAL_DIRECTION_COUNT = 4;
	static const size_t ORDINAL_DIRECTION_COUNT  = 4;

	/**
	 * Represents a primary direction (i.e. north, southeast, east).
	 */
	class PrimaryDirection
	{
	public:
		static const PrimaryDirection NONE;
		static const PrimaryDirection EAST;
		static const PrimaryDirection NORTHEAST;
		static const PrimaryDirection NORTH;
		static const PrimaryDirection NORTHWEST;
		static const PrimaryDirection WEST;
		static const PrimaryDirection SOUTHWEST;
		static const PrimaryDirection SOUTH;
		static const PrimaryDirection SOUTHEAST;

		PrimaryDirection();
		PrimaryDirection( const PrimaryDirection& other );
		~PrimaryDirection();

		bool IsValid() const;
		bool IsCardinal() const;
		bool IsOrdinal() const;

		bool operator==( PrimaryDirection other ) const;
		bool operator!=( PrimaryDirection other ) const;
		bool operator<( PrimaryDirection other ) const;
		bool operator<=( PrimaryDirection other ) const;
		bool operator>( PrimaryDirection other ) const;
		bool operator>=( PrimaryDirection other ) const;

		const HashString& GetName() const;
		Vec2s GetOffset() const;
		PrimaryDirection GetOppositeDirection() const;
		unsigned char GetIndex() const;

		static PrimaryDirection GetDirectionByName( const HashString& directionName );

	private:
		enum Direction
		{
			DIRECTION_NONE,
			DIRECTION_EAST,
			DIRECTION_NORTHEAST,
			DIRECTION_NORTH,
			DIRECTION_NORTHWEST,
			DIRECTION_WEST,
			DIRECTION_SOUTHWEST,
			DIRECTION_SOUTH,
			DIRECTION_SOUTHEAST
		};

		struct DirectionInfo
		{
			DirectionInfo( const HashString& name, const Vec2s& offset, Direction oppositeDirection ) :
				name( name ), offset( offset ), oppositeDirection( oppositeDirection )
			{ }

			unsigned char oppositeDirection;
			HashString name;
			Vec2s offset;
		};

		static const size_t DIRECTION_COUNT = 9;
		static const DirectionInfo DIRECTION_INFO[ DIRECTION_COUNT ];
		static const DirectionInfo& GetDirectionInfo( unsigned char index );

		PrimaryDirection( unsigned char index );

		unsigned char mIndex;
	};


	extern const PrimaryDirection PRIMARY_DIRECTIONS[ PRIMARY_DIRECTION_COUNT ];
	extern const PrimaryDirection CARDINAL_DIRECTIONS[ CARDINAL_DIRECTION_COUNT ];
	extern const PrimaryDirection ORDINAL_DIRECTIONS[ ORDINAL_DIRECTION_COUNT ];


	inline PrimaryDirection::PrimaryDirection() :
		mIndex( DIRECTION_NONE )
	{ }


	inline PrimaryDirection::PrimaryDirection( const PrimaryDirection& other ) :
		mIndex( other.mIndex )
	{ }


	inline PrimaryDirection::~PrimaryDirection() { }


	inline PrimaryDirection::PrimaryDirection( unsigned char index ) :
		mIndex( index )
	{ }


	inline bool PrimaryDirection::IsValid() const
	{
		return ( mIndex > 0 && mIndex < DIRECTION_COUNT );
	}


	inline bool PrimaryDirection::IsCardinal() const
	{
		// If the index is odd, this is a cardinal direction.
		return ( mIndex & 1 ) && IsValid();
	}


	inline bool PrimaryDirection::IsOrdinal() const
	{
		// If the index is even, this is an ordinal direction.
		return !( mIndex & 1 ) && IsValid();
	}


	inline bool PrimaryDirection::operator==( PrimaryDirection other ) const
	{
		return ( mIndex == other.mIndex );
	}


	inline bool PrimaryDirection::operator!=( PrimaryDirection other ) const
	{
		return ( mIndex != other.mIndex );
	}


	inline bool PrimaryDirection::operator<( PrimaryDirection other ) const
	{
		return ( mIndex < other.mIndex );
	}


	inline bool PrimaryDirection::operator<=( PrimaryDirection other ) const
	{
		return ( mIndex <= other.mIndex );
	}


	inline bool PrimaryDirection::operator>( PrimaryDirection other ) const
	{
		return ( mIndex > other.mIndex );
	}


	inline bool PrimaryDirection::operator>=( PrimaryDirection other ) const
	{
		return ( mIndex >= other.mIndex );
	}


	inline const HashString& PrimaryDirection::GetName() const
	{
		return GetDirectionInfo( mIndex ).name;
	}


	inline Vec2s PrimaryDirection::GetOffset() const
	{
		return GetDirectionInfo( mIndex ).offset;
	}


	inline PrimaryDirection PrimaryDirection::GetOppositeDirection() const
	{
		return PrimaryDirection( GetDirectionInfo( mIndex ).oppositeDirection );
	}


	inline unsigned char PrimaryDirection::GetIndex() const
	{
		return mIndex;
	}


	inline PrimaryDirection PrimaryDirection::GetDirectionByName( const HashString& directionName )
	{
		PrimaryDirection result = NONE;

		for( unsigned char index = 0; index < PRIMARY_DIRECTION_COUNT; ++index )
		{
			PrimaryDirection direction = PRIMARY_DIRECTIONS[ index ];

			if( direction.GetName() == directionName )
			{
				// Find the PrimaryDirection with the specified name (if any).
				result = direction;
				break;
			}
		}

		return result;
	}


	inline const PrimaryDirection::DirectionInfo& PrimaryDirection::GetDirectionInfo( unsigned char index )
	{
		assertion( index >= 0 && index < DIRECTION_COUNT, "Cannot get PrimaryDirection info for invalid index %d!", index );
		return DIRECTION_INFO[ index ];
	}
}
