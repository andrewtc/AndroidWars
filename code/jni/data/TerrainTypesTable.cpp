#include "androidwars.h"

using namespace mage;


MAGE_AUTO_GENERATE_TABLE_NAME( TerrainTypesTable, TerrainType );


TerrainTypesTable::TerrainTypesTable( Database* database )
	: Table( database )
{ }


TerrainTypesTable::~TerrainTypesTable() { }


void TerrainTypesTable::OnLoadRecordFromXml( TerrainType* terrainType, XmlReader::XmlReaderIterator xmlIterator )
{
	// Read in all attributes.
	terrainType->mCoverBonus = xmlIterator.GetAttributeAsInt( "coverBonus", 0 );
}


void TerrainTypesTable::OnLoadRecordFromJSON( TerrainType* terrainType, const rapidjson::Value& object )
{
	// Read in all attributes.
	terrainType->mDisplayName = GetJSONStringValue( object, "animationSet", "" );
	terrainType->mAnimationSetName = Database::FormatAnimationName( GetJSONStringValue( object, "animationSet", "" ) );
	terrainType->mAnimationSetPath = Database::FormatAnimationPath( GetJSONStringValue( object, "animationSet", "" ) );
	terrainType->mIsCapturable = GetJSONBoolValue( object, "isCapturable", false );
	terrainType->mCoverBonus = GetJSONIntValue( object, "coverBonus", 0 );
}
