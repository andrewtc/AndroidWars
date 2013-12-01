#pragma once

namespace mage
{
	/**
	 * Represents a Unit on the game board.
	 */
	class Unit : public MapObject
	{
		DECLARE_RTTI;
	public:
		Unit( const std::string& name );
		Unit( UnitType* unitType );
		virtual ~Unit();

		void Init();

		virtual void OnLoadProperty( const std::string& name, const std::string& value );
		virtual void OnLoadFinished();
		virtual void OnDraw( const Camera& camera ) const;

		UnitType* GetUnitType() const;
		void SetTilePos( int x, int y );
		void SetTilePos( const Vec2i& tilePos );
		Vec2i GetTilePos() const;

		bool DrawSelected;

	protected:
		UnitType* mUnitType;
		Vec2i mTilePos;
		Sprite* mSprite;
	};


	inline UnitType* Unit::GetUnitType() const
	{
		return mUnitType;
	}


	inline void Unit::SetTilePos( int x, int y )
	{
		SetTilePos( Vec2i( x, y ) );
	}


	inline void Unit::SetTilePos( const Vec2i& tilePos )
	{
		mTilePos = tilePos;
	}


	inline Vec2i Unit::GetTilePos() const
	{
		return mTilePos;
	}
}
