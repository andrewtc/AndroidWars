#include "androidwars.h"

using namespace mage;


MAGE_IMPLEMENT_ABILITY( UnitWaitAbility, "Wait" )


UnitWaitAbility::UnitWaitAbility( Map* map ) :
	UnitAbility( map )
{ }


UnitWaitAbility::~UnitWaitAbility() { }


void UnitWaitAbility::DetermineAvailableActions( const Unit* unit, const Path& movementPath, Actions& result ) const
{
	// Determine whether the Unit can wait in the destination square.
	Map* map = unit->GetMap();
	Map::Iterator destinationTile = map->GetTile( movementPath.GetDestination() );

	if( unit->CanOccupyTile( destinationTile ) )
	{
		// If the Unit can wait in the destination tile, add a new action for it.
		Action* waitAction = new Action();

		waitAction->UnitID = unit->GetID();
		waitAction->MovementPath = movementPath;

		result.push_back( waitAction );
	}
}


void UnitWaitAbility::ProcessUnitAction( UnitAbility::Action* action )
{
	// Cast the Action to the required type.
	UnitWaitAbility::Action* waitAction = dynamic_cast< UnitWaitAbility::Action* >( action );
	assertion( waitAction, "Could not cast Action to type UnitWaitAbility::Action!" );

	// TODO: Anything special about waiting.
}


void UnitWaitAbility::Action::SaveToJSON( rapidjson::Document& document, rapidjson::Value& object ) const
{
	// TODO
}


void UnitWaitAbility::Action::LoadFromJSON( const rapidjson::Value& object )
{
	// TODO
}
