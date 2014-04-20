#pragma once

namespace mage
{
	/**
	 * Represents a Unit on the game board.
	 */
	class Unit
	{
		DECLARE_RTTI;

	public:
		typedef Delegate< const Map::Iterator& > OnTileChangedDelegate;

		static const int MAX_HEALTH = 10;

		Unit();
		virtual ~Unit();

		bool IsInitialized() const;

		void SaveToJSON( rapidjson::Document& document, rapidjson::Value& object );
		void LoadFromJSON( const rapidjson::Value& object );

		void SetUnitType( UnitType* unitType );
		UnitType* GetUnitType() const;

		Map::Iterator GetTile() const;
		Vec2s GetTilePos() const;
		short GetTileX() const;
		short GetTileY() const;

		void SetOwner( Faction* faction );
		Faction* GetOwner() const;

		MovementType* GetMovementType() const;
		int GetMovementRange() const;
		int GetMovementCostAcrossTerrain( TerrainType* terrainType ) const;
		int CalculatePathCost( const Path& path ) const;
		bool CanMoveAcrossTerrain( TerrainType* terrainType ) const;
		void Teleport( const Vec2s& tilePos );
		void Teleport( Map::Iterator tile );
		void Move( const Path& path );
		bool IsOwnedBy( Faction* faction ) const;

		bool CanAttack( const Unit& target ) const;
		void Attack( Unit& target );
		int CalculateDamagePercentage( const Unit& target, int weaponIndex ) const;
		float GetDefenseBonus() const;
		bool CanTarget( const Unit& target ) const;
		bool IsInRange( const Unit& target ) const;
		bool IsInRangeFromTile( const Unit& target, const Map::ConstIterator& tile ) const;
		int GetDistanceToUnit( const Unit& target ) const;
		bool CanFireWeapon( int weaponIndex ) const;
		int GetBestAvailableWeaponAgainst( const UnitType* unitType ) const;
		int GetBestAvailableWeaponAgainst( const Unit& target ) const;

		void SetHealth( int health );
		void ResetHealth();
		void TakeDamage( int damageAmount, Unit* instigator = nullptr );
		void Die();
		int GetHealth() const;
		bool IsAlive() const;
		bool IsDead() const;
		float GetHealthScale() const;

		void SetAmmo( int ammo );
		void ResetAmmo();
		void ConsumeAmmo( int ammo );
		int GetAmmo() const;
		bool HasAmmo() const;

		void SetSupplies( int supplies );
		void ResetSupplies();
		void ConsumeSupplies( int supplies );
		int GetSupplies() const;
		bool HasSupplies() const;

		void SetActive( bool active );
		void Activate();
		void Deactivate();
		bool IsActive() const;

		Map* GetMap() const;

		std::string ToString() const;

	private:
		void Init( Map* map, const Map::Iterator& tile );
		void Destroy();

		void OnTurnStart( int turnIndex );
		void OnTurnEnd( int turnIndex );

		void SetTile( Map::Iterator tile );

		bool mIsAlive;
		bool mIsActive;
		int mHealth;
		int mAmmo;
		int mSupplies;
		Map* mMap;
		UnitType* mUnitType;
		Faction* mOwner;
		Map::Iterator mTile;

	public:
		Event< const Map::Iterator& > OnTileChanged;
		Event< const Map::Iterator& > OnTeleport;
		Event< const Path& > OnMove;
		Event< int, Unit* > OnTakeDamage;
		Event<> OnDeath;
		Event<> OnDestroyed;

		friend class Map;
		friend class Faction;
	};
}
