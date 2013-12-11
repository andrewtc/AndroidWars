#pragma once

namespace mage
{
	/**
	 * Table that keeps track of MovementTypes.
	 */
	class MovementTypesTable : public Table< MovementType >
	{
	public:
		MovementTypesTable( Database* database );
		virtual ~MovementTypesTable();

	protected:
		virtual const char* GetXmlElementName() const;
		virtual void LoadRecordFromXml( MovementType* movementType, XmlReader::XmlReaderIterator xmlIterator );
	};


	inline const char* MovementTypesTable::GetXmlElementName() const
	{
		return "MovementType";
	}
}
