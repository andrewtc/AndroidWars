#pragma once

namespace mage
{
	class Faction
	{
	public:
		static const float INACTIVE_COLOR_VALUE_SHIFT;

		typedef std::set< Unit* > Units;
		typedef std::set< Map::Iterator > Tiles;

		void SaveToJSON( rapidjson::Document& document, rapidjson::Value& value );
		void LoadFromJSON( const rapidjson::Value& object );

		Map* GetMap() const;

		void SetFunds( int funds );
		void AddFunds( int funds );
		int CalculateIncome() const;
		int GetFunds() const;

		const Units& GetUnits() const;
		size_t GetUnitCount() const;
		bool HasUnits() const;

		const Tiles& GetTiles() const;
		size_t GetTileCount() const;
		bool HasTiles() const;

		void SetHeadquarters( Map::Iterator tile );
		void ClearHeadquarters();
		Map::Iterator GetHeadquarters() const;
		bool HasHeadquarters() const;

		void SetColor( const Color& color );
		Color GetColor() const;
		Color GetInactiveColor() const;

		void SetControllable( bool controllable );
		bool IsControllable() const;

	private:
		Faction( Map* map );
		~Faction();

		void OnTurnStart( int turnIndex );
		void OnTurnEnd( int turnIndex );

		void UnitGained( Unit* unit );
		void UnitLost( Unit* unit );

		void TileGained( const Map::Iterator& tile );
		void TileLost( const Map::Iterator& tile );

		bool mIsControllable;
		int mFunds;
		Map* mMap;
		Color mColor;
		Map::Iterator mHeadquarters;
		Units mUnits;
		Tiles mTiles;

	public:
		Event< const Color& > OnColorChanged;

		friend class Game;
		friend class Map;
		friend class Unit;
	};
}
