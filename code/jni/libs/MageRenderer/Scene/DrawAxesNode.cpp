#include "RendererLib.h"

using namespace mage;

MAGE_IMPLEMENT_RTTI( Node, DrawAxesNode );

//---------------------------------------
DrawAxesNode::DrawAxesNode( float length )
	: mLength( length )
{}
//---------------------------------------
void DrawAxesNode::OnDraw()
{
	DrawAxes( WorldTransform.GetMatrix(), mLength );
	Node::OnDraw();
}
//---------------------------------------