#include "androidwars.h"

using namespace mage;

IMPLEMENT_RTTI( mage, Widget, TextField );


const HashString TextField::BACKGROUND_ELEMENT_NAME = "background";
const HashString TextField::TEXT_ELEMENT_NAME = "text";
const HashString TextField::DEFAULT_ANIMATION_NAME = "default";


TextField::TextField( WidgetManager* manager, const HashString& name ) :
	Widget( manager, name )
{ }


TextField::~TextField() { }


void TextField::OnLoadFromXML( const XmlReader::XmlReaderIterator& xml )
{
	Widget::OnLoadFromXML( xml );

	// Get the name of the background sprite.
	HashString animationSetName = xml.GetAttributeAsCString( "background", "" );

	// Create the background element.
	Graphic* backgroundElement = new Graphic( GetManager(), BACKGROUND_ELEMENT_NAME );
	backgroundElement->SetSprite( animationSetName, DEFAULT_ANIMATION_NAME );
	AddChild( backgroundElement );

	// Get the default text.
	std::string text = xml.GetAttributeAsCString( "text", "" );

	// Create the text element.
	Label* textElement = new Label( GetManager(), TEXT_ELEMENT_NAME );
	textElement->SetText( text );
	AddChild( textElement );
}


void TextField::OnLoadFromDictionary( const Dictionary& dictionary )
{
	Widget::OnLoadFromDictionary( dictionary );
}


void TextField::OnInit()
{
	Widget::OnInit();
}


void TextField::OnDraw( const Camera& camera )
{
	Widget::OnDraw( camera );
}
