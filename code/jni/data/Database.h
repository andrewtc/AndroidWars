#pragma once

namespace mage
{
	/**
	 * Stores all game data.
	 */
	class Database
	{
	public:
		Database();
		~Database();

		void LoadDataFromFile( const char* file );
		void LoadDataFromXML( XmlReader::XmlReaderIterator rootIterator );
		void ClearData();

		TerrainTypesTable TerrainTypes;
		UnitTypesTable UnitTypes;
		MovementTypesTable MovementTypes;
	};
}
