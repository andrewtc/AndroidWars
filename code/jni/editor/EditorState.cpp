#include "androidwars.h"

using namespace mage;


EditorState::EditorState() :
	mCamera( gWindowWidth, gWindowHeight ),
	mBrushToolInputState( nullptr ),
	mEraserToolInputState( nullptr ),
	mToolPalette( nullptr ),
	mTilePalette( nullptr ),
	mIsPanningCamera( false )
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


ListLayout* EditorState::GetTilePalette() const
{
	return mTilePalette;
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
	mMap.FillMaxArea( tile );

	TerrainType* seaTerrainType = mScenario.TerrainTypes.FindByName( "Sea" );
	tile.SetTerrainType( seaTerrainType );
	mMap.Fill( tile, RectS( 2, 2, 4, 4 ) );

	mMap.Init( &mScenario );

	// Initialize the World.
	mWorld.Init( &mMap );

	// Set camera bounds.
	// TODO: Move this elsewhere.
	mWorld.GetCamera()->SetWorldBounds( mWorld.GetCameraBounds() );

	// Center the Camera.
	mWorld.CenterCamera();

	// Create the tool palette Widget and show it.
	mToolPalette = gWidgetManager->CreateWidgetFromTemplate( "ToolPalette" );

	if( mToolPalette )
	{
		gWidgetManager->GetRootWidget()->AddChild( mToolPalette );
		mToolPalette->Show();
	}

	// Create the tile palette Widget and hide it.
	mTilePalette = gWidgetManager->CreateWidgetFromTemplate< ListLayout >( "TilePalette" );

	if( mTilePalette )
	{
		gWidgetManager->GetRootWidget()->AddChild( mTilePalette );
		mTilePalette->Hide();

		// Add all tile type selectors to the palette.
		BuildTilePalette();
	}

	// Bind callbacks for tool palette buttons.
	Button* brushToolButton = mToolPalette->GetChildByName< Button >( "brushToolButton" );
	Button* eraserToolButton = mToolPalette->GetChildByName< Button >( "eraserToolButton" );

	if( brushToolButton )
	{
		brushToolButton->SetOnClickDelegate( [ this ]()
		{
			// Switch to the brush tool.
			DebugPrintf( "Switching to brush tool." );
			ChangeState( mBrushToolInputState );
		});
	}

	if( eraserToolButton )
	{
		eraserToolButton->SetOnClickDelegate( [ this ]()
		{
			// Switch to the eraser tool.
			DebugPrintf( "Switching to eraser tool." );
			ChangeState( mEraserToolInputState );
		});
	}

	// Create input states.
	mBrushToolInputState = CreateState< BrushToolInputState >();
	mBrushToolInputState->SetTileTemplate( CreateDefaultTileTemplate() );

	mEraserToolInputState = CreateState< EraserToolInputState >();

	// Allow the user to paint tiles.
	ChangeState( mBrushToolInputState );
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


bool EditorState::OnPointerDown( const Pointer& pointer )
{
	if( GetPointerCount() > 1 )
	{
		// Start translating the Camera if multi-touch.
		mIsPanningCamera = true;
	}

	return GameState::OnPointerDown( pointer );
}


bool EditorState::OnPointerUp( const Pointer& pointer )
{
	bool wasHandled = false;

	if( mIsPanningCamera )
	{
		if( GetPointerCount() == 1 )
		{
			// If the last pointer is being removed, stop panning the camera.
			mIsPanningCamera = false;
		}
	}
	else
	{
		// Otherwise, let the state handle the event.
		wasHandled = GameState::OnPointerUp( pointer );
	}

	return wasHandled;
}


bool EditorState::OnPointerMotion( const Pointer& activePointer, const PointersByID& pointersByID )
{
	bool wasHandled = false;

	if( mIsPanningCamera )
	{
		if( activePointer.isMoving )
		{
			// If multiple pointers are down, pan the Camera.
			mWorld.GetCamera()->TranslateLookAt( -activePointer.GetDisplacement() );
			wasHandled = true;
		}
	}
	else
	{
		// Otherwise, let the active state handle the event.
		wasHandled = GameState::OnPointerMotion( activePointer, pointersByID );
	}

	return wasHandled;
}


void EditorState::BuildTilePalette()
{
	if( mTilePalette )
	{
		// Clear all items in the tile palette.
		mTilePalette->DestroyAllItems();

		// Get the WidgetTemplate for the tile selector.
		WidgetTemplate* tileSelectorTemplate = gWidgetManager->GetTemplate( "TileSelector" );

		if( tileSelectorTemplate )
		{
			const TerrainTypesTable::RecordsByHashedName& terrainTypes = mScenario.TerrainTypes.GetRecords();
			for( auto it = terrainTypes.begin(); it != terrainTypes.end(); ++it )
			{
				// Add a button for each TerrainType in the Scenario.
				TerrainType* terrainType = it->second;
				Button* selector = mTilePalette->CreateItem< Button >( *tileSelectorTemplate );

				if( selector )
				{
					// Get the icon for this selector.
					Graphic* icon = selector->GetChildByName< Graphic >( "icon" );

					if( icon )
					{
						// Use the TerrainType sprite as the image for the selector.
						icon->SetSprite( terrainType->GetAnimationSetName(), "Idle" );
					}
					else
					{
						WarnFail( "Could not set icon for tile selector button \"%s\" because no \"icon\" Graphic was found!", selector->GetFullName().c_str() );
					}

					selector->SetOnClickDelegate( [ this, terrainType ]()
					{
						// Build a template tile.
						Tile tile = CreateTileTemplate( terrainType );

						// Change the selected tile type.
						mBrushToolInputState->SetTileTemplate( tile );
					});
				}
			}
		}
		else
		{
			WarnFail( "Could not build tile palette because no tile selector template was found!" );
		}
	}
}
