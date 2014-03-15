#pragma once

namespace mage
{
	/**
	 * Stores all game data.
	 */
	class Database
	{
	public:
		static const char* const SPRITE_DIRECTORY;
		static const char* const ANIMATION_FILE_EXTENSION;

		static std::string FormatAnimationPath( const std::string& animationName );
		static std::string FormatAnimationName( const std::string& animationName );

		Database();
		~Database();

		void LoadDataFromFile( const std::string& filePath );
		void LoadDataFromFile( const char* filePath );
		void LoadDataFromString( const std::string& data );
		void LoadDataFromString( const char* data );
		void LoadDataFromXML( XmlReader::XmlReaderIterator rootIterator );
		void LoadDataFromJSON( const rapidjson::Value& object );
		void ClearData();

		void DebugPrintData() const;

		TerrainTypesTable TerrainTypes;
		UnitTypesTable UnitTypes;
		MovementTypesTable MovementTypes;
	};
}
