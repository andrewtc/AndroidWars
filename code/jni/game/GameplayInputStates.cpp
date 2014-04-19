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

		// Select the UnitSprite.
		mapView->SelectUnitSprite( unitSpriteToSelect );
	}
	else
	{
		// Deselect the Unit.
		mapView->DeselectUnitSprite();
	}

	return false; //InputState::OnPointerDown( pointer );
}


bool SelectUnitInputState::OnPointerUp( const Pointer& pointer )
{
	GameplayState* owner = GetOwnerDerived();
	MapView* mapView = owner->GetMapView();

	// Deselect the currently selected UnitSprite (if any).
	mapView->DeselectUnitSprite();

	bool wasHandled = false; //InputState::OnPointerUp( pointer );
	return wasHandled;
}


bool SelectUnitInputState::OnPointerMotion( const Pointer& activePointer, const PointersByID& pointersByID )
{
	bool wasHandled = false; //InputState::OnPointerMotion( activePointer, pointersByID );
	return wasHandled;
}
