#include "androidwars.h"

using namespace mage;


Database::Database() { }


Database::~Database() { }


void Database::LoadTerrainTypes( const char* file )
{
	// Open the terrain types file.
	XmlReader xmlReader( file );

	if( !xmlReader.Fail() )
	{
		DebugPrintf( "Successfully opened terrain file \"%s\"!", file );
	}
}
