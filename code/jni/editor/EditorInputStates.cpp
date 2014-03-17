#include "androidwars.h"

using namespace mage;


PaintTilesInputState::PaintTilesInputState( GameState* owner ) :
	DerivedInputState( owner ),
	mSelectedTerrainType( nullptr )
{ }


PaintTilesInputState::~PaintTilesInputState() { }


void PaintTilesInputState::OnEnter( const Dictionary& parameters )
{
	// Get the selected terrain type (if specified).
	TerrainType* terrainType = nullptr;
	Dictionary::DictionaryError error = parameters.Get( "selectedTerrainType", terrainType );

	if( error == Dictionary::DErr_SUCCESS )
	{
		// Set the selected terrain type.
		SetSelectedTerrainType( terrainType );
	}
}


void PaintTilesInputState::OnExit()
{
}


bool PaintTilesInputState::OnPointerDown( float x, float y, size_t which )
{
	return true;
}


bool PaintTilesInputState::OnPointerMotion( float x, float y, float dx, float dy, size_t which )
{
	PaintTile( x, y );
	return true;
}


bool PaintTilesInputState::OnPointerUp( float x, float y, size_t which )
{
	PaintTile( x, y );
	return true;
}


void PaintTilesInputState::SetSelectedTerrainType( TerrainType* terrainType )
{
	mSelectedTerrainType = terrainType;
}


void PaintTilesInputState::PaintTile( float x, float y )
{
	// TODO: Universal tile to world conversion function.
	Vec2s tilePos( x / World::TILE_WORLD_SCALE, y / World::TILE_WORLD_SCALE );

	// Paint the Tile at the tile coordinates.
	Map::Iterator iterator = GetOwnerDerived()->GetMap()->GetTile( tilePos );

	if( iterator.IsValid() )
	{
		iterator->SetTerrainType( mSelectedTerrainType );
	}
}
