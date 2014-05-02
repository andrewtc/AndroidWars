#pragma once

namespace mage
{
	/**
	 * Ability that defines how Units move to a location and wait.
	 */
	class UnitWaitAbility : public UnitAbility
	{
		MAGE_DECLARE_ABILITY

	public:
		UnitWaitAbility( Map* map );
		virtual ~UnitWaitAbility();

		virtual void DetermineAvailableActions( const Unit* unit, const Path& movementPath, Actions& result ) const;

	protected:
		virtual void ProcessUnitAction( UnitAbility::Action* action );
	};


	/**
	 * Action that encapsulates a Unit moving to a location and waiting.
	 */
	class UnitWaitAbility::Action : public UnitAbility::Action
	{
		MAGE_IMPLEMENT_ACTION

	public:
		Action() { }
		virtual ~Action() { }

		virtual void SaveToJSON( rapidjson::Document& document, rapidjson::Value& object ) const;
		virtual void LoadFromJSON( const rapidjson::Value& object );
	};
}
