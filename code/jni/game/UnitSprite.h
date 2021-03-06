#pragma once

namespace mage
{
	class UnitSprite
	{
	public:
		static const char* const DEFAULT_ANIMATION_NAME;

		UnitSprite( MapView* mapView, Unit* unit );
		~UnitSprite();

		bool IsInitialized() const;

		void Update( float elapsedTime );
		void Draw( const Camera& camera );

		void SetPosition( const Vec2f& position );
		Vec2f GetPosition() const;

		void SetUnit( Unit* unit );
		Unit* GetUnit() const;

		Sprite* GetSprite() const;
		RectF GetWorldBounds() const;
		bool IsVisible() const;
		MapView* GetMapView() const;

	private:
		void Init();
		void Destroy();

		void OnOwnerColorChanged( const Color& color );
		void OnUnitOwnerChanged( Faction* owner, Faction* formerOwner );
		void OnUnitTileChanged( const Map::Iterator& tile );
		void OnUnitTakeDamage( int health, Unit* instigator );
		void OnUnitHealthChanged( int health );
		void OnUnitDestroyed();
		void OnUnitActivate();
		void OnUnitDeactivate();

		void UpdateColor();
		void UpdateTilePosition();

		bool mIsInitialized;
		bool mIsVisible;
		MapView* mMapView;
		Unit* mUnit;
		Sprite* mSprite;

		friend class MapView;
	};
}
