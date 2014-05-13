#pragma once

namespace mage
{
	class ConstructUnitAbility : public Ability
	{
		MAGE_DECLARE_ABILITY

	public:
		ConstructUnitAbility( Map* map );
		virtual ~ConstructUnitAbility();

		virtual void DetermineAvailableActionsForTile( const Vec2s& tilePos, Actions& result );
		virtual void ProcessAction( Ability::Action* action );
	};


	class ConstructUnitAbility::Action : public Ability::Action
	{
		MAGE_IMPLEMENT_ACTION

	public:
		Action();
		~Action();

		virtual void SaveToJSON( rapidjson::Document& document, rapidjson::Value& object ) const;
		virtual void LoadFromJSON( const rapidjson::Value& object );

		int UnitID;
		int FactionID;
		int BuildCost;
		HashString UnitTypeID;
		Vec2s TilePos;
	};
}
