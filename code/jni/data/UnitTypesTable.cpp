#include "androidwars.h"

using namespace mage;


UnitTypesTable::UnitTypesTable( Database* database )
	: Table( database )
{ }


UnitTypesTable::~UnitTypesTable() { }


void UnitTypesTable::LoadRecordFromXml( UnitType* unitType, XmlReader::XmlReaderIterator xmlIterator )
{
	// Read in the sprite for this UnitType and pre-load it.
	unitType->mAnimationSetPath = UnitType::FormatAnimationPath( xmlIterator.GetAttributeAsString( "animationSet" ) );
	unitType->mAnimationSetName = UnitType::FormatAnimationName( xmlIterator.GetAttributeAsString( "animationSet" ) );
	unitType->LoadAnimation();

	// Read in the unit display name (if it exists).
	unitType->mDisplayName = xmlIterator.GetAttributeAsString( "displayName", "" );

	// Read in unit gameplay properties.
	unitType->mMovementRange = xmlIterator.GetAttributeAsInt( "movementRange", 0 );
	unitType->mMovementTypeName = xmlIterator.GetAttributeAsString( "movementType" );
	unitType->mAttackRange = xmlIterator.GetAttributeAsIntRange( "attackRange" );
}
