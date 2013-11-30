#include "androidwars.h"

using namespace mage;

Database::Database()
	: TerrainTypes( this )
	, UnitTypes( this )
{ }


Database::~Database() { }


void Database::LoadGameData()
{
	// Load all game data.
	TerrainTypes.LoadRecordsFromFile( "data/Terrain.xml" );
	UnitTypes.LoadRecordsFromFile( "data/Units.xml" );
}
