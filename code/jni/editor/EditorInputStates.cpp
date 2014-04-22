#include "androidwars.h"

using namespace mage;


BrushToolInputState::BrushToolInputState( GameState* owner ) :
	DerivedInputState( owner ),
	mTileTemplate()
{ }


BrushToolInputState::~BrushToolInputState() { }


void BrushToolInputState::OnEnter( const Dictionary& parameters )
{
	EditorState* owner = GetOwnerDerived();

	// Get the selected Tile template (if specified).
	Tile tileTemplate;
	Dictionary::DictionaryError error = parameters.Get( "tileTemplate", tileTemplate );

	if( error == Dictionary::DErr_SUCCESS )
	{
		// Set the selected Tile template.
		SetTileTemplate( tileTemplate );
	}

	// Show the tile palette.
	owner->GetTilePalette()->Show();
}


void BrushToolInputState::OnExit()
{
	EditorState* owner = GetOwnerDerived();

	// Hide the tile palette.
	owner->GetTilePalette()->Hide();
}


bool BrushToolInputState::OnPointerDown( const Pointer& pointer )
{
	return false; //InputState::OnPointerDown( pointer );
}


bool BrushToolInputState::OnPointerUp( const Pointer& pointer )
{
	bool wasHandled = false; //InputState::OnPointerUp( pointer );

	if( !wasHandled && !pointer.hasMoved )
	{
		// Paint a tile.
		GetOwnerDerived()->PaintTileAt( pointer.position.x, pointer.position.y, mTileTemplate );
		wasHandled = true;
	}

	return wasHandled;
}


bool BrushToolInputState::OnPointerMotion( const Pointer& activePointer, const PointersByID& pointersByID )
{
	bool wasHandled = false; //InputState::OnPointerMotion( activePointer, pointersByID );
	EditorState* owner = GetOwnerDerived();

	if( !wasHandled )
	{
		// Paint a tile.
		GetOwnerDerived()->PaintTileAt( activePointer.position.x, activePointer.position.y, mTileTemplate );
		wasHandled = true;
	}

	return wasHandled;
}


void BrushToolInputState::SetTileTemplate( const Tile& tile )
{
	// Set the terrain type of the template Tile.
	mTileTemplate = tile;
}


PlaceToolInputState::PlaceToolInputState( GameState* owner ) :
	DerivedInputState( owner ),
	mUnitPlacementSprite( nullptr ),
	mSelectedFaction( nullptr ),
	mSelectedUnitType( nullptr )
{ }


PlaceToolInputState::~PlaceToolInputState() { }


void PlaceToolInputState::OnEnter( const Dictionary& parameters )
{
	EditorState* owner = GetOwnerDerived();
}


void PlaceToolInputState::OnExit()
{
	// Destroy the Unit placement Sprite (if it exists).
	DestroyUnitPlacementSprite();
}


void PlaceToolInputState::OnDraw()
{
	if( mUnitPlacementSprite )
	{
		EditorState* owner = GetOwnerDerived();
		MapView* mapView = owner->GetMapView();

		// Draw the Unit placement sprite.
		mUnitPlacementSprite->OnDraw( *mapView->GetCamera() );
	}
}


bool PlaceToolInputState::OnPointerDown( const Pointer& pointer )
{
	// Make sure there is a valid UnitType and Faction selected.
	if( mSelectedUnitType && mSelectedFaction && pointer.IsActivePointer() )
	{
		EditorState* owner = GetOwnerDerived();
		MapView* mapView = owner->GetMapView();

		// If this is the active pointer, create a new Unit placement sprite under it.
		CreateUnitPlacementSprite( mapView->ScreenToWorldCoords( pointer.position ) );
	}

	return false; //InputState::OnPointerDown( pointer );
}


bool PlaceToolInputState::OnPointerUp( const Pointer& pointer )
{
	bool wasHandled = false; //InputState::OnPointerUp( pointer );

	if( !wasHandled && mUnitPlacementSprite )
	{
		if( GetPointerCount() == 1 )
		{
			// If the last Pointer leaves the screen, destroy the Unit placement Sprite.
			DestroyUnitPlacementSprite();

			// Place a Unit.
			Unit* unit = CreateUnitAtScreenCoords( pointer.position );

			wasHandled = true;
		}
		else
		{
			// Otherwise, move the pointer to the new active pointer position.
			const Pointer& activePointer = GetActivePointer();
			MoveUnitPlacementSprite( activePointer.position );
		}
	}

	return wasHandled;
}


bool PlaceToolInputState::OnPointerMotion( const Pointer& activePointer, const PointersByID& pointersByID )
{
	bool wasHandled = false; //InputState::OnPointerMotion( activePointer, pointersByID );

	if( !wasHandled && mUnitPlacementSprite && activePointer.isMoving )
	{
		EditorState* owner = GetOwnerDerived();
		MapView* mapView = owner->GetMapView();

		// Move the Unit placement Sprite to the position of the active pointer.
		MoveUnitPlacementSprite( mapView->ScreenToWorldCoords( activePointer.position ) );

		wasHandled = true;
	}

	return wasHandled;
}


void PlaceToolInputState::SetSelectedUnitType( UnitType* unitType )
{
	mSelectedUnitType = unitType;
}


void PlaceToolInputState::SetSelectedFaction( Faction* faction )
{
	mSelectedFaction = faction;
}


void PlaceToolInputState::CreateUnitPlacementSprite( const Vec2f& position )
{
	assertion( mUnitPlacementSprite == nullptr, "Tried to create another Unit placement Sprite when one already exists!" );

	if( mSelectedUnitType )
	{
		if( mSelectedFaction )
		{
			// Get the color of the selected Faction and the UnitType animation set name.
			HashString animationSet = mSelectedUnitType->GetAnimationSetName();
			Color factionColor = mSelectedFaction->GetColor();

			// Create a new Unit placement Sprite.
			mUnitPlacementSprite = SpriteManager::CreateSprite( animationSet, Vec2f::ZERO, UnitSprite::DEFAULT_ANIMATION_NAME );
			mUnitPlacementSprite->DrawColor = factionColor;

			// Move the Unit placement sprite into position.
			MoveUnitPlacementSprite( position );
		}
		else
		{
			WarnFail( "Cannot create Unit placement Sprite because no Faction was selected!" );
		}
	}
	else
	{
		WarnFail( "Cannot create Unit placement Sprite because no UnitType was selected!" );
	}
}


void PlaceToolInputState::MoveUnitPlacementSprite( const Vec2f& position )
{
	if( mUnitPlacementSprite )
	{
		// Get the bounds of the animation.
		RectI bounds = mUnitPlacementSprite->GetClippingRectForCurrentAnimation();

		// Calculate an offset to center the sprite.
		Vec2f offset( -1.0f * bounds.CenterX(), -1.0f * bounds.CenterY() );

		// Move the Sprite to the position with an offset.
		mUnitPlacementSprite->Position = ( position + offset );
	}
}


void PlaceToolInputState::DestroyUnitPlacementSprite()
{
	if( mUnitPlacementSprite )
	{
		SpriteManager::DestroySprite( mUnitPlacementSprite );
	}
}


Unit* PlaceToolInputState::CreateUnitAtScreenCoords( const Vec2f& screenCoords )
{
	Unit* unit = nullptr;

	if( mSelectedUnitType )
	{
		if( mSelectedFaction )
		{
			EditorState* owner = GetOwnerDerived();
			MapView* mapView = owner->GetMapView();
			Map* map = owner->GetMap();

			// Get the Tile at the screen coords.
			Vec2f worldPos = mapView->ScreenToWorldCoords( screenCoords );
			Vec2s tilePos = mapView->WorldToTileCoords( worldPos );
			Map::Iterator tile = map->GetTile( tilePos );

			if( tile.IsValid() && tile->IsEmpty() && mSelectedUnitType->CanMoveAcrossTerrain( tile->GetTerrainType() ) )
			{
				// If the selected UnitType can be placed into the Tile, create a new Unit.
				DebugPrintf( "Placing Unit at tile (%d,%d)!", tilePos.x, tilePos.y );
				unit = map->CreateUnit( mSelectedUnitType, mSelectedFaction, tilePos );
			}
			else
			{
				WarnFail( "Cannot place Unit into tile (%d,%d)!", tilePos.x, tilePos.y );
			}
		}
		else
		{
			WarnFail( "Cannot create Unit because no Faction was selected!" );
		}
	}
	else
	{
		WarnFail( "Cannot create Unit because no UnitType was selected!" );
	}

	return unit;
}


EraserToolInputState::EraserToolInputState( GameState* owner ) :
	DerivedInputState( owner )
{ }


EraserToolInputState::~EraserToolInputState() { }


void EraserToolInputState::OnEnter( const Dictionary& parameters ) { }


void EraserToolInputState::OnExit() { }


bool EraserToolInputState::OnPointerDown( const Pointer& pointer )
{
	if( pointer.IsActivePointer() )
	{
		// Destroy the Unit at the pointer location.
		DestroyUnitAtScreenCoords( pointer.position );
	}

	return false; //InputState::OnPointerDown( pointer );
}


bool EraserToolInputState::OnPointerUp( const Pointer& pointer )
{
	return false;
}


bool EraserToolInputState::OnPointerMotion( const Pointer& activePointer, const PointersByID& pointersByID )
{
	bool wasHandled = false; //InputState::OnPointerMotion( activePointer, pointersByID );

	if( !wasHandled && activePointer.isMoving )
	{
		// Destroy the Unit at the pointer location.
		DestroyUnitAtScreenCoords( activePointer.position );

		wasHandled = true;
	}

	return wasHandled;
}


void EraserToolInputState::DestroyUnitAtScreenCoords( const Vec2f& screenCoords )
{
	EditorState* owner = GetOwnerDerived();
	MapView* mapView = owner->GetMapView();
	Map* map = owner->GetMap();

	// Get the UnitSprite under the pointer (if any).
	UnitSprite* unitSprite = mapView->GetUnitSpriteAtScreenCoords( screenCoords );

	if( unitSprite )
	{
		// If there is a Unit under the pointer, destroy it.
		Unit* unit = unitSprite->GetUnit();
		DebugPrintf( "Erasing UnitSprite at Tile (%d,%d)!", unit->GetTileX(), unit->GetTileY() );
		map->DestroyUnit( unit );
	}
}
