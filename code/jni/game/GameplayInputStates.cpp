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

	Unit* unit = nullptr;

	// Calculate the position of the active pointer.
	const Pointer& activePointer = GetActivePointer();
	Vec2f activePointerWorldCoords = mapView->ScreenToWorldCoords( activePointer.position );
	Vec2s activePointerTileCoords = mapView->WorldToTileCoords( activePointerWorldCoords );

	if( error == Dictionary::DErr_SUCCESS )
	{
		// If a UnitSprite to select was specified, get the Unit for the specified UnitSprite.
		assertion( unitSpriteToSelect, "Cannot enter MoveUnitInputState because an invalid UnitSprite to select was specified!" );
		unit = unitSpriteToSelect->GetUnit();

		// Only allow the local player to move this Unit if it is his own and the Unit is active.
		Player* localPlayer = game->GetLocalPlayer();
		Faction* currentFaction = game->GetCurrentFaction();
		mAllowMovement = ( currentFaction && game->PlayerControlsFaction( localPlayer, currentFaction ) && unit->GetOwner() == currentFaction && unit->IsActive() );

		// Select the UnitSprite.
		mapView->SelectUnitSprite( unitSpriteToSelect, mAllowMovement );

		// Reset the last tile position.
		mLastPointerTilePos = activePointerTileCoords;
	}
	else
	{
		// Otherwise, use the currently selected UnitSprite.
		assertion( mapView->HasSelectedUnitSprite(), "Cannot enter MoveUnitInputState because no UnitSprite is currently selected and no UnitSprite to select was specified!" );
		unit = mapView->GetSelectedUnitSprite()->GetUnit();
	}

	// Recalculate the arrow using the active pointer's current Tile position.
	RecalculateArrow( activePointerTileCoords );

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
		// Get the new tile position of the active pointer.
		Vec2f pointerWorldPos = mapView->ScreenToWorldCoords( activePointer.position );
		Vec2s pointerTilePos = mapView->WorldToTileCoords( pointerWorldPos );

		if( pointerTilePos != mLastPointerTilePos )
		{
			// Recalculate the arrow, taking into account the new pointer position.
			RecalculateArrow( pointerTilePos );
		}
	}

	bool wasHandled = false; //InputState::OnPointerMotion( activePointer, pointersByID );
	return wasHandled;
}


void MoveUnitInputState::RecalculateArrow( const Vec2s& pointerTilePos )
{
	GameplayState* owner = GetOwnerDerived();
	MapView* mapView = owner->GetMapView();

	// Get the selected UnitSprite.
	UnitSprite* selectedUnitSprite = mapView->GetSelectedUnitSprite();
	Unit* selectedUnit = selectedUnitSprite->GetUnit();

	//DebugPrintf( "Pointer is over tile (%d,%d).", pointerTilePos.x, pointerTilePos.y );

	// Determine whether the TileSprite at this location is selected.
	TileSprite* tileSprite = mapView->GetTileSpriteAtTileCoords( pointerTilePos );

	if( tileSprite && tileSprite->IsSelected() )
	{
		// If the TileSprite is selected, determine whether the new tile is already in the path.
		Path& selectedUnitPath = mapView->GetSelectedUnitPath();
		int index = selectedUnitPath.GetIndexOfWaypoint( pointerTilePos );

		if( index >= 0 )
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
		// Determine the list of available actions for the selected Unit.
		mapView->DetermineAvailableActionsForSelectedUnit();

		// Rebuild the menu of actions.
		RebuildActionsMenu();

		// Show the action menu Widget.
		mActionMenu->Show();
	}
}


void SelectActionInputState::OnExit()
{
	GameplayState* owner = GetOwnerDerived();
	MapView* mapView = owner->GetMapView();

	if( mActionMenu )
	{
		// Hide the action menu Widget.
		mActionMenu->Hide();
	}
}


bool SelectActionInputState::OnPointerDown( const Pointer& pointer )
{
	GameplayState* owner = GetOwnerDerived();
	MapView* mapView = owner->GetMapView();

	// Let the WidgetManager process the event.
	bool wasHandled = InputState::OnPointerDown( pointer );

	if( !wasHandled )
	{
		// If the event was not processed by a Widget, check to see if the pointer is
		// over a tile that is selected.
		TileSprite* tileSpriteUnderPointer = mapView->GetTileSpriteAtScreenCoords( pointer.position );
		wasHandled = ( tileSpriteUnderPointer && tileSpriteUnderPointer->IsSelected() );

		if( wasHandled )
		{
			// If the pointer is over a selected TileSprite, let the player reselect the Unit movement path.
			owner->ChangeState( owner->GetMoveUnitInputState() );
		}
	}

	return wasHandled;
}


