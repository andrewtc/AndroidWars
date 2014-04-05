#pragma once

namespace mage
{
	class MapView;

	/**
	 * The visual representation of a Tile on the Map.
	 */
	class TileSprite
	{
	public:
		static HashString ChooseTileVariation( const Map::ConstIterator& tile );

		TileSprite();
		~TileSprite();

		void Init( MapView* mapView, const Vec2s& tilePos );

		void Update( float elapsedTime );
		void Draw();

		void UpdateSprite();

		Map::Iterator GetTile() const;
		Vec2s GetTilePos() const;
		short GetTileX() const;
		short GetTileY() const;

	private:
		void DestroySprite();

		static bool TileMatchesVariation( const Map::ConstIterator& tile, const Variation* variation );

		MapView* mMapView;
		Sprite* mSprite;
		Map::Iterator mTile;
	};
}
