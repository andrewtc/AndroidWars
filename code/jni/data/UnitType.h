#pragma once

namespace mage
{
	class Weapon;

	/**
	 * Stores common information between all Units of a particular type.
	 */
	class UnitType : public Record
	{
	public:
		static const char* const SPRITE_DIRECTORY;
		static const char* const ANIMATION_FILE_EXTENSION;

		static std::string FormatAnimationPath( const std::string& animationName );
		static std::string FormatAnimationName( const std::string& animationName );

		UnitType( const HashString& name );
		virtual ~UnitType();

		void AddWeapon( const Weapon& weapon );
		int GetNumWeapons() const;
		Weapon& GetWeaponByIndex( int index );
		const Weapon& GetWeaponByIndex( int index ) const;

		int GetMovementRange() const;
		int GetMaxHP() const { return mMaxHP; }
		const IntRange& GetAttackRange() const { return mAttackRange; }
		int GetMovementCostAcrossTerrain( TerrainType* terrainType ) const;
		bool CanMoveAcrossTerrain( TerrainType* terrainType ) const;
		HashString GetAnimationSetName() const;
		MovementType* GetMovementType() const;

	protected:
		void LoadAnimation();

		int mMovementRange;
		int mMaxHP;
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


	inline Weapon& UnitType::GetWeaponByIndex( int index )
	{
		return mWeapons[ index ];
	}


	inline const Weapon& UnitType::GetWeaponByIndex( int index ) const
	{
		return mWeapons[ index ];
	}


	inline int UnitType::GetMovementRange() const
	{
		return mMovementRange;
	}


	inline int UnitType::GetMovementCostAcrossTerrain( TerrainType* terrainType ) const
	{
		MovementType* movementType = GetMovementType();
		assertion( movementType, "No MovementType \"%s\" found for UnitType \"%s\"!", mMovementTypeName.GetString().c_str(), GetName().GetString().c_str() );
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
