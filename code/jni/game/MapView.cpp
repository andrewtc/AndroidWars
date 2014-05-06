#include "androidwars.h"

using namespace mage;

const float MapView::TILE_WORLD_SCALE = 64.0f;
const float MapView::INVERSE_TILE_WORLD_SCALE = ( 1.0f / TILE_WORLD_SCALE );

const float MapView::MAP_BORDER_SCALE = ( TILE_WORLD_SCALE * 5.0f );


MapView::MapView() :
	mMap( nullptr ),
	mDefaultFont( nullptr ),
	mCamera( gWindowWidth, gWindowHeight ),
	mSelectedUnitSprite( nullptr ),
	mTargetedUnitSprite( nullptr ),
	mCurrentMapAnimation( nullptr )
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
		CreateUnitSprite( unit );
	});

	// Center the Camera initially.
	CenterCamera();

	// Listen for when the Map is resized.
	mMap->OnResize.AddCallback( this, &MapView::MapResized );

	// Listen for when Units are created.
	mMap->OnUnitCreated.AddCallback( this, &MapView::UnitCreated );

	// Listen for when the Map is changed.
	mMap->OnTileChanged.AddCallback( this, &MapView::TileChanged );
}


void MapView::Destroy()
{
	assertion( !IsInitialized(), "Cannot destroy MapView that is not initialized!" );

	// Remove the callbacks.
	mMap->OnResize.RemoveCallback( this, &MapView::MapResized );
	mMap->OnUnitCreated.RemoveCallback( this, &MapView::UnitCreated );
	mMap->OnTileChanged.RemoveCallback( this, &MapView::TileChanged );

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

	for( auto it = mUnitSprites.begin(); it != mUnitSprites.end(); ++it )
	{
		UnitSprite* unitSprite = *it;
		unitSprite->Update( elapsedTime );
	}

	if( mCurrentMapAnimation )
	{
		// If the MapView is currently playing an animation, update it.
		mCurrentMapAnimation->Update( elapsedTime );

		if( mCurrentMapAnimation->IsFinished() )
		{
			// If the animation finished, destroy it.
			delete mCurrentMapAnimation;
			mCurrentMapAnimation = nullptr;
		}
	}

	while( !mCurrentMapAnimation && mScheduledMapAnimations.size() > 0 )
	{
		// If no MapAnimation is playing and there is at least one scheduled MapAnimation,
		// start the next MapAnimation.
		mCurrentMapAnimation = mScheduledMapAnimations.front();
		mScheduledMapAnimations.pop_front();
		mCurrentMapAnimation->Start( this );

		if( mCurrentMapAnimation->IsFinished() )
		{
			// If the animation finished immediately, destroy it.
			delete mCurrentMapAnimation;
			mCurrentMapAnimation = nullptr;
		}
	}

	if( mArrowSprite.IsInitialized() )
	{
		// Update the arrow sprite (if necessary).
		mArrowSprite.Update( elapsedTime );
	}

	if( mTargetSprite.IsInitialized() )
	{
		mTargetSprite.Update( elapsedTime );
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

	if( mTargetSprite.IsInitialized() )
	{
		// Draw the target over the targeted UnitSprite.
		mTargetSprite.Draw( mCamera );
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


TileSprite* MapView::GetTileSpriteAtScreenCoords( float screenX, float screenY )
{
	return GetTileSpriteAtScreenCoords( Vec2f( screenX, screenY ) );
}


const TileSprite* MapView::GetTileSpriteAtScreenCoords( float screenX, float screenY ) const
{
	return GetTileSpriteAtScreenCoords( Vec2f( screenX, screenY ) );
}


TileSprite* MapView::GetTileSpriteAtScreenCoords( const Vec2f& screenCoords )
{
	Vec2f worldCoords = ScreenToWorldCoords( screenCoords );
	return GetTileSpriteAtWorldCoords( worldCoords );
}


const TileSprite* MapView::GetTileSpriteAtScreenCoords( const Vec2f& screenCoords ) const
{
	Vec2f worldCoords = ScreenToWorldCoords( screenCoords );
	return GetTileSpriteAtWorldCoords( worldCoords );
}


TileSprite* MapView::GetTileSpriteAtWorldCoords( float worldX, float worldY )
{
	return GetTileSpriteAtWorldCoords( Vec2f( worldX, worldY ) );
}


const TileSprite* MapView::GetTileSpriteAtWorldCoords( float worldX, float worldY ) const
{
	return GetTileSpriteAtWorldCoords( Vec2f( worldX, worldY ) );
}


TileSprite* MapView::GetTileSpriteAtWorldCoords( const Vec2f& worldCoords )
{
	return MapView::GetTileSpriteAtTileCoords( WorldToTileCoords( worldCoords ) );
}


const TileSprite* MapView::GetTileSpriteAtWorldCoords( const Vec2f& worldCoords ) const
{
	return MapView::GetTileSpriteAtTileCoords( WorldToTileCoords( worldCoords ) );
}


TileSprite* MapView::GetTileSpriteAtTileCoords( short tileX, short tileY )
{
	return GetTileSpriteAtTileCoords( Vec2s( tileX, tileY ) );
}


const TileSprite* MapView::GetTileSpriteAtTileCoords( short tileX, short tileY ) const
{
	return GetTileSpriteAtTileCoords( Vec2s( tileX, tileY ) );
}


TileSprite* MapView::GetTileSpriteAtTileCoords( const Vec2s& tileCoords )
{
	TileSprite* result = nullptr;

	// Get an iterator to the TileSprite at the specified coordinates.
	TileSpritesGrid::Iterator iterator = mTileSprites.GetTile( tileCoords );

	if( iterator.IsValid() )
	{
		// If the position is valid, return the TileSprite at the Grid position (if any).
		result = &( *iterator );
	}

	return result;
}


const TileSprite* MapView::GetTileSpriteAtTileCoords( const Vec2s& tileCoords ) const
{
	const TileSprite* result = nullptr;

	// Get an iterator to the TileSprite at the specified coordinates.
	TileSpritesGrid::ConstIterator iterator = mTileSprites.GetTile( tileCoords );

	if( iterator.IsValid() )
	{
		// If the position is valid, return the TileSprite at the Grid position (if any).
		result = &( *iterator );
	}

	return result;
}


UnitSprite* MapView::CreateUnitSprite( Unit* unit )
{
	DebugPrintf( "Creating UnitSprite at (%d,%d)!", unit->GetTileX(), unit->GetTileY() );

	// Create and initialize a new UnitSprite.
	UnitSprite* unitSprite = new UnitSprite( this, unit );
	unitSprite->Init();

	// Add the UnitSprite to the list of UnitSprites.
	mUnitSprites.push_back( unitSprite );

	return unitSprite;
}


void MapView::DestroyUnitSprite( UnitSprite* unitSprite )
{
	assertion( unitSprite->GetMapView() == this, "Cannot destroy UnitSprite that was created by another MapView!" );

	Unit* unit = unitSprite->GetUnit();
	DebugPrintf( "Destroying UnitSprite at (%d,%d)!", unit->GetTileX(), unit->GetTileY() );

	for( auto it = mUnitSprites.begin(); it != mUnitSprites.end(); ++it )
	{
		if( *it == unitSprite )
		{
			// Remove the UnitSprite from the list of UnitSprites.
			mUnitSprites.erase( it );
			break;
		}
	}

	if( mTargetedUnitSprite == unitSprite )
	{
		// If the targeted UnitSprite is destroyed, destroy the TargetSprite.
		UntargetUnitSprite();
	}

	// Destroy the UnitSprite.
	unitSprite->Destroy();
	delete unitSprite;
}


UnitSprite* MapView::GetUnitSpriteForUnit( Unit* unit ) const
{
	UnitSprite* result = nullptr;

	if( unit )
	{
		for( auto it = mUnitSprites.begin(); it != mUnitSprites.end(); ++it )
		{
			UnitSprite* unitSprite = *it;

			if( unitSprite->GetUnit() == unit )
			{
				result = unitSprite;
				break;
			}
		}
	}
	else
	{
		WarnFail( "Cannot get UnitSprite for null Unit!" );
	}

	return result;
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

		if( unitSprite->IsVisible() )
		{
			RectF bounds = unitSprite->GetWorldBounds();

			if( bounds.Contains( worldCoords.x, worldCoords.y ) )
			{
				// If a Sprite is found at this location, return it.
				result = unitSprite;
				break;
			}
		}
	}

	return result;
}


void MapView::SelectUnitSprite( UnitSprite* unitSprite, bool showArrow )
{
	if( mSelectedUnitSprite )
	{
		// Deselect previously selected UnitSprite.
		DeselectAllTiles();
	}

	// Set the new UnitSprite.
	mSelectedUnitSprite = unitSprite;
	UnitSpriteSelected( mSelectedUnitSprite, showArrow );

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


void MapView::TargetUnitSprite( UnitSprite* unitSprite )
{
	// Set the new targeted UnitSprite.
	mTargetedUnitSprite = unitSprite;

	if( mTargetedUnitSprite )
	{
		if( !mTargetSprite.IsInitialized() )
		{
			// Initialize the TargetSprite (if necessary).
			mTargetSprite.Init();
		}

		// Update the position of the TargetSprite.
		mTargetSprite.SetPosition( mTargetedUnitSprite->GetPosition() );
	}
	else if( mTargetSprite.IsInitialized() )
	{
		// If the current Unit is untargeted, destroy the TargetSprite.
		mTargetSprite.Destroy();
	}
}


void MapView::UntargetUnitSprite()
{
	TargetUnitSprite( nullptr );
}


UnitSprite* MapView::GetTargetedUnitSprite() const
{
	return mTargetedUnitSprite;
}


bool MapView::HasTargetedUnitSprite() const
{
	return ( mTargetedUnitSprite != nullptr );
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


void MapView::DetermineAvailableActionsForSelectedUnit()
{
	if( mSelectedUnitSprite )
	{
		// Get the selected Unit and Path.
		Unit* unit = mSelectedUnitSprite->GetUnit();
		const Path& path = mArrowSprite.GetPath();

		// Determine the available actions for the selected Unit and store them for later use.
		mMap->DetermineAvailableActions( unit, path, mSelectedUnitActions );
	}
	else
	{
		WarnFail( "Could not determine available actions for selected Unit because no UnitSprite is selected!" );
	}
}


const Actions& MapView::GetAvailableActionsForSelectedUnit() const
{
	return mSelectedUnitActions;
}


MapAnimation* MapView::ScheduleAnimationForAction( Ability::Action* action )
{
	DebugPrintf( "Scheduling animation..." );

	MapAnimation* result = nullptr;

	// Try casting the action to a UnitAbility::Action.
	UnitAbility::Action* unitAction = dynamic_cast< UnitAbility::Action* >( action );

	if( unitAction )
	{
		// If this is a Unit action, get the Unit for this Action.
		Unit* unit = mMap->GetUnitByID( unitAction->UnitID );
		assertion( unit, "Cannot schedule MapAnimation for invalid Unit ID (%d)!", unitAction->UnitID );

		UnitSprite* unitSprite = GetUnitSpriteForUnit( unit );
		assertion( unitSprite, "Cannot schedule MapAnimation for Unit because no associated UnitSprite was found!" );

		// Schedule a new move Action following the movement path.
		// TODO: Allow for traps.
		result = ScheduleMapAnimation< UnitMoveMapAnimation >( unitSprite, unitAction->MovementPath );
	}

	return result;
}


MapAnimation* MapView::GetCurrentMapAnimation() const
{
	return mCurrentMapAnimation;
}


bool MapView::IsPlayingMapAnimation() const
{
	return ( mCurrentMapAnimation != nullptr );
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


void MapView::UnitCreated( Unit* unit )
{
	if( IsInitialized() )
	{
		// Create a UnitSprite for the Unit.
		CreateUnitSprite( unit );
	}
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


void MapView::UnitSpriteSelected( UnitSprite* unitSprite, bool showArrow )
{
	if( showArrow && unitSprite )
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
