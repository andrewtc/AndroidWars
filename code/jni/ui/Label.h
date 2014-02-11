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
		Label( const std::string& name, Widget* parent );
		virtual ~Label();

		virtual void OnDraw( const Camera& camera ) const;

		void SetText( const std::string& text );
		virtual void SetText( const char* text );
		std::string GetText() const;

		Color TextColor;

	protected:
		virtual void OnLoadFromXML( const XmlReader::XmlReaderIterator& xml );
		virtual void OnLoadFromDictionary( const Dictionary& dictionary );
		virtual void OnInit();

		BitmapFont* mFont;
		std::string mText;
		float mTextScale;
		int mMaxLineLength;
		Vec2f mTextDrawOffset;
	};


	inline std::string Label::GetText() const
	{
		return mText;
	}
}
