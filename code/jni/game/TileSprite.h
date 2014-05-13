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
		static const Color DEFAULT_COLOR;
		static const Color SELECTED_COLOR;

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

		void SetSelected( bool selected );
		void Select();
		void Deselect();
		bool IsSelected() const;

	private:
		void DestroySprite();

		void TileOwnerChanged( Faction* formerOwner, Faction* owner );

		void UpdateColor();

		static bool TileMatchesVariation( const Map::ConstIterator& tile, const Variation* variation );

		bool mIsSelected;
		MapView* mMapView;
		Sprite* mSprite;
		Map::Iterator mTile;
	};
}
