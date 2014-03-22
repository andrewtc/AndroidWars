#include <MageApp.h>
#include "androidwars.h"

using namespace mage;

MAGE_IMPLEMENT_RTTI( Widget, Button );

//---------------------------------------
Button::Button( WidgetManager* manager, const HashString& name )
	: Widget( manager, name )
	, mIsEnabled( true )
	, mIsPressed( false )
{ }
//---------------------------------------
Button::~Button() { }
//---------------------------------------
void Button::OnLoadFromTemplate( const WidgetTemplate& widgetTemplate )
{
	Widget::OnLoadFromTemplate( widgetTemplate );

	// Load the name of the label element (if any).
	mLabelElementName = widgetTemplate.GetProperty( "labelElement" );

	if( widgetTemplate.HasProperty( "label" ) )
	{
		// Load the button label (if any).
		SetLabel( widgetTemplate.GetProperty( "label", "" ) );
	}
}
//---------------------------------------
void Button::OnInit()
{
	Widget::OnInit();
}
//---------------------------------------
bool Button::OnPointerDown( const Pointer& pointer )
{
	bool wasHandled = false;
	Widget::OnPointerDown( pointer );

	if( mIsEnabled )
	{
		// Press the button.
		mIsPressed = true;
		wasHandled = true;
	}

	return wasHandled;
}
//---------------------------------------
bool Button::OnPointerUp( const Pointer& pointer )
{
	bool wasHandled = false;
	Widget::OnPointerUp( pointer );

	if( mIsPressed )
	{
		wasHandled = true;

		// Unpress the button.
		mIsPressed = false;

		if( mOnClick.IsValid() )
		{
			// Fire the click callback.
			mOnClick.Invoke();
		}
	}

	return wasHandled;
}
//---------------------------------------
void Button::Disable()
{
	mIsEnabled = false;
}
//---------------------------------------
void Button::Enable()
{
	mIsEnabled = true;
}
//---------------------------------------
void Button::SetOnClickDelegate( OnClickDelegate onClick )
{
	mOnClick = onClick;
}
//---------------------------------------
void Button::ClearOnClickDelegate()
{
	mOnClick = OnClickDelegate();
}
//---------------------------------------
Button::OnClickDelegate Button::GetOnClickDelegate() const
{
	return mOnClick;
}
//---------------------------------------
bool Button::HasOnClickDelegate() const
{
	return mOnClick.IsValid();
}
//---------------------------------------
void Button::SetLabelElementName( const HashString& name )
{
	mLabelElementName = name;
}
//---------------------------------------
HashString Button::GetLabelElementName() const
{
	return mLabelElementName;
}
//---------------------------------------
Label* Button::GetLabelElement() const
{
	return GetChildByName< Label >( mLabelElementName );
}
//---------------------------------------
bool Button::HasLabelElement() const
{
	return ( GetLabelElement() != nullptr );
}
//---------------------------------------
void Button::SetLabel( const std::string& text )
{
	Label* labelElement = GetLabelElement();

	if( labelElement )
	{
		// Set the text of the label.
		labelElement->SetText( text );
	}
	else
	{
		WarnFail( "Could not set label for Button \"%s\" because no label element \"%s\" was found!", GetFullName().c_str(), mLabelElementName.GetCString() );
	}
}
//---------------------------------------
std::string Button::GetLabel() const
{
	std::string result;
	Label* labelElement = GetLabelElement();

	if( labelElement )
	{
		// Get the text of the label.
		result = labelElement->GetText();
	}
	else
	{
		WarnFail( "Could not get label for Button \"%s\" because no label element \"%s\" was found!", GetFullName().c_str(), mLabelElementName.GetCString() );
	}

	return result;
}
//---------------------------------------
