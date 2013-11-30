#include <MageApp.h>
#include "Widget.h"
#include "Label.h"

using namespace mage;

//---------------------------------------
Label::Label( const std::string& name, const XmlReader::XmlReaderIterator& itr, Widget* parent )
	: Widget( name, itr, parent )
	, TextScale( 0.5f )
	, TextColor( Color::WHITE )
	, MaxLineLength( -1 )
	, mFont( 0 )
{
	Text = itr.GetAttributeAsString( "text", "" );
	const char* fontName = itr.GetAttributeAsCString( "font", 0 );
	if ( fontName )
	{
		mFont = new BitmapFont( fontName );
		// Use text area if no sprite
		if ( !mSprite )
		{
			mHeight = mFont->GetLineHeight( TextScale );
			mWidth = mFont->GetLineWidth( Text.c_str(), TextScale );
			DebugPrintf( "Widget : Dim Adjusted : '%s' w=%.3f h=%.3f\n", mName.GetString().c_str(), mWidth, mHeight );
		}
	}
}
//---------------------------------------
Label::~Label()
{}
//---------------------------------------
void Label::OnDraw( const Camera& camera ) const
{
	Widget::OnDraw( camera );
	if ( mFont )
	{
		Vec2f pos = GetPosition();
		DrawText( pos.x, pos.y, mFont, TextColor, TextScale, MaxLineLength, Text.c_str() );
	}
}
//---------------------------------------
