#pragma once

namespace mage
{
	/**
	 * Table that keeps track of UnitTypes.
	 */
	class UnitTypesTable : public Table< UnitType >
	{
	public:
		UnitTypesTable( Database* database );
		virtual ~UnitTypesTable();

	protected:
		virtual const char* GetXmlElementName() const;
		virtual void LoadRecordFromXml( UnitType* unitType, XmlReader::XmlReaderIterator xmlIterator );
	};


	inline const char* UnitTypesTable::GetXmlElementName() const
	{
		return "UnitType";
	}
}
