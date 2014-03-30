#pragma once

namespace mage
{
	class BrushToolInputState;
	class EraserToolInputState;

	class EditorState : public GameState
	{
	public:
		EditorState();
		virtual ~EditorState();

		Map* GetMap();
		const Map* GetMap() const;

		World* GetWorld();
		const World* GetWorld() const;

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

		bool mIsPanningCamera;

		BrushToolInputState* mBrushToolInputState;
		EraserToolInputState* mEraserToolInputState;
		Widget* mToolPalette;
		ListLayout* mTilePalette;

		Camera mCamera;
		Scenario mScenario;
		Map mMap;
		World mWorld;
	};
}
