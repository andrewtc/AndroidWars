#include "RendererLib.h"

using namespace mage;

ArrayList< Color > IKVisualizer::BoneColors;

//---------------------------------------
IKVisualizer::IKVisualizer()
{
	if ( BoneColors.empty() )
	{
		BoneColors.push_back( Color::BLUE );
		BoneColors.push_back( Color::ORANGE );
		BoneColors.push_back( Color::CYAN );
		BoneColors.push_back( Color::PINK );
	}
}
//---------------------------------------
IKVisualizer::~IKVisualizer()
{}
//---------------------------------------
void IKVisualizer::OnDraw( Node* node ) const
{
	// Only draw IKNodes
	if ( !node->IsExactly( IKNode::TYPE ) )
		return;

	IKNode* iknode = (IKNode*) node;
	Vec2f target = iknode->Controller.Chain.Target->WorldTransform.GetTranslation(); //Position;
	uint32 numJoints = iknode->Controller.Chain.GetNumJoints();
	float a = 0;
	Vec2f P0, P1, LastP;

	for ( uint32 i = 0; i < numJoints; ++i )
	{
		const IKJoint& j = *iknode->Controller.Chain.GetJointAt( i );

		a += j.Angle;

		float c = std::cosf( a );
		float s = std::sinf( a );

		Color C = BoneColors[ i % BoneColors.size() ];

		if ( i > 0 )
		{
			P0 = LastP;
		}
		else
		{
			if ( iknode->Controller.Chain.Base )
				P0 = iknode->Controller.Chain.Base->WorldTransform.GetTranslation();
			else
				P0 = Vec2f::ZERO;
		}

		P1 = P0 + Vec2f( c * j.Length, s * j.Length );
		LastP = P1;

		//ToWorldSpace( P0 );
		//ToWorldSpace( P1 );

		DrawLine( P0 , P1, 2.0f, C );
	}

	//ToWorldSpace( target );
	//DrawCircle( target.x, target.y, 5.0f, Color::GREY );
}
//---------------------------------------