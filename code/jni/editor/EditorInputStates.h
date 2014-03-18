#pragma once

namespace mage
{
	class BrushToolInputState : public DerivedInputState< EditorState >
	{
	public:
		void SetTileTemplate( const Tile& tile );

	private:
		BrushToolInputState( GameState* owner );
		~BrushToolInputState();

		virtual void OnEnter( const Dictionary& parameters );
		virtual void OnExit();

		virtual bool OnPointerDown( float x, float y, size_t which );
		virtual bool OnPointerMotion( float x, float y, float dx, float dy, size_t which );
		virtual bool OnPointerUp( float x, float y, size_t which );

		Tile mTileTemplate;

		friend class GameState;
	};


	class EraserToolInputState : public DerivedInputState< EditorState >
	{
	private:
		EraserToolInputState( GameState* owner );
		~EraserToolInputState();

		virtual void OnEnter( const Dictionary& parameters );
		virtual void OnExit();

		virtual bool OnPointerDown( float x, float y, size_t which );
		virtual bool OnPointerMotion( float x, float y, float dx, float dy, size_t which );
		virtual bool OnPointerUp( float x, float y, size_t which );

		Tile mTileTemplate;

		friend class GameState;
	};
}
