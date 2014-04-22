#include "androidwars.h"

using namespace mage;


SelectUnitInputState::SelectUnitInputState( GameState* owner ) :
	DerivedInputState( owner )
{ }


SelectUnitInputState::~SelectUnitInputState() { }


void SelectUnitInputState::OnEnter( const Dictionary& parameters )
{
	GameplayState* owner = GetOwnerDerived();
	MapView* mapView = owner->GetMapView();

	// Deselect the current UnitSprite (if any).
	mapView->DeselectUnitSprite();

	// Show the gameplay interface.
	Widget* gameplayInterface = owner->GetGameplayInterface();

	if( gameplayInterface )
	{
		gameplayInterface->Show();
	}
}


void SelectUnitInputState::OnExit()
{
	GameplayState* owner = GetOwnerDerived();

	// Hide the gameplay interface.
	Widget* gameplayInterface = owner->GetGameplayInterface();

	if( gameplayInterface )
	{
		gameplayInterface->Hide();
	}
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
		owner->ChangeState( owner->GetMoveUnitInputState(), parameters );
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
	DerivedInputState( owner ),
	mAllowMovement( false )
{ }


MoveUnitInputState::~MoveUnitInputState() { }


void MoveUnitInputState::OnEnter( const Dictionary& parameters )
{
	GameplayState* owner = GetOwnerDerived();
	MapView* mapView = owner->GetMapView();
	Game* game = owner->GetGame();

	// Get the UnitSprite to select.
	UnitSprite* unitSpriteToSelect;
	Dictionary::DictionaryError error = parameters.Get( "unit", unitSpriteToSelect );
	assertion( error == Dictionary::DErr_SUCCESS, "No \"unit\" property was provided to MoveUnitInputState!" );

	// Get the Unit for this UnitSprite.
	Unit* unit = unitSpriteToSelect->GetUnit();

	// Only allow the local player to move this Unit if it is his own and the Unit is active.
	Player* localPlayer = game->GetLocalPlayer();
	Faction* currentFaction = game->GetCurrentFaction();
	mAllowMovement = ( currentFaction && game->PlayerControlsFaction( localPlayer, currentFaction ) && unit->GetOwner() == currentFaction && unit->IsActive() );

	// Select the UnitSprite.
	assertion( unitSpriteToSelect, "Cannot select null UnitSprite for MoveUnitInputState!" );
	mapView->SelectUnitSprite( unitSpriteToSelect, mAllowMovement );

	// Keep track of the tile pos of the active pointer.
	mLastPointerTilePos = unit->GetTilePos();

	// TODO: Show unit info.
}


void MoveUnitInputState::OnExit()
{
	GameplayState* owner = GetOwnerDerived();
	MapView* mapView = owner->GetMapView();

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
		if( mAllowMovement )
		{
			// If the last pointer left the screen, go to the action menu state.
			owner->ChangeState( owner->GetSelectActionInputState() );
		}
		else
		{
			// Otherwise, go back to unit selection.
			owner->ChangeState( owner->GetSelectUnitInputState() );
		}
	}

	bool wasHandled = false; //InputState::OnPointerUp( pointer );
	return wasHandled;
}


