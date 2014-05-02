#include "androidwars.h"

using namespace mage;


Ability::Ability( Map* map ) :
	mMap( map )
{
	assertion( mMap, "Cannot create Ability without a Map!" );
}


Ability::~Ability() { }


bool Ability::CanProcessAction( const Action* action ) const
{
	return ( action && ( action->GetType() == GetType() ) );
}


bool Ability::Action::IsType( const HashString& type ) const
{
	return ( GetType() == type );
}


Map* Ability::GetMap() const
{
	return mMap;
}


SingleTargetComponent::Action::Action() :
	TargetID( -1 )
{ }


void SingleTargetComponent::Action::SaveToJSON( rapidjson::Document& document, rapidjson::Value& object ) const
{
	// Add the target Unit ID to the JSON object.
	JSON::SaveInt( document, object, "target", TargetID );
}


void SingleTargetComponent::Action::LoadFromJSON( const rapidjson::Value& object )
{
	// Load teh target Unit ID from the JSON object.
	JSON::LoadInt( object, "target", TargetID );
}
