#pragma once


namespace mage
{
	class Unit;

	/**
	 * Represents an ability that can be performed by a Unit.
	 */
	class UnitAbility : public Ability
	{
	public:
		class Action;

		UnitAbility( Map* map );
		virtual ~UnitAbility();

		virtual void DetermineAvailableActions( const Unit* unit, const Path& movementPath, Actions& result ) const = 0;
		virtual void ProcessAction( Ability::Action* action );

	protected:
		virtual void ProcessUnitAction( UnitAbility::Action* action ) = 0;
	};


	/**
	 * Represents an action that can be performed by a Unit.
	 */
	class UnitAbility::Action : public Ability::Action
	{
	public:
		Action();
		virtual ~Action();

		void SaveToJSON( rapidjson::Document& document, rapidjson::Value& object ) const;
		void LoadFromJSON( const rapidjson::Value& object );

		bool MoveWasSuccessful;
		int UnitID;
		Path MovementPath;
	};
}
