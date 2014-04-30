#include "androidwars.h"

#include "CoreLib.h"

using namespace mage;

const char* const Scenario::SPRITE_DIRECTORY = "sprites";
const char* const Scenario::ANIMATION_FILE_EXTENSION = "anim";


std::string Scenario::FormatAnimationPath( const std::string& animationName )
{
	// Construct a path to the animation and return the result.
	std::stringstream formatter;
	formatter << SPRITE_DIRECTORY << "/" << animationName << "." << ANIMATION_FILE_EXTENSION;
	return formatter.str();
}


std::string Scenario::FormatAnimationName( const std::string& animationName )
{
	// Construct a name for the animation and return the result.
	std::stringstream formatter;
	formatter << animationName << "." << ANIMATION_FILE_EXTENSION;
	return formatter.str();
}


Scenario::Scenario():
	TerrainTypes( this ),
	UnitTypes( this ),
	MovementTypes( this )
{ }


Scenario::~Scenario() { }


bool Scenario::LoadDataFromFile( const std::string& filePath )
{
	return LoadDataFromFile( filePath.c_str() );
}


bool Scenario::LoadDataFromFile( const char* filePath )
{
	bool success = false;

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
		DebugPrintf( "Loading Scenario data from file \"%s\"...", filePath );
		LoadDataFromString( data );

		// Return that the file was opened successfully.
		success = true;
	}
	else
	{
		WarnFail( "Could not open data file \"%s\"!", filePath );
	}

	return success;
}


void Scenario::LoadDataFromString( const std::string& data )
{
	LoadDataFromString( data.c_str() );
}


void Scenario::LoadDataFromString( const char* data )
{
	// Parse the file.
	rapidjson::Document document;
	document.Parse< 0 >( data );

	if( !document.HasParseError() )
	{
		// If the file was loaded successfully, load the Scenario from the JSON result.
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

		WarnFail( "Error parsing Scenario JSON data at line %d, column %d: %s", line, column, document.GetParseError() );
	}
}


void Scenario::LoadDataFromJSON( const rapidjson::Value& object )
{
	assertion( object.IsObject(), "Could not load data from JSON because the JSON value specified is not an object!" );

	// Load Scenario fields.
	SetName( GetJSONStringValue( object, "name", "" ) );
	SetDefaultTerrainTypeName( GetJSONStringValue( object, "defaultTerrainType", "" ) );

	// Load all game data.
	TerrainTypes.LoadRecordsFromJSON( object );
	UnitTypes.LoadRecordsFromJSON( object );
	MovementTypes.LoadRecordsFromJSON( object );

	//DebugPrintData();
}


void Scenario::ClearData()
{
	// Clear all game data.
	TerrainTypes.DeleteAllRecords();
	UnitTypes.DeleteAllRecords();
	MovementTypes.DeleteAllRecords();
}


void Scenario::DebugPrintData() const
{
	DebugPrintf( "Scenario \"%s\":", mName.GetCString() );

	// Print all records in the Scenario.
	TerrainTypes.DebugPrintData();
	UnitTypes.DebugPrintData();
	MovementTypes.DebugPrintData();
}


void Scenario::SetName( const HashString& name )
{
	mName = name;
}


HashString Scenario::GetName() const
{
	return mName;
}


void Scenario::SetDefaultTerrainType( TerrainType* defaultTerrainType )
{
	assertion( defaultTerrainType == nullptr || defaultTerrainType->GetScenario() == this,
			   "Cannot set default TerrainType to %s because that record is not part of this Scenario (\"%s\")!",
			   defaultTerrainType->ToString(), mName.GetCString() );
	SetDefaultTerrainTypeName( defaultTerrainType->GetName() );
}


TerrainType* Scenario::GetDefaultTerrainType()
{
	return TerrainTypes.FindByName( mDefaultTerrainTypeName );
}


const TerrainType* Scenario::GetDefaultTerrainType() const
{
	return TerrainTypes.FindByName( mDefaultTerrainTypeName );
}


void Scenario::SetDefaultTerrainTypeName( const HashString& defaultTerrainTypeName )
{
	mDefaultTerrainTypeName = defaultTerrainTypeName;
}


HashString Scenario::GetDefaultTerrainTypeName() const
{
	return mDefaultTerrainTypeName;
}
