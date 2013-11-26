#pragma once

namespace mage
{
	class TerrainType;
	class XmlReader;

	/**
	 * Stores all game data.
	 */
	class Database
	{
	public:
		Database();
		~Database();

		void LoadTerrainTypes( const char* file );

		TerrainType* GetTerrainType( const std::string& name ) const;
		TerrainType* GetTerrainType( int hashedName ) const;

	protected:
		void LoadTerrainType( XmlReader& xmlReader );

		typedef std::map< int, TerrainType* > TerrainTypesByHashedName;

		TerrainTypesByHashedName mTileTypesByHashedName;
	};
}
