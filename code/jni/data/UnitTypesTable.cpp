#include "androidwars.h"

using namespace mage;


UnitTypesTable::UnitTypesTable( Database* database )
	: Table( database )
{ }


UnitTypesTable::~UnitTypesTable() { }


void UnitTypesTable::LoadRecordFromXml( UnitType* unitType, XmlReader::XmlReaderIterator xmlIterator )
{
	// Read in the sprite for this UnitType and pre-load it.
	unitType->mAnimationSetName = UnitType::FormatAnimationPath( xmlIterator.GetAttributeAsString( "animationSet" ) );
	unitType->LoadAnimation();

	// Read in the unit display name (if it exists).
	unitType->mDisplayName = xmlIterator.GetAttributeAsString( "displayName", "" );
}
