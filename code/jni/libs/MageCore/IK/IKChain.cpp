#include "CoreLib.h"

using namespace mage;

//---------------------------------------
IKChain::IKChain()
{}
//---------------------------------------
IKChain::~IKChain()
{}
//---------------------------------------
void IKChain::OnUpdate( uint32 iterations )
{
	ArrayList< WorldSpaceBone > worldBones;
	const Vec2f& target = Target->WorldTransform.GetTranslation();//Target->Position;

	LocalToWorld( worldBones, mChain );
	for ( uint32 i = 0; i < iterations; ++i )
	{
		ConstraintRelaxation( worldBones, target );
	}
	WorldToLocal( worldBones, mChain, target );
}
//---------------------------------------
void IKChain::InsertJointAt( uint32 i, IKJoint* joint )
{
	if ( i >= mChain.size() )
	{
		mChain.push_back( joint );
	}
	else
	{
		mChain.insert_at( i, joint );
	}
}
//---------------------------------------
void IKChain::InsertJointTail( IKJoint* joint )
{
	mChain.push_back( joint );
}
//---------------------------------------
IKJoint* IKChain::GetJointAt( uint32 i ) const
{
	IKJoint* j = 0;
	if ( i < mChain.size() )
	{
		j = mChain[i];	
	}
	return j;
}
//---------------------------------------
void IKChain::RemoveJointTail()
{
	if ( !mChain.empty() )
	{
		delete mChain.back();
		mChain.pop_back();
	}
}
//---------------------------------------
void IKChain::LocalToWorld( ArrayList< WorldSpaceBone >& worldBones, const ArrayList< IKJoint* >& localBones )
{
	size_t numBones = localBones.size();

	if ( numBones == 0 )
	{
		WarnInfo( "IK_WorldToLocal : numBones == 0" );
		return;
	}

	WorldSpaceBone root( Vec2f::ZERO, localBones[0]->Length, localBones[0]->Weight );
	worldBones.push_back( root );

	float prevAngle = localBones[0]->Angle;
	float prevAngleC = std::cosf( prevAngle );
	float prevAngleS = std::sinf( prevAngle );

	for ( size_t i = 1; i < numBones; ++i )
	{
		const WorldSpaceBone& prevWorld = worldBones[i-1];
		const IKJoint& prevLocal        = *localBones[i-1];

		WorldSpaceBone nextWorld(
			prevWorld.Pos + Vec2f( prevAngleC * prevLocal.Length, prevAngleS * prevLocal.Length ),
			localBones[i]->Length,
			localBones[i]->Weight
		);

		worldBones.push_back( nextWorld );

		prevAngle += localBones[i]->Angle;
		prevAngleC = std::cosf( prevAngle );
		prevAngleS = std::sinf( prevAngle );
	}
}
//---------------------------------------
void IKChain::WorldToLocal( const ArrayList< WorldSpaceBone >& worldBones, ArrayList< IKJoint* >& localBones, const Vec2f& target )
{
	if ( worldBones.size() != localBones.size() )
	{
		WarnInfo( "IK_WorldToLocal : worldBones.size() != localBones.size()" );
		return;
	}

	size_t numBones = localBones.size();

	if ( numBones == 0 )
	{
		WarnInfo( "IK_WorldToLocal : numBones == 0" );
		return;
	}

	float prevWorldAngle = 0;
	for ( size_t i = 0; i < numBones - 1; ++i )
	{
		Vec2f nextPos = worldBones[i+1].Pos - worldBones[i].Pos;
		float worldAngle = nextPos.Angle();
		float nextAngle = WrapAngle( worldAngle - prevWorldAngle );

		localBones[i]->Angle = nextAngle;
		prevWorldAngle = worldAngle;
	}

	// Orient the end bone towards the IK target
	int i = numBones - 1;
	Vec2f nextPos = target - worldBones[i].Pos;
	float worldAngle = nextPos.Angle();
	float nextAngle = WrapAngle( worldAngle - prevWorldAngle );

	localBones[i]->Angle = nextAngle;
}
//---------------------------------------
void IKChain::ConstraintRelaxation( ArrayList< WorldSpaceBone >& worldBones, const Vec2f& taget )
{
	const float epsilion = 0.0001f;
	size_t numBones = worldBones.size();

	if ( numBones == 0 )
		return;

	// Constrain end bone to target
	int i = numBones - 1;
	Vec2f toTarget = taget - worldBones[i].Pos;
	float toTargetLenSqr = toTarget.LengthSqr();
	if ( toTargetLenSqr > epsilion )
	{
		float toTargetLen = std::sqrtf( toTargetLenSqr );
		float toTargetScale = worldBones[i].Length / toTargetLen - 1.0f;
		worldBones[i].Pos -= toTargetScale * toTarget;
	}

	// Relax the chain
	for ( i = numBones - 2; i >= 1; --i )
	{
		WorldSpaceBone& parent = worldBones[i];
		WorldSpaceBone& child  = worldBones[i+1];

		Vec2f toChild = child.Pos - parent.Pos;
		
		float toChildLenSqr = toChild.LengthSqr();
		float totalWeight   = parent.Weight + child.Weight;
		if ( toTargetLenSqr > epsilion && totalWeight > epsilion )
		{
			float toChildLen   = std::sqrtf( toChildLenSqr );
			float toChildScale = ( parent.Length / toChildLen - 1.0f ) / totalWeight;
			float parentScale  = toChildScale * parent.Weight;
			float childScale   = toChildScale * child.Weight;

			parent.Pos -= parentScale * toChild;
			child.Pos  += childScale  * toChild;
		}
	}

	// Constrain first child to the root
	if ( numBones > 1 )
	{
		Vec2f toChild = worldBones[1].Pos - worldBones[0].Pos;
		float toChildLenSqr = toChild.LengthSqr();
		if ( toChildLenSqr > epsilion )
		{
			float toChildLen = std::sqrtf( toChildLenSqr );
			float childScale = worldBones[0].Length / toChildLen - 1.0f;
			worldBones[1].Pos += childScale * toChild;
		}
	}
}
//---------------------------------------
float IKChain::WrapAngle( float angle )
{
	angle = std::fmodf( angle, Mathf::TWO_PI );
	if ( angle < -Mathf::PI )
	{
		angle += Mathf::TWO_PI;
	}
	else if ( angle > Mathf::PI )
	{
		angle -= Mathf::TWO_PI;
	}
	return angle;
}
//---------------------------------------