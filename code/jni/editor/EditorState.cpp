#include "androidwars.h"

using namespace mage;


EditorState::EditorState()
{ }


EditorState::~EditorState()
{ }


void EditorState::OnEnter( const Dictionary& parameters )
{
}


void EditorState::OnUpdate( float elapsedTime )
{
	GameState::OnUpdate( elapsedTime );
}


void EditorState::OnDraw()
{
	GameState::OnDraw();
}


void EditorState::OnExit()
{

}


void EditorState::OnScreenSizeChanged( int32 width, int32 height )
{
	GameState::OnScreenSizeChanged( width, height );
}


bool EditorState::OnPointerDown( float x, float y, size_t which )
{
	return GameState::OnPointerDown( x, y, which );
}


bool EditorState::OnPointerUp( float x, float y, size_t which )
{
	return GameState::OnPointerUp( x, y, which );
}


bool EditorState::OnPointerMotion( float x, float y, float dx, float dy, size_t which )
{
	return GameState::OnPointerMotion( x, y, dx, dy, which );
}
