/*
 * Author      : Matthew Johnson
 * Date        : 3/Nov/2013
 * Description :
 *   
 */
 
#pragma once

namespace mage
{

	class IKChain
	{
	public:
		IKChain();
		~IKChain();

		void OnUpdate( uint32 iterations );

		void InsertJointAt( uint32 i, IKJoint* joint );
		void InsertJointTail( IKJoint* joint );
		void RemoveJointTail();
		IKJoint* GetJointAt( uint32 i ) const;
		uint32 GetNumJoints() const	{ return mChain.size(); }

		IKTarget* Target;

	private:
		struct WorldSpaceBone
		{
			WorldSpaceBone( const Vec2f& pos, float length, float weight=1.0f )
				: Pos( pos ), Length( length ), Weight( weight )
			{}
			Vec2f Pos;
			float Length;
			float Weight;
		};

		static void LocalToWorld( ArrayList< WorldSpaceBone >& worldBones, const ArrayList< IKJoint* >& localBones );
		static void WorldToLocal( const ArrayList< WorldSpaceBone >& worldBones, ArrayList< IKJoint* >& localBones, const Vec2f& target );
		static void ConstraintRelaxation( ArrayList< WorldSpaceBone >& worldBones, const Vec2f& taget );

		// Convert the angle to equal angle inside [-pi,pi]
		static float WrapAngle( float angle );

		ArrayList< IKJoint* > mChain;
	};

}