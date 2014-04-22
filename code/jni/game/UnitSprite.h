#pragma once

namespace mage
{
	class UnitSprite
	{
	public:
		static const float MOVE_ANIMATION_SPEED;

		UnitSprite( MapView* mapView, Unit* unit );
		~UnitSprite();

		void Init();
		void Destroy();
		bool IsInitialized() const;

		void Update( float elapsedTime );
		void Draw( const Camera& camera );

		void SetPosition( const Vec2f& position );
		Vec2f GetPosition() const;

		void SetUnit( Unit* unit );
		Unit* GetUnit() const;

		Sprite* GetSprite() const;
		RectF GetWorldBounds() const;

	private:
		void OnOwnerColorChanged( const Color& color );
		void OnUnitOwnerChanged( Faction* owner, Faction* formerOwner );
		void OnUnitTeleport( const Map::Iterator& tile );
		void OnUnitMove( const Path& path );
		void OnUnitActivate();
		void OnUnitDeactivate();

		void UpdateColor();

		bool mIsInitialized;
		bool mIsMoving;
		float mMoveAnimationTimer;
		float mMoveAnimationSpeed;
		Vec2f mMoveAnimationOrigin;
		Path mMovementPath;
		MapView* mMapView;
		Unit* mUnit;
		Sprite* mSprite;
	};
}
