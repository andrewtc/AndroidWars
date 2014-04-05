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

		Vec2f GetPosition() const;

	private:
		bool mIsInitialized;
		MapView* mMapView;
		Unit* mUnit;
		Sprite* mSprite;
	};
}
