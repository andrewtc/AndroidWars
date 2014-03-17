#include "androidwars.h"

using namespace mage;


const int Unit::MAX_HP;


MAGE_IMPLEMENT_RTTI( MapObject, Unit );


Unit::Unit( const std::string& name )
	: MapObject( name )
	, mGame( nullptr )
	, mUnitType( nullptr )
	, mSprite( nullptr )
	, mOwner( nullptr )
	, mOwnerIndex( -1 )
	, mHP( 0 )
	, mAP( 0 )
	, mAmmo( -1 )
	, mWasLoadedByMap( false )
{ }


Unit::~Unit() { }


/** Load the Xml properties from the MapObject */
void Unit::OnLoadProperty( const std::string& name, const std::string& value )
{
	bool parseWasSuccessful = false;

	if ( name == "UnitType" )
	{
		// Read in the name of the UnitType to use for this Unit.
		parseWasSuccessful = true;
		mUnitTypeName = value;
	}
	else if ( name == "Owner" )
	{
		// Read the Player index from the property.
		parseWasSuccessful = StringUtil::StringToType( value, &mOwnerIndex );
	}
	else if ( name == "Ammo" )
	{
		// Read in ammo amount.
		parseWasSuccessful = StringUtil::StringToType( value, &mAmmo );
	}

	assertion( parseWasSuccessful, "Could not parse %s value for %s! (\"%s\" specified.)", name.c_str(), ToString(), value.c_str() );
}


void Unit::OnLoadFinished()
{
	// Mark that this Unit was loaded by the TileMap.
	mWasLoadedByMap = true;
}


