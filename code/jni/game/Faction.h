#pragma once

namespace mage
{
	class Faction
	{
	public:
		typedef std::set< Unit* > Units;
		typedef std::set< Map::Iterator > Tiles;

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

	private:
		Faction( Map* map );
		~Faction();

		void OnTurnStart( int turnIndex );
		void OnTurnEnd( int turnIndex );

		void UnitGained( Unit* unit );
		void UnitLost( Unit* unit );

		void TileGained( const Map::Iterator& tile );
		void TileLost( const Map::Iterator& tile );

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
