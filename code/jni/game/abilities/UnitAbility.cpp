#include "androidwars.h"

using namespace mage;


UnitAbility::UnitAbility( Map* map ) :
	Ability( map )
{ }


UnitAbility::~UnitAbility() { }


void UnitAbility::ProcessAction( Ability::Action* action )
{
	// Cast the Action to the proper type.
	UnitAbility::Action* unitAction = dynamic_cast< UnitAbility::Action* >( action );
	assertion( unitAction, "Could not cast Action to type UnitAbility::Action!" );

	// Get the unit for this Action.
	Unit* unit = GetMap()->GetUnitByID( unitAction->UnitID );

	if( unit )
	{
		// Move the Unit along the movement path.
		unitAction->MoveWasSuccessful = unit->Move( unitAction->MovementPath );

		if( unitAction->MoveWasSuccessful )
		{
			// If the move was successful, let the derived type process the Unit action.
			ProcessUnitAction( unitAction );
		}

		// Deactivate the Unit.
		unit->Deactivate();
	}
	else
	{
		WarnFail( "Could not process UnitAbility::Action because an invalid Unit ID (%d) was supplied!", unitAction->UnitID );
	}
}


UnitAbility::Action::Action() :
	UnitID( -1 ),
	MoveWasSuccessful( false )
{ }


UnitAbility::Action::~Action() { }


void UnitAbility::Action::SaveToJSON( rapidjson::Document& document, rapidjson::Value& object ) const
{
	assertion( object.IsObject(), "Cannot save UnitAbility::Action to JSON because the target JSON value is not an object!" );

	// Save the Unit ID.
	JSON::SaveInt( document, object, "unit", UnitID );

	// Save the movement path.
	rapidjson::Value pathJSON;
	pathJSON.SetObject();

	MovementPath.SaveToJSON( document, pathJSON );

	object.AddMember( "movementPath", pathJSON, document.GetAllocator() );
}


void UnitAbility::Action::LoadFromJSON( const rapidjson::Value& object )
{
	assertion( object.IsObject(), "Cannot load UnitAbility::Action from JSON because the target JSON value is not an object!" );

	// Load the object ID.
	JSON::LoadInt( object, "unit", UnitID );

	if( object.HasMember( "movementPath" ) )
	{
		// Load the movement path.
		const rapidjson::Value& pathJSON = object[ "movementPath" ];
		MovementPath.LoadFromJSON( pathJSON );
	}
	else
	{
		WarnFail( "No \"movementPath\" object found in JSON for UnitAbility::Action!" );
	}
}
