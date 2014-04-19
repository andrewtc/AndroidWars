#pragma once

namespace mage
{
	class MapView
	{
	public:
		static const float TILE_WORLD_SCALE;
		static const float INVERSE_TILE_WORLD_SCALE;

		static const float MAP_BORDER_SCALE;

		typedef Grid< TileSprite, MAP_SIZE_POWER_OF_TWO > TileSpritesGrid;
		typedef std::vector< UnitSprite* > UnitSprites;

		MapView();
		~MapView();

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

		void SetDefaultFont( BitmapFont* font );
		BitmapFont* GetDefaultFont() const;

		TileSpritesGrid& GetTileSprites();
		const TileSpritesGrid& GetTileSprites() const;

		UnitSprite* CreateUnitSprite( Unit* unit );
		UnitSprite* GetUnitSpriteAtScreenCoords( float screenX, float screenY ) const;
		UnitSprite* GetUnitSpriteAtScreenCoords( const Vec2f& screenCoords ) const;
		UnitSprite* GetUnitSpriteAtWorldCoords( float worldX, float worldY ) const;
		UnitSprite* GetUnitSpriteAtWorldCoords( const Vec2f& worldCoords ) const;
		UnitSprite* GetUnitSpriteAtTileCoords( short tileX, short tileY ) const;
		UnitSprite* GetUnitSpriteAtTileCoords( const Vec2s& tilePos ) const;

		void SelectUnitSprite( UnitSprite* unitSprite );
		void DeselectUnitSprite();

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
		void MapResized( const Vec2s& oldSize, const Vec2s& newSize );
		void TileChanged( const Map::Iterator& tile );
		void UnitSpriteSelected( UnitSprite* unitSprite );

		void SelectAllReachableTilesForUnit( Unit* unit );
		void DeselectAllTiles();

		Map* mMap;
		UnitSprite* mSelectedUnitSprite;
		BitmapFont* mDefaultFont;
		Camera mCamera;
		ArrowSprite mArrowSprite;
		UnitSprites mUnitSprites;
		TileSpritesGrid mTileSprites;
	};
}
