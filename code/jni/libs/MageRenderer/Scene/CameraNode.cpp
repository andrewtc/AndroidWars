#include "RendererLib.h"

using namespace mage;

MAGE_IMPLEMENT_RTTI( Node, CameraNode );

//---------------------------------------
CameraNode::CameraNode( Camera* camera )
	: Node( "CameraNode" )
	, mCamera( camera )
{}
//---------------------------------------
CameraNode::~CameraNode()
{}
//---------------------------------------
void CameraNode::OnUpdate( float currentTime )
{
	Node::OnUpdate( currentTime );

	LocalTransform.SetTranslation( -mCamera->GetPosition() );
}
//---------------------------------------