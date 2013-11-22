/*
 * Author      : Matthew Johnson
 * Date        : 3/Nov/2013
 * Description :
 *   Provides a hook for adding debug visuals to nodes.
 */
 
#pragma once

namespace mage
{

	class Node;
	class NodeVisualizer
	{
	protected:
		NodeVisualizer() {}
		virtual ~NodeVisualizer() {}

	public:
		virtual void OnDraw( Node* node ) const = 0;
	};

}