#include "androidwars.h"

using namespace mage;


Record::Record( const HashString& name )
	: mName( name )
{ }


Record::~Record() { }


// ---------------------- TerrainTypesTable ----------------------

TerrainTypesTable::TerrainTypesTable() { }


TerrainTypesTable::~TerrainTypesTable() { }


TerrainType* TerrainTypesTable::LoadRecordFromXml( XmlReader::XmlReaderIterator xmlIterator )
{
	// TODO
	return nullptr;
}


// ---------------------- UnitTypesTable ----------------------

UnitTypesTable::UnitTypesTable() { }


UnitTypesTable::~UnitTypesTable() { }


UnitType* UnitTypesTable::LoadRecordFromXml( XmlReader::XmlReaderIterator xmlIterator )
{
	// TODO
	return nullptr;
}


// ---------------------- Database ----------------------

Database::Database() { }


Database::~Database() { }
