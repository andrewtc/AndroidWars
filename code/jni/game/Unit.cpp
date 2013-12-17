#include "androidwars.h"

using namespace mage;


const int Unit::MAX_HP;


MAGE_IMPLEMENT_RTTI( MapObject, Unit );


Unit::Unit( const std::string& name )
: MapObject( name )
	, mUnitType( nullptr )
	, mSprite( nullptr )
	, mOwner( nullptr )
	, mHP( 0 )
	, mAP( 0 )
	, mAmmo( -1 )
{}


Unit::Unit( UnitType* unitType, Player* owner )
	: MapObject( "Unit" )
	, mUnitType( unitType )
	, mSprite( nullptr )
	, mOwner( owner )
	, mHP( 0 )
	, mAP( 0 )
	, mAmmo( -1 )
{ }


Unit::~Unit() { }


/** Load the Xml properties from the MapObject */
void Unit::OnLoadProperty( const std::string& name, const std::string& value )
{
	bool parseWasSuccessful = false;

	if ( name == "UnitType" )
	{
		parseWasSuccessful = true;
		mUnitType = gDatabase->UnitTypes.FindByName( value );
		assertion( mUnitType, "UnitType \"%s\" not found!", value.c_str() );
	}
	else if ( name == "Owner" )
	{
		// Read the Player index from the property.
		int index;
		parseWasSuccessful = StringUtil::StringToType( value, &index );

		// Grab the owning player.
		mOwner = gGame->GetPlayer( index );
		mOwner->OnGainUnit();
		assertion( mOwner, "Invalid Player index %d specified for %s!", ToString() );
	}
	else if ( name == "Ammo" )
	{
		// Read in ammo amount.
		int ammo;
		parseWasSuccessful = StringUtil::StringToType( value, &ammo );
		mAmmo = ammo;
	}

	assertion( parseWasSuccessful, "Could not parse %s value for %s! (\"%s\" specified.)", name.c_str(), ToString(), value.c_str() );
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
	// Make sure the Unit has a valid UnitType.
	assertion( mUnitType != nullptr, "Unit::Init(): \"%s\" does not have a valid UnitType!", mName.GetString().c_str() );

	// Format debug name string.
	static const size_t BUFFER_SIZE = 256;
	char buffer[ BUFFER_SIZE ];

	snprintf( buffer, BUFFER_SIZE, "%s \"%s\"", mUnitType->GetName().GetString().c_str(), mName.GetString().c_str() );
	mDebugName = buffer;

	// Make sure the Unit has a valid Player.
	assertion( mOwner != nullptr, "Unit::Init(): %s does not have an owner Player!", ToString() );

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

	// Set initial resources.
	mHP = mUnitType->GetMaxHP();

	if( mAmmo >= 0 )
	{
		// Make sure ammo value is valid.
		SetAmmo( mAmmo );
	}
	else
	{
		// If the ammo value is uninitialized, set it to the maximum amount.
		mAmmo = mUnitType->GetMaxAmmo();
	}

	mDestination = Position;

	mAP = 2;

	DebugPrintf( "%s initialized!", ToString() );
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


Player* Unit::GetOwner() const
{
	return mOwner;
}


int Unit::GetMovementRange() const
{
	// Get the maximum movement range for this unit.
	UnitType* type = GetUnitType();
	int movementRange = type->GetMovementRange();

	// TODO: Take supplies into account.

	return movementRange;
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


bool Unit::CanAttack( const Unit& target ) const
{
	DebugPrintf( "Checking whether %s can attack %s...", ToString(), target.ToString() );

	// Make sure this Unit is still alive.
	bool isAlive = IsAlive();
	DebugPrintf( "%s is %s.", ToString(), ( isAlive ? "ALIVE" : "DEAD" ) );

	// Check whether the target is in range.
	bool isInRange = IsInRange( target );
	DebugPrintf( "%s %s in range.", target.ToString(), ( isInRange ? "IS" : "IS NOT" ) );

	// Check whether this Unit can target the other Unit.
	bool canTarget = CanTarget( target );
	DebugPrintf( "%s %s target %s.", ToString(), ( isInRange ? "CAN" : "CANNOT" ), target.ToString() );

	bool result = ( isAlive && isInRange && canTarget );
	DebugPrintf( "RESULT: %s %s attack %s.", ToString(), ( result ? "CAN" : "CANNOT" ), target.ToString() );

	return result;
}


void Unit::Attack( Unit& target )
{
	DebugPrintf( "%s attacks %s.", ToString(), target.ToString() );

	// Get the best weapon to use against the target.
	int bestWeaponIndex = GetBestAvailableWeaponAgainst( target );
	assertion( bestWeaponIndex > -1, "Cannot calculate damage: No weapon can currently target that Unit!" );

	const Weapon bestWeapon = mUnitType->GetWeaponByIndex( bestWeaponIndex );
	DebugPrintf( "Best weapon: %d (%s)", bestWeaponIndex, bestWeapon.ToString() );

	// Calculate damage percentage (before randomness).
	int damagePercentage = CalculateDamagePercentage( target, bestWeaponIndex );

	// Separate the percentage into the guaranteed damage amount and the amount that
	// will contribute to the the extra damage roll.
	int guaranteedDamage = ( damagePercentage / 10 );
	int extraDamageChance = ( damagePercentage % 10 );

	DebugPrintf( "Guaranteed damage: %d", guaranteedDamage );
	DebugPrintf( "Extra damage chance: %d in 10", extraDamageChance );

	// Apply the guaranteed damage amount.
	int totalDamage = guaranteedDamage;

	// Roll a 10-sided die to see if extra damage should be applied.
	int extraDamageRoll = RNG::RandomInRange( 1, 10 );
	bool success = ( extraDamageChance >= extraDamageRoll );
	DebugPrintf( "Extra damage roll %s! (Rolled a %d, needed %d or lower to pass.)", ( success ? "SUCCEEDED" : "FAILED" ), extraDamageRoll, extraDamageChance );

	if( success )
	{
		// Add one point of extra damage if the extra damage roll succeeded.
		++totalDamage;
	}

	DebugPrintf( "TOTAL DAMAGE: %d", totalDamage );

	// Apply the damage to the target Unit.
	target.TakeDamage( totalDamage, this );

	if( bestWeapon.ConsumesAmmo() )
	{
		// Consume ammo equal to the amount that the weapon uses per shot.
		int ammoConsumed = bestWeapon.GetAmmoPerShot();
		ConsumeAmmo( ammoConsumed );
		DebugPrintf( "Weapon consumed %d ammo. (%d ammo remaining)", ammoConsumed, mAmmo );
	}

	// TODO: Replace AP system with single move + action system.
	ConsumeAP( 1 );
}


int Unit::CalculateDamagePercentage( const Unit& target, int weaponIndex ) const
{
	int result = 0;

	// Get the best weapon to use against the target.
	const Weapon& weapon = mUnitType->GetWeaponByIndex( weaponIndex );

	DebugPrintf( "Calculating damage of %s against %s with weapon %d (%s)...", ToString(), target.ToString(),
				 weaponIndex, weapon.ToString() );

	// Get the base amount of damage to apply.
	int baseDamagePercentage = weapon.GetDamagePercentageAgainstUnitType( target.GetUnitType() );
	assertion( baseDamagePercentage > 0, "Cannot calculate damage: weapon cannot target Unit!" );

	DebugPrintf( "Base damage percentage: %d%%", baseDamagePercentage );

	// Scale the damage amount based on the current health of this Unit.
	float healthScale = GetHealthScale();
	DebugPrintf( "Health scaling factor: %f", healthScale );

	// Scale the damage amount based on the target's defense bonus.
	float targetDefenseBonus = target.GetDefenseBonus();
	float targetDefenseScale = Mathf::Clamp( 1.0f - targetDefenseBonus, 0.0f, 1.0f );
	DebugPrintf( "Target defense bonus: %f (%f x damage)", targetDefenseBonus, targetDefenseScale );

	// Calculate idealized damage amount.
	result = (int) ( baseDamagePercentage * healthScale * targetDefenseScale );
	DebugPrintf( "TOTAL DAMAGE: %d (%d x %f x %f)", result, baseDamagePercentage, healthScale, targetDefenseScale );

	return result;
}


float Unit::GetDefenseBonus() const
{
	float result;

	DebugPrintf( "Calculating defense bonus for %s...", ToString() );

	// Get the defensive bonus supplied by the current tile.
	TerrainType* terrainType = gGame->GetTerrainTypeOfTile( GetTilePos() );
	int coverBonus = terrainType->GetCoverBonus();
	float coverBonusScale = ( coverBonus * 0.1f );
	DebugPrintf( "Cover bonus: %d (%f)", coverBonus, coverBonusScale );

	// Scale the cover bonus by the Unit's current health.
	float healthScale = GetHealthScale();
	DebugPrintf( "Health scale: %f x cover bonus", healthScale );

	result = Mathf::Clamp( coverBonusScale * healthScale, 0.0f, 1.0f );
	DebugPrintf( "TOTAL DEFENSE BONUS: %f (%f x %f)", result, healthScale, coverBonusScale );

	return result;
}


bool Unit::CanTarget( const Unit& target ) const
{
	// Return whether this Unit has a weapon that can attack the target.
	int bestWeaponIndex = GetBestAvailableWeaponAgainst( target.GetUnitType() );
	return ( bestWeaponIndex >= 0 );
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


int Unit::GetDistanceToUnit( const Unit& target ) const
{
	return mTilePos.GetManhattanDistanceTo( target.mTilePos );
}


bool Unit::CanFireWeapon( int weaponIndex ) const
{
	Weapon& weapon = mUnitType->GetWeaponByIndex( weaponIndex );
	return ( !weapon.ConsumesAmmo() || ( weapon.GetAmmoPerShot() <= mAmmo ) );
}


int Unit::GetBestAvailableWeaponAgainst( const UnitType* unitType ) const
{
	assertion( unitType, "Cannot get best available weapon against NULL UnitType!" );

	int bestDamagePercentage = 0;
	int bestWeaponIndex = -1;

	DebugPrintf( "Choosing best weapon for %s against %s...", ToString(), unitType->ToString() );

	for( int i = 0; i < mUnitType->GetNumWeapons(); ++i )
	{
		// Check each weapon to see which one is the best.
		const Weapon& weapon = mUnitType->GetWeaponByIndex( i );
		int damagePercentage = weapon.GetDamagePercentageAgainstUnitType( unitType );

		bool isBestChoice = ( damagePercentage > bestDamagePercentage );
		bool canFire = CanFireWeapon( i );
		bool canTarget = weapon.CanTargetUnitType( unitType );

		if( canFire && canTarget && isBestChoice )
		{
			// If this weapon can fire at the enemy and is better than any other weapon (so far),
			// save it as the best choice.
			bestDamagePercentage = damagePercentage;
			bestWeaponIndex = i;
		}

		DebugPrintf( "Weapon %d (%s) %s fire at a %s (%d%% damage) %s %s currently fire", i, weapon.ToString(), ( canTarget ? "CAN" : "CANNOT" ),
					 unitType->ToString(), damagePercentage, ( canTarget == canFire ? "and" : "but" ), ( canFire ? "CAN" : "CANNOT" ) );
	}

	if( bestWeaponIndex > -1 )
	{
		DebugPrintf( "BEST CHOICE: Weapon %d (%s)", bestWeaponIndex, mUnitType->GetWeaponByIndex( bestWeaponIndex ).ToString() );
	}
	else
	{
		DebugPrintf( "NO WEAPON AVAILABLE!" );
	}

	return bestWeaponIndex;
}


void Unit::SetHP( int hp )
{
	// Set the current HP of the Unit.
	mHP = Mathi::Clamp( hp, 0, MAX_HP );

	if( IsDead() )
	{
		// Run the destroyed event.
		OnDestroyed();

		// If the Unit is now dead, schedule it for removal from the Game.
		gGame->RemoveUnit( this );
	}
}


inline void Unit::TakeDamage( int damageAmount, Unit* instigator )
{
	if( instigator != nullptr )
	{
		DebugPrintf( "%s took %d damage from %s.", ToString(), damageAmount, instigator->ToString() );
	}
	else
	{
		DebugPrintf( "%s took %d damage.", ToString(), damageAmount );
	}

	// Reduce the HP of this Unit.
	SetHP( mHP - damageAmount );
}


void Unit::OnDestroyed()
{
	DebugPrintf( "%s has been destroyed!", ToString() );
	gGame->PostMessageFormat( mOwner->GetPlayerColor(), "%s has been destroyed", ToString() );
	mOwner->OnLoseUnit();
	gGame->CheckVictory();
}


void Unit::ResetAP()
{
	mAP = 2;
	mSprite->DrawColor = mSelectionColor;
}

void Unit::ConsumeAP( int ap )
{
	--mAP;
	if ( mAP <= 0 )
	{
		mAP = 0;
		mSprite->DrawColor = mDefaultColor;
	}
}
