#include "androidwars.h"

using namespace mage;


BrushToolInputState::BrushToolInputState( GameState* owner ) :
	DerivedInputState( owner ),
	mTileTemplate()
{ }


BrushToolInputState::~BrushToolInputState() { }


void BrushToolInputState::OnEnter( const Dictionary& parameters )
{
	// Get the selected Tile template (if specified).
	Tile tileTemplate;
	Dictionary::DictionaryError error = parameters.Get( "tileTemplate", tileTemplate );

	if( error == Dictionary::DErr_SUCCESS )
	{
		// Set the selected Tile template.
		SetTileTemplate( tileTemplate );
	}
}


void BrushToolInputState::OnExit()
{
}


bool BrushToolInputState::OnPointerDown( float x, float y, size_t which )
{
	return false; //InputState::OnPointerDown( x, y, which );
}


bool BrushToolInputState::OnPointerMotion( float x, float y, float dx, float dy, size_t which )
{
	bool wasHandled = false; //InputState::OnPointerMotion( x, y, dx, dy, which );

	if( !wasHandled )
	{
		// Paint a tile.
		GetOwnerDerived()->PaintTileAt( x, y, mTileTemplate );
		wasHandled = true;
	}

	return wasHandled;
}


bool BrushToolInputState::OnPointerUp( float x, float y, size_t which )
{
	bool wasHandled = false; //InputState::OnPointerUp( x, y, which );

	if( !wasHandled )
	{
		// Paint a tile.
		GetOwnerDerived()->PaintTileAt( x, y, mTileTemplate );
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


bool EraserToolInputState::OnPointerDown( float x, float y, size_t which )
{
	return false; //InputState::OnPointerDown( x, y, which );
}


bool EraserToolInputState::OnPointerMotion( float x, float y, float dx, float dy, size_t which )
{
	bool wasHandled = false; //InputState::OnPointerMotion( x, y, dx, dy, which );

	if( !wasHandled )
	{
		// Paint a tile.
		GetOwnerDerived()->PaintTileAt( x, y, mTileTemplate );
		wasHandled = true;
	}

	return wasHandled;
}


bool EraserToolInputState::OnPointerUp( float x, float y, size_t which )
{
	bool wasHandled = false; //InputState::OnPointerUp( x, y, which );

	if( !wasHandled )
	{
		// Paint a tile.
		GetOwnerDerived()->PaintTileAt( x, y, mTileTemplate );
		wasHandled = true;
	}

	return wasHandled;
}
