/*
 * Author      : Matthew Johnson
 * Date        : 26/Nov/2013
 * Description :
 *   
 */
 
#pragma once


namespace mage
{

	class Button : public Widget
	{
		DECLARE_RTTI;

	public:
		typedef Delegate< void > OnClickDelegate;

		Button( WidgetManager* manager, const HashString& name );

		void Disable();
		void Enable();

		void SetOnClickDelegate( OnClickDelegate onClick );
		void ClearOnClickDelegate();
		OnClickDelegate GetOnClickDelegate() const;
		bool HasOnClickDelegate() const;

		void SetLabelElementName( const HashString& name );
		HashString GetLabelElementName() const;
		Label* GetLabelElement() const;
		bool HasLabelElement() const;

		void SetLabel( const std::string& text );
		std::string GetLabel() const;

	protected:
		virtual ~Button();

		virtual void OnLoadFromTemplate( const WidgetTemplate& widgetTemplate );
		virtual void OnInit();

		virtual bool OnPointerDown( const Pointer& pointer );
		virtual bool OnPointerUp( const Pointer& pointer );

	private:
		bool mIsEnabled;
		bool mIsPressed;
		OnClickDelegate mOnClick;
		HashString mLabelElementName;
	};
}
