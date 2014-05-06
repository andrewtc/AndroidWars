#include "androidwars.h"

using namespace mage;


MAGE_IMPLEMENT_ABILITY( ConstructUnitAbility, "ConstructUnit" );


ConstructUnitAbility::ConstructUnitAbility( Map* map ) :
	Ability( map )
{ }


ConstructUnitAbility::~ConstructUnitAbility() { }


void ConstructUnitAbility::ProcessAction( Ability::Action* action )
{
	// Cast the action to the proper type.
	ConstructUnitAbility::Action* constructUnitAction = dynamic_cast< ConstructUnitAbility::Action* >( action );
	assertion( constructUnitAction, "Could not cast Ability::Action to proper type for ConstructUnitAbility!" );

	// Get the UnitType.
	UnitType* unitType = GetMap()->GetScenario()->UnitTypes.FindByName( constructUnitAction->UnitTypeID );
	assertion( unitType, "Invalid UnitType ID (\"%s\") specified for ConstructUnitAbility Action!", constructUnitAction->UnitTypeID.GetCString() );

	// Get the target Tile.
	Map::Iterator tile = GetMap()->GetTile( constructUnitAction->TilePos );
	assertion( tile.IsValid(), "Invalid Tile position (%d,%d) specified for ConstructUnitAbility Action!", constructUnitAction->TilePos.x, constructUnitAction->TilePos.y );

	// Get the Faction.
	Faction* faction = GetMap()->GetFactionByIndex( constructUnitAction->FactionIndex );
	assertion( faction, "Invalid Faction index (%d) specified for ConstructUnitAbility Action!", constructUnitAction->FactionIndex );

	// Create a new, inactive Unit at the specified location.
	Unit* unit = GetMap()->CreateUnit( unitType, faction, constructUnitAction->TilePos );
	assertion( unit, "Could not create Unit for ConstructUnitAbility Action!" );
	unit->Deactivate();
}


ConstructUnitAbility::Action::Action() :
	FactionIndex( -1 )
{ }


ConstructUnitAbility::Action::~Action() { }


void ConstructUnitAbility::Action::SaveToJSON( rapidjson::Document& document, rapidjson::Value& object )
{
	JSON::SaveInt( document, object, "faction", FactionIndex );
	JSON::SaveString( document, object, "unitType", UnitTypeID.GetString() );
	JSON::SaveVec2i( document, object, "tilePos", TilePos );
}


void ConstructUnitAbility::Action::LoadFromJSON( const rapidjson::Value& object )
{
	// TODO
}
