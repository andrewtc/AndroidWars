/*
 * Author      : Matthew Johnson
 * Date        : 9/Nov/2013
 * Description :
 *   
 */
 
#pragma once

namespace mage
{

	class BasicNodeVisualizer
		: public NodeVisualizer
	{
	public:
		BasicNodeVisualizer( float radius );
		~BasicNodeVisualizer();

		virtual void OnDraw( Node* node ) const;

		float Radius;
	};

}