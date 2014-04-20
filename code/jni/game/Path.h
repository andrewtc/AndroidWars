#pragma once

namespace mage
{
	/**
	 * Represents a path a Unit can take through a TileMap.
	 */
	class Path
	{
	public:
		typedef Delegate< void > OnChangedDelegate;

		Path();
		Path( const Path& other );
		void operator=( const Path& other );
		~Path();

		void SetOrigin( const Vec2s& origin );
		Vec2s GetOrigin() const;

		void AddDirection( PrimaryDirection direction );
		PrimaryDirection GetDirection( size_t index ) const;
		Vec2s GetWaypoint( size_t index ) const;
		int GetIndexOfWaypoint( const Vec2s& waypoint ) const;
		bool ContainsWaypoint( const Vec2s& waypoint ) const;
		Vec2s GetDestination() const;
		void RemoveWaypointsAfterIndex( size_t index );
		void Clear();

		size_t GetLength() const;
		bool IsValid() const;

	protected:
		Vec2s mOrigin;
		std::vector< PrimaryDirection > mDirections;

	public:
		Event<> OnChanged;
	};


	inline Path::Path() { }


	inline Path::Path( const Path& other ) :
		mOrigin( other.mOrigin ),
		mDirections( other.mDirections )
	{
		// Don't copy event callbacks.
	}


	inline void Path::operator=( const Path& other )
	{
		// Don't copy event callbacks.
		mOrigin = other.mOrigin;
		mDirections = other.mDirections;
	}


	inline Path::~Path() { }


	inline void Path::SetOrigin( const Vec2s& origin )
	{
		mOrigin = origin;
		OnChanged.Invoke();
	}


	inline Vec2s Path::GetOrigin() const
	{
		return mOrigin;
	}


	inline void Path::AddDirection( PrimaryDirection direction )
	{
		mDirections.push_back( direction );
		OnChanged.Invoke();
	}


	inline PrimaryDirection Path::GetDirection( size_t index ) const
	{
		size_t length = GetLength();
		assertion( index < length, "Direction index %d is out of bounds! (%d elements)", index, length );
		return mDirections[ index ];
	}


	inline Vec2s Path::GetWaypoint( size_t index ) const
	{
		Vec2s waypoint = mOrigin;

		for( size_t i = 0, length = mDirections.size(); i < index; ++i )
		{
			// Calculate the position of the waypoint at the specified index.
			PrimaryDirection direction = GetDirection( i );
			waypoint += direction.GetOffset();
		}

		return waypoint;
	}


	inline int Path::GetIndexOfWaypoint( const Vec2s& waypoint ) const
	{
		int result = -1;
		Vec2s currentWaypoint = mOrigin;

		for( size_t index = 0, length = mDirections.size(); index < length; ++index )
		{
			if( currentWaypoint == waypoint )
			{
				// If the requested waypoint is found, return true.
				result = (int) index;
				break;
			}

			// Calculate the position of the next waypoint.
			PrimaryDirection direction = GetDirection( index );
			currentWaypoint += direction.GetOffset();
		}

		return result;
	}


	inline bool Path::ContainsWaypoint( const Vec2s& waypoint ) const
	{
		return ( GetIndexOfWaypoint( waypoint ) >= 0 );
	}


	inline Vec2s Path::GetDestination() const
	{
		return GetWaypoint( mDirections.size() - 1 );
	}


	inline void Path::RemoveWaypointsAfterIndex( size_t index )
	{
		// Remove all directions after the specified index.
		assertion( index < mDirections.size(), "Cannot remove waypoints after index because the index (%d) is out of bounds!", index );
		mDirections.erase( mDirections.begin() + index, mDirections.end() );
		OnChanged.Invoke();
	}


	inline void Path::Clear()
	{
		mDirections.clear();
		OnChanged.Invoke();
	}


	inline size_t Path::GetLength() const
	{
		return mDirections.size();
	}


	inline bool Path::IsValid() const
	{
		return ( GetLength() > 0 );
	}
}
