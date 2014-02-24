#include "androidwars.h"

using namespace mage;

MAGE_IMPLEMENT_RTTI( Widget, Label );

//---------------------------------------
Label::Label( WidgetManager* manager, const HashString& name )
	: Widget( manager, name )
	, mTextScale( 1.0f )
	, TextColor( Color::WHITE )
	, mMaxLineLength( -1 )
	, mFont( nullptr )
{ }
//---------------------------------------
void Label::OnLoadFromXML( const XmlReader::XmlReaderIterator& xml )
{
	Widget::OnLoadFromXML( xml );

	TextColor = xml.GetAttributeAsColor( "textColor", Color::WHITE );

	// Look up the font by its name.
	const char* fontName = xml.GetAttributeAsCString( "font", "" );
	SetFont( GetManager()->GetFontByName( fontName ) );

	// Set the text of the label.
	SetText( xml.GetAttributeAsCString( "text", "" ) );
}
//---------------------------------------
void Label::OnLoadFromDictionary( const Dictionary& dictionary )
{
	Widget::OnLoadFromDictionary( dictionary );

	// Look up the font by its name.
	TextColor = dictionary.Get( "textColor", Color::WHITE );
	const char* fontName = dictionary.Get< const char* >( "font", "" );
	SetFont( GetManager()->GetFontByName( fontName ) );

	// Set the text of the label.
	SetText( dictionary.Get( "text", "" ) );
}
//---------------------------------------
void Label::OnInit()
{
	Widget::OnInit();
}
//---------------------------------------
Label::~Label()
{}
//---------------------------------------
void Label::OnDraw( const Camera& camera )
{
	// If not visible, do not process this Widget or its children
	if ( !IsVisible() )
		return;

	Widget::OnDraw( camera );

	if( mFont )
	{
		Vec2f pos = FindPosition() + mTextDrawOffset;
		DrawText( pos.x, pos.y, mFont, TextColor, mTextScale, mMaxLineLength, mText.c_str() );
	}
}
//---------------------------------------
void Label::SetFont( BitmapFont* font )
{
	if( mFont != font )
	{
		// Set the font.
		mFont = font;

		// Recalculate the label bounds.
		RecalculateSize();
	}
}
//---------------------------------------
BitmapFont* Label::GetFont() const
{
	return mFont;
}
//---------------------------------------
void Label::SetText( const std::string& text )
{
	SetText( text.c_str() );
}
//---------------------------------------
void Label::SetText( const char* text )
{
	assertion( text != nullptr, "Cannot set Label text to null string!" );
	DebugPrintf( "Setting label text to \"%s\".", text );

	// Set the label text.
	mText = text;

	// Recalculate the size of the label.
	RecalculateSize();
}
//---------------------------------------
void Label::RecalculateSize()
{
	DebugPrintf( "Recalculating size..." );
	if( mFont )
	{
		// Resize the label to hold all the text.
		SetSize( mFont->GetLineWidth( mText.c_str(), mTextScale ),
				 mFont->GetLineHeight( mTextScale ) );
	}
}
//---------------------------------------
