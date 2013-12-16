#include "androidwars.h"

using namespace mage;


MAGE_IMPLEMENT_RTTI( Record, MovementType );


MovementType::MovementType( const HashString& name ) :
	Record( name )
{ }


MovementType::~MovementType() { }
