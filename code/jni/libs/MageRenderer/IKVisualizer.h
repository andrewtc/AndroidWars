/*
 * Author      : Matthew Johnson
 * Date        : 3/Nov/2013
 * Description :
 *   
 */
 
#pragma once

namespace mage
{

	class IKVisualizer
		: public NodeVisualizer
	{
	public:
		IKVisualizer();
		virtual ~IKVisualizer();

		virtual void OnDraw( Node* node ) const;

		static ArrayList< Color > BoneColors;
	};

}