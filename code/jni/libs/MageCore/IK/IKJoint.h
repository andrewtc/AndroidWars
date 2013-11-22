/*
 * Author      : Matthew Johnson
 * Date        : 3/Nov/2013
 * Description :
 *   
 */
 
#pragma once

namespace mage
{

	class IKTarget
		: public Node
	{
		DECLARE_RTTI;
	public:
		IKTarget();
		virtual ~IKTarget();

		//Vec2f Position;
	};

	class IKJoint
		: public Object
	{
		DECLARE_RTTI;
	public:
		IKJoint();
		IKJoint( float angle, float length, float weight=1.0f );
		~IKJoint();

		float Angle;
		float Length;
		float Weight;
	};

}