#pragma once

namespace mage
{
	/**
	 * Represents a path a Unit can take through a TileMap.
	 */
	class Path
	{
	public:
		Path();
		~Path();

		void AddWaypoint( const Vec2i& waypoint );
		Vec2i& GetWaypoint( size_t index );
		const Vec2i& GetWaypoint( size_t index ) const;
		Vec2i& GetNextWaypoint();
		const Vec2i& GetNextWaypoint() const;
		Vec2i& GetDestination();
		const Vec2i& GetDestination() const;
		Vec2i PopNextWaypoint();
		void Clear();

		size_t GetNumWaypoints() const;
		bool IsValid() const;

	protected:
		std::vector< Vec2i > mWaypoints;
	};


	inline Path::Path() { }


	inline Path::~Path() { }


	inline void Path::AddWaypoint( const Vec2i& waypoint )
	{
		mWaypoints.push_back( waypoint );
	}


	inline Vec2i& Path::GetWaypoint( size_t index )
	{
		size_t numWaypoints = GetNumWaypoints();
		assertion( index < numWaypoints, "Waypoint index %d is out of bounds! (%d elements)", index, numWaypoints );
		return mWaypoints[ numWaypoints - index - 1 ];
	}


	inline const Vec2i& Path::GetWaypoint( size_t index ) const
	{
		assertion( index < GetNumWaypoints(), "Waypoint index %d is out of bounds! (%d elements)", index, GetNumWaypoints() );
		return mWaypoints[ index ];
	}


	inline Vec2i& Path::GetNextWaypoint()
	{
		return mWaypoints.back();
	}


	inline const Vec2i& Path::GetNextWaypoint() const
	{
		return mWaypoints.back();
	}


	inline Vec2i& Path::GetDestination()
	{
		return mWaypoints.front();
	}


	inline const Vec2i& Path::GetDestination() const
	{
		return mWaypoints.front();
	}


	inline Vec2i Path::PopNextWaypoint()
	{
		Vec2i waypoint = mWaypoints.back();
		mWaypoints.pop_back();
		return waypoint;
	}


	inline void Path::Clear()
	{
		mWaypoints.clear();
	}


	inline size_t Path::GetNumWaypoints() const
	{
		return mWaypoints.size();
	}


	inline bool Path::IsValid() const
	{
		return ( GetNumWaypoints() > 0 );
	}
}
