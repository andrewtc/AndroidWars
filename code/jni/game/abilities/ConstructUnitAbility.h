#pragma once

namespace mage
{
	class ConstructUnitAbility : public Ability
	{
		MAGE_DECLARE_ABILITY

	public:
		ConstructUnitAbility( Map* map );
		virtual ~ConstructUnitAbility();

		virtual void ProcessAction( Ability::Action* action );
	};


	class ConstructUnitAbility::Action : public Ability::Action
	{
		MAGE_IMPLEMENT_ACTION

	public:
		Action();
		~Action();

		virtual void SaveToJSON( rapidjson::Document& document, rapidjson::Value& object );
		virtual void LoadFromJSON( const rapidjson::Value& object );

		int FactionIndex;
		HashString UnitTypeID;
		Vec2s TilePos;
	};
}
