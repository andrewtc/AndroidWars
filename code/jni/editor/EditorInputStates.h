#pragma once

namespace mage
{
	class PaintTilesInputState : public DerivedInputState< EditorState >
	{
		PaintTilesInputState( GameState* owner );
		~PaintTilesInputState();

		virtual void OnEnter( const Dictionary& parameters );
		virtual void OnExit();

		virtual bool OnPointerDown( float x, float y, size_t which );
		virtual bool OnPointerMotion( float x, float y, float dx, float dy, size_t which );
		virtual bool OnPointerUp( float x, float y, size_t which );

		void SetSelectedTerrainType( TerrainType* terrainType );

		void PaintTile( float x, float y );

		TerrainType* mSelectedTerrainType;

		friend class GameState;
	};
}
