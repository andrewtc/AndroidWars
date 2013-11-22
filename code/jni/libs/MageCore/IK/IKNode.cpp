#include "CoreLib.h"

using namespace mage;

MAGE_IMPLEMENT_RTTI( Node, IKNode );

//---------------------------------------
IKNode::IKNode()
	: Node( "IKNode" )
{
	Attach( &Controller );
}
//---------------------------------------
IKNode::~IKNode()
{}
//---------------------------------------