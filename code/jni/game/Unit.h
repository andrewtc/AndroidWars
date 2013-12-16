#pragma once

namespace mage
{
	/**
	 * Represents a Unit on the game board.
	 */
	class Unit : public MapObject
	{
		DECLARE_RTTI;
	public:
		static const int MAX_HP = 10;

		Unit( const std::string& name );
		Unit( UnitType* unitType, Player* owner );
		virtual ~Unit();

		void Init();

		virtual void OnLoadProperty( const std::string& name, const std::string& value );
		virtual void OnLoadFinished();
		virtual void OnDraw( const Camera& camera ) const;
		virtual void OnUpdate( float dt );

		UnitType* GetUnitType() const;
		void SetTilePos( int x, int y );
		void SetTilePos( const Vec2i& tilePos );
		Vec2i GetTilePos() const;

		Player* GetOwner() const;
		int GetMovementRange() const;
		int GetMovementCostAcrossTerrain( TerrainType* terrainType ) const;
		bool CanMoveAcrossTerrain( TerrainType* terrainType ) const;
		bool IsOwnedBy( Player* player ) const { return mOwner == player; }
		void Select();
		void Deselect();
		bool IsAlive() const { return mHP != 0; }
		void SetDestination( const Vec2i& tilePos );

		bool CanAttack( const Unit& target ) const;
		void Attack( Unit& target );
		int CalculateDamageAgainst( const Unit& target, int weaponIndex, bool calculateWithRandomness ) const;
		float GetDefenseBonus() const;
		bool CanTarget( const Unit& target ) const;
		bool IsInRange( const Unit& target ) const;
		int GetDistanceToUnit( const Unit& target ) const;
		bool CanFireWeapon( int weaponIndex ) const;
		int GetBestAvailableWeaponAgainst( const UnitType* unitType ) const;
		int GetBestAvailableWeaponAgainst( const Unit& target ) const;

		void SetHP( int hp );
		int GetHP() const;
		void TakeDamage( int damageAmount, Unit* instigator = nullptr );
		bool IsDead() const;
		float GetHealthScale() const;
		void OnDestroyed();

		void SetAmmo( int ammo );
		void ConsumeAmmo( int ammo );
		void ResetAmmo();
		int GetAmmo() const;
		bool HasAmmo() const;

		void ConsumeAP( int ap );
		void ResetAP();
		int GetRemainingAP() const { return mAP; }
		int GetTotalAP() const { return 2; }

	protected:
		UnitType* mUnitType;
		Vec2i mTilePos;
		Sprite* mSprite;
		Player* mOwner;
		Color mSelectionColor;
		Color mDefaultColor;
		int mHP;
		Vec2f mDestination;
		int mAP;
		int mAmmo;
	};


	inline UnitType* Unit::GetUnitType() const
	{
		return mUnitType;
	}


	inline void Unit::SetTilePos( int x, int y )
	{
		SetTilePos( Vec2i( x, y ) );
	}


	inline Vec2i Unit::GetTilePos() const
	{
		return mTilePos;
	}


	inline int Unit::GetMovementCostAcrossTerrain( TerrainType* terrainType ) const
	{
		assertion( mUnitType, "No UnitType found for Unit \"%s\"!", GetName().c_str() );
		return mUnitType->GetMovementCostAcrossTerrain( terrainType );
	}


	inline bool Unit::CanMoveAcrossTerrain( TerrainType* terrainType ) const
	{
		return ( GetMovementCostAcrossTerrain( terrainType ) > -1 );
	}


	inline int Unit::GetBestAvailableWeaponAgainst( const Unit& target ) const
	{
		return GetBestAvailableWeaponAgainst( target.GetUnitType() );
	}


	inline int Unit::GetHP() const
	{
		return mHP;
	}


	inline bool Unit::IsDead() const
	{
		return ( mHP <= 0 );
	}


	inline float Unit::GetHealthScale() const
	{
		return ( (float) mHP / MAX_HP );
	}


	inline void Unit::SetAmmo( int ammo )
	{
		mAmmo = Mathi::Clamp( ammo, 0, mUnitType->GetMaxAmmo() );
	}


	inline void Unit::ConsumeAmmo( int ammo )
	{
		SetAmmo( mAmmo - ammo );
	}


	inline void Unit::ResetAmmo()
	{
		mAmmo = mUnitType->GetMaxAmmo();
	}


	inline int Unit::GetAmmo() const
	{
		return mAmmo;
	}


	inline bool Unit::HasAmmo() const
	{
		return ( mAmmo > 0 );
	}
}
