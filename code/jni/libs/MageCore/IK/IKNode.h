/*
 * Author      : Matthew Johnson
 * Date        : 3/Nov/2013
 * Description :
 *   
 */
 
#pragma once

namespace mage
{

	class IKNode
		: public Node
	{
		DECLARE_RTTI;
	public:
		IKNode();
		virtual ~IKNode();

		IKController Controller;
	};

}