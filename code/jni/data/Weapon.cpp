#include "androidwars.h"

using namespace mage;


MAGE_IMPLEMENT_RTTI( Record, Weapon );


Weapon::Weapon( const HashString& name ) :
	Record( name )
{ }


Weapon::~Weapon() { }


void Weapon::SetDamagePercentageAgainstUnitType( const HashString& unitTypeName, int damagePercentage )
{
	if( damagePercentage > 0 )
	{
		mDamagePercentagesByUnitTypeName[ unitTypeName ] = damagePercentage;
	}
}


int Weapon::GetDamagePercentageAgainstUnitType( const HashString& unitTypeName ) const
{
	int result = 0;

	auto it = mDamagePercentagesByUnitTypeName.find( unitTypeName );

	if( it != mDamagePercentagesByUnitTypeName.end() )
	{
		result = it->second;
	}

	return result;
}
