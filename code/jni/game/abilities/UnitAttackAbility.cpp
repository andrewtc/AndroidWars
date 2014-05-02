#include "androidwars.h"

using namespace mage;


MAGE_IMPLEMENT_ABILITY( UnitAttackAbility, "Attack" )


UnitAttackAbility::UnitAttackAbility( Map* map ) :
	UnitAbility( map )
{ }


UnitAttackAbility::~UnitAttackAbility() { }


void UnitAttackAbility::DetermineAvailableActions( const Unit* unit, const Path& movementPath, Actions& result ) const
{
	// Get the UnitType of the Unit.
	UnitType* unitType = unit->GetUnitType();

	if( unitType->HasWeapons() )
	{
		// If this Unit has weapons, determine whether the Unit can wait in the destination square.
		Map* map = unit->GetMap();
		Map::Iterator destinationTile = map->GetTile( movementPath.GetDestination() );

		if( unit->CanOccupyTile( destinationTile ) )
		{
			// If the Unit can stop here, get the list of Units in attack range from this location.
			Map::Units unitsInRange;
			map->FindUnitsInRange( destinationTile.GetPosition(), unitType->GetAttackRange(), unitsInRange );

			for( auto it = unitsInRange.begin(); it != unitsInRange.end(); ++it )
			{
				Unit* target = *it;

				if( unit->CanAttack( target ) )
				{
					// Add an attack action to the list of actions.
					Action* attackAction = new Action();

					attackAction->UnitID = unit->GetID();
					attackAction->TargetID = target->GetID();
					attackAction->MovementPath = movementPath;

					result.push_back( attackAction );
				}
			}
		}
	}
}


void UnitAttackAbility::ProcessUnitAction( UnitAbility::Action* action )
{
	// Cast the action to the proper type.
	UnitAttackAbility::Action* attackAction = dynamic_cast< UnitAttackAbility::Action* >( action );
	assertion( attackAction, "Could not cast Action to type UnitAttackAbility::Action!" );

	// Get the Unit from the Action.
	Unit* unit = GetMap()->GetUnitByID( attackAction->UnitID );

	if( unit )
	{
		// Get the target Unit from the Action.
		Unit* target = GetMap()->GetUnitByID( attackAction->TargetID );

		if( target )
		{
			// Make the Unit attack the target.
			unit->Attack( target );

			if( target->IsAlive() )
			{
				// Allow the target to counter-attack the first Unit.
				// TODO: Disallow for indirect fire Units.
				target->Attack( unit );
			}
		}
		else
		{
			WarnFail( "Could not process Attack action because an invalid target ID was supplied (%d)!", attackAction->TargetID );
		}
	}
	else
	{
		WarnFail( "Could not process Attack action because an invalid Unit ID was supplied (%d)!", attackAction->UnitID );
	}
}


void UnitAttackAbility::Action::SaveToJSON( rapidjson::Document& document, rapidjson::Value& object ) const
{
	// TODO
}


void UnitAttackAbility::Action::LoadFromJSON( const rapidjson::Value& object )
{
	// TODO
}
