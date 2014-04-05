#pragma once

namespace mage
{
	/**
	 * Stores all game data.
	 */
	class Scenario
	{
	public:
		static const char* const SPRITE_DIRECTORY;
		static const char* const ANIMATION_FILE_EXTENSION;

		static std::string FormatAnimationPath( const std::string& animationName );
		static std::string FormatAnimationName( const std::string& animationName );

		Scenario();
		~Scenario();

		bool LoadDataFromFile( const std::string& filePath );
		bool LoadDataFromFile( const char* filePath );
		void LoadDataFromString( const std::string& data );
		void LoadDataFromString( const char* data );
		void LoadDataFromXML( XmlReader::XmlReaderIterator rootIterator );
		void LoadDataFromJSON( const rapidjson::Value& object );
		void ClearData();

		void DebugPrintData() const;

		void SetName( const HashString& name );
		HashString GetName() const;

		void SetDefaultTerrainType( TerrainType* defaultTerrainType );
		TerrainType* GetDefaultTerrainType();
		const TerrainType* GetDefaultTerrainType() const;
		void SetDefaultTerrainTypeName( const HashString& defaultTerrainTypeName );
		HashString GetDefaultTerrainTypeName() const;

		TerrainTypesTable TerrainTypes;
		UnitTypesTable UnitTypes;
		MovementTypesTable MovementTypes;

	protected:
		HashString mName;
		HashString mDefaultTerrainTypeName;
	};
}