bool MoveUnitInputState::OnPointerMotion( const Pointer& activePointer, const PointersByID& pointersByID )
{
	GameplayState* owner = GetOwnerDerived();
	MapView* mapView = owner->GetMapView();

	if( mAllowMovement && activePointer.isMoving )
	{
		// Get the selected UnitSprite.
		UnitSprite* selectedUnitSprite = mapView->GetSelectedUnitSprite();
		Unit* selectedUnit = selectedUnitSprite->GetUnit();

		// Get the new tile position of the active pointer.
		Vec2f pointerWorldPos = mapView->ScreenToWorldCoords( activePointer.position );
		Vec2s pointerTilePos = mapView->WorldToTileCoords( pointerWorldPos );

		if( pointerTilePos != mLastPointerTilePos )
		{
			//DebugPrintf( "Pointer is over tile (%d,%d).", pointerTilePos.x, pointerTilePos.y );

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
					int previousIndex = std::max( index - 1, 0 );
					DebugPrintf( "Removing waypoints after index %d.", previousIndex );
					selectedUnitPath.RemoveWaypointsAfterIndex( previousIndex );
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


SelectActionInputState::SelectActionInputState( GameState* owner ) :
	DerivedInputState( owner )
{
	// Create the action menu Widget.
	mActionMenu = gWidgetManager->CreateWidgetFromTemplate< ListLayout >( "Actions" );

	if( mActionMenu )
	{
		// Add the action menu.
		gWidgetManager->GetRootWidget()->AddChild( mActionMenu );
	}
	else
	{
		WarnFail( "Could not create action menu Widget!" );
	}
}


SelectActionInputState::~SelectActionInputState()
{
	if( mActionMenu )
	{
		// Destroy the action menu Widget.
		gWidgetManager->DestroyWidget( mActionMenu );
	}
}


void SelectActionInputState::OnEnter( const Dictionary& parameters )
{
	GameplayState* owner = GetOwnerDerived();
	MapView* mapView = owner->GetMapView();

	if( mActionMenu )
	{
		// Get the selected Unit.
		UnitSprite* selectedUnitSprite = mapView->GetSelectedUnitSprite();
		assertion( selectedUnitSprite, "Cannot display actions because no UnitSprite is selected!" );
		Unit* unit = selectedUnitSprite->GetUnit();

		// Clear any previous menu items.
		mActionMenu->DestroyAllItems();

		// Get the action menu button template.
		WidgetTemplate* itemTemplate = gWidgetManager->GetTemplate( "ActionMenuButton" );

		if( itemTemplate )
		{
			// Build a list of actions for the selected Unit.
			Button* waitButton = CreateActionButton( *itemTemplate, "Wait" );
			waitButton->SetOnClickDelegate( Button::OnClickDelegate( this, &SelectActionInputState::OnWaitButtonPressed ) );

			Button* cancelButton = CreateActionButton( *itemTemplate, "Cancel" );
			cancelButton->SetOnClickDelegate( Button::OnClickDelegate( this, &SelectActionInputState::OnCancelButtonPressed ) );
		}
		else
		{
			WarnFail( "Could not create item button template for action menu!" );
		}

		// Show the action menu Widget.
		mActionMenu->Show();
	}
}


void SelectActionInputState::OnExit()
{
	GameplayState* owner = GetOwnerDerived();
	MapView* mapView = owner->GetMapView();

	// Deselect the currently selected UnitSprite.
	mapView->DeselectUnitSprite();

	if( mActionMenu )
	{
		// Hide the action menu Widget.
		mActionMenu->Hide();
	}
}


Button* SelectActionInputState::CreateActionButton( WidgetTemplate& widgetTemplate, const std::string& label )
{
	// Create the button.
	Button* result = mActionMenu->CreateItem< Button >( widgetTemplate );

	if( result )
	{
		// If the button was created successfully, set the label.
		result->SetLabel( label );
	}
	else
	{
		WarnFail( "Could not create item Button \"%s\" for action menu!", label.c_str() );
	}

	return result;
}


void SelectActionInputState::OnWaitButtonPressed()
{
	GameplayState* owner = GetOwnerDerived();
	MapView* mapView = owner->GetMapView();

	// Get the currently selected Unit.
	UnitSprite* selectedUnitSprite = mapView->GetSelectedUnitSprite();
	assertion( selectedUnitSprite, "Cannot display actions because no UnitSprite is selected!" );
	Unit* unit = selectedUnitSprite->GetUnit();

	// Move the selected Unit along the selected path.
	const Path& path = mapView->GetSelectedUnitPath();
	unit->Move( path );

	// Exit the state.
	owner->ChangeState( owner->GetSelectUnitInputState() );
}


void SelectActionInputState::OnCancelButtonPressed()
{
	GameplayState* owner = GetOwnerDerived();
	MapView* mapView = owner->GetMapView();

	// Exit the state.
	owner->ChangeState( owner->GetSelectUnitInputState() );
}
