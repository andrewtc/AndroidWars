/*
 * Author      : Matthew Johnson
 * Date        : 26/Nov/2013
 * Description :
 *   
 */
 
#pragma once


namespace mage
{

	class Button
		: public Label
	{
	DECLARE_RTTI;
	public:
		Button( const std::string& name, Widget* parent );

		struct ButtonStyle
		{
			HashString SpriteName;
			HashString DefaultAnimName;
			HashString PressedAnimName;
			HashString SelectedAnimName;
			HashString SelectedSFXName;
			Color PressedColor;
			Color DisabledColor;
			bool WrapText;
		};

		static ButtonStyle* CreateButtonStyle( const HashString& name );
		static ButtonStyle* GetButtonStyle( const HashString& name );
		static void DestroyAllButtonStyles();

		virtual void SetText( const char* text );

		void Disable();
		void Enable();
	protected:
		virtual ~Button();

		virtual void OnLoadFromXML( const XmlReader::XmlReaderIterator& xml );
		virtual void OnLoadFromDictionary( const Dictionary& dictionary );
		virtual void OnInit();

		virtual bool OnPointerDown( float x, float y );
		virtual bool OnPointerUp( float x, float y );

	private:
		HashString mOnClickEvent;
		HashString mStyleName;
		Color mDefaultColor;
		Color mPressedColor;
		bool mEnabled;

		static HashMap< ButtonStyle* > sButtonStyles;
	};
}
