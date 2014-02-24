#pragma once

namespace mage
{
	class TextField : public Widget
	{
		DECLARE_RTTI;

	public:
		static const HashString BACKGROUND_ELEMENT_NAME;
		static const HashString TEXT_ELEMENT_NAME;
		static const HashString DEFAULT_ANIMATION_NAME;

		TextField( WidgetManager* manager, const HashString& name );
		virtual ~TextField();

		virtual void OnDraw( const Camera& camera );

		Graphic* GetBackgroundElement() const;
		Label* GetTextElement() const;

	protected:
		virtual void OnLoadFromXML( const XmlReader::XmlReaderIterator& xml );
		virtual void OnLoadFromDictionary( const Dictionary& dictionary );
		virtual void OnInit();
	};


	inline Graphic* TextField::GetBackgroundElement() const
	{
		return GetChildByName< Graphic >( BACKGROUND_ELEMENT_NAME );
	}


	inline Label* TextField::GetTextElement() const
	{
		return GetChildByName< Label >( TEXT_ELEMENT_NAME );
	}
}
