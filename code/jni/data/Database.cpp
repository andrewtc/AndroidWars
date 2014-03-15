#include "androidwars.h"

#include "CoreLib.h"

using namespace mage;

const char* const Database::SPRITE_DIRECTORY = "sprites";
const char* const Database::ANIMATION_FILE_EXTENSION = "anim";


std::string Database::FormatAnimationPath( const std::string& animationName )
{
	// Construct a path to the animation and return the result.
	std::stringstream formatter;
	formatter << SPRITE_DIRECTORY << "/" << animationName << "." << ANIMATION_FILE_EXTENSION;
	return formatter.str();
}


std::string Database::FormatAnimationName( const std::string& animationName )
{
	// Construct a name for the animation and return the result.
	std::stringstream formatter;
	formatter << animationName << "." << ANIMATION_FILE_EXTENSION;
	return formatter.str();
}


Database::Database()
	: TerrainTypes( this )
	, UnitTypes( this )
	, MovementTypes( this )
{ }


Database::~Database() { }


void Database::LoadDataFromFile( const std::string& filePath )
{
	LoadDataFromFile( filePath.c_str() );
}


void Database::LoadDataFromFile( const char* filePath )
{
	/*
	// Open the data file.
	XmlReader xmlReader( file );
	assertion( !xmlReader.Fail(), "Could not load game data from file \"%s\"!", file );

	// Get the root element.
	XmlReader::XmlReaderIterator rootIterator = xmlReader.ReadRoot();

	// Load all game data.
	LoadDataFromXML( rootIterator );
	*/

	// Open the data file.
	char* data = nullptr;
	unsigned int fileSize;
	OpenDataFile( filePath, data, fileSize );

	if( data )
	{
		// Load the data from the file.
		DebugPrintf( "Loading Database data from file \"%s\"...", filePath );
		LoadDataFromString( data );
	}
	else
	{
		WarnFail( "Could not open data file \"%s\"!", filePath );
	}
}


void Database::LoadDataFromString( const std::string& data )
{
	LoadDataFromString( data.c_str() );
}


void Database::LoadDataFromString( const char* data )
{
	// Parse the file.
	rapidjson::Document document;
	document.Parse< 0 >( data );

	if( !document.HasParseError() )
	{
		// If the file was loaded successfully, load the database from the JSON result.
		LoadDataFromJSON( document );
	}
	else
	{
		// Get the offset of the error.
		size_t errorOffset = document.GetErrorOffset();

		// Determine the number of lines before the error.
		size_t line = 1;
		const char* lastLineEnding = data;
		const char* it;

		for( it = data; it < ( data + errorOffset ); ++it )
		{
			if( *it == '\n' )
			{
				// If a newline is encountered, go to the next line.
				++line;

				// Keep track of where the last line ended.
				lastLineEnding = it;
			}
		}

		// Get the column offset of the error.
		size_t column = ( it - lastLineEnding );

		WarnFail( "Error parsing Database JSON data at line %d, column %d: %s", line, column, document.GetParseError() );
	}
}


void Database::LoadDataFromXML( XmlReader::XmlReaderIterator rootIterator )
{
	// Load all game data.
	TerrainTypes.LoadRecordsFromXML( rootIterator );
	UnitTypes.LoadRecordsFromXML( rootIterator );
	MovementTypes.LoadRecordsFromXML( rootIterator );
}


void Database::LoadDataFromJSON( const rapidjson::Value& object )
{
	assertion( object.IsObject(), "Could not load data from JSON because the JSON value specified is not an object!" );

	// Load all game data.
	TerrainTypes.LoadRecordsFromJSON( object );
	UnitTypes.LoadRecordsFromJSON( object );
	MovementTypes.LoadRecordsFromJSON( object );

	DebugPrintData();
}


void Database::ClearData()
{
	// Clear all game data.
	TerrainTypes.DeleteAllRecords();
	UnitTypes.DeleteAllRecords();
	MovementTypes.DeleteAllRecords();
}


void Database::DebugPrintData() const
{
	DebugPrintf( "DATABASE:" );

	// Print all records in the database.
	TerrainTypes.DebugPrintData();
	UnitTypes.DebugPrintData();
	MovementTypes.DebugPrintData();
}

