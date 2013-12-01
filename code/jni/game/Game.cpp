#include "androidwars.h"

using namespace mage;


const char* Game::MAPS_FOLDER_PATH = "maps";
const char* Game::MAP_FILE_EXTENSION = "tmx";


Game* Game::Create( int numPlayers, const std::string& mapName )
{
	// Create a new Game.
	Game* game = new Game();

	// Set the map for this Game.
	game->SetMapName( mapName );

	for( int i = 0; i < numPlayers; ++i )
	{
		// Create new players for the Game.
		Player* player = new Player();
		game->AddPlayer( player );
	}

	return game;
}


std::string Game::FormatMapPath( const std::string& mapName )
{
	std::stringstream formatter;
	formatter << MAPS_FOLDER_PATH << "/" << mapName << "." << MAP_FILE_EXTENSION;
	return formatter.str();
}


Game::Game()
	: mNextPlayerIndex( 0 )
	, mStatus( STATUS_NOT_STARTED )
	, mCamera( nullptr )
{
	// Add map object creation callback.
	mMap.SetNewMapObjectCB( &SpawnObjectFromXml );
}


Game::~Game() { }


void Game::Start()
{
	int numPlayers = GetNumPlayers();

	DebugPrintf( "Starting Game with %d players...", numPlayers );

	// Make sure a valid map was specified.
	assertion( !mMapName.empty(), "Cannot start Game because no map name was specified!" );

	// Make sure the number of Players makes sense.
	assertion( numPlayers >= MIN_PLAYERS, "Cannot start Game with fewer than %d players! (%d requested)", MIN_PLAYERS, numPlayers );
	assertion( numPlayers <= MAX_PLAYERS, "Cannot start Game with more than %d players! (%d requested)", MAX_PLAYERS, numPlayers );

	// Make sure the game hasn't been started yet.
	assertion( IsNotStarted(), "Cannot start Game that has already been started!" );

	// Start the game.
	mStatus = STATUS_IN_PROGRESS;

	// Create a TileMap for this Game.
	std::string pathToMapFile = FormatMapPath( mMapName );
	mMap.Load( pathToMapFile.c_str() );

	mCamera->SetWorldBounds( mMap.GetMapBounds() );

	// Spawn a test Unit. TODO remove this
	UnitType* unitType = gDatabase->UnitTypes.FindByName( "Infantry" );
	Unit* testUnit = SpawnUnit( unitType, 5, 5 );
	mMap.AddMapObject( testUnit );

}


void Game::OnUpdate( float dt )
{
	if( IsInProgress() )
	{
		// Update the world.
		mMap.OnUpdate( dt );
	}
}


void Game::OnDraw()
{
	if( IsInProgress() && mCamera )
	{
		// Draw the world.
		mMap.OnDraw( *mCamera );
	}
}


void Game::AddPlayer( Player* player )
{
	// Make sure the Player is not in the Game already.
	assertion( !HasPlayer( player ), "Cannot add Player to Game that is already part of the Game!" );
	assertion( !player->HasGame(), "Cannot add Player to Game that is already part of another Game!" );

	// Add the player to the list of Players.
	mPlayers.push_back( player );

	// Notify the Player that it is part of the Game and assign it a unique index.
	player->mGame = this;
	player->mIndex = mNextPlayerIndex;
	++mNextPlayerIndex;
}


MapObject* Game::SpawnObjectFromXml( const XmlReader::XmlReaderIterator& xmlIterator )
{
	MapObject* result = nullptr;

	// Get the name of the element in question.
	std::string name = xmlIterator.GetAttributeAsString( "name", "" );
	std::string type = xmlIterator.GetAttributeAsString( "type", "" );

	if( type == "Unit" )
	{
		// Spawn a new unit with the specified UnitType.
		result = new Unit( name );

		// Load generic properties
		TileMap::LoadDefaultMapObjectFields( *result, xmlIterator );
	}
	// Object is unknown - just use the default loading
	else
	{
		result = TileMap::DefaultNewMapObjectFn( xmlIterator );
	}

	return result;
}


Unit* Game::SpawnUnit( UnitType* unitType, int x, int y )
{
	// Create a new Unit of the specified type at the specified location on the map.
	Unit* unit = new Unit( unitType );
	unit->SetTilePos( x, y );

	// Initialize the Unit.
	unit->Init();

	return unit;
}


void Game::OnTouchEvent( float x, float y )
{
	static void( *fn )( Unit* ) = []( Unit* unit ) -> void { unit->DrawSelected = false; };
	MapObject* obj = mMap.GetFirstObjectAt( Vec2f( x, y ) + mCamera->GetPosition() );
	mMap.ForeachObjectOfType( fn );
	if ( obj && obj->IsExactly( Unit::TYPE ) )
	{
		Unit* unit = (Unit*) obj;
		unit->DrawSelected = true;
	}
}
