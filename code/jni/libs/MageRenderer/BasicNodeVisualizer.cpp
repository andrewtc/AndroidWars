#include "RendererLib.h"

using namespace mage;

//---------------------------------------
BasicNodeVisualizer::BasicNodeVisualizer( float radius )
{
	Radius = radius;
}
//---------------------------------------
BasicNodeVisualizer::~BasicNodeVisualizer()
{}
//---------------------------------------
void BasicNodeVisualizer::OnDraw( Node* node ) const
{
	PushMatrix();

	LoadTransform2D( node->WorldTransform );

	DrawCircle( 0, 0, Radius, DrawColor );
	DrawAxes( Radius );

	PopMatrix();

	Node* parnet = node->GetParent();
	if ( parnet && parnet->GetParent() )
	{
		DrawLine( node->WorldTransform.GetTranslation(), parnet->WorldTransform.GetTranslation(), 1.0f, DrawColor );
	}
}
//---------------------------------------