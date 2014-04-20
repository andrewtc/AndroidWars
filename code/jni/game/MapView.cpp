#include "androidwars.h"

using namespace mage;

const float MapView::TILE_WORLD_SCALE = 64.0f;
const float MapView::INVERSE_TILE_WORLD_SCALE = ( 1.0f / TILE_WORLD_SCALE );

const float MapView::MAP_BORDER_SCALE = ( TILE_WORLD_SCALE * 5.0f );


MapView::MapView() :
	mMap( nullptr ),
	mDefaultFont( nullptr ),
	mCamera( gWindowWidth, gWindowHeight ),
	mSelectedUnitSprite( nullptr )
{ }


MapView::~MapView()
{
	if( IsInitialized() )
	{
		Destroy();
	}
}


void MapView::Init( Map* map )
{
	assertion( !IsInitialized(), "Cannot initialize " STRINGIFY( MapView ) " that has already been initialized!" );

	// Set the Map.
	mMap = map;
	assertion( mMap, "Cannot initialize MapView without a valid Map!" );

	// Make sure a default Font was loaded.
	assertion( mDefaultFont, "Cannot initialize " STRINGIFY( MapView ) " without a valid default Font!" );

	// Resize to fill the whole tile buffer (for initialization).
	mTileSprites.ForEachTileInMaxArea( [this]( const TileSpritesGrid::Iterator& tileSprite )
	{
		// Initialize all TileSprites.
		tileSprite->Init( this, tileSprite.GetPosition() );
	});

	// Create initial TileSprites.
	MapResized( Vec2s::ZERO, mMap->GetSize() );

	// Create initial UnitSprites.
	mMap->ForEachUnit( [this]( Unit* unit )
	{
		DebugPrintf( "Creating UnitSprite at (%d,%d)!", unit->GetTileX(), unit->GetTileY() );
		CreateUnitSprite( unit );
	});

	// Center the Camera initially.
	CenterCamera();

	// Listen for when the Map is resized.
	mMap->OnResize.AddCallback( this, &MapView::MapResized );

	// Listen for when the Map is changed.
	mMap->OnTileChanged.AddCallback( this, &MapView::TileChanged );
}


void MapView::Destroy()
{
	assertion( !IsInitialized(), "Cannot destroy MapView that is not initialized!" );

	// Remove the resize callback.
	mMap->OnResize.RemoveCallback( this, &MapView::MapResized );

	// Reset the Map reference.
	mMap = nullptr;
}


void MapView::Update( float elapsedTime )
{
	mTileSprites.ForEachTile( [ this, elapsedTime ]( const TileSpritesGrid::Iterator& tileSprite )
	{
		// Update the TileSprite that represents this Tile.
		tileSprite->Update( elapsedTime );
	});

	if( mArrowSprite.IsInitialized() )
	{
		// Draw the arrow sprite (if necessary).
		mArrowSprite.Update( elapsedTime );
	}
}


void MapView::Draw()
{
	mTileSprites.ForEachTile( [ this ]( const TileSpritesGrid::Iterator& tileSprite )
	{
		// Draw the TileSprite that represents this Tile.
		tileSprite->Draw();
	});

	for( auto it = mUnitSprites.begin(); it != mUnitSprites.end(); ++it )
	{
		// Draw all non-selected UnitSprites.
		UnitSprite* unitSprite = *it;

		if( unitSprite != mSelectedUnitSprite )
		{
			unitSprite->Draw( mCamera );
		}
	}

	if( mArrowSprite.IsInitialized() )
	{
		// If there is a selected Unit, draw the arrow path.
		mArrowSprite.Draw( mCamera );
	}

	if( mSelectedUnitSprite )
	{
		// Draw the selected Unit.
		mSelectedUnitSprite->Draw( mCamera );
	}
}


Map* MapView::GetMap() const
{
	return mMap;
}


RectF MapView::GetMapBounds() const
{
	return RectF( 0.0f, 0.0f, mMap->GetWidth() * TILE_WORLD_SCALE, mMap->GetHeight() * TILE_WORLD_SCALE );
}


