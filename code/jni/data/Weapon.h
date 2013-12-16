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

		int GetAmmoPerShot() const;
		bool ConsumesAmmo() const;

		HashString GetName() const;
		std::string GetDisplayName() const;

	protected:
		int mAmmoPerShot;
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


	inline int Weapon::GetAmmoPerShot() const
	{
		return mAmmoPerShot;
	}


	inline bool Weapon::ConsumesAmmo() const
	{
		return ( mAmmoPerShot > 0 );
	}


	inline HashString Weapon::GetName() const
	{
		return mName;
	}
}
