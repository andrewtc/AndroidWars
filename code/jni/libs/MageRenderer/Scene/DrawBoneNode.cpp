#include "RendererLib.h"

using namespace mage;

MAGE_IMPLEMENT_RTTI( Node, DrawBoneNode );

//---------------------------------------
DrawBoneNode::DrawBoneNode( float radius )
	: Node( "DrawBoneNode" )
	, mRadius( radius )
	, DrawColor( Color::WHITE )
	, DebugKeyframes( true )
{
	Attach( &KeyframeCont );
	OriginalTransform = WorldTransform;
}
//---------------------------------------
DrawBoneNode::~DrawBoneNode()
{}
//---------------------------------------
void DrawBoneNode::OnDraw()
{
	Node::OnDraw();

	PushMatrix();

	LoadTransform2D( WorldTransform );

	DrawCircle( 0, 0, mRadius, DrawColor );
	DrawAxes( mRadius );

	PopMatrix();

	Node* parnet = GetParent();
	if ( parnet )
	{
		DrawLine( WorldTransform.GetTranslation(), parnet->WorldTransform.GetTranslation(), 1.0f, DrawColor );
	}

	if ( DebugKeyframes )
	{
		PushMatrix();
		Transform2D tm = OriginalTransform;
		Transform2D lastTM = tm;
		for ( int i = 0; i < KeyframeCont.PositionKeys.size(); ++i )
		{
			tm.SetTranslation( KeyframeCont.PositionKeys[i].Position );
			LoadTransform2D( tm );
			DrawPoint( 0, 0, 2.0f, Color::CYAN );
		}

		for ( int i = 0; i < KeyframeCont.PositionRotationKeys.size(); ++i )
		{
			tm.SetRotation( KeyframeCont.PositionRotationKeys[i].Rotation );
			//tm.SetTranslation( LocalToWorld( KeyframeCont.PositionRotationKeys[i].Position ) );
			tm.SetTranslation( KeyframeCont.PositionRotationKeys[i].Position );

			LoadTransform2D( tm );
			DrawAxes( 20 );
			if ( i > 0 )
			{
				LoadIdentity();
				DrawLine( tm.GetTranslation(), lastTM.GetTranslation(), 1.0f, DrawColor );
			}
			lastTM = tm;
		}
		PopMatrix();
	}
}
//---------------------------------------