RectF MapView::GetCameraBounds() const
{
	RectF cameraBounds = GetMapBounds();

	cameraBounds.Left -= MAP_BORDER_SCALE;
	cameraBounds.Top -= MAP_BORDER_SCALE;
	cameraBounds.Right += MAP_BORDER_SCALE;
	cameraBounds.Bottom += MAP_BORDER_SCALE;

	return cameraBounds;
}


Camera* MapView::GetCamera()
{
	return &mCamera;
}


const Camera* MapView::GetCamera() const
{
	return &mCamera;
}


void MapView::CenterCamera()
{
	// Set the position of the Camera to the center of the MapView.
	RectF bounds = GetCameraBounds();
	mCamera.LookAt( Vec2f( bounds.CenterX(), bounds.CenterY() ) );
}


void MapView::SetDefaultFont( BitmapFont* font )
{
	mDefaultFont = font;
}


BitmapFont* MapView::GetDefaultFont() const
{
	return mDefaultFont;
}



MapView::TileSpritesGrid& MapView::GetTileSprites()
{
	return mTileSprites;
}



const MapView::TileSpritesGrid& MapView::GetTileSprites() const
{
	return mTileSprites;
}


UnitSprite* MapView::CreateUnitSprite( Unit* unit )
{
	// Create and initialize a new UnitSprite.
	UnitSprite* unitSprite = new UnitSprite( this, unit );
	unitSprite->Init();

	// Add the UnitSprite to the list of UnitSprites.
	mUnitSprites.push_back( unitSprite );

	return unitSprite;
}


UnitSprite* MapView::GetUnitSpriteAtScreenCoords( float screenX, float screenY ) const
{
	return GetUnitSpriteAtScreenCoords( Vec2f( screenX, screenY ) );
}


UnitSprite* MapView::GetUnitSpriteAtScreenCoords( const Vec2f& screenCoords ) const
{
	Vec2f worldCoords = ScreenToWorldCoords( screenCoords );
	return GetUnitSpriteAtWorldCoords( worldCoords );
}


UnitSprite* MapView::GetUnitSpriteAtWorldCoords( float worldX, float worldY ) const
{
	return GetUnitSpriteAtWorldCoords( Vec2f( worldX, worldY ) );
}


UnitSprite* MapView::GetUnitSpriteAtWorldCoords( const Vec2f& worldCoords ) const
{
	UnitSprite* result = nullptr;

	for( auto it = mUnitSprites.begin(); it != mUnitSprites.end(); ++it )
	{
		// Get the bounds of the UnitSprite.
		UnitSprite* unitSprite = *it;
		RectF bounds = unitSprite->GetWorldBounds();

		if( bounds.Contains( worldCoords.x, worldCoords.y ) )
		{
			// If a Sprite is found at this location, return it.
			result = unitSprite;
			break;
		}
	}

	return result;
}


void MapView::SelectUnitSprite( UnitSprite* unitSprite )
{
	if( mSelectedUnitSprite )
	{
		// Deselect previously selected UnitSprite.
		DeselectAllTiles();
	}

	// Set the new UnitSprite.
	mSelectedUnitSprite = unitSprite;
	UnitSpriteSelected( mSelectedUnitSprite );

	if( mSelectedUnitSprite )
	{
		// Select the new UnitSprite.
		SelectAllReachableTilesForUnit( mSelectedUnitSprite->GetUnit() );
	}
}


void MapView::DeselectUnitSprite()
{
	SelectUnitSprite( nullptr );
}


UnitSprite* MapView::GetSelectedUnitSprite() const
{
	return mSelectedUnitSprite;
}


bool MapView::HasSelectedUnitSprite() const
{
	return ( mSelectedUnitSprite != nullptr );
}


const ArrowSprite& MapView::GetArrowSprite() const
{
	return mArrowSprite;
}


Path& MapView::GetSelectedUnitPath()
{
	return mArrowSprite.GetPath();
}


const Path& MapView::GetSelectedUnitPath() const
{
	return mArrowSprite.GetPath();
}


Vec2f MapView::WorldToScreenCoords( const Vec2f& worldCoords ) const
{
	return WorldToScreenCoords( worldCoords.x, worldCoords.y );
}


Vec2f MapView::WorldToScreenCoords( float worldX, float worldY ) const
{
	return Vec2f( worldX, worldY ) - mCamera.GetPosition();
}


