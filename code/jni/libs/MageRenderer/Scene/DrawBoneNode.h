/*
 * Author      : Matthew Johnson
 * Date        : 15/Oct/2013
 * Description :
 *   Debug draw for skeletal bone.
 *   Also has a keyframe controller.
 */
 
#pragma once

namespace mage
{

	class DrawBoneNode
		: public Node
	{
		DECLARE_RTTI;
	public:
		DrawBoneNode( float radius );
		virtual ~DrawBoneNode();

		void OnDraw();

		float GetRadius() const			{ return mRadius; }

		KeyframeController KeyframeCont;
		Color DrawColor;
		bool DebugKeyframes;
		Transform2D OriginalTransform;
	private:
		float mRadius;
	};

}