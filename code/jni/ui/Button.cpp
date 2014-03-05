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
}
//---------------------------------------
void Button::OnInit()
{
	Widget::OnInit();
}
//---------------------------------------
bool Button::OnPointerDown( float x, float y )
{
	bool wasHandled = false;
	Widget::OnPointerDown( x, y );

	if( mIsEnabled )
	{
		// Press the button.
		mIsPressed = true;
		wasHandled = true;
	}

	return wasHandled;
}
//---------------------------------------
bool Button::OnPointerUp( float x, float y )
{
	bool wasHandled = false;
	Widget::OnPointerUp( x, y );

	if( mIsPressed )
	{
		wasHandled = true;

		// Unpress the button.
		mIsPressed = false;

		if( mOnClick.IsValid() )
		{
			// Fire the click callback.
			mOnClick.Invoke( x, y );
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
