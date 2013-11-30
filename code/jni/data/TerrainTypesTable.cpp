#include "androidwars.h"

using namespace mage;


TerrainTypesTable::TerrainTypesTable( Database* database )
	: Table( database )
{ }


TerrainTypesTable::~TerrainTypesTable() { }


void TerrainTypesTable::LoadRecordFromXml( TerrainType* terrainType, XmlReader::XmlReaderIterator xmlIterator )
{
	// Read in all attributes.
	terrainType->mCoverBonus = xmlIterator.GetAttributeAsInt( "coverBonus", 0 );
}