#pragma once

namespace mage
{
	class Weapon;

	/**
	 * Stores common information between all Units of a particular type.
	 */
	class UnitType : public UnitTypesTable::Record
	{
	public:
		static const int DEFAULT_MAX_SUPPLIES = 99;

		UnitType( const HashString& name );
		virtual ~UnitType();

		void AddWeapon( const Weapon& weapon );
		int GetNumWeapons() const;
		bool HasWeapons() const;
		Weapon& GetWeaponByIndex( int index );
		const Weapon& GetWeaponByIndex( int index ) const;

		int GetMaxAmmo() const;
		int GetMaxSupplies() const;

		MovementType* GetMovementType() const;
		int GetMovementRange() const;
		const IntRange& GetAttackRange() const { return mAttackRange; }
		int GetMovementCostAcrossTerrain( TerrainType* terrainType ) const;
		bool CanMoveAcrossTerrain( TerrainType* terrainType ) const;
		HashString GetAnimationSetName() const;

	protected:
		void LoadAnimation();

		int mMovementRange;
		int mMaxAmmo;
		int mMaxSupplies;
		IntRange mAttackRange;
		HashString mMovementTypeName;
		HashString mAnimationSetName;		// Name of the animation set, Tank.anim
		std::string mAnimationSetPath;		// Path to the animation set, sprites/Tank.anim
		std::string mDisplayName;
		std::vector< Weapon > mWeapons;

		friend class UnitTypesTable;
	};


	inline void UnitType::AddWeapon( const Weapon& weapon )
	{
		mWeapons.push_back( weapon );
	}


	inline int UnitType::GetNumWeapons() const
	{
		return (int) mWeapons.size();
	}


	inline bool UnitType::HasWeapons() const
	{
		return ( mWeapons.size() > 0 );
	}


	inline Weapon& UnitType::GetWeaponByIndex( int index )
	{
		assertion( index >= 0 && index < mWeapons.size(), "Weapon index %d is out of range!", index );
		return mWeapons[ index ];
	}


	inline const Weapon& UnitType::GetWeaponByIndex( int index ) const
	{
		assertion( index >= 0 && index < mWeapons.size(), "Weapon index %d is out of range!", index );
		return mWeapons[ index ];
	}


	inline int UnitType::GetMaxAmmo() const
	{
		return mMaxAmmo;
	}


	inline int UnitType::GetMaxSupplies() const
	{
		return mMaxSupplies;
	}


	inline int UnitType::GetMovementRange() const
	{
		return mMovementRange;
	}


	inline int UnitType::GetMovementCostAcrossTerrain( TerrainType* terrainType ) const
	{
		const MovementType* movementType = GetMovementType();
		assertion( movementType, "No MovementType \"%s\" found for %s!", mMovementTypeName.GetCString(), ToString() );
		return movementType->GetMovementCostAcrossTerrain( terrainType );
	}


	inline bool UnitType::CanMoveAcrossTerrain( TerrainType* terrainType ) const
	{
		return ( GetMovementCostAcrossTerrain( terrainType ) > -1 );
	}


	inline HashString UnitType::GetAnimationSetName() const
	{
		return mAnimationSetName;
	}
}
