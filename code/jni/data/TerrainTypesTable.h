#pragma once

namespace mage
{
	/**
	 * Table that keeps track of TerrainTypes.
	 */
	class TerrainTypesTable : public Table< TerrainType >
	{
	public:
		TerrainTypesTable( Database* database );
		virtual ~TerrainTypesTable();

	protected:
		virtual const char* GetXmlElementName() const;
		virtual void LoadRecordFromXml( TerrainType* terrainType, XmlReader::XmlReaderIterator xmlIterator );
	};


	inline const char* TerrainTypesTable::GetXmlElementName() const
	{
		return "TerrainType";
	}
}
