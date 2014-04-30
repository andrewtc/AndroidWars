#include "androidwars.h"

using namespace mage;

#define MAGE_GENERATE_ABILITY( abilityClass, name, targetType ) \
	const HashString abilityClass::NAME = name; \
	const UnitAbility::TargetType abilityClass::TARGET_TYPE = targetType;


MAGE_GENERATE_ABILITY( UnitWaitAbility, "Wait", TARGET_TYPE_NONE );
MAGE_GENERATE_ABILITY( UnitAttackAbility, "Attack", TARGET_TYPE_SINGLE );


bool UnitAbility::ProcessMove( Unit* unit, const Path& path )
{
	assertion( unit, "Cannot process move for null Unit!" );

	bool success = false;

	// Tell the Unit to move along the path.
	// TODO: Check for traps.
	Map* map = unit->GetMap();
	unit->Move( path );
	success = true;

	// Return whether the Unit successfully moved along the Path.
	return success;
}


UnitWaitAbility::UnitWaitAbility() { }


UnitWaitAbility::~UnitWaitAbility() { }


void UnitWaitAbility::DetermineAvailableActions( const Unit* unit, const Path& movementPath, Actions& result ) const
{
	// Determine whether the Unit can wait in the destination square.
	Map* map = unit->GetMap();
	Map::Iterator destinationTile = map->GetTile( movementPath.GetDestination() );

	if( unit->CanOccupyTile( destinationTile ) )
	{
		// If the Unit can wait in the destination tile, add a new action for it.
		Action waitAction( NAME );

		waitAction.Parameters.Set( "unit", ( Unit* ) unit );
		waitAction.Parameters.Set( "path", Path( movementPath ) );

		result.push_back( waitAction );
	}
}


void UnitWaitAbility::ProcessAction( Action& action )
{
	Dictionary::DictionaryError error;

	// Get the Unit from the Action.
	Unit* unit = nullptr;
	error = action.Parameters.Get( "unit", unit );

	if( error == Dictionary::DErr_SUCCESS )
	{
		// Get the Path from the Action.
		Path path;
		error = action.Parameters.Get( "path", path );

		if( error == Dictionary::DErr_SUCCESS )
		{
			// Tell the Unit to move along the path.
			Map* map = unit->GetMap();

			// Make the Unit move.
			ProcessMove( unit, path );

			// Deactivate the Unit.
			unit->Deactivate();
		}
		else
		{
			WarnFail( "Could not process Action \"%s\" because no Path was specified!", action.Type.GetCString() );
		}
	}
	else
	{
		WarnFail( "Could not process Action \"%s\" because no Unit was specified!", action.Type.GetCString() );
	}
}


UnitAttackAbility::UnitAttackAbility() { }


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
					// TODO: Add important data.
					Action attackAction( NAME );
					result.push_back( attackAction );
				}
			}
		}
	}
}


void UnitAttackAbility::ProcessAction( Action& action )
{
	// TODO
}
