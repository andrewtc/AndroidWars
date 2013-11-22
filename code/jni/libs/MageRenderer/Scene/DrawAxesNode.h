/*
 * Author      : Matthew Johnson
 * Date        : 14/Oct/2013
 * Description :
 *   Draw debug axes to show position/orientation/scale.
 */
 
#pragma once

namespace mage
{

	class DrawAxesNode
		: public Node
	{
		DECLARE_RTTI;
	public:
		DrawAxesNode( float length );

		void OnDraw();

	private:
		float mLength;
	};

}