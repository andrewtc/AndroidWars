#include "CoreLib.h"

using namespace mage;

MAGE_IMPLEMENT_RTTI( Controller, AnimationController );

//---------------------------------------
AnimationController::AnimationController()
	: Repeat( RT_NO_REPEAT )
	, MinTime( 0.0f )
	, MaxTime( 0.0f )
	, Playing( true )
	, mTime( -Mathf::MAX_REAL )
{}
//---------------------------------------
AnimationController::~AnimationController()
{}
//---------------------------------------
bool AnimationController::OnUpdate( float currentTime )
{
	if ( Playing && ( currentTime == -Mathf::MAX_REAL || currentTime != mTime ) )
	{
		mTime = currentTime;
		return true;
	}
	return false;
}
//---------------------------------------
float AnimationController::GetNormalTime( float time )
{
	float normTime = time;

	if ( Repeat == RT_NO_REPEAT )
	{
		if ( normTime < MinTime )
			normTime = MinTime;
		else if ( normTime > MinTime ) 
			normTime = MaxTime;
	}
	else
	{
		float timeRange = MaxTime - MinTime;
		if ( timeRange > 0.0f )
		{
			float x = ( normTime - MinTime ) / timeRange;
			float intTime = std::floor( x );
			float fracTime = x - intTime;

			// In this case x will be 1.0f
			// Making fracTime == 0 when it should be 1
			if ( x == 1.0f )
				fracTime = 1.0f;

			if ( Repeat == RT_LOOP )
				normTime = MinTime + fracTime * timeRange;
			// RT_CYCLE
			else
			{
				// Playing Backwards
				if ( ((int) intTime) & 1 )
					normTime = MaxTime - fracTime * timeRange;
				// Forwards
				else
					normTime = MinTime + fracTime * timeRange;
			}
		}
		// Min/Max are equal
		else
			normTime = MinTime;
	}

	return normTime;
}
//---------------------------------------