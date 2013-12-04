#include <MageApp.h>
#include "Widget.h"
#include "Label.h"
#include "Button.h"

using namespace mage;

//---------------------------------------
HashMap< Button::ButtonStyle > Button::sButtonStyles;
//---------------------------------------
Button::Button( const std::string& name, const XmlReader::XmlReaderIterator& itr, Widget* parent )
	: Label( name, itr, parent )
{
	mStyleName = itr.GetAttributeAsString( "style" );
	ButtonStyle& style = sButtonStyles[ mStyleName ];
	SetSprite( style.SpriteName );

	mOnClickEvent = itr.GetAttributeAsString( "onClickEvent", "__DUMMY_EVENT__" );
}
//---------------------------------------
Button::~Button()
{}
//---------------------------------------
bool Button::OnPointerDown( float x, float y )
{
	ButtonStyle& style = sButtonStyles[ mStyleName ];
	if ( !Widget::OnPointerDown( x, y ) )
	{
		// If event is inside frame, fire event
		if ( mSprite )
		{
			RectI r;
			Vec2f pos = GetPosition();
			r.Left = pos.x;
			r.Top = pos.y;
			r.Right = r.Left + mWidth;
			r.Bottom = r.Top + mHeight;
			if ( r.Contains( (int) x, (int) y ) )
			{

				// Play pressed anim
				mSprite->PlayAnimation( style.PressedAnimName );
				return true;
			}
		}
	}
	mSprite->PlayAnimation( style.DefaultAnimName );
	return false;
}
//---------------------------------------
bool Button::OnPointerUp( float x, float y )
{
	ButtonStyle& style = sButtonStyles[ mStyleName ];
	if ( !Widget::OnPointerDown( x, y ) )
	{
		// If event is inside frame, fire event
		if ( mSprite )
		{
			RectI r;
			Vec2f pos = GetPosition();
			r.Left = pos.x;
			r.Top = pos.y;
			r.Right = r.Left + mWidth;
			r.Bottom = r.Top + mHeight;
			if ( r.Contains( (int) x, (int) y ) )
			{
				ButtonStyle& style = sButtonStyles[ mStyleName ];
				// Play selected anim
				mSprite->PlayAnimation( style.SelectedAnimName );
				// Fire pressed event
				EventManager::FireEvent( mOnClickEvent );
				return true;
			}
		}
	}
	mSprite->PlayAnimation( style.DefaultAnimName );
	return false;
}
//---------------------------------------
