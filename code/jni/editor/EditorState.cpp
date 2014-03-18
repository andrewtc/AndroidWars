#include "androidwars.h"

using namespace mage;


EditorState::EditorState() :
	mCamera( gWindowWidth, gWindowHeight ),
	mWorld( &mMap ),
	mBrushToolInputState( nullptr ),
	mEraserToolInputState( nullptr ),
	mToolPalette( nullptr )
{ }


EditorState::~EditorState()
{
}


Map* EditorState::GetMap()
{
	return &mMap;
}


const Map* EditorState::GetMap() const
{
	return &mMap;
}


World* EditorState::GetWorld()
{
	return &mWorld;
}


const World* EditorState::GetWorld() const
{
	return &mWorld;
}


Tile EditorState::CreateTileTemplate( TerrainType* terrainType )
{
	// Create and return the Tile template.
	Tile tileTemplate;
	tileTemplate.SetTerrainType( terrainType );
	return tileTemplate;
}


Tile EditorState::CreateDefaultTileTemplate()
{
	// Get the default TerrainType for this scenario.
	TerrainType* defaultTerrainType = mScenario.GetDefaultTerrainType();

	// Create and return a Tile template for the default TerrainType.
	return CreateTileTemplate( defaultTerrainType );
}


void EditorState::PaintTileAt( float x, float y, const Tile& tile )
{
	// Find the Tile the user pressed.
	Vec2f worldCoords = mWorld.ScreenToWorldCoords( x, y );
	Vec2s tilePos = mWorld.WorldToTileCoords( worldCoords );

	if( mMap.IsValidTilePos( tilePos ) )
	{
		// If the Tile at the coordinates is valid, paint the tile.
		mMap.SetTile( tilePos, tile );
	}
}


void EditorState::OnEnter( const Dictionary& parameters )
{
	// Create the database.
	mScenario.LoadDataFromFile( "data/Data.json" );

	// Create a new Map.
	mMap.Resize( 16, 12 );

	// Set camera bounds.
	// TODO: Move this elsewhere.
	mWorld.GetCamera()->SetWorldBounds( mWorld.GetCameraBounds() );

	// Center the Camera.
	mWorld.CenterCamera();

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

	// Create the tool palette Widget and show it.
	mToolPalette = gWidgetManager->CreateWidgetFromTemplate( "ToolPalette" );
	gWidgetManager->GetRootWidget()->AddChild( mToolPalette );
	mToolPalette->Show();

	// Bind callbacks for tool palette buttons.
	Button* brushToolButton = mToolPalette->GetChildByName< Button >( "brushToolButton" );
	Button* eraserToolButton = mToolPalette->GetChildByName< Button >( "eraserToolButton" );

	if( brushToolButton )
	{
		brushToolButton->SetOnClickDelegate( [ this ]( float x, float y )
		{
			// Switch to the brush tool.
			ChangeState( mBrushToolInputState );
		});
	}

	if( eraserToolButton )
	{
		eraserToolButton->SetOnClickDelegate( [ this ]( float x, float y )
		{
			// Switch to the eraser tool.
			ChangeState( mEraserToolInputState );
		});
	}

	// Create input states.
	mBrushToolInputState = CreateState< BrushToolInputState >();
	TerrainType* cityTerrainType = mScenario.TerrainTypes.FindByName( "City" );
	mBrushToolInputState->SetTileTemplate( CreateTileTemplate( cityTerrainType ) );

	mEraserToolInputState = CreateState< EraserToolInputState >();

	// Allow the user to paint tiles.

	Dictionary dictionary;
	dictionary.Set( "tileTemplate", CreateTileTemplate( cityTerrainType ) );

	ChangeState( mBrushToolInputState, dictionary );
}


void EditorState::OnUpdate( float elapsedTime )
{
	GameState::OnUpdate( elapsedTime );

	// Update the World.
	mWorld.Update( elapsedTime );
}


void EditorState::OnDraw()
{
	// Draw the World.
	mWorld.Draw();

	// Draw Widgets.
	GameState::OnDraw();
}


void EditorState::OnExit()
{
	// Destroy the tool palette.
	gWidgetManager->DestroyWidget( mToolPalette );

	// Destroy all states.
	DestroyState( mBrushToolInputState );

	// Destroy the Map.
	mMap.Destroy();
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
