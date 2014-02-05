#include "androidwars.h"

using namespace mage;

Database::Database()
	: TerrainTypes( this )
	, UnitTypes( this )
	, MovementTypes( this )
{ }


Database::~Database() { }


void Database::LoadDataFromFile( const char* file )
{
	// Open the data file.
	XmlReader xmlReader( file );
	assertion( !xmlReader.Fail(), "Could not load game data from file \"%s\"!", file );

	// Get the root element.
	XmlReader::XmlReaderIterator rootIterator = xmlReader.ReadRoot();

	// Load all game data.
	LoadDataFromXML( rootIterator );
}


void Database::LoadDataFromXML( XmlReader::XmlReaderIterator rootIterator )
{
	// Load all game data.
	TerrainTypes.LoadRecordsFromXML( rootIterator );
	UnitTypes.LoadRecordsFromXML( rootIterator );
	MovementTypes.LoadRecordsFromXML( rootIterator );
}


void Database::ClearData()
{
	// Clear all game data.
	TerrainTypes.DeleteAllRecords();
	UnitTypes.DeleteAllRecords();
	MovementTypes.DeleteAllRecords();
}
