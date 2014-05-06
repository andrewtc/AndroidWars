#include "androidwars.h"

using namespace mage;


const int Unit::MAX_HEALTH;


MAGE_IMPLEMENT_RTTI( MapObject, Unit );


Unit::Unit() :
	mMap( nullptr ),
	mID( -1 ),
	mUnitType( nullptr ),
	mOwner( nullptr ),
	mHealth( MAX_HEALTH ),
	mAmmo( 0 ),
	mSupplies( 0 ),
	mIsAlive( true ),
	mIsActive( true )
{ }


Unit::~Unit()
{
	if( IsInitialized() )
	{
		Destroy();
	}
}


void Unit::Init( Map* map, size_t unitID, const Map::Iterator& tile )
{
	assertion( !IsInitialized(), "Cannot initialize Unit that has already been initialized!" );

	// Keep track of the Game that spawned this Unit.
	mMap = map;
	assertion( mMap, "Cannot initialize Unit because no Map was specified!" );

	// Keep track of the Unit's unique ID.
	mID = unitID;
	assertion( mID > 0, "Cannot initialize Unit because the unit ID (%d) is invalid!", mID );

	// Make sure a valid UnitType was specified for this Unit.
	assertion( mUnitType, "Could not initialize Unit because no UnitType was specified!" );

	// Make sure a valid owner was specified.
	assertion( mOwner, "Could not initialize Unit because no owning Faction was specified!" );

	// Notify the owner that it owns this Unit.
	mOwner->UnitGained( this );

	// Keep track of the Unit's current tile.
	mTile = tile;
	assertion( mTile.IsValid(), "Cannot initialize Unit at invalid Map tile (%d,%d)!", mTile.GetX(), mTile.GetY() );
	assertion( mTile->IsEmpty(), "Cannot initialize Unit at Map tile (%d,%d) because a Unit already exists at that location!", mTile.GetX(), mTile.GetY() );
}


bool Unit::IsInitialized() const
{
	return ( mMap != nullptr );
}


void Unit::Destroy()
{
	assertion( IsInitialized(), "Cannot destroy Unit that has not been initialized!" );

	// Run the destroyed event.
	OnDestroyed.Invoke();

	// Disassociate the Unit from its current Tile.
	mTile = Map::Iterator();

	// Notify the owner that the Unit died.
	mOwner->UnitLost( this );

	// Reset the unit ID.
	mID = -1;

	// Disassociate the Unit from the Map.
	mMap = nullptr;
}


void Unit::OnTurnStart( int turnIndex )
{
	// Consume supplies.
	MovementType* movementType = GetMovementType();
	ConsumeSupplies( movementType->GetSuppliesConsumedPerTurn() );

	if( movementType->RequiresSuppliesToSurvive() && !HasSupplies() )
	{
		// If the Unit runs out of supplies and it needs them to survive, kill it.
		Die();
	}
}


