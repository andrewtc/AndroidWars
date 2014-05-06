#pragma once

namespace mage
{
	class UnitReinforceAbility : public UnitAbility
	{
		MAGE_DECLARE_ABILITY

	public:
		UnitReinforceAbility( Map* map );
		virtual ~UnitReinforceAbility();

		virtual void DetermineAvailableActions( const Unit* unit, const Path& movementPath, Actions& result ) const;

	protected:
		virtual void ProcessUnitAction( UnitAbility::Action* action );
	};


	class UnitReinforceAbility::Action : public UnitAbility::Action
	{
		MAGE_IMPLEMENT_ACTION

	public:
		Action();
		virtual ~Action();

		virtual void SaveToJSON( rapidjson::Document& document, rapidjson::Value& object ) const;
		virtual void LoadFromJSON( const rapidjson::Value& object );

		int TargetUnitID;
		int CombinedHealth;
		int CombinedSupplies;
		int CombinedAmmo;
	};
}
