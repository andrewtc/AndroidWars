#include "RendererLib.h"

using namespace mage;

//---------------------------------------
PointMass::PointMass( const Vec3f& position, float invMass )
	: Position( position )
	, InverseMass( invMass )
	, mDamping( 0.98f )
{}
//---------------------------------------
PointMass::~PointMass()
{}
//---------------------------------------
void PointMass::ApplyForce( const Vec3f& force )
{
	mAcceleration += force * InverseMass;
}
//---------------------------------------
void PointMass::IncreaseDamping( float factor )
{
	mDamping *= factor;
}
//---------------------------------------
void PointMass::OnUpdate( float dt )
{
	Velocity += mAcceleration;
	Position += Velocity;
	mAcceleration = Vec3f::ZERO;
	if ( Velocity.LengthSqr() < Mathf::EPSILON )
		Velocity = Vec3f::ZERO;
	Velocity *= mDamping;
	mDamping = 0.98f;
}
//---------------------------------------