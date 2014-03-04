#include "androidwars.h"

using namespace mage;

MAGE_IMPLEMENT_RTTI( Widget, Label );

//---------------------------------------
Label::Label( WidgetManager* manager, const HashString& name )
	: Widget( manager, name )
	, mTextScale( 1.0f )
	, mTextColor( Color::BLACK )
	, mMaxLineLength( -1 )
	, mFont( nullptr )
{ }
//---------------------------------------
void Label::OnLoadFromTemplate( const WidgetTemplate& widgetTemplate )
{
	Widget::OnLoadFromTemplate( widgetTemplate );

	mTextColor = widgetTemplate.GetPropertyAsColor( "textColor", Color::BLACK );

	// Look up the font by its name.
	std::string fontName = widgetTemplate.GetProperty( "font", "", true );
	SetFont( GetManager()->GetFontByName( fontName ) );

	// Set the text of the label.
	SetText( widgetTemplate.GetProperty( "text" ) );
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
		Vec2f pos = CalculatePosition() + mTextDrawOffset;
		DrawText( pos.x, pos.y, mFont, mTextColor, mTextScale, mMaxLineLength, mText.c_str() );
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
void Label::SetTextColor( const Color& textColor )
{
	mTextColor = textColor;
}
//---------------------------------------
Color Label::GetTextColor() const
{
	return mTextColor;
}
//---------------------------------------
void Label::RecalculateSize()
{
	DebugPrintf( "Recalculating Label size..." );
	if( mFont )
	{
		// Resize the label to hold all the text.
		SetSize( mFont->GetLineWidth( mText.c_str(), mTextScale ),
				 mFont->GetLineHeight( mTextScale ) );
		DebugPrintf( "Label size is now (%f, %f).", GetWidth(), GetHeight() );
	}
	else
	{
		WarnFail( "Could not recalculate Label size because no Font was found!" );
	}
}
//---------------------------------------
