#pragma once

namespace mage
{
	class TextField : public Widget
	{
		DECLARE_RTTI;

	public:
		TextField( WidgetManager* manager, const HashString& name );
		virtual ~TextField();

		virtual void OnDraw( const Camera& camera );

		Label* GetTextElement() const;

		void SetText( const std::string& text );
		void SetText( const char* text );
		std::string GetText() const;

	protected:
		virtual void OnLoadFromTemplate( const WidgetTemplate& widgetTemplate );
		virtual void OnInit();

		HashString mTextElementName;
	};
}