Vec2f MapView::ScreenToWorldCoords( const Vec2f& screenCoords ) const
{
	return ScreenToWorldCoords( screenCoords.x, screenCoords.y );
}


Vec2f MapView::ScreenToWorldCoords( float screenX, float screenY ) const
{
	return Vec2f( screenX, screenY ) + mCamera.GetPosition();
}


Vec2s MapView::WorldToTileCoords( const Vec2f& worldCoords ) const
{
	return WorldToTileCoords( worldCoords.x, worldCoords.y );
}


Vec2s MapView::WorldToTileCoords( float worldX, float worldY ) const
{
	return Vec2s( worldX * INVERSE_TILE_WORLD_SCALE, worldY * INVERSE_TILE_WORLD_SCALE );
}


Vec2f MapView::TileToWorldCoords( const Vec2s& tileCoords ) const
{
	return TileToWorldCoords( tileCoords.x, tileCoords.y );
}


Vec2f MapView::TileToWorldCoords( short tileX, short tileY ) const
{
	return Vec2f( tileX * TILE_WORLD_SCALE, tileY * TILE_WORLD_SCALE );
}


bool MapView::IsInitialized() const
{
	return ( mMap != nullptr );
}


void MapView::MapResized( const Vec2s& oldSize, const Vec2s& newSize )
{
	DebugPrintf( "Resized map from (%d,%d) to (%d,%d).", oldSize.x, oldSize.y, newSize.x, newSize.y );

	// Determine the area of TileSprites affected by the resize.
	short refreshWidth  = std::max( oldSize.x, newSize.x );
	short refreshHeight = std::max( oldSize.y, newSize.y );
	RectS area( 0, 0, refreshWidth, refreshHeight );

	mTileSprites.ForEachTileInArea( area, []( const TileSpritesGrid::Iterator& tileSprite )
	{
		// Update the sprites of all TileSprites in the refresh area.
		tileSprite->UpdateSprite();
	});

	// Resize the TileSprites grid.
	mTileSprites.Resize( newSize );

	// Refresh the Camera bounds.
	mCamera.SetWorldBounds( GetCameraBounds() );
}


void MapView::TileChanged( const Map::Iterator& tile )
{
	// Update the TileSprite for this tile.
	TileSpritesGrid::Iterator tileSprite = mTileSprites.GetTile( tile.GetPosition() );
	tileSprite->UpdateSprite();

	tileSprite.ForEachAdjacent( []( const TileSpritesGrid::Iterator& adjacent )
	{
		// Update adjacent TileSprites.
		adjacent->UpdateSprite();
	});
}


void MapView::UnitSpriteSelected( UnitSprite* unitSprite )
{
	if( unitSprite )
	{
		// If a Unit was selected, initialize the arrow sprite (if necessary).
		if( !mArrowSprite.IsInitialized() )
		{
			DebugPrintf( "Initializing ArrowSprite..." );
			mArrowSprite.Init( this );
		}

		// Determine the location of the Unit for the selected UnitSprite.
		Unit* unit = unitSprite->GetUnit();
		Vec2s tilePos = unit->GetTilePos();

		// Reset the path of the arrow sprite.
		DebugPrintf( "Setting ArrowSprite..." );
		Path path;
		path.SetOrigin( tilePos );

		mArrowSprite.SetPath( path );
	}
	else
	{
		// Otherwise, destroy the arrow sprite.
		if( mArrowSprite.IsInitialized() )
		{
			DebugPrintf( "Destroying ArrowSprite..." );
			mArrowSprite.Destroy();
		}
	}
}


void MapView::SelectAllReachableTilesForUnit( Unit* unit )
{
	// Select all reachable tiles for the Unit.
	mMap->ForEachReachableTile( unit, [this]( const Map::Iterator& tile, const Unit* unit )
	{
		// Get the TileSprite for this position.
		MapView::TileSpritesGrid::Iterator tileSpriteIt = mTileSprites.GetTile( tile.GetPosition() );

		if( tileSpriteIt.IsValid() )
		{
			// Select the TileSprite.
			tileSpriteIt->Select();
		}
	});
}


void MapView::DeselectAllTiles()
{
	mTileSprites.ForEachTile( [this]( const TileSpritesGrid::Iterator& tile )
	{
		// Deselect all tiles.
		tile->Deselect();
	});
}
