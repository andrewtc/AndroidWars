#pragma once

namespace mage
{
	class MovementType;

	/**
	 * Table that keeps track of MovementTypes.
	 */
	class MovementTypesTable : public Table< MovementTypesTable, MovementType >
	{
	public:
		static const char* const MOVEMENT_COSTS_JSON_PROPERTY_NAME;

		MovementTypesTable( Scenario* scenario );
		virtual ~MovementTypesTable();

	protected:
		virtual void OnLoadRecordFromXml( MovementType* movementType, XmlReader::XmlReaderIterator xmlIterator );
		virtual void OnLoadRecordFromJSON( MovementType* movementType, const rapidjson::Value& object );
	};
}
