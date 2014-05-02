#pragma once

namespace mage
{
	/**
	 * Ability that defines how Units attack each other.
	 */
	class UnitAttackAbility : public UnitAbility, public SingleTargetComponent
	{
		MAGE_DECLARE_ABILITY

	public:
		UnitAttackAbility( Map* map );
		virtual ~UnitAttackAbility();

		virtual void DetermineAvailableActions( const Unit* unit, const Path& movementPath, Actions& result ) const;

	protected:
		virtual void ProcessUnitAction( UnitAbility::Action* action );
	};


	/**
	 * Action that encapsulates one Unit attacking another.
	 */
	class UnitAttackAbility::Action : public UnitAbility::Action, public SingleTargetComponent::Action
	{
		MAGE_IMPLEMENT_ACTION

	public:
		Action() { }
		virtual ~Action() { }

		virtual void SaveToJSON( rapidjson::Document& document, rapidjson::Value& object ) const;
		virtual void LoadFromJSON( const rapidjson::Value& object );
	};
}
