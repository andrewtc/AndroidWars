/*
 * Author      : Matthew Johnson
 * Date        : 20/Oct/2013
 * Description :
 *   
 */
 
#pragma once

namespace mage
{

	//---------------------------------------
	// Keyframe structs
	struct Keyframe
	{
		float Time;
		
		bool operator<( const Keyframe& kf ) const	{ return Time < kf.Time; }
	};
	//---------------------------------------
	struct PositionKey
		: public virtual Keyframe
	{
		Vec2f Position;
	};
	//---------------------------------------
	struct RotationKey
		: public virtual Keyframe
	{
		float Rotation;
	};
	//---------------------------------------
	struct PositionRotationKey
		: public RotationKey
		, public PositionKey
	{
	};
	//---------------------------------------


	//---------------------------------------
	class KeyframeController
		: public AnimationController
	{
		DECLARE_RTTI;
	public:
		KeyframeController();
		virtual ~KeyframeController();

		bool OnUpdate( float currentTime );

		void InsertPositionRotationKey( const PositionRotationKey& key );
		void RemovePositionRotationKey( float time );
		float GetNextPositionRotationKeyTime();
		float GetPrevPositionRotationKeyTime();

		void Reset();

		// Keyframes
		ArrayList< PositionKey > PositionKeys;
		ArrayList< RotationKey > RotationKeys;
		ArrayList< PositionRotationKey > PositionRotationKeys;

	protected:
		// Get a keyframe at a specific time
		// Keyframe indices are returned through i0 and i1
		// normTime is the normalized keyframe time
		// lastIndex is a hint for the last lookup result and is updated with the new result
		template< typename TKey >
		static void GetKeyframe( float time, const ArrayList< TKey >& keys,
			int& lastIndex, float& normTime, int& i0, int& i1 );

		// Interpolation helpers
		Vec2f GetInterpolatedPosition( float time, int i0, int i1 ) const;
		float GetInterpolatedRotation( float time, int i0, int i1 ) const;
		void GetInterpolatedPositionRotation( Vec2f& pos, float& rot, float time, int i0, int i1 ) const;

		// Cached indices from last key lookups
		int mLastPositionIndex;
		int mLastRotationIndex;
		int mLastPositionRotationIndex;
	};
	//---------------------------------------

}