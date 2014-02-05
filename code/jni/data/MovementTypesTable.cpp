#include "androidwars.h"

using namespace mage;


MAGE_AUTO_GENERATE_TABLE_NAME( MovementTypesTable, MovementType );


MovementTypesTable::MovementTypesTable( Database* database )
	: Table( database )
{ }


MovementTypesTable::~MovementTypesTable() { }


void MovementTypesTable::LoadRecordFromXml( MovementType* movementType, XmlReader::XmlReaderIterator xmlIterator )
{
	// Read in the movement cost info for this movement type.
	static const char* movementTypeName = "MovementCost";

	for( XmlReader::XmlReaderIterator movementCostIterator = xmlIterator.NextChild( movementTypeName );
		 movementCostIterator.IsValid(); movementCostIterator = movementCostIterator.NextSibling( movementTypeName ) )
	{
		HashString movementTypeName = movementCostIterator.GetAttributeAsString( "terrainType" );
		int movementCost = movementCostIterator.GetAttributeAsInt( "cost" );

		movementType->AddMovementCost( movementTypeName, movementCost );
		DebugPrintf( "Found movement cost of %d over terrain \"%s\".", movementCost, movementTypeName.GetString().c_str() );
	}
}
