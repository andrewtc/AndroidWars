#include "androidwars.h"

using namespace mage;


MAGE_AUTO_GENERATE_TABLE_NAME( UnitTypesTable, UnitType );

const char* const UnitTypesTable::WEAPONS_JSON_PROPERTY_NAME = "weapons";
const char* const UnitTypesTable::DAMAGE_JSON_PROPERTY_NAME = "damage";


UnitTypesTable::UnitTypesTable( Scenario* scenario )
	: Table( scenario )
{ }


UnitTypesTable::~UnitTypesTable() { }


void UnitTypesTable::OnLoadRecordFromJSON( UnitType* unitType, const rapidjson::Value& object )
{
	// Read in the sprite for this UnitType and pre-load it.
	unitType->mAnimationSetPath = Scenario::FormatAnimationPath( GetJSONStringValue( object, "animationSet", "" ) );
	unitType->mAnimationSetName = Scenario::FormatAnimationName( GetJSONStringValue( object, "animationSet", "" ) );
	unitType->LoadAnimation();

	// Read in the unit display name (if it exists).
	unitType->mDisplayName = GetJSONStringValue( object, "displayName", "" );

	// Read in unit gameplay properties.
	unitType->mMovementRange    = GetJSONIntValue( object, "movementRange", 0 );
	unitType->mMovementTypeName = GetJSONStringValue( object, "movementType", "" );
	StringUtil::ParseIntRange( GetJSONStringValue( object, "attackRange", "" ), IntRange( 0, 0 ), unitType->mAttackRange );
	unitType->mMaxAmmo          = GetJSONIntValue( object, "ammo", 0 );
	unitType->mMaxSupplies      = GetJSONIntValue( object, "supplies", UnitType::DEFAULT_MAX_SUPPLIES );

	if( object.HasMember( WEAPONS_JSON_PROPERTY_NAME ) )
	{
		// Read in all weapon data.
		const rapidjson::Value& weaponsArray = object[ WEAPONS_JSON_PROPERTY_NAME ];

		if( weaponsArray.IsArray() )
		{
			for( auto it = weaponsArray.Begin(); it != weaponsArray.End(); ++it )
			{
				const rapidjson::Value& weaponObject = ( *it );

				if( weaponObject.IsObject() )
				{
					// Grab the name from the XML element.
					HashString weaponName = GetJSONStringValue( weaponObject, "name", "" );

					if( !weaponName.GetString().empty() )
					{
						// Load all weapon data.
						Weapon weapon( weaponName );
						LoadWeaponFromJSON( weapon, weaponObject );
						unitType->AddWeapon( weapon );
					}
					else
					{
						WarnFail( "Cannot create Weapon for %s without a valid name!", unitType->ToString() );
					}
				}
				else
				{
					WarnFail( "Could not parse Weapon for %s because JSON element %d is not an object!", unitType->ToString(), ( it - weaponsArray.Begin() ) );
				}
			}
		}
		else
		{
			WarnFail( "Could not parse Weapons for %s because the JSON value \"%s\" is not an array!", unitType->ToString(), WEAPONS_JSON_PROPERTY_NAME );
		}
	}
}


void UnitTypesTable::LoadWeaponFromJSON( Weapon& weapon, const rapidjson::Value& object )
{
	// Load properties.
	weapon.mDisplayName = GetJSONStringValue( object, "displayName", "" );
	weapon.mAmmoPerShot = GetJSONIntValue( object, "ammoPerShot", 0 );

	DebugPrintf( "Loaded Weapon \"%s\" (%s ammo)", weapon.GetName().GetCString(), ( weapon.ConsumesAmmo() ? "uses" : "does not use" ) );

	if( object.HasMember( DAMAGE_JSON_PROPERTY_NAME ) )
	{
		// Parse the array of damage values.
		const rapidjson::Value& damageArray = object[ DAMAGE_JSON_PROPERTY_NAME ];

		if( damageArray.IsArray() )
		{
			for( auto it = damageArray.Begin(); it != damageArray.End(); ++it )
			{
				const rapidjson::Value& damageObject = ( *it );

				if( damageObject.IsObject() )
				{
					// Load all damage information.
					HashString unitTypeName = GetJSONStringValue( damageObject, "unitType", "" );

					if( !unitTypeName.GetString().empty() )
					{
						int damagePercentage = GetJSONIntValue( damageObject, "percentage", 0 );

						DebugPrintf( "Damage against \"%s\": %d%%", unitTypeName.GetCString(), damagePercentage );

						weapon.SetDamagePercentageAgainstUnitType( unitTypeName, damagePercentage );
					}
					else
					{
						WarnFail( "Cannot load damage value for Weapon \"%s\" without a valid UnitType name!", weapon.GetName().GetCString() );
					}
				}
				else
				{
					WarnFail( "Could not parse damage value for Weapon \"%s\" because JSON element %d is not an object!", weapon.GetName().GetCString(), ( it - damageArray.Begin() ) );
				}
			}
		}
		else
		{
			WarnFail( "Could not parse damage value for Weapon \"%s\" because the JSON value \"%s\" is not an array!", weapon.GetName().GetCString(), DAMAGE_JSON_PROPERTY_NAME );
		}
	}
}
