#include "RendererLib.h"

using namespace mage;

MAGE_IMPLEMENT_RTTI( Node, SpriteNode );

//---------------------------------------
SpriteNode::SpriteNode()
	: Node( "SpriteNode" )
	, mSprite( 0 )
	, mUV( 0, 0, 1, 1 )
{}
//---------------------------------------
SpriteNode::SpriteNode( Texture2D* Sprite )
	: Node( "SpriteNode" )
	, mSprite( Sprite )
	, mUV( 0, 0, 1, 1 )
{}
//---------------------------------------
SpriteNode::SpriteNode( Texture2D* Sprite, const RectF& UV )
	: Node( "SpriteNode" )
	, mSprite( Sprite )
	, mUV( UV )
{}
//---------------------------------------
SpriteNode::~SpriteNode()
{
	mSprite = 0;
}
//---------------------------------------
void SpriteNode::OnDraw()
{
	PushMatrix();
	LoadTransform2D( WorldTransform );
	DrawRect( mSprite, 0, 0, mUV );
	PopMatrix();
}
//---------------------------------------