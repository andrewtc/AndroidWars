#include "androidwars.h"

using namespace mage;

Unit::Unit( const std::string& name )
: MapObject( name )
	, mUnitType( nullptr )
	, mSprite( nullptr )
{}


Unit::Unit( UnitType* unitType )
	: MapObject( "Unit" )
	, mUnitType( unitType )
	, mSprite( nullptr )
{ }


Unit::~Unit() { }


/** Load the Xml properties from the MapObject */
void Unit::OnLoadProperty( const std::string& name, const std::string& value )
{
	if ( name == "UnitType" )
	{
		mUnitType = gDatabase->UnitTypes.FindByName( value );
		assertion( mUnitType, "UnitType \"%s\" not found!", value.c_str() );
	}
	else
	{
		assertion( false, "Unit '%s' missing UnitType property!", mName.GetString().c_str() );
	}
}


void Unit::OnLoadFinished()
{
	Init();
}


void Unit::Init()
{
	// Set the position of the Unit based on its tile position.
	//Position = gGame->TileToWorldCoords( mTilePos );

	// Create a sprite for this Unit.
	if ( mUnitType )
	{
		mSprite = SpriteManager::CreateSprite( mUnitType->GetAnimationSetName(), Position, "Idle" );
		DebugPrintf( "Unit \"%s\" initialized!", mName.GetString().c_str() );
	}
	else
	{
		assertion( false, "Unit::Init() '%s' has UnitType null!", mName.GetString().c_str() );
	}
}


void Unit::OnDraw( const Camera& camera ) const
{
	if ( mSprite )
	{
		// Draw the sprite at the location of the Unit.
		mSprite->Position = Position;
		mSprite->OnDraw( camera );
	}
	// Fallback to debugdraw on missing graphics
	else
	{
		MapObject::OnDraw( camera );
	}
}
