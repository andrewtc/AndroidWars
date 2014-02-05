#pragma once

namespace mage
{
	class UnitType;
	class Weapon;

	/**
	 * Table that keeps track of UnitTypes.
	 */
	class UnitTypesTable : public Table< UnitTypesTable, UnitType >
	{
	public:
		UnitTypesTable( Database* database );
		virtual ~UnitTypesTable();

	protected:
		virtual const char* GetXmlElementName() const;
		virtual void LoadRecordFromXml( UnitType* unitType, XmlReader::XmlReaderIterator xmlIterator );
		void LoadWeaponFromXml( Weapon& weapon, XmlReader::XmlReaderIterator xmlIterator );
	};


	inline const char* UnitTypesTable::GetXmlElementName() const
	{
		return "UnitType";
	}
}
