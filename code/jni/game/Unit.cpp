#include "androidwars.h"

using namespace mage;


Unit::Unit( UnitType* unitType )
	: MapObject( "" )
	, mUnitType( unitType )
	, mSprite( nullptr )
{ }


Unit::~Unit() { }


void Unit::Init()
{
	// Set the position of the Unit based on its tile position.
	Position = gGame->TileToWorldCoords( mTilePos );

	// Create a sprite for this Unit.
	mSprite = SpriteManager::CreateSprite( mUnitType->GetAnimationSetName(), Position, "Idle" );

	DebugPrintf( "Unit \"%s\" initialized!", mName.GetString().c_str() );
}


void Unit::OnDraw( const Camera& camera )
{
	// Draw the sprite at the location of the Unit.
	mSprite->Position = Position;
	mSprite->OnDraw( camera );
}
