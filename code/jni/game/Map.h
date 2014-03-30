#pragma once

namespace mage
{
	class Tile
	{
	public:
		Tile();
		Tile( const Tile& other );
		~Tile();

		void operator=( const Tile& other );

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

	public:
		Event<> OnChanged;
	};


	/**
	 * Holds information about each tile on the map and allows tile data to be manipulated.
	 */
	class Map : public Grid< Tile, MAP_SIZE_POWER_OF_TWO >
	{
	public:
		typedef Delegate< void, const Map::Iterator& > OnTileChangedCallback;

		typedef Delegate< void, Unit* > ForEachUnitCallback;
		typedef Delegate< void, const Unit* > ForEachConstUnitCallback;

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

		Unit* SpawnUnit();
		void DestroyUnit( Unit* unit );

		void ForEachUnit( ForEachUnitCallback callback );
		void ForEachUnit( ForEachConstUnitCallback callback ) const;

		Scenario* GetScenario() const;

	private:
		void TileChanged( const Iterator& tile );

		bool mIsInitialized;
		Scenario* mScenario;
		std::vector< Unit* > mUnits;

	public:
		Event< const Iterator& > OnTileChanged;

		friend class Tile;
	};
}
