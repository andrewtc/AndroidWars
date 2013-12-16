#include "androidwars.h"

using namespace mage;


MAGE_IMPLEMENT_RTTI( Record, Record );


Record::Record( const HashString& name )
	: mName( name )
{ }


Record::~Record() { }


void Record::GenerateDebugName() const
{
	// Format a debug name for the Record.
	static const int BUFFER_SIZE = 256;
	char buffer[ BUFFER_SIZE ];
	snprintf( buffer, BUFFER_SIZE, "%s \"%s\"", GetType().GetName(), mName.GetString().c_str() );

	// Store the debug name.
	mDebugName = buffer;
}
