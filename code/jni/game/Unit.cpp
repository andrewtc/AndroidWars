#include "androidwars.h"

using namespace mage;

MAGE_IMPLEMENT_RTTI( MapObject, Unit );

Unit::Unit( const std::string& name )
: MapObject( name )
	, mUnitType( nullptr )
	, mSprite( nullptr )
	, mOwner( nullptr )
	, mHP( 0 )
{}


Unit::Unit( UnitType* unitType, Player* owner )
	: MapObject( "Unit" )
	, mUnitType( unitType )
	, mSprite( nullptr )
	, mOwner( owner )
	, mHP( 0 )
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
	else if ( name == "Owner" )
	{
		// Read the Player index from the property.
		int index;
		bool success = StringUtil::StringToType( value, &index );
		assertion( success, "Could not parse Owner value \"%s\". Must be a positive integer.", value.c_str() );

		// Grab the owning player.
		mOwner = gGame->GetPlayer( index );
		assertion( mOwner, "Invalid Player index %d specified for Unit \"%s\"!", mName.GetString().c_str() );
	}
}


void Unit::OnLoadFinished()
{
	// Determine the tile location of the unit.
	Vec2i tilePos = gGame->GetMap()->WorldToTile( Position );

	// Snap the unit to the tile grid.
	SetTilePos( tilePos );

	Init();
}


void Unit::Init()
{
	assertion( mUnitType != nullptr, "Unit::Init() '%s' does not have a valid UnitType!", mName.GetString().c_str() );
	assertion( mOwner != nullptr, "Unit::Init() '%s' does not have an owner Player!", mName.GetString().c_str() );

	// Create a sprite for this Unit.
	mSprite = SpriteManager::CreateSprite( mUnitType->GetAnimationSetName(), Position, "Idle" );
	BoundingRect = mSprite->GetClippingRectForCurrentAnimation();
	mSprite->DrawColor = mOwner->GetPlayerColor();
	mSelectionColor = mSprite->DrawColor;
	mDefaultColor = mSelectionColor;
	mDefaultColor.r /= 2;
	mDefaultColor.g /= 2;
	mDefaultColor.b /= 2;
	Deselect();
	// Set hit points
	mHP = mUnitType->GetMaxHP();

	mDestination = Position;

	DebugPrintf( "Unit \"%s\" initialized!", mName.GetString().c_str() );
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

	// Draw HP
	Game* game = mOwner->GetGame();
	BitmapFont* fnt = game->GetDefaultFont();
	Vec2f textPos = Position - camera.GetPosition();
	float h = mSprite ? mSprite->GetClippingRectForCurrentAnimation().Height() / 2 : 0;
	DrawTextFormat( textPos.x, textPos.y + h - fnt->GetLineHeight(), fnt, "%d", mHP );
}


void Unit::OnUpdate( float dt )
{
	float delta = ( Position - mDestination ).LengthSqr();
	if ( delta > 1 )
	{
		Vec2f vel = mDestination - Position;
		vel.Normalize();
		vel *= 100;
		Position += vel * dt;

		delta = ( Position - mDestination ).LengthSqr();
		if ( delta < 1 )
			gGame->OnUnitReachedDestination( this );
	}
}


void Unit::SetTilePos( const Vec2i& tilePos )
{
	// Update the tile position.
	mTilePos = tilePos;

	// Update the position of the object in the world.
	Position = gGame->GetMap()->TileToWorld( tilePos );
}


void Unit::SetDestination( const Vec2i& tilePos )
{
	mDestination = gGame->GetMap()->TileToWorld( tilePos );
	mTilePos = gGame->GetMap()->WorldToTile( mDestination );
}


int Unit::GetMovementRange() const
{
	// Get the maximum movement range for this unit.
	UnitType* type = GetUnitType();
	int movementRange = type->GetMovementRange();

	// TODO: Take supplies into account.

	return movementRange;
}


bool Unit::IsInRange( const Unit& target ) const
{
	UnitType* type = GetUnitType();
	const IntRange& range = type->GetAttackRange();
	DebugPrintf( "Unit pos (%d, %d) : trg pos (%d %d) d=%d r=[%d,%d]",
			mTilePos.x, mTilePos.y,
			target.mTilePos.x, target.mTilePos.y,
			GetDistanceToUnit( target ),
			range.Min, range.Max );
	return range.IsValueInRange( GetDistanceToUnit( target ) );
}
\

int Unit::GetDistanceToUnit( const Unit& target ) const
{
	return mTilePos.GetManhattanDistanceTo( target.mTilePos );
}


void Unit::Select()
{
	//mSprite->DrawColor = mSelectionColor;
	mSprite->Scale.Set( 1.15f, 1.15f );
}

void Unit::Deselect()
{
	//mSprite->DrawColor = mDefaultColor;
	mSprite->Scale.Set( 1.0f, 1.0f );
}

void Unit::Attack( Unit& target ) const
{
	// TODO look up the correct value based on this type and target type
	target.mHP -= 1;
}
