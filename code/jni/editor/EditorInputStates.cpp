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


EraserToolInputState::EraserToolInputState( GameState* owner ) :
	DerivedInputState( owner ),
	mTileTemplate()
{ }


EraserToolInputState::~EraserToolInputState() { }


void EraserToolInputState::OnEnter( const Dictionary& parameters )
{
	EditorState* owner = GetOwnerDerived();

	// Set the Tile template to the default tile for this Scenario.
	mTileTemplate = owner->CreateDefaultTileTemplate();
}


void EraserToolInputState::OnExit()
{
}


bool EraserToolInputState::OnPointerDown( const Pointer& pointer )
{
	return false; //InputState::OnPointerDown( pointer );
}


bool EraserToolInputState::OnPointerUp( const Pointer& pointer )
{
	bool wasHandled = false; //InputState::OnPointerUp( pointer );

	if( !wasHandled )
	{
		// Paint a tile.
		GetOwnerDerived()->PaintTileAt( pointer.position.x, pointer.position.y, mTileTemplate );
		wasHandled = true;
	}

	return wasHandled;
}


bool EraserToolInputState::OnPointerMotion( const Pointer& activePointer, const PointersByID& pointersByID )
{
	bool wasHandled = false; //InputState::OnPointerMotion( activePointer, pointersByID );

	if( !wasHandled )
	{
		// Paint a tile.
		GetOwnerDerived()->PaintTileAt( activePointer.position.x, activePointer.position.y, mTileTemplate );
		wasHandled = true;
	}

	return wasHandled;
}
