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

		Unit( const std::string& name = "Unit" );
		virtual ~Unit();

		void Init( Game* game );
		bool IsInitialized() const;

		virtual void OnLoadProperty( const std::string& name, const std::string& value );
		virtual void OnLoadFinished();
		virtual void OnDraw( const Camera& camera ) const;
		virtual void OnUpdate( float dt );

		void SaveToJSON( rapidjson::Document& document, rapidjson::Value& object );
		void LoadFromJSON( const rapidjson::Value& object );

		void SetUnitType( UnitType* unitType );
		UnitType* GetUnitType() const;
		void SetTilePos( int x, int y );
		void SetTilePos( const Vec2i& tilePos );
		Vec2i GetTilePos() const;

		void SetOwner( Player* player );
		Player* GetOwner() const;
		int GetMovementRange() const;
		int GetMovementCostAcrossTerrain( TerrainType* terrainType ) const;
		bool CanMoveAcrossTerrain( TerrainType* terrainType ) const;
		bool IsOwnedBy( Player* player ) const { return mOwner == player; }
		void Select();
		void Deselect();
		void SetDestination( const Vec2i& tilePos );

		bool CanAttack( const Unit& target ) const;
		void Attack( Unit& target );
		int CalculateDamagePercentage( const Unit& target, int weaponIndex ) const;
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
		bool IsAlive() const;
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
		int GetTotalHP() const;

		const char* ToString() const;

	protected:
		Game* mGame;
		std::string mDebugName;
		HashString mUnitTypeName;
		UnitType* mUnitType;
		Vec2i mTilePos;
		Sprite* mSprite;
		int mOwnerIndex;
		Player* mOwner;
		Color mSelectionColor;
		Color mDefaultColor;
		int mHP;
		Vec2f mDestination;
		int mAP;
		int mAmmo;
		bool mWasLoadedByMap;
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
		assertion( terrainType, "Cannot get movement cost because null TerrainType was specified!" );
		assertion( mUnitType, "No UnitType found for %s!", ToString() );
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


	inline bool Unit::IsAlive() const
	{
		return ( mHP > 0 );
	}


	inline bool Unit::IsDead() const
	{
		return !IsAlive();
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


	inline const char* Unit::ToString() const
	{
		return mDebugName.c_str();
	}
}
