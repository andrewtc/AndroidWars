#include "androidwars.h"

using namespace mage;


EditorState::EditorState() :
	mCamera( gWindowWidth, gWindowHeight ),
	mWorld( &mMap ),
	mPaintTilesInputState( nullptr )
{ }


EditorState::~EditorState()
{ }


Map* EditorState::GetMap()
{
	return &mMap;
}


const Map* EditorState::GetMap() const
{
	return &mMap;
}


void EditorState::OnEnter( const Dictionary& parameters )
{
	// Create the database.
	mScenario.LoadDataFromFile( "data/Data.json" );

	// Create a new Map.
	mMap.Resize( 16, 12 );

	// Paint the Map with default tiles.
	TerrainType* defaultTerrainType = mScenario.GetDefaultTerrainType();
	assertion( defaultTerrainType != nullptr, "No default TerrainType found for this Scenario!" );

	Tile tile;
	tile.SetTerrainType( defaultTerrainType );
	mMap.Fill( tile );

	TerrainType* seaTerrainType = mScenario.TerrainTypes.FindByName( "Sea" );
	tile.SetTerrainType( seaTerrainType );
	mMap.Fill( tile, RectS( 2, 2, 4, 4 ) );

	mMap.Init( &mScenario );

	// Allow the user to paint tiles.
	TerrainType* cityTerrainType = mScenario.TerrainTypes.FindByName( "City" );
	Dictionary dictionary;
	dictionary.Set( "selectedTerrainType", cityTerrainType );

	mPaintTilesInputState = CreateState< PaintTilesInputState >();
	ChangeState( mPaintTilesInputState, dictionary );
}


void EditorState::OnUpdate( float elapsedTime )
{
	GameState::OnUpdate( elapsedTime );

	// Update the World.
	mWorld.Update( elapsedTime );
}


void EditorState::OnDraw()
{
	GameState::OnDraw();

	// Draw the World.
	mWorld.Draw( mCamera );
}


void EditorState::OnExit()
{

}


void EditorState::OnScreenSizeChanged( int32 width, int32 height )
{
	GameState::OnScreenSizeChanged( width, height );
}


bool EditorState::OnPointerDown( float x, float y, size_t which )
{
	return GameState::OnPointerDown( x, y, which );
}


bool EditorState::OnPointerUp( float x, float y, size_t which )
{
	return GameState::OnPointerUp( x, y, which );
}


bool EditorState::OnPointerMotion( float x, float y, float dx, float dy, size_t which )
{
	return GameState::OnPointerMotion( x, y, dx, dy, which );
}
