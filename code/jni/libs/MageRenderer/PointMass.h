/*
 * Author      : Matthew Johnson
 * Date        : 17/Sep/2013
 * Description :
 *   
 */
 
#pragma once

namespace mage
{

	class PointMass
	{
	public:
		PointMass( const Vec3f& position, float invMass );
		~PointMass();

		void ApplyForce( const Vec3f& force );
		void IncreaseDamping( float factor );
		void OnUpdate( float dt );

		Vec3f Position;
		Vec3f Velocity;
		float InverseMass;

	private:
		Vec3f mAcceleration;
		float mDamping;
	};

}