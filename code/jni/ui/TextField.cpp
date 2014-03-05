#include "androidwars.h"

using namespace mage;

IMPLEMENT_RTTI( mage, Widget, TextField );


TextField::TextField( WidgetManager* manager, const HashString& name ) :
	Widget( manager, name )
{ }


TextField::~TextField() { }


void TextField::OnLoadFromTemplate( const WidgetTemplate& widgetTemplate )
{
	Widget::OnLoadFromTemplate( widgetTemplate );

	// Get the text element name.
	mTextElementName = widgetTemplate.GetProperty( "textElement", "", true );

	if( widgetTemplate.HasProperty( "text" ) )
	{
		// Set the text of the label.
		SetText( widgetTemplate.GetProperty( "text" ) );
	}
}


void TextField::OnInit()
{
	Widget::OnInit();
}


void TextField::OnDraw( const Camera& camera )
{
	Widget::OnDraw( camera );
}


Label* TextField::GetTextElement() const
{
	return GetChildByName< Label >( mTextElementName );
}


void TextField::SetText( const std::string& text )
{
	SetText( text.c_str() );
}


void TextField::SetText( const char* text )
{
	Label* textElement = GetTextElement();

	if( textElement )
	{
		// If the text element was found, set its text property.
		textElement->SetText( text );
	}
	else
	{
		WarnFail( "Could not set text for Label \"%s\": No text element \"%s\" was found!", GetName().GetCString(), mTextElementName.GetCString() );
	}
}


std::string TextField::GetText() const
{
	std::string result;
	Label* textElement = GetTextElement();

	if( textElement )
	{
		// If the text element was found, return its text property.
		result = textElement->GetText();
	}
	else
	{
		WarnFail( "Could not get text for Label \"%s\": No text element \"%s\" was found!", GetName().GetCString(), mTextElementName.GetCString() );
	}

	return textElement->GetText();
}
