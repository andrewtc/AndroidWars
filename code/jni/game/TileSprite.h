#pragma once

namespace mage
{
	class World;

	/**
	 * The visual representation of a Tile on the Map.
	 */
	class TileSprite
	{
	public:
		static HashString ChooseTileVariation( const Map::ConstIterator& tile );

		TileSprite();
		~TileSprite();

		void Init( World* world, const Vec2s& tilePos );

		void Update( float elapsedTime );
		void Draw();

		void UpdateSprite();

		Map::Iterator GetTile() const;

	private:
		void DestroySprite();

		World* mWorld;
		Sprite* mSprite;
		Map::Iterator mTile;
	};
}
