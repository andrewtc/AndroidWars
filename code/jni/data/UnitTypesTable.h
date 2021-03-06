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
		static const char* const WEAPONS_JSON_PROPERTY_NAME;
		static const char* const DAMAGE_JSON_PROPERTY_NAME;

		UnitTypesTable( Scenario* scenario );
		virtual ~UnitTypesTable();

	protected:
		virtual void OnLoadRecordFromJSON( UnitType* unitType, const rapidjson::Value& object );
		void LoadWeaponFromJSON( Weapon& weapon, const rapidjson::Value& object );
	};
}
