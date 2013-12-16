#include <MageApp.h>
#include "Widget.h"
#include "Label.h"

using namespace mage;

MAGE_IMPLEMENT_RTTI( Widget, Label );

//---------------------------------------
Label::Label( const std::string& name, const XmlReader::XmlReaderIterator& itr, Widget* parent )
	: Widget( name, itr, parent )
	, mTextScale( 1.0f )
	, TextColor( Color::WHITE )
	, mMaxLineLength( -1 )
	, mFont( 0 )
{
	mText = itr.GetAttributeAsCString( "text", "" );
	TextColor = itr.GetAttributeAsColor( "textColor", Color::WHITE );
	const char* fontName = itr.GetAttributeAsCString( "font", 0 );
	if ( fontName )
	{
		mFont = GetFontByName( fontName );
	}
	// Use text area if no sprite
	if ( mFont && !mSprite && !mFixedSizeSprite )
	{
		mHeight = mFont->GetLineHeight( mTextScale );
		mWidth = mFont->GetLineWidth( mText.c_str(), mTextScale );

		UpdateLayout();
		DebugPrintf( "Widget : Dim Adjusted : '%s' w=%.3f h=%.3f\n", mName.GetString().c_str(), mWidth, mHeight );
	}
}
//---------------------------------------
Label::~Label()
{}
//---------------------------------------
void Label::OnDraw( const Camera& camera ) const
{
	// If not visible, do not process this Widget or its children
	if ( !Visible )
		return;

	Widget::OnDraw( camera );
	if ( mFont )
	{
		Vec2f pos = GetPosition() + mTextDrawOffset;
		DrawText( pos.x, pos.y, mFont, TextColor, mTextScale, mMaxLineLength, mText.c_str() );
	}
}
//---------------------------------------
void Label::SetText( const std::string& text )
{
	SetText( text.c_str() );
}
//---------------------------------------
void Label::SetText( const char* text )
{
	mText = text;
}
//---------------------------------------
