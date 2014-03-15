#pragma once

namespace mage
{
	class TerrainType;

	/**
	 * Table that keeps track of TerrainTypes.
	 */
	class TerrainTypesTable : public Table< TerrainTypesTable, TerrainType >
	{
	public:
		TerrainTypesTable( Database* database );
		virtual ~TerrainTypesTable();

	protected:
		virtual void OnLoadRecordFromXml( TerrainType* terrainType, XmlReader::XmlReaderIterator xmlIterator );
		virtual void OnLoadRecordFromJSON( TerrainType* terrainType, const rapidjson::Value& object );
	};
}
