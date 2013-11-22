/*
 * Author      : Matthew Johnson
 * Date        : 14/Oct/2013
 * Description :
 *   
 */
 
#pragma once

namespace mage
{

	class SpriteNode
		: public Node
	{
		DECLARE_RTTI;
	public:
		SpriteNode();
		SpriteNode( Texture2D* Sprite );
		SpriteNode( Texture2D* Sprite, const RectF& UV );
		virtual ~SpriteNode();

		void OnDraw();

		void SetSprite( Texture2D* sprite )									{ mSprite = sprite; }
		void SetUV( const RectF& UV )										{ mUV = UV; }
		void SetSpriteAndUV( Texture2D* sprite, const RectF& UV )			{ SetSprite( sprite ); SetUV( UV ); }

	private:
		Texture2D* mSprite;
		RectF mUV;
	};

}