/*
 * Author      : Matthew Johnson
 * Date        : 3/Nov/2013
 * Description :
 *   
 */
 
#pragma once

namespace mage
{

	class IKController
		: public Controller
	{
		DECLARE_RTTI;
	public:
		IKController();
		virtual ~IKController();

		bool OnUpdate( float currentTime );

		uint32 Iterations;
		IKChain Chain;
	};

}