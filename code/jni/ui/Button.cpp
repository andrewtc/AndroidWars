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
		// Get the bounds of the button.
		RectF bounds = CalculateBounds();

		// Press the button.
		mIsPressed = true;
		return true;
	}

	return wasHandled;
}
//---------------------------------------
bool Button::OnPointerUp( float x, float y )
{
	// TODO
	/*
	// If not visible, do not process this Widget or its children
	if ( !IsVisible() || !mIsEnabled )
		return false;

	if ( !Widget::OnPointerUp( x, y ) )
	{
		// Get the bounds of the Button.
		RectF bounds = CalculateBounds();

		if( bounds.Contains( x, y ) && mIsPressed )
		{
			if( mOnClick.IsValid() )
			{
				// Fire click callback.
				mOnClick.Invoke( x, y );
			}

			// Unpress the button.
			mIsPressed = false;
			return true;
		}
	}*/

	return false;
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
