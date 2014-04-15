#pragma once

namespace mage
{
	class BrushToolInputState;
	class PlaceToolInputState;
	class EraserToolInputState;

	class EditorState : public GameState
	{
	public:
		EditorState();
		virtual ~EditorState();

		Map* GetMap();
		const Map* GetMap() const;

		MapView* GetMapView();
		const MapView* GetMapView() const;

		Tile CreateTileTemplate( TerrainType* terrainType );
		Tile CreateDefaultTileTemplate();
		void PaintTileAt( float x, float y, const Tile& tile );

		ListLayout* GetTilePalette() const;

	private:
		virtual void OnEnter( const Dictionary& parameters );
		virtual void OnUpdate( float elapsedTime );
		virtual void OnDraw();
		virtual void OnExit();

		virtual void OnScreenSizeChanged( int32 width, int32 height );
		virtual bool OnPointerDown( const Pointer& pointer );
		virtual bool OnPointerUp( const Pointer& pointer );
		virtual bool OnPointerMotion( const Pointer& activePointer, const PointersByID& pointersByID );

		void BuildTilePalette();
		void BuildUnitPalette();

		bool mIsPanningCamera;

		BrushToolInputState* mBrushToolInputState;
		PlaceToolInputState* mPlaceToolInputState;
		EraserToolInputState* mEraserToolInputState;
		Widget* mToolPalette;
		ListLayout* mTilePalette;
		ListLayout* mUnitPalette;

		//Camera mCamera;
		Scenario mScenario;
		Map mMap;
		MapView mMapView;
	};
}
