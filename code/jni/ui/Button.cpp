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

	mFixedSizeSprite = style.WrapText;

	if ( mFont && style.WrapText )
	{
		mHeight = mFont->GetLineHeight( mTextScale * 1.2f );
		mWidth = mFont->GetLineWidth( mText.c_str(), mTextScale * 1.2f );
	}

	SetSprite( style.SpriteName );

	mOnClickEvent = itr.GetAttributeAsString( "onClickEvent", "__DUMMY_EVENT__" );
	mDefaultColor = mDrawColor;
	mPressedColor = style.PressedColor;

	// Call SetText to apply any special formatting when text is set
	SetText( mText.c_str() );
}
//---------------------------------------
Button::~Button()
{}
//---------------------------------------
void Button::SetText( const char* text )
{
	Label::SetText( text );
	if ( mFont )
	{
		float w = mFont->GetLineWidth( text, mTextScale );
		float h = mFont->GetLineHeight( mTextScale );
		float offsetX = ( mWidth - w ) / 2.0f;
		float offsetY = ( mHeight - h )/ 2.0f;
		mTextDrawOffset.Set( offsetX, offsetY );
	}
}
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
				mSprite->DrawColor = mPressedColor;
				return true;
			}
		}
	}
	mSprite->PlayAnimation( style.DefaultAnimName );
	mSprite->DrawColor = mDefaultColor;
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
				mSprite->DrawColor = mDefaultColor;
				// Fire pressed event
				EventManager::FireEvent( mOnClickEvent );
				return true;
			}
		}
	}
	mSprite->PlayAnimation( style.DefaultAnimName );
	mSprite->DrawColor = mDefaultColor;
	return false;
}
//---------------------------------------
