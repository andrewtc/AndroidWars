#include <MageApp.h>
#include "androidwars.h"

using namespace mage;

MAGE_IMPLEMENT_RTTI( Label, Button );

//---------------------------------------
HashMap< Button::ButtonStyle* > Button::sButtonStyles;
//---------------------------------------
Button::ButtonStyle* Button::CreateButtonStyle( const HashString& name )
{
	assertion( !name.GetString().empty(), "Cannot create ButtonStyle without a valid name!" );
	assertion( GetButtonStyle( name ) == nullptr, "Cannot create button style \"%s\" because a style with that name already exists!" );

	// Create a new style and add it to the list of styles.
	ButtonStyle* style = new ButtonStyle();
	sButtonStyles[ name ] = style;

	return style;
}
//---------------------------------------
Button::ButtonStyle* Button::GetButtonStyle( const HashString& name )
{
	assertion( !name.GetString().empty(), "Cannot get ButtonStyle without a valid name!" );
	ButtonStyle* result = nullptr;

	auto it = sButtonStyles.find( name );

	if( it != sButtonStyles.end() )
	{
		result = it->second;
	}

	return result;
}
//---------------------------------------
void Button::DestroyAllButtonStyles()
{
	for( auto it = sButtonStyles.begin(); it != sButtonStyles.end(); ++it )
	{
		delete it->second;
	}
	sButtonStyles.clear();
}
//---------------------------------------
Button::Button( const std::string& name, Widget* parent )
	: Label( name, parent )
	, mEnabled( true )
{
}
//---------------------------------------
Button::~Button()
{}
//---------------------------------------
void Button::OnLoadFromXML( const XmlReader::XmlReaderIterator& xml )
{
	Label::OnLoadFromXML( xml );

	mStyleName = xml.GetAttributeAsString( "style" );

	mOnClickEvent = xml.GetAttributeAsString( "onClickEvent", "__DUMMY_EVENT__" );
}
//---------------------------------------
void Button::OnLoadFromDictionary( const Dictionary& dictionary )
{
	Label::OnLoadFromDictionary( dictionary );

	mStyleName = dictionary.Get( "style", "" );

	mOnClickEvent = dictionary.Get( "onClickEvent", "__DUMMY_EVENT__" );
}
//---------------------------------------
void Button::OnInit()
{
	Label::OnInit();

	// Look up style.
	ButtonStyle* style = GetButtonStyle( mStyleName );
	assertion( style, "ButtonStyle \"%s\" not found!", mStyleName.GetCString() );

	mFixedSizeSprite = style->WrapText;

	if ( mFont && style->WrapText )
	{
		mHeight = mFont->GetLineHeight( mTextScale * 1.2f );
		mWidth = mFont->GetLineWidth( mText.c_str(), mTextScale * 1.2f );
	}

	SetSprite( style->SpriteName );

	mDefaultColor = mDrawColor;
	mPressedColor = style->PressedColor;

	// Call SetText to apply any special formatting when text is set
	SetText( mText.c_str() );
}
//---------------------------------------
void Button::SetText( const char* text )
{
	//Label::SetText( text );
	mText = text;
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
	// If not visible, do not process this Widget or its children
	if ( !Visible || !mEnabled )
		return false;

	ButtonStyle* style = Button::GetButtonStyle( mStyleName );
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
				mSprite->PlayAnimation( style->PressedAnimName );
				mSprite->DrawColor = mPressedColor;
				return true;
			}
		}
	}
	mSprite->PlayAnimation( style->DefaultAnimName );
	mSprite->DrawColor = mDefaultColor;
	return false;
}
//---------------------------------------
bool Button::OnPointerUp( float x, float y )
{
	// If not visible, do not process this Widget or its children
	if ( !Visible || !mEnabled )
		return false;

	ButtonStyle* style = Button::GetButtonStyle( mStyleName );
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
				ButtonStyle* style = Button::GetButtonStyle( mStyleName );
				// Play selected anim
				mSprite->PlayAnimation( style->SelectedAnimName );
				mSprite->DrawColor = mDefaultColor;
				// Play SFX
				if ( style->SelectedSFXName.GetHash() != 0 )
				{
					gSoundManager->PlaySound( gSoundManager->GetSoundClip( style->SelectedSFXName ) );
				}
				// Fire pressed event
				Dictionary params;
				params.Set( "Widget", (Widget*) this );
				EventManager::FireEvent( mOnClickEvent, params );
				return true;
			}
		}
	}
	mSprite->PlayAnimation( style->DefaultAnimName );
	mSprite->DrawColor = mDefaultColor;
	return false;
}
//---------------------------------------
void Button::Disable()
{
	ButtonStyle* style = Button::GetButtonStyle( mStyleName );
	mSprite->DrawColor = style->DisabledColor;
	mEnabled = false;
}
//---------------------------------------
void Button::Enable()
{
	mSprite->DrawColor = mDefaultColor;
	mEnabled = true;
}
//---------------------------------------