void Unit::OnTurnEnd( int turnIndex )
{
	// Reactivate this Unit.
	Activate();
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
	// TODO: Save Units organized by Faction.
	//rapidjson::Value ownerIndexValue;
	//ownerIndexValue.SetInt( mOwner->GetIndex() );
	//object.AddMember( "owner", ownerIndexValue, allocator );

	// Save position.
	Vec2i tilePosition = GetTilePos();

	rapidjson::Value valueX;
	valueX.SetInt( tilePosition.x );
	object.AddMember( "x", valueX, allocator );

	rapidjson::Value valueY;
	valueY.SetInt( tilePosition.y );
	object.AddMember( "y", valueY, allocator );
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


UnitType* Unit::GetUnitType() const
{
	return mUnitType;
}


Map::Iterator Unit::GetTile() const
{
	return mTile;
}


Vec2s Unit::GetTilePos() const
{
	return mTile.GetPosition();
}


short Unit::GetTileX() const
{
	return mTile.GetX();
}


short Unit::GetTileY() const
{
	return mTile.GetY();
}


void Unit::SetOwner( Faction* owner )
{
	assertion( owner, "Cannot give Unit to null Player!" );

	// Give the Unit to the new owner.
	Faction* formerOwner = mOwner;
	mOwner = owner;

	if( IsInitialized() && mOwner != formerOwner )
	{
		if( formerOwner != nullptr )
		{
			// If there was a previous owner, notify it that it no longer owns this Unit.
			formerOwner->UnitLost( this );
		}

		// Notify the new player that it gained a Unit.
		mOwner->UnitGained( this );

		// Fire the owner changed event.
		OnOwnerChanged.Invoke( owner, formerOwner );
	}
}


Faction* Unit::GetOwner() const
{
	return mOwner;
}


MovementType* Unit::GetMovementType() const
{
	return mUnitType->GetMovementType();
}


int Unit::GetMovementRange() const
{
	// Get the maximum movement range for this unit.
	UnitType* type = GetUnitType();
	int movementRange = type->GetMovementRange();

	// Take supplies into account.
	return std::min( movementRange, mSupplies );
}


int Unit::GetMovementCostAcrossTerrain( TerrainType* terrainType ) const
{
	assertion( terrainType, "Cannot get movement cost because null TerrainType was specified!" );
	assertion( mUnitType, "No UnitType found for Unit!" );
	return mUnitType->GetMovementCostAcrossTerrain( terrainType );
}


int Unit::CalculatePathCost( const Path& path ) const
{
	int totalCost = 0;

	// Get the current tile for this Unit.
	Map::Iterator currentTile = mMap->GetTile( path.GetOrigin() );

	for( size_t i = 0, length = path.GetLength(); i < length; ++i )
	{
		// Get the adjacent tile in the direction specified.
		PrimaryDirection direction = path.GetDirection( i );
		currentTile = currentTile.GetAdjacent( direction );

		if( currentTile.IsValid() )
		{
			// Add the cost of entering the tile to the path.
			int movementCost = GetMovementCostAcrossTerrain( currentTile->GetTerrainType() );
			totalCost += movementCost;
		}
	}

	return totalCost;
}


bool Unit::CanMoveAcrossTerrain( TerrainType* terrainType ) const
{
	return ( GetMovementCostAcrossTerrain( terrainType ) > -1 );
}


bool Unit::CanEnterTile( const Map::ConstIterator& tile ) const
{
	bool result = false;

	if( tile->IsEmpty() || tile->GetUnit()->GetOwner() == mOwner )
	{
		// If the Tile is not occupied by an enemy Unit, check the Tile's TerrainType.
		MovementType* movementType = GetMovementType();
		TerrainType* terrainType = tile->GetTerrainType();

		// Return true only if the Unit can cross the TerrainType of the Tile.
		result = movementType->CanMoveAcrossTerrain( terrainType );
	}

	return result;
}


bool Unit::CanOccupyTile( const Map::ConstIterator& tile ) const
{
	return ( tile.IsValid() && CanEnterTile( tile ) && ( tile->IsEmpty() || tile->GetUnit() == this ) );
}


bool Unit::CanMoveAlongPath( const Path& path ) const
{
	// Get a valid path from the proposed path.
	Path validPath;
	GetValidPath( path, validPath );

	// Return true if the validated path is exactly the same length as the proposed one.
	return ( path.GetLength() == validPath.GetLength() );
}


void Unit::GetValidPath( const Path& path, Path& result ) const
{
	// Clear the result.
	result.Clear();

	// Set the origin of the result.
	result.SetOrigin( path.GetOrigin() );

	// Get the origin tile.
	Map::ConstIterator tile = mMap->GetTile( path.GetOrigin() );

	for( size_t index = 0, length = path.GetLength(); index < length; ++index )
	{
		// Move in the next direction.
		PrimaryDirection direction = path.GetDirection( index );
		tile = tile.GetAdjacent( direction );

		if( CanEnterTile( tile ) )
		{
			// If the Tile is enterable, add a direction to the result.
			result.AddDirection( direction );
		}
		else
		{
			// Otherwise, stop adding to the result.
			break;
		}
	}
}


void Unit::Teleport( const Vec2s& tilePos )
{
	// Teleport to the Tile at the position.
	Map::Iterator tile = mMap->GetTile( tilePos );
	Teleport( tile );
}


void Unit::Teleport( Map::Iterator tile )
{
	// Set the tile for the Unit.
	SetTile( tile );

	// Fire the teleport event.
	OnTeleport.Invoke( tile );
}


bool Unit::Move( const Path& path )
{
	bool success = false;

	if( IsActive() )
	{
		// Check if the Unit is able to move along the Path.
		Path validPath;
		GetValidPath( path, validPath );

		if( validPath.GetLength() > 0 )
		{
			// Move to the destination tile.
			Map::Iterator destinationTile = mMap->GetTile( validPath.GetDestination() );

			if( CanOccupyTile( destinationTile ) )
			{
				// If the Unit can occupy the destination (i.e. a friendly Unit is not in the Tile),
				// Move the Unit to the destination tile.
				SetTile( destinationTile );
			}
			else
			{
				// Otherwise, remove the Unit from its current Tile (i.e. take the Unit off the board).
				RemoveFromCurrentTile();
			}

			// Consume supplies equal to the cost of traversing the verified path.
			int pathCost = CalculatePathCost( validPath );
			ConsumeSupplies( pathCost );
		}

		// Determine whether the Unit was able to move along the original Path.
		success = ( validPath.GetLength() == path.GetLength() );

		if( !success )
		{
			// TODO: It's a trap!
		}

		// Fire the move event.
		OnMove.Invoke( validPath );
	}
	else
	{
		WarnFail( "Cannot order inactive Unit to move!" );
	}

	return success;
}


void Unit::SetTile( Map::Iterator tile )
{
	if( tile != mTile )
	{
		// Tell the previous Tile that the Unit left.
		if( mTile.IsValid() )
		{
			mTile->ClearUnit();
		}

		// Update the current Tile for the Unit.
		mTile = tile;

		if( mTile.IsValid() )
		{
			// If the Unit is moving to a different Tile, make sure the tile is not occupied.
			assertion( tile->IsEmpty(), "Cannot place Unit into Tile (%d,%d) because the tile is occupied by another Unit!", tile.GetX(), tile.GetY() );

			// If not moving to an invalid tile, tell the new Tile that the Unit has entered.
			mTile->SetUnit( this );
		}

		// Fire the tile changed event.
		OnTileChanged.Invoke( mTile );
	}
}


void Unit::RemoveFromCurrentTile()
{
	// Move the Unit to an invalid tile.
	SetTile( Map::Iterator() );
}


bool Unit::IsOwnedBy( Faction* faction ) const
{
	return ( mOwner == faction );
}


bool Unit::CanAttack( const Unit* target ) const
{
	assertion( target, "Cannot check whether Unit can attack null Unit!" );

	DebugPrintf( "Checking whether %s can attack %s...", ToString().c_str(), target->ToString().c_str() );

	// Make sure this Unit is still alive.
	bool isAlive = IsAlive();
	DebugPrintf( "%s is %s.", ToString().c_str(), ( isAlive ? "ALIVE" : "DEAD" ) );

	// Make sure this Unit is an enemy.
	bool isEnemy = mMap->AreEnemies( mOwner, target->GetOwner() );
	DebugPrintf( "%s %s an enemy.", target->ToString().c_str(), ( isEnemy ? "IS" : "IS NOT" ) );

	// Check whether this Unit can target the other Unit.
	bool canTarget = CanTarget( target );
	DebugPrintf( "%s %s target %s.", ToString().c_str(), ( canTarget ? "CAN" : "CANNOT" ), target->ToString().c_str() );

	bool result = ( isAlive && isEnemy && canTarget );
	DebugPrintf( "RESULT: %s %s attack %s.", ToString().c_str(), ( result ? "CAN" : "CANNOT" ), target->ToString().c_str() );

	return result;
}


void Unit::Attack( Unit* target )
{
	assertion( target, "Cannot order Unit to attack null Unit!" );

	DebugPrintf( "%s attacks %s.", ToString().c_str(), target->ToString().c_str() );

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
	target->TakeDamage( totalDamage, this );

	if( bestWeapon.ConsumesAmmo() )
	{
		// Consume ammo equal to the amount that the weapon uses per shot.
		int ammoConsumed = bestWeapon.GetAmmoPerShot();
		ConsumeAmmo( ammoConsumed );
		DebugPrintf( "Weapon consumed %d ammo. (%d ammo remaining)", ammoConsumed, mAmmo );
	}
}


int Unit::CalculateDamagePercentage( const Unit* target, int weaponIndex ) const
{
	assertion( target, "Cannot calculate damage percentage against null Unit!" );

	int result = 0;

	// Get the best weapon to use against the target.
	const Weapon& weapon = mUnitType->GetWeaponByIndex( weaponIndex );

	DebugPrintf( "Calculating damage of %s against %s with weapon %d (%s)...", ToString().c_str(), target->ToString().c_str(),
				 weaponIndex, weapon.GetName().GetCString() );

	// Get the base amount of damage to apply.
	int baseDamagePercentage = weapon.GetDamagePercentageAgainstUnitType( target->GetUnitType() );
	assertion( baseDamagePercentage > 0, "Cannot calculate damage: weapon cannot target Unit!" );

	DebugPrintf( "Base damage percentage: %d%%", baseDamagePercentage );

	// Scale the damage amount based on the current health of this Unit.
	float healthScale = GetHealthScale();
	DebugPrintf( "Health scaling factor: %f", healthScale );

	// Scale the damage amount based on the target's defense bonus.
	float targetDefenseBonus = target->GetDefenseBonus();
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

	DebugPrintf( "Calculating defense bonus for %s...", ToString().c_str() );

	// Get the defensive bonus supplied by the current tile.
	TerrainType* terrainType = mTile->GetTerrainType();
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


bool Unit::CanTarget( const Unit* target ) const
{
	assertion( target, "Cannot check whether Unit can target null Unit!" );

	// Return whether this Unit has a weapon that can attack the target.
	int bestWeaponIndex = GetBestAvailableWeaponAgainst( target->GetUnitType() );
	return ( bestWeaponIndex >= 0 );
}


bool Unit::IsInRange( const Unit* target ) const
{
	assertion( target, "Cannot check whether Unit is within range of null Unit!" );
	return IsInRangeFromTile( target, mTile );
}


bool Unit::IsInRangeFromTile( const Unit* target, const Map::ConstIterator& tile ) const
{
	assertion( target, "Cannot check whether Unit is in range of null Unit!" );
	assertion( tile.IsValid(), "Cannot test range to target from invalid Tile!" );

	// Get the range of the Unit's unit type.
	UnitType* type = GetUnitType();
	const IntRange& range = type->GetAttackRange();

	// Find the distance from the tile to the target.
	short distanceToTarget = tile.GetPosition().GetManhattanDistanceTo( target->GetTilePos() );

	// Determine whether this Unit is in range.
	bool isInRange = range.IsValueInRange( distanceToTarget );
	DebugPrintf( "%s %s in range of %s from tile (%d,%d).", target->ToString().c_str(), ( isInRange ? "is" : "is NOT" ), ToString().c_str(), tile.GetX(), tile.GetY() );

	return isInRange;
}


bool Unit::IsInRangeFromLocation( const Unit* target, const Vec2s& location ) const
{
	Map::ConstIterator tile = mMap->GetTile( location );
	return IsInRangeFromTile( target, tile );
}


int Unit::GetDistanceToUnit( const Unit* target ) const
{
	assertion( target, "Cannot get distance of Unit to null Unit!" );
	return GetTilePos().GetManhattanDistanceTo( target->GetTilePos() );
}


bool Unit::CanFireWeapon( int weaponIndex ) const
{
	Weapon& weapon = mUnitType->GetWeaponByIndex( weaponIndex );
	return ( !weapon.ConsumesAmmo() || ( weapon.GetAmmoPerShot() <= mAmmo ) );
}


int Unit::GetBestAvailableWeaponAgainst( const Unit* target ) const
{
	assertion( target, "Cannot get best available Weapon against null Unit!" );
	return GetBestAvailableWeaponAgainst( target->GetUnitType() );
}


int Unit::GetBestAvailableWeaponAgainst( const UnitType* unitType ) const
{
	assertion( unitType, "Cannot get best available weapon against NULL UnitType!" );

	int bestDamagePercentage = 0;
	int bestWeaponIndex = -1;

	DebugPrintf( "Choosing best weapon for %s against %s...", ToString().c_str(), unitType->ToString() );

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


void Unit::SetHealth( int health )
{
	// Set the current HP of the Unit.
	int verifiedHealth = Mathi::Clamp( health, 0, MAX_HEALTH );

	if( mHealth != verifiedHealth )
	{
		mHealth = verifiedHealth;

		if( IsInitialized() )
		{
			// Fire the health changed event.
			OnHealthChanged.Invoke( mHealth );

			if( mHealth == 0 )
			{
				// If the Unit runs out of health, kill it.
				Die();
			}
		}
	}
}


void Unit::AddHealth( int health )
{
	SetHealth( mHealth + health );
}


void Unit::ResetHealth()
{
	// Reset the Unit's health to maximum.
	SetHealth( MAX_HEALTH );
}


void Unit::TakeDamage( int damageAmount, Unit* instigator )
{
	assertion( IsInitialized(), "Unit cannot take damage because it has not been initialized!" );

	// Fire the damaged event.
	OnTakeDamage.Invoke( damageAmount, instigator );

	// Reduce the HP of this Unit.
	SetHealth( mHealth - damageAmount );
}


void Unit::Die()
{
	assertion( IsInitialized(), "Cannot kill Unit that has not been initialized!" );

	if( mIsAlive )
	{
		// Mark the Unit as dead.
		mIsAlive = false;

		// Run the death event.
		OnDeath.Invoke();

		// Notify the Map that this Unit died.
		mMap->UnitDied( this );
	}
}


int Unit::GetHealth() const
{
	return mHealth;
}


bool Unit::IsDamaged() const
{
	return ( mHealth < MAX_HEALTH );
}


bool Unit::IsAlive() const
{
	return mIsAlive;
}


bool Unit::IsDead() const
{
	return !mIsAlive;
}


float Unit::GetHealthScale() const
{
	return ( (float) mHealth / MAX_HEALTH );
}


bool Unit::CanReinforce( Unit* target ) const
{
	// Allow different Units owned by the same Faction with the same UnitType to reinforce each other, as long as the target is partially damaged.
	return ( target && target != this && target->IsAlive() && target->GetOwner() == mOwner && target->GetUnitType() == mUnitType && target->GetHealth() < MAX_HEALTH );
}


void Unit::SetAmmo( int ammo )
{
	mAmmo = Mathi::Clamp( ammo, 0, mUnitType->GetMaxAmmo() );
}


void Unit::AddAmmo( int ammo )
{
	SetAmmo( mAmmo + ammo );
}


void Unit::ResetAmmo()
{
	mAmmo = mUnitType->GetMaxAmmo();
}


void Unit::ConsumeAmmo( int ammo )
{
	SetAmmo( mAmmo - ammo );
}


int Unit::GetAmmo() const
{
	return mAmmo;
}


bool Unit::HasAmmo() const
{
	return ( mAmmo > 0 );
}


void Unit::SetSupplies( int supplies )
{
	mSupplies = Mathi::Clamp( supplies, 0, mUnitType->GetMaxSupplies() );
	DebugPrintf( "Unit now has %d supplies.", mSupplies );
}


void Unit::AddSupplies( int supplies )
{
	SetSupplies( mSupplies + supplies );
}


void Unit::ResetSupplies()
{
	mSupplies = mUnitType->GetMaxSupplies();
}


void Unit::ConsumeSupplies( int supplies )
{
	DebugPrintf( "Consuming %d supplies from Unit.", supplies );
	SetSupplies( mSupplies - supplies );
}


int Unit::GetSupplies() const
{
	return mSupplies;
}


bool Unit::HasSupplies() const
{
	return ( mSupplies > 0 );
}


void Unit::SetActive( bool active )
{
	if( mIsActive != active )
	{
		mIsActive = active;

		if( IsInitialized() )
		{
			if( mIsActive )
			{
				// Call the activate event.
				OnActivate.Invoke();
			}
			else
			{
				// Call the deactivate event.
				OnDeactivate.Invoke();
			}
		}
	}
}


void Unit::Activate()
{
	SetActive( true );
}


void Unit::Deactivate()
{
	SetActive( false );
}


bool Unit::IsActive() const
{
	return mIsActive;
}


bool Unit::IsInactive() const
{
	return !mIsActive;
}


Map* Unit::GetMap() const
{
	return mMap;
}


int Unit::GetID() const
{
	return mID;
}


std::string Unit::ToString() const
{
	std::stringstream formatter;
	formatter << mUnitType->GetName().GetString() << " (" << mTile.GetX() << "," << mTile.GetY() << ")";
	return formatter.str();
}
