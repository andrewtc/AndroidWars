#include "androidwars.h"

using namespace mage;


MAGE_AUTO_GENERATE_TABLE_NAME( MovementTypesTable, MovementType );

const char* const MovementTypesTable::MOVEMENT_COSTS_JSON_PROPERTY_NAME = "movementCosts";


MovementTypesTable::MovementTypesTable( Scenario* scenario )
	: Table( scenario )
{ }


MovementTypesTable::~MovementTypesTable() { }


void MovementTypesTable::OnLoadRecordFromXml( MovementType* movementType, XmlReader::XmlReaderIterator xmlIterator )
{
	// Read in the movement cost info for this movement type.
	static const char* movementCostName = "MovementCost";

	for( XmlReader::XmlReaderIterator movementCostIterator = xmlIterator.NextChild( movementCostName );
		 movementCostIterator.IsValid(); movementCostIterator = movementCostIterator.NextSibling( movementCostName ) )
	{
		HashString movementTypeName = movementCostIterator.GetAttributeAsString( "terrainType" );
		int movementCost = movementCostIterator.GetAttributeAsInt( "cost" );

		movementType->AddMovementCost( movementTypeName, movementCost );
		DebugPrintf( "Found movement cost of %d over terrain \"%s\".", movementCost, movementTypeName.GetString().c_str() );
	}
}


void MovementTypesTable::OnLoadRecordFromJSON( MovementType* movementType, const rapidjson::Value& object )
{
	if( object.HasMember( MOVEMENT_COSTS_JSON_PROPERTY_NAME ) )
	{
		const rapidjson::Value& movementCostsArray = object[ MOVEMENT_COSTS_JSON_PROPERTY_NAME ];

		if( movementCostsArray.IsArray() )
		{
			for( auto it = movementCostsArray.Begin(); it != movementCostsArray.End(); ++it )
			{
				const rapidjson::Value& movementCostObject = ( *it );

				if( movementCostObject.IsObject() )
				{
					HashString movementTypeName = GetJSONStringValue( movementCostObject, "terrainType", "" );

					if( !movementTypeName.GetString().empty() )
					{
						int movementCost = GetJSONIntValue( movementCostObject, "cost", 1 );

						movementType->AddMovementCost( movementTypeName, movementCost );
						DebugPrintf( "Found movement cost of %d over terrain \"%s\".", movementCost, movementTypeName.GetString().c_str() );
					}
					else
					{
						WarnFail( "Cannot load movement cost for %s without a valid TerrainType name!", movementType->ToString() );
					}
				}
				else
				{
					WarnFail( "Could not parse movement cost for %s because JSON element %d is not an object!", movementType->ToString(), ( it - movementCostsArray.Begin() ) );
				}
			}
		}
		else
		{
			WarnFail( "Could not parse movement cost for %s because the JSON value \"%s\" is not an array!", movementType->ToString(), MOVEMENT_COSTS_JSON_PROPERTY_NAME );
		}
	}
}
