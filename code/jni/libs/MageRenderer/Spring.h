/*
 * Author      : Matthew Johnson
 * Date        : 17/Sep/2013
 * Description :
 *   
 */
 
#pragma once

namespace mage
{

	class PointMass;
	class Spring
	{
	public:
		Spring( PointMass* p1, PointMass* p2, float k, float damping );
		~Spring();

		void OnUpdate( float dt );

		PointMass* P1;
		PointMass* P2;
		float Length;
		float K;
		float Damping;
	};

}