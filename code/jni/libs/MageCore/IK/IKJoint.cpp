#include "CoreLib.h"

using namespace mage;

//---------------------------------------
MAGE_IMPLEMENT_RTTI( Object, IKJoint );
MAGE_IMPLEMENT_RTTI( Node, IKTarget );
//---------------------------------------


//---------------------------------------
IKTarget::IKTarget()
	: Node( "IKTarget" )
{}
//---------------------------------------
IKTarget::~IKTarget()
{}
//---------------------------------------


//---------------------------------------
IKJoint::IKJoint()
	: Object( "IKJoint" )
{}
//---------------------------------------
IKJoint::IKJoint( float angle, float length, float weight )
	: Object( "IKJoint" )
	, Angle( angle )
	, Length( length )
	, Weight( weight )
{}
//---------------------------------------
IKJoint::~IKJoint()
{}
//---------------------------------------