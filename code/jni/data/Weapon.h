#pragma once

namespace mage
{
	/**
	 * Stores damage information
	 */
	class Weapon
	{
	public:
		Weapon();
		~Weapon();
		
		void SetDamagePercentageAgainstUnitType( const HashString& unitTypeName, int damagePercentage );
		void SetDamagePercentageAgainstUnitType( const UnitType* unitType, int damagePercentage );
		int GetDamagePercentageAgainstUnitType( const HashString& unitTypeName ) const;
		int GetDamagePercentageAgainstUnitType( const UnitType* unitType ) const;
		bool CanTargetUnitType( const HashString& unitTypeName ) const;
		bool CanTargetUnitType( const UnitType* unitType ) const;

		HashString getName() const;
		std::string getDisplayName() const;

	protected:
		HashString mName;
		std::string mDisplayName;
		HashMap< int > mDamagePercentagesByUnitTypeName;

		friend class UnitTypesTable;
	};


	inline void Weapon::SetDamagePercentageAgainstUnitType( const UnitType* unitType, int damagePercentage )
	{
		SetDamagePercentageAgainstUnitType( unitType->GetName(), damagePercentage );
	}


	inline int Weapon::GetDamagePercentageAgainstUnitType( const UnitType* unitType ) const
	{
		return GetDamagePercentageAgainstUnitType( unitType->GetName() );
	}


	inline bool Weapon::CanTargetUnitType( const HashString& unitTypeName ) const
	{
		return ( GetDamagePercentageAgainstUnitType( unitTypeName ) > 0 );
	}


	inline bool Weapon::CanTargetUnitType( const UnitType* unitType ) const
	{
		return CanTargetUnitType( unitType->GetName() );
	}
}
