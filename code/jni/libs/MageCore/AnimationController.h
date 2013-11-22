/*
 * Author      : Matthew Johnson
 * Date        : 20/Oct/2013
 * Description :
 *   
 */
 
#pragma once

namespace mage
{

	class AnimationController
		: public Controller
	{
		DECLARE_RTTI;
	protected:
		AnimationController();
	public:
		virtual ~AnimationController();

		virtual bool OnUpdate( float currentTime );

		// Get the time relative to this animation controller
		float GetNormalTime( float time );

		enum RepeatType
		{
			RT_NO_REPEAT,
			RT_LOOP,
			RT_CYCLE
		};

		RepeatType Repeat;
		float MinTime;
		float MaxTime;
		bool Playing;

	protected:
		float mTime;
	};

}