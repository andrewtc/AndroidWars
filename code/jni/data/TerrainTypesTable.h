#pragma once

namespace mage
{
	class Variation;
	class TerrainType;

	/**
	 * Table that keeps track of TerrainTypes.
	 */
	class TerrainTypesTable : public Table< TerrainTypesTable, TerrainType >
	{
	public:
		TerrainTypesTable( Scenario* scenario );
		virtual ~TerrainTypesTable();

	protected:
		virtual void OnLoadRecordFromJSON( TerrainType* terrainType, const rapidjson::Value& object );

		void LoadVariationFromJSON( TerrainType* terrainType, const rapidjson::Value& object );
		void LoadVariationConditionFromJSON( Variation* variation, const rapidjson::Value& object );
	};
}
