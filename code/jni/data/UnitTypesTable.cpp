#include "androidwars.h"

using namespace mage;


MAGE_AUTO_GENERATE_TABLE_NAME( UnitTypesTable, UnitType );


UnitTypesTable::UnitTypesTable( Database* database )
	: Table( database )
{ }


UnitTypesTable::~UnitTypesTable() { }


void UnitTypesTable::LoadRecordFromXml( UnitType* unitType, XmlReader::XmlReaderIterator xmlIterator )
{
	// Read in the sprite for this UnitType and pre-load it.
	unitType->mAnimationSetPath = UnitType::FormatAnimationPath( xmlIterator.GetAttributeAsString( "animationSet" ) );
	unitType->mAnimationSetName = UnitType::FormatAnimationName( xmlIterator.GetAttributeAsString( "animationSet" ) );
	unitType->LoadAnimation();

	// Read in the unit display name (if it exists).
	unitType->mDisplayName = xmlIterator.GetAttributeAsString( "displayName", "" );

	// Read in unit gameplay properties.
	unitType->mMovementRange    = xmlIterator.GetAttributeAsInt( "movementRange", 0 );
	unitType->mMovementTypeName = xmlIterator.GetAttributeAsString( "movementType" );
	unitType->mAttackRange      = xmlIterator.GetAttributeAsIntRange( "attackRange" );
	unitType->mMaxHP            = xmlIterator.GetAttributeAsInt( "hp", 1 );
	unitType->mMaxAmmo          = xmlIterator.GetAttributeAsInt( "ammo", 0 );
	unitType->mMaxSupplies      = xmlIterator.GetAttributeAsInt( "supplies", UnitType::DEFAULT_MAX_SUPPLIES );

	// Read in all weapon data.
	XmlReader::XmlReaderIterator weaponsElement = xmlIterator.NextChild( "Weapons" );

	if( weaponsElement.IsValid() )
	{
		for( auto weaponIterator = weaponsElement.NextChild( "Weapon" );
			 weaponIterator.IsValid(); weaponIterator = weaponIterator.NextSibling( "Weapon" ) )
		{
			// Grab the name from the XML element.
			HashString weaponName = weaponIterator.GetAttributeAsString( "name" );
			assertion( !weaponName.GetString().empty(), "Cannot create Weapon for %s without a valid name!", unitType->ToString() );

			// Load all weapon data.
			Weapon weapon( weaponName );
			LoadWeaponFromXml( weapon, weaponIterator );
			unitType->AddWeapon( weapon );
		}
	}
}


void UnitTypesTable::LoadWeaponFromXml( Weapon& weapon, XmlReader::XmlReaderIterator xmlIterator )
{
	// Load properties.
	weapon.mDisplayName = xmlIterator.GetAttributeAsString( "displayName", "" );
	weapon.mAmmoPerShot = xmlIterator.GetAttributeAsInt( "ammoPerShot", 0 );

	DebugPrintf( "Loaded weapon %s (%s ammo)", weapon.GetName().GetCString(), ( weapon.ConsumesAmmo() ? "uses" : "does not use" ) );

	for( auto damageIterator = xmlIterator.NextChild( "Damage" );
		 damageIterator.IsValid(); damageIterator = damageIterator.NextSibling( "Damage" ) )
	{
		// Load all damage information.
		HashString unitTypeName = damageIterator.GetAttributeAsString( "unitType" );
		int damagePercentage = damageIterator.GetAttributeAsInt( "percentage", 0 );

		DebugPrintf( "Damage against \"%s\": %d%%", unitTypeName.GetString().c_str(), damagePercentage );

		weapon.SetDamagePercentageAgainstUnitType( unitTypeName, damagePercentage );
	}
}
