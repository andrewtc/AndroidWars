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
		typedef Delegate< const Map::Iterator& > OnTeleportDelegate;
		typedef Delegate< const Path& > OnMoveDelegate;
		typedef Delegate< int, Unit* > OnTakeDamageDelegate;
		typedef Delegate< Faction*, Faction* > OnOwnerChangedDelegate;

		static const int MAX_HEALTH = 10;

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
		bool CanEnterTile( const Map::ConstIterator& tile ) const;
		bool CanOccupyTile( const Map::ConstIterator& tile ) const;
		bool CanMoveAlongPath( const Path& path ) const;
		void GetValidPath( const Path& path, Path& result ) const;
		void Teleport( const Vec2s& tilePos );
		void Teleport( Map::Iterator tile );
		bool Move( const Path& path );
		bool IsOwnedBy( Faction* faction ) const;

		bool CanAttack( const Unit* target ) const;
		void Attack( Unit* target );
		int CalculateDamagePercentage( const Unit* target, int weaponIndex ) const;
		float GetDefenseBonus() const;
		bool CanTarget( const Unit* target ) const;
		bool IsInRange( const Unit* target ) const;
		bool IsInRangeFromTile( const Unit* target, const Map::ConstIterator& tile ) const;
		bool IsInRangeFromLocation( const Unit* target, const Vec2s& location ) const;
		int GetDistanceToUnit( const Unit* target ) const;
		bool CanFireWeapon( int weaponIndex ) const;
		int GetBestAvailableWeaponAgainst( const UnitType* unitType ) const;
		int GetBestAvailableWeaponAgainst( const Unit* target ) const;

		void SetHealth( int health );
		void AddHealth( int health );
		void ResetHealth();
		void TakeDamage( int damageAmount, Unit* instigator = nullptr );
		void Die();
		int GetHealth() const;
		bool IsDamaged() const;
		bool IsAlive() const;
		bool IsDead() const;
		float GetHealthScale() const;

		bool CanReinforce( Unit* target ) const;

		void SetAmmo( int ammo );
		void AddAmmo( int ammo );
		void ResetAmmo();
		void ConsumeAmmo( int ammo );
		int GetAmmo() const;
		bool HasAmmo() const;

		void SetSupplies( int supplies );
		void AddSupplies( int supplies );
		void ResetSupplies();
		void ConsumeSupplies( int supplies );
		int GetSupplies() const;
		bool HasSupplies() const;

		void SetActive( bool active );
		void Activate();
		void Deactivate();
		bool IsActive() const;
		bool IsInactive() const;

		Map* GetMap() const;
		int GetID() const;

		std::string ToString() const;

	private:
		Unit();
		virtual ~Unit();

		void Init( Map* map, size_t unitID, const Map::Iterator& tile );
		void Destroy();

		void OnTurnStart( int turnIndex );
		void OnTurnEnd( int turnIndex );

		void SetTile( Map::Iterator tile );
		void RemoveFromCurrentTile();

		bool mIsAlive;
		bool mIsActive;
		int mHealth;
		int mAmmo;
		int mSupplies;
		int mID;
		Map* mMap;
		UnitType* mUnitType;
		Faction* mOwner;
		Map::Iterator mTile;

	public:
		Event< Faction*, Faction* > OnOwnerChanged;
		Event< const Map::Iterator& > OnTileChanged;
		Event< const Map::Iterator& > OnTeleport;
		Event< const Path& > OnMove;
		Event< int, Unit* > OnTakeDamage;
		Event< int > OnHealthChanged;
		Event<> OnActivate;
		Event<> OnDeactivate;
		Event<> OnDeath;
		Event<> OnDestroyed;

		friend class Map;
		friend class Faction;
	};
}
