#pragma once

namespace mage
{
	class TextField : public Widget
	{
		DECLARE_RTTI;

	public:
		TextField( const std::string& name, Widget* parent );
		virtual ~TextField();

		virtual void OnDraw( const Camera& camera ) const;

		void SetText( const std::string& text );
		std::string GetText() const;

	protected:
		virtual void OnLoadFromXML( const XmlReader::XmlReaderIterator& xml );
		virtual void OnLoadFromDictionary( const Dictionary& dictionary );
		virtual void OnInit();

		BitmapFont* mFont;
		std::string mText;
		Vec2f mTextDrawOffset;
		Color mTextColor;
		float mTextScale;
		int mMaxLineLength;
	};


	inline void TextField::SetText( const std::string& text )
	{
		mText = text;
	}


	inline std::string TextField::GetText() const
	{
		return mText;
	}
}
