#include "androidwars.h"

using namespace mage;


MAGE_IMPLEMENT_ABILITY( UnitReinforceAbility, "Reinforce" );


UnitReinforceAbility::UnitReinforceAbility( Map* map ) :
	UnitAbility( map )
{ }


UnitReinforceAbility::~UnitReinforceAbility() { }


void UnitReinforceAbility::DetermineAvailableActions( const Unit* unit, const Path& movementPath, Actions& result ) const
{
	// Get the destination Tile.
	Map::Iterator destinationTile = GetMap()->GetTile( movementPath.GetDestination() );

	if( destinationTile.IsValid() && unit->CanEnterTile( destinationTile ) )
	{
		// If the Unit can enter the destination Tile, check to see if there is a friendly Unit in the tile.
		Unit* target = destinationTile->GetUnit();

		if( target && unit->CanReinforce( target ) )
		{
			// If there is a Unit in the destination tile and this Unit can reinforce it, add a reinforce action.
			UnitReinforceAbility::Action* reinforceAction = new UnitReinforceAbility::Action();

			// Add necessary data to the action.
			reinforceAction->UnitID = unit->GetID();
			reinforceAction->MovementPath = movementPath;
			reinforceAction->TargetUnitID = target->GetID();

			// Calculate the amount of health, supplies, and ammo that the reinforced Unit will have.
			UnitType* unitType = target->GetUnitType();
			reinforceAction->CombinedHealth = std::min( unit->GetHealth() + target->GetHealth(), Unit::MAX_HEALTH );
			reinforceAction->CombinedSupplies = std::min( unit->GetSupplies() + target->GetSupplies(), unitType->GetMaxSupplies() );
			reinforceAction->CombinedAmmo = std::min( unit->GetAmmo() + target->GetAmmo(), unitType->GetMaxAmmo() );

			result.push_back( reinforceAction );
		}
	}
}


void UnitReinforceAbility::ProcessUnitAction( UnitAbility::Action* action )
{
	// Cast the ability to the proper type.
	UnitReinforceAbility::Action* reinforceAction = dynamic_cast< UnitReinforceAbility::Action* >( action );
	assertion( reinforceAction, "Cannot process UnitAbility::Action for UnitReinforceAbility because the Action could not be cast to the proper type!" );

	// Get the Unit.
	Unit* unit = GetMap()->GetUnitByID( reinforceAction->UnitID );
	assertion( unit, "Invalid Unit ID (%d) specified for reinforce action!", reinforceAction->UnitID );

	// Get the target.
	Unit* targetUnit = GetMap()->GetUnitByID( reinforceAction->TargetUnitID );
	assertion( targetUnit, "Invalid target Unit ID (%d) specified for reinforce action!", reinforceAction->TargetUnitID );

	// Add the health, supplies, and ammo of the Unit to the target Unit.
	targetUnit->AddHealth( unit->GetHealth() );
	targetUnit->AddSupplies( unit->GetSupplies() );
	targetUnit->AddAmmo( unit->GetAmmo() );

	// TODO: Game state synchronization check to make sure health, supplies, and ammo of target are all correct.

	// Destroy the current Unit.
	GetMap()->DestroyUnit( unit );

	// Deactivate the target Unit (if active).
	targetUnit->Deactivate();
}


UnitReinforceAbility::Action::Action() :
	CombinedHealth( -1 ),
	CombinedSupplies( -1 ),
	CombinedAmmo( -1 )
{ }


UnitReinforceAbility::Action::~Action() { }


void UnitReinforceAbility::Action::SaveToJSON( rapidjson::Document& document, rapidjson::Value& object ) const
{
	// Save all UnitAbility info.
	UnitAbility::Action::SaveToJSON( document, object );

	// Save the target ID.
	JSON::SaveInt( document, object, "target", TargetUnitID );
}


void UnitReinforceAbility::Action::LoadFromJSON( const rapidjson::Value& object )
{
	// Load all UnitAbility info.
	UnitAbility::Action::LoadFromJSON( object );

	// Load the target ID.
	JSON::LoadInt( object, "target", TargetUnitID );
}
