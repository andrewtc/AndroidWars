#include "androidwars.h"

using namespace mage;

IMPLEMENT_RTTI( mage, Widget, TextField );


TextField::TextField( const std::string& name, Widget* parent ) :
	Widget( name, parent ),
	mTextScale( 1.0f ),
	mTextColor( Color::WHITE ),
	mMaxLineLength( -1 ),
	mFont( 0 )
{ }


TextField::~TextField() { }


void TextField::OnLoadFromXML( const XmlReader::XmlReaderIterator& xml )
{
	Widget::OnLoadFromXML( xml );
}


void TextField::OnLoadFromDictionary( const Dictionary& dictionary )
{
	Widget::OnLoadFromDictionary( dictionary );
}


void TextField::OnInit()
{
	Widget::OnInit();
}


void TextField::OnDraw( const Camera& camera ) const
{
	Widget::OnDraw( camera );
}
