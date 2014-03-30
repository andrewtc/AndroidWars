#pragma once

namespace mage
{
	class World
	{
	public:
		static const float TILE_WORLD_SCALE;
		static const float INVERSE_TILE_WORLD_SCALE;

		static const float MAP_BORDER_SCALE;

		World();
		~World();

		void Init( Map* map );
		void Destroy();

		void Update( float elapsedTime );
		void Draw();

		Map* GetMap() const;
		RectF GetMapBounds() const;
		RectF GetCameraBounds() const;

		Camera* GetCamera();
		const Camera* GetCamera() const;
		void CenterCamera();

		Vec2f WorldToScreenCoords( const Vec2f& worldCoords ) const;
		Vec2f WorldToScreenCoords( float worldX, float worldY ) const;
		Vec2f ScreenToWorldCoords( const Vec2f& screenCoords ) const;
		Vec2f ScreenToWorldCoords( float screenX, float screenY ) const;

		Vec2s WorldToTileCoords( const Vec2f& worldCoords ) const;
		Vec2s WorldToTileCoords( float worldX, float worldY ) const;
		Vec2f TileToWorldCoords( const Vec2s& tileCoords ) const;
		Vec2f TileToWorldCoords( short tileX, short tileY ) const;

		bool IsInitialized() const;

	private:
		typedef Grid< TileSprite, MAP_SIZE_POWER_OF_TWO > TileSpritesGrid;

		void MapResized( const Vec2s& oldSize, const Vec2s& newSize );
		void TileChanged( const Map::Iterator& tile );

		Map* mMap;
		Camera mCamera;
		TileSpritesGrid mTileSprites;
	};
}
