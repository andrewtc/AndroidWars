/*
 * Author      : Matthew Johnson
 * Date        : 26/Nov/2013
 * Description :
 *   
 */
 
#pragma once

namespace mage
{

	class Label
		: public Widget
	{
	DECLARE_RTTI;
	public:
		Label( const std::string& name, const XmlReader::XmlReaderIterator& itr, Widget* parent );
		virtual ~Label();

		virtual void OnDraw( const Camera& camera ) const;

		void SetText( const std::string& text );
		virtual void SetText( const char* text );

		Color TextColor;

	protected:
		BitmapFont* mFont;
		std::string mText;
		float mTextScale;
		int mMaxLineLength;
		Vec2f mTextDrawOffset;
	};

}