void SelectActionInputState::RebuildActionsMenu()
{
	GameplayState* owner = GetOwnerDerived();
	MapView* mapView = owner->GetMapView();

	// Get the selected Unit.
	UnitSprite* selectedUnitSprite = mapView->GetSelectedUnitSprite();
	assertion( selectedUnitSprite, "Cannot display actions because no UnitSprite is selected!" );
	Unit* unit = selectedUnitSprite->GetUnit();

	if( mActionMenu )
	{
		// Clear any previous menu items.
		mActionMenu->DestroyAllItems();

		// Get the action menu button template.
		WidgetTemplate* itemTemplate = gWidgetManager->GetTemplate( "ActionMenuButton" );

		if( itemTemplate )
		{
			// Get the list of available Actions for this unit.
			Actions actions = mapView->GetAvailableActionsForSelectedUnit();
			std::set< HashString > uniqueActionTypes;

			for( auto it = actions.begin(); it != actions.end(); ++it )
			{
				// Determine the list of unique actions for this Unit.
				Ability::Action* action = *it;
				uniqueActionTypes.insert( action->GetType() );
			}

			for( auto it = uniqueActionTypes.begin(); it != uniqueActionTypes.end(); ++it )
			{
				// Add a button for each available Action.
				const HashString& type = *it;
				Button* actionButton = CreateActionButton( *itemTemplate, type.GetString() );

				if( actionButton )
				{
					actionButton->SetOnClickDelegate( [ this, type ]()
					{
						// When the button is clicked, fire the action button event.
						OnActionButtonPressed( type );
					});
				}
			}

			// Add the "Cancel" button.
			Button* cancelButton = CreateActionButton( *itemTemplate, "Cancel" );

			if( cancelButton )
			{
				cancelButton->SetOnClickDelegate( Button::OnClickDelegate( this, &SelectActionInputState::OnCancelButtonPressed ) );
			}
		}
		else
		{
			WarnFail( "Could not create item button template for action menu!" );
		}
	}
	else
	{
		WarnFail( "Could not rebuild actions menu because the menu Widget was not properly initialized!" );
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


void SelectActionInputState::OnActionButtonPressed( const HashString& type )
{
	GameplayState* owner = GetOwnerDerived();
	MapView* mapView = owner->GetMapView();
	Map* map = owner->GetMap();

	// Get the currently selected Unit.
	UnitSprite* selectedUnitSprite = mapView->GetSelectedUnitSprite();
	assertion( selectedUnitSprite, "Cannot select Action because no UnitSprite is selected!" );
	Unit* unit = selectedUnitSprite->GetUnit();

	// Get the selected Path.
	const Path& path = mapView->GetSelectedUnitPath();

	// Get the list of actions for the selected Unit.
	const Actions& actions = mapView->GetAvailableActionsForSelectedUnit();

	// Look up the ability by its name.
	Ability* ability = map->GetAbilityByType( type );

	if( ability )
	{
		// See if the ability targets a single Unit.
		SingleTargetComponent* singleTargetAbility = dynamic_cast< SingleTargetComponent* >( ability );

		if( singleTargetAbility )
		{
			// Add the Action type to the dictionary.
			Dictionary parameters;
			parameters.Set( "actionType", HashString( type ) );

			// Go to the InputState for selecting a target.
			owner->ChangeState( owner->GetSelectTargetInputState(), parameters );
		}
		else
		{
			Actions::const_iterator it;

			for( it = actions.begin(); it != actions.end(); ++it )
			{
				Ability::Action* action = *it;

				if( action->GetType() == type )
				{
					// Get the first Action in the list of actions.
					break;
				}
			}

			if( it != actions.end() )
			{
				// Perform the first Action of the specified type in the list.
				Ability::Action* action = *it;
				owner->PerformAction( action );
			}
			else
			{
				WarnFail( "Could not perform Action \"%s\" because no Action was found in the list of available Actions for the selected Unit!", type.GetCString() );
			}

			// Deselect the currently selected UnitSprite.
			mapView->DeselectUnitSprite();
		}
	}
	else
	{
		WarnFail( "Could not perform action \"%s\" because no Ability with that name was found!", type.GetCString() );
	}
}


void SelectActionInputState::OnCancelButtonPressed()
{
	GameplayState* owner = GetOwnerDerived();
	MapView* mapView = owner->GetMapView();

	// Deselect the currently selected UnitSprite.
	mapView->DeselectUnitSprite();

	// Exit the state.
	owner->ChangeState( owner->GetSelectUnitInputState() );
}


SelectTargetInputState::SelectTargetInputState( GameState* owner ) :
	DerivedInputState( owner ),
	mSelectionMenu( nullptr )
{ }


SelectTargetInputState::~SelectTargetInputState() { }


void SelectTargetInputState::OnEnter( const Dictionary& parameters )
{
	GameplayState* owner = GetOwnerDerived();
	MapView* mapView = owner->GetMapView();

	// Get the type of Action to use for targeting.
	Dictionary::DictionaryError error = parameters.Get( "actionType", mActionType );
	assertion( error == Dictionary::DErr_SUCCESS, "Cannot enter SelectTargetInputState because no Action type was specified!" );

	// Select all valid Actions for the selected UnitSprite.
	Actions actions = mapView->GetAvailableActionsForSelectedUnit();

	for( auto it = actions.begin(); it != actions.end(); ++it )
	{
		const Ability::Action* action = *it;

		if( action->GetType() == mActionType )
		{
			mValidActions.push_back( *it );
		}
	}

	if( actions.size() > 0 )
	{
		// Get the first valid Action.
		const Ability::Action* action = mValidActions[ 0 ];

		// Get the UnitSprite for the first Action and target it.
		UnitSprite* unitSprite = GetTargetUnitSpriteForAction( action );

		if( unitSprite )
		{
			mapView->TargetUnitSprite( unitSprite );
		}
		else
		{
			WarnFail( "No initial UnitSprite found!" );
		}
	}
	else
	{
		WarnFail( "No valid Actions found of type \"%s\"!", mActionType.GetCString() );
	}

	// Create the selection menu.
	// TODO: Display custom information about each action.
	mSelectionMenu = gWidgetManager->CreateWidgetFromTemplate< ListLayout >( "Actions", "selectionMenu" );

	if( mSelectionMenu )
	{
		// Display the selection menu.
		gWidgetManager->GetRootWidget()->AddChild( mSelectionMenu );

		// Build the selection menu.
		WidgetTemplate* itemTemplate = gWidgetManager->GetTemplate( "ActionMenuButton" );

		if( itemTemplate )
		{
			// Create the confirm button.
			Button* confirmButton = mSelectionMenu->CreateItem< Button >( *itemTemplate );

			if( confirmButton )
			{
				// If the button was created successfully, set the label and callback.
				confirmButton->SetLabel( "Confirm" );
				confirmButton->SetOnClickDelegate( Delegate< void >( this, &SelectTargetInputState::OnConfirmButtonPressed ) );
			}
			else
			{
				WarnFail( "Could not create item confirm button for selection menu!" );
			}

			// Create the cancel button.
			Button* cancelButton = mSelectionMenu->CreateItem< Button >( *itemTemplate );

			if( cancelButton )
			{
				// If the button was created successfully, set the label and callback.
				cancelButton->SetLabel( "Cancel" );
				cancelButton->SetOnClickDelegate( Delegate< void >( this, &SelectTargetInputState::OnCancelButtonPressed ) );
			}
			else
			{
				WarnFail( "Could not create item cancel button for selection menu!" );
			}
		}
		else
		{
			WarnFail( "Could not build selection menu because the item template was not found!" );
		}
	}
	else
	{
		WarnFail( "Could not create selection menu!" );
	}
}


void SelectTargetInputState::OnExit()
{
	// Clear the list of Actions.
	mValidActions.clear();

	if( mSelectionMenu )
	{
		// Destroy the selection menu.
		gWidgetManager->DestroyWidget( mSelectionMenu );
	}
}


bool SelectTargetInputState::OnPointerDown( const Pointer& pointer )
{
	GameplayState* owner = GetOwnerDerived();
	MapView* mapView = owner->GetMapView();

	if( pointer.IsActivePointer() )
	{
		// Get the UnitSprite under the pointer (if any).
		UnitSprite* unitSprite = mapView->GetUnitSpriteAtScreenCoords( pointer.position );

		if( unitSprite && IsValidTarget( unitSprite->GetUnit() ) )
		{
			// If a valid UnitSprite was selected, target the UnitSprite.
			mapView->TargetUnitSprite( unitSprite );
		}
	}

	return false; //InputState::OnPointerDown( pointer );
}


Unit* SelectTargetInputState::GetTargetForAction( const Ability::Action* action ) const
{
	GameplayState* owner = GetOwnerDerived();
	MapView* mapView = owner->GetMapView();
	Map* map = mapView->GetMap();

	Unit* result = nullptr;

	// Try casting the Action to the required type.
	const SingleTargetComponent::Action* singleTargetAction = dynamic_cast< const SingleTargetComponent::Action* >( action );

	if( singleTargetAction )
	{
		// Get the Unit matching the target ID.
		result = map->GetUnitByID( singleTargetAction->TargetID );

		if( !result )
		{
			WarnFail( "Invalid target ID (%d) specified for Action!", singleTargetAction->TargetID );
		}
	}
	else
	{
		WarnFail( "Cannot get target Unit for Action because the Action specified does not target a single Unit!" );
	}

	return result;
}


UnitSprite* SelectTargetInputState::GetTargetUnitSpriteForAction( const Ability::Action* action ) const
{
	UnitSprite* result = nullptr;

	GameplayState* owner = GetOwnerDerived();
	MapView* mapView = owner->GetMapView();

	// Get the Unit for the specified Action.
	Unit* target = GetTargetForAction( action );

	if( target )
	{
		// Look up the UnitSprite for the Unit.
		result = mapView->GetUnitSpriteForUnit( target );

		if( !result )
		{
			WarnFail( "No UnitSprite found for target!" );
		}
	}

	return result;
}


bool SelectTargetInputState::IsValidTarget( const Unit* unit ) const
{
	bool result = false;

	for( auto it = mValidActions.begin(); it != mValidActions.end(); ++it )
	{
		const Ability::Action* action = *it;

		if( GetTargetForAction( action ) == unit )
		{
			result = true;
			break;
		}
	}

	return result;
}


void SelectTargetInputState::OnConfirmButtonPressed()
{
	GameplayState* owner = GetOwnerDerived();
	MapView* mapView = owner->GetMapView();
	Map* map = mapView->GetMap();

	// Get the currently targeted UnitSprite.
	UnitSprite* targetSprite = mapView->GetTargetedUnitSprite();

	if( targetSprite )
	{
		// Get the currently targeted Unit.
		Unit* target = targetSprite->GetUnit();

		for( auto it = mValidActions.begin(); it != mValidActions.end(); ++it )
		{
			Ability::Action* action = *it;

			if( GetTargetForAction( action ) == target )
			{
				// Find the Action that targets the currently targeted Unit and perform it.
				owner->PerformAction( action );
				break;
			}
		}
	}
	else
	{
		WarnFail( "Could not perform action because no target was selected!" );
	}

	// Untarget the currently targeted UnitSprite.
	mapView->UntargetUnitSprite();

	// Deselect the currently selected UnitSprite.
	mapView->DeselectUnitSprite();
}


void SelectTargetInputState::OnCancelButtonPressed()
{
	GameplayState* owner = GetOwnerDerived();
	MapView* mapView = owner->GetMapView();

	// Untarget the current target.
	mapView->UntargetUnitSprite();

	// Exit the state.
	owner->ChangeState( owner->GetSelectActionInputState() );
}


PerformActionInputState::PerformActionInputState( GameState* owner ) :
	DerivedInputState( owner ),
	mAction( nullptr ),
	mMapAnimation( nullptr )
{ }


PerformActionInputState::~PerformActionInputState() { }


void PerformActionInputState::OnEnter( const Dictionary& parameters )
{
	GameplayState* owner = GetOwnerDerived();
	MapView* mapView = owner->GetMapView();

	// Get the Action to perform.
	Dictionary::DictionaryError error = parameters.Get( "action", mAction );
	assertion( error == Dictionary::DErr_SUCCESS, "No Ability::Action was specified for PerformActionInputState!" );

	// Start a new Map animation for the Action.
	mMapAnimation = mapView->ScheduleAnimationForAction( mAction );

	if( mMapAnimation )
	{
		// Wait for the animation to finish before exiting the state.
		mMapAnimation->OnFinished.AddCallback( this, &PerformActionInputState::OnAnimationFinished );
	}
	else
	{
		WarnFail( "No animation scheduled for Action!" );
	}
}


void PerformActionInputState::OnExit()
{
	// Clear the values.
	mAction = nullptr;
	mMapAnimation = nullptr;
}


void PerformActionInputState::OnAnimationFinished()
{
	DebugPrintf( "MapAnimation finished for PerformActionInputState." );

	GameplayState* owner = GetOwnerDerived();
	Map* map = owner->GetMap();

	// If the animation finished immediately, perform the Action on the Map.
	map->PerformAction( mAction );

	// Go back to Unit selection.
	owner->ChangeState( owner->GetSelectUnitInputState() );
}

