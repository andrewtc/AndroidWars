#include "CoreLib.h"

using namespace mage;

MAGE_IMPLEMENT_RTTI( Object, Controller );

//---------------------------------------
Controller::Controller()
{}
//---------------------------------------
Controller::Controller( const std::string& name )
	: Object( name )
	, mObject( 0 )
{}
//---------------------------------------
Controller::~Controller()
{
}
//---------------------------------------