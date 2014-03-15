#pragma once

namespace mage
{
	class EditorState : public GameState
	{
	public:
		EditorState();
		virtual ~EditorState();

	private:
		virtual void OnEnter( const Dictionary& parameters );
		virtual void OnUpdate( float elapsedTime );
		virtual void OnDraw();
		virtual void OnExit();

		virtual void OnScreenSizeChanged( int32 width, int32 height );
		virtual bool OnPointerDown( float x, float y, size_t which );
		virtual bool OnPointerUp( float x, float y, size_t which );
		virtual bool OnPointerMotion( float x, float y, float dx, float dy, size_t which );
	};
}
