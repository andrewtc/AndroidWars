#include "androidwars.h"

using namespace mage;


Record::Record( const HashString& name )
	: mName( name )
{ }


Record::~Record() { }


// ---------------------- TerrainTypesTable ----------------------

TerrainTypesTable::TerrainTypesTable() { }


TerrainTypesTable::~TerrainTypesTable() { }


void TerrainTypesTable::LoadRecordFromXml( TerrainType* terrainType, XmlReader::XmlReaderIterator xmlIterator )
{
	// Read in all attributes.
	terrainType->mCoverBonus = xmlIterator.GetAttributeAsInt( "coverBonus", 0 );
}


// ---------------------- UnitTypesTable ----------------------

UnitTypesTable::UnitTypesTable() { }


UnitTypesTable::~UnitTypesTable() { }


void UnitTypesTable::LoadRecordFromXml( UnitType* unitType, XmlReader::XmlReaderIterator xmlIterator )
{
	// TODO
}


// ---------------------- Database ----------------------

Database::Database() { }


Database::~Database() { }


void Database::LoadGameData()
{
	// Load all game data.
	TerrainTypes.LoadRecordsFromFile( "data/Terrain.xml" );
	UnitTypes.LoadRecordsFromFile( "data/Units.xml" );
}
