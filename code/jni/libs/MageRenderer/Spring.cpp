#include "RendererLib.h"

using namespace mage;

//---------------------------------------
Spring::Spring( PointMass* p1, PointMass* p2, float k, float damping )
	: P1( p1 )
	, P2( p2 )
	, K( k )
	, Damping( damping )
{
	Length = ( p2->Position - p1->Position ).Length() * 0.95f;
}
//---------------------------------------
Spring::~Spring()
{}
//---------------------------------------
void Spring::OnUpdate( float dt )
{
	Vec3f x = P1->Position - P2->Position;
	float len = x.Length();

	if ( len <= Length )
		return;

	x = ( x / len ) * ( len - Length );
	Vec3f dv = P2->Velocity - P1->Velocity;
	Vec3f force = K * x - dv * Damping;
	P1->ApplyForce( -force );
	P2->ApplyForce( force );
}
//---------------------------------------