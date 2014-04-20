#include "androidwars.h"

using namespace mage;


SelectUnitInputState::SelectUnitInputState( GameState* owner ) :
	DerivedInputState( owner )
{ }


SelectUnitInputState::~SelectUnitInputState() { }


void SelectUnitInputState::OnEnter( const Dictionary& parameters )
{
	GameplayState* owner = GetOwnerDerived();

	// Show the tile palette.
	//owner->GetTilePalette()->Show();
}


void SelectUnitInputState::OnExit()
{
	GameplayState* owner = GetOwnerDerived();

	// Hide the tile palette.
	//owner->GetTilePalette()->Hide();
}


bool SelectUnitInputState::OnPointerDown( const Pointer& pointer )
{
	GameplayState* owner = GetOwnerDerived();
	MapView* mapView = owner->GetMapView();

	// See if a UnitSprite was tapped.
	UnitSprite* unitSpriteToSelect = mapView->GetUnitSpriteAtScreenCoords( pointer.position );

	if( unitSpriteToSelect )
	{
		Unit* unit = unitSpriteToSelect->GetUnit();

		Vec2s tilePos = unit->GetTilePos();
		DebugPrintf( "PointerDown on Unit at (%d,%d)!", tilePos.x, tilePos.y );

		// Go to the unit movement InputState.
		Dictionary parameters;
		parameters.Set( "Unit", unitSpriteToSelect );
		owner->PushState( owner->GetMoveUnitInputState(), parameters );
	}

	return false; //InputState::OnPointerDown( pointer );
}


bool SelectUnitInputState::OnPointerUp( const Pointer& pointer )
{
	GameplayState* owner = GetOwnerDerived();
	MapView* mapView = owner->GetMapView();

	bool wasHandled = false; //InputState::OnPointerUp( pointer );
	return wasHandled;
}


bool SelectUnitInputState::OnPointerMotion( const Pointer& activePointer, const PointersByID& pointersByID )
{
	bool wasHandled = false; //InputState::OnPointerMotion( activePointer, pointersByID );
	return wasHandled;
}


MoveUnitInputState::MoveUnitInputState( GameState* owner ) :
	DerivedInputState( owner )
{ }


MoveUnitInputState::~MoveUnitInputState() { }


void MoveUnitInputState::OnEnter( const Dictionary& parameters )
{
	GameplayState* owner = GetOwnerDerived();
	MapView* mapView = owner->GetMapView();

	// Get the UnitSprite to select.
	UnitSprite* unitSpriteToSelect;
	Dictionary::DictionaryError error = parameters.Get( "unit", unitSpriteToSelect );
	assertion( error == Dictionary::DErr_SUCCESS, "No \"unit\" property was provided to MoveUnitInputState!" );

	// Select the UnitSprite.
	assertion( unitSpriteToSelect, "Cannot select null UnitSprite for MoveUnitInputState!" );
	mapView->SelectUnitSprite( unitSpriteToSelect );

	// Keep track of the tile pos of the active pointer.
	mLastPointerTilePos = unitSpriteToSelect->GetUnit()->GetTilePos();

	// TODO: Show unit info.
}


void MoveUnitInputState::OnExit()
{
	GameplayState* owner = GetOwnerDerived();
	MapView* mapView = owner->GetMapView();

	// Deselect the currently selected UnitSprite (if any).
	mapView->DeselectUnitSprite();

	// TODO: Hide unit info.
}


bool MoveUnitInputState::OnPointerDown( const Pointer& pointer )
{
	GameplayState* owner = GetOwnerDerived();
	MapView* mapView = owner->GetMapView();

	return false; //InputState::OnPointerDown( pointer );
}


bool MoveUnitInputState::OnPointerUp( const Pointer& pointer )
{
	GameplayState* owner = GetOwnerDerived();
	MapView* mapView = owner->GetMapView();

	if( GetPointerCount() == 1 )
	{
		// If the last pointer left the screen, leave the current InputState.
		owner->PopState();
	}

	bool wasHandled = false; //InputState::OnPointerUp( pointer );
	return wasHandled;
}


bool MoveUnitInputState::OnPointerMotion( const Pointer& activePointer, const PointersByID& pointersByID )
{
	GameplayState* owner = GetOwnerDerived();
	MapView* mapView = owner->GetMapView();

	if( activePointer.isMoving )
	{
		// Get the selected UnitSprite.
		UnitSprite* selectedUnitSprite = mapView->GetSelectedUnitSprite();
		Unit* selectedUnit = selectedUnitSprite->GetUnit();

		// Get the new tile position of the active pointer.
		Vec2f pointerWorldPos = mapView->ScreenToWorldCoords( activePointer.position );
		Vec2s pointerTilePos = mapView->WorldToTileCoords( pointerWorldPos );

		if( pointerTilePos != mLastPointerTilePos )
		{
			DebugPrintf( "Pointer is over tile (%d,%d).", pointerTilePos.x, pointerTilePos.y );

			// Determine whether the TileSprite is selected at this location.
			MapView::TileSpritesGrid::Iterator tileSpriteIt = mapView->GetTileSprites().GetTile( pointerTilePos );

			if( tileSpriteIt->IsSelected() )
			{
				// If the tile is selected, determine whether the new tile is already in the path.
				Path& selectedUnitPath = mapView->GetSelectedUnitPath();
				int index = selectedUnitPath.GetIndexOfWaypoint( pointerTilePos );

				if( index != -1 )
				{
					// If the tile is already in the selected Unit path, truncate the path after the tile.
					selectedUnitPath.RemoveWaypointsAfterIndex( index );
				}
				else
				{
					// Otherwise, determine whether the new tile is adjacent to the old one.
					bool isAdjacentTile = false;
					PrimaryDirection direction;

					for( size_t i = 0; i < CARDINAL_DIRECTION_COUNT; ++i )
					{
						// Check the previous known tile position for adjacency.
						PrimaryDirection directionToCheck = CARDINAL_DIRECTIONS[ i ];

						if( mLastPointerTilePos + directionToCheck.GetOffset() == pointerTilePos )
						{
							// If the new tile is adjacent to the previous one, save the direction of movement.
							isAdjacentTile = true;
							direction = directionToCheck;
							break;
						}
					}

					bool recalculatePath = false;

					if( isAdjacentTile )
					{
						// If the new tile is adjacent, add a new direction to the path.
						selectedUnitPath.AddDirection( direction );

						// If the tile is adjacent to the last one, check to see if it is reachable.
						int pathCost = selectedUnit->CalculatePathCost( selectedUnitPath );
						bool canReachDestination = ( pathCost <= selectedUnit->GetMovementRange() );

						if( !canReachDestination )
						{
							// If the path isn't reachable, recalculate the path.
							recalculatePath = true;
						}
					}
					else
					{
						// Otherwise, recalculate the path.
						recalculatePath = true;
					}

					if( recalculatePath )
					{
						// Calculate the shortest path to the new tile position.
						mapView->GetMap()->FindBestPathToTile( selectedUnit, pointerTilePos, selectedUnitPath );
					}
				}

				// Keep track of the last tile position of the active pointer.
				mLastPointerTilePos = pointerTilePos;
			}
		}
	}

	bool wasHandled = false; //InputState::OnPointerMotion( activePointer, pointersByID );
	return wasHandled;
}

