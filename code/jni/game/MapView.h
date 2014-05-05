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
		typedef std::deque< MapAnimation* > MapAnimations;

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

		UnitSprite* GetUnitSpriteForUnit( Unit* unit ) const;
		UnitSprite* GetUnitSpriteAtScreenCoords( float screenX, float screenY ) const;
		UnitSprite* GetUnitSpriteAtScreenCoords( const Vec2f& screenCoords ) const;
		UnitSprite* GetUnitSpriteAtWorldCoords( float worldX, float worldY ) const;
		UnitSprite* GetUnitSpriteAtWorldCoords( const Vec2f& worldCoords ) const;
		UnitSprite* GetUnitSpriteAtTileCoords( short tileX, short tileY ) const;
		UnitSprite* GetUnitSpriteAtTileCoords( const Vec2s& tilePos ) const;

		void SelectUnitSprite( UnitSprite* unitSprite, bool showArrow = true );
		void DeselectUnitSprite();
		UnitSprite* GetSelectedUnitSprite() const;
		bool HasSelectedUnitSprite() const;

		void TargetUnitSprite( UnitSprite* unitSprite );
		void UntargetUnitSprite();
		UnitSprite* GetTargetedUnitSprite() const;
		bool HasTargetedUnitSprite() const;

		const ArrowSprite& GetArrowSprite() const;
		Path& GetSelectedUnitPath();
		const Path& GetSelectedUnitPath() const;
		void DetermineAvailableActionsForSelectedUnit();
		const Actions& GetAvailableActionsForSelectedUnit() const;

		template< class MapAnimationSubclass, typename... ParameterTypes >
		MapAnimationSubclass* ScheduleMapAnimation( ParameterTypes... parameters );
		MapAnimation* ScheduleAnimationForAction( Ability::Action* action );
		MapAnimation* GetCurrentMapAnimation() const;
		bool IsPlayingMapAnimation() const;

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
		UnitSprite* CreateUnitSprite( Unit* unit );
		void DestroyUnitSprite( UnitSprite* unitSprite );

		void MapResized( const Vec2s& oldSize, const Vec2s& newSize );
		void UnitCreated( Unit* unit );
		void TileChanged( const Map::Iterator& tile );
		void UnitSpriteSelected( UnitSprite* unitSprite, bool showArrow );

		void SelectAllReachableTilesForUnit( Unit* unit );
		void DeselectAllTiles();

		Map* mMap;
		UnitSprite* mSelectedUnitSprite;
		UnitSprite* mTargetedUnitSprite;
		BitmapFont* mDefaultFont;
		MapAnimation* mCurrentMapAnimation;
		Camera mCamera;
		Actions mSelectedUnitActions;
		TargetSprite mTargetSprite;
		ArrowSprite mArrowSprite;
		UnitSprites mUnitSprites;
		MapAnimations mScheduledMapAnimations;
		TileSpritesGrid mTileSprites;

		friend class UnitSprite;
	};


	template< class MapAnimationSubclass, typename... ParameterTypes >
	MapAnimationSubclass* MapView::ScheduleMapAnimation( ParameterTypes... parameters )
	{
		// Create a new MapAnimation of the specified type.
		MapAnimationSubclass* derivedMapAnimation = new MapAnimationSubclass( parameters... );

		// Cast the MapAnimation to the base type.
		MapAnimation* mapAnimation = dynamic_cast< MapAnimation* >( derivedMapAnimation );
		assertion( mapAnimation != nullptr, "Could not schedule MapAnimation because the animation type specified is not a subclass of MapAnimation!" );

		// Add the animation to the animation queue.
		mScheduledMapAnimations.push_back( mapAnimation );

		// Return the newly scheduled MapAnimation.
		return derivedMapAnimation;
	}
}