void Unit::Init( Game* game )
{
	DebugPrintf( "Initializing Unit \"%s\".", mDebugName.c_str() );

	// Keep track of the Game that spawned this Unit.
	mGame = game;

	// Make sure a valid Game was specified.
	assertion( mGame, "Cannot initialize Unit because it is not associated with a valid Game!" );

	if( mUnitType == nullptr )
	{
		// Load the UnitType for this Unit (if necessary).
		mUnitType = mGame->GetScenario()->UnitTypes.FindByName( mUnitTypeName );
		assertion( mUnitType, "UnitType \"%s\" not found!", mUnitTypeName.GetCString() );
	}

	// Format debug name string.
	static const size_t BUFFER_SIZE = 256;
	char buffer[ BUFFER_SIZE ];

	snprintf( buffer, BUFFER_SIZE, "%s \"%s\"", mUnitType->GetName().GetCString(), mName.GetCString() );
	mDebugName = buffer;

	if( mOwner == nullptr )
	{
		// If the player does not have  the owning player.
		Player* owner = mGame->GetPlayer( mOwnerIndex );
		assertion( owner, "Invalid Player index %d specified for %s!", mOwnerIndex, ToString() );
		SetOwner( owner );
	}

	if( mWasLoadedByMap )
	{
		// Determine the tile location of the unit.
		Vec2i tilePos = mGame->GetMap()->WorldToTile( Position );

		// Snap the unit to the tile grid.
		SetTilePos( tilePos );
	}

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


bool Unit::IsInitialized() const
{
	return ( mGame != nullptr );
}


void Unit::OnDraw( const Camera& camera ) const
{
	assertion( IsInitialized(), "Cannot draw Unit because it is not initialized!", ToString() );

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
	assertion( IsInitialized(), "Cannot update Unit because it is not initialized!", ToString() );

	float delta = ( Position - mDestination ).LengthSqr();
	if ( delta > 1 )
	{
		Vec2f vel = mDestination - Position;
		vel.Normalize();
		vel *= 100;
		Position += vel * dt;

		delta = ( Position - mDestination ).LengthSqr();
		if ( delta < 1 )
			mGame->OnUnitReachedDestination( this );
	}
}

void Unit::SaveToJSON( rapidjson::Document& document, rapidjson::Value& object )
{
	auto& allocator = document.GetAllocator();

	// Save UnitType name.
	std::string unitTypeName = mUnitType->GetName().GetString();
	rapidjson::Value unitTypeValue;
	unitTypeValue.SetString( unitTypeName.c_str(), allocator );
	object.AddMember( "unitType", unitTypeValue, allocator );

	// Save owner Player index.
	rapidjson::Value ownerIndexValue;
	ownerIndexValue.SetInt( mOwner->GetIndex() );
	object.AddMember( "owner", ownerIndexValue, allocator );

	// Save position.
	Vec2i tilePosition = GetTilePos();

	rapidjson::Value xValue;
	xValue.SetInt( tilePosition.x );
	object.AddMember( "x", xValue, allocator );

	rapidjson::Value yValue;
	yValue.SetInt( tilePosition.y );
	object.AddMember( "y", yValue, allocator );
}


void Unit::LoadFromJSON( const rapidjson::Value& object )
{
	// TODO
}


void Unit::SetUnitType( UnitType* unitType )
{
	mUnitType = unitType;

	if( IsInitialized() )
	{
		assertion( unitType, "Cannot set UnitType of Unit to null!" );
	}
}


void Unit::SetTilePos( const Vec2i& tilePos )
{
	// Update the tile position.
	mTilePos = tilePos;

	// Update the position of the object in the world.
	Position = mGame->GetMap()->TileToWorld( tilePos );

	// Update the destination.
	mDestination = Position;
}


void Unit::SetDestination( const Vec2i& tilePos )
{
	mDestination = mGame->GetMap()->TileToWorld( tilePos );
	mTilePos = mGame->GetMap()->WorldToTile( mDestination );
}


void Unit::SetOwner( Player* owner )
{
	// Give the Unit to the new owner.
	Player* formerOwner = mOwner;
	mOwner = owner;

	if( IsInitialized() )
	{
		assertion( owner, "Cannot give Unit to null Player!" );

		if( formerOwner != nullptr )
		{
			// If there was a previous owner, notify it that it no longer owns this Unit.
			formerOwner->OnLoseUnit( this );
		}

		// Notify the new player that it gained a Unit.
		mOwner->OnGainUnit( this );
	}
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
	DebugPrintf( "Best weapon: %d (%s)", bestWeaponIndex, bestWeapon.GetName().GetCString() );

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
				 weaponIndex, weapon.GetName().GetCString() );

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
	TerrainType* terrainType = mGame->GetTerrainTypeOfTile( GetTilePos() );
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

		DebugPrintf( "Weapon %d (%s) %s fire at a %s (%d%% damage) %s %s currently fire", i, weapon.GetName().GetCString(), ( canTarget ? "CAN" : "CANNOT" ),
					 unitType->ToString(), damagePercentage, ( canTarget == canFire ? "and" : "but" ), ( canFire ? "CAN" : "CANNOT" ) );
	}

	if( bestWeaponIndex > -1 )
	{
		DebugPrintf( "BEST CHOICE: Weapon %d (%s)", bestWeaponIndex, mUnitType->GetWeaponByIndex( bestWeaponIndex ).GetName().GetCString() );
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
		mGame->RemoveUnit( this );
	}
}


void Unit::TakeDamage( int damageAmount, Unit* instigator )
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
	mGame->PostMessageFormat( mOwner->GetPlayerColor(), "%s has been destroyed", ToString() );
	mOwner->OnLoseUnit( this );
	mGame->CheckVictory();
}


void Unit::ResetAP()
{
	mAP = 2;
	mSprite->DrawColor = mSelectionColor;
}

void Unit::ConsumeAP( int ap )
{
	mAP -= ap;
	if ( mAP <= 0 )
	{
		mAP = 0;
		mSprite->DrawColor = mDefaultColor;
	}
}

int Unit::GetTotalHP() const
{
	return mUnitType->GetMaxHP();
}
