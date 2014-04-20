#pragma once

namespace mage
{
	class UnitSprite
	{
	public:
		UnitSprite( MapView* mapView, Unit* unit );
		~UnitSprite();

		void Init();
		void Destroy();
		bool IsInitialized() const;

		void Update( float elapsedTime );
		void Draw( const Camera& camera );

		void StartMoveAnimation( const Path& path );
		void CancelMoveAnimation();

		void SetPosition( const Vec2f& position );
		Vec2f GetPosition() const;

		void SetUnit( Unit* unit );
		Unit* GetUnit() const;

		Sprite* GetSprite() const;
		RectF GetWorldBounds() const;

	private:
		void OnUnitTeleport( const Map::Iterator& tile );
		void OnUnitMove( const Path& path );

		bool mIsInitialized;
		bool mIsMoving;
		float mMoveAnimationTimer;
		Path mMovementPath;
		MapView* mMapView;
		Unit* mUnit;
		Sprite* mSprite;
	};
}
