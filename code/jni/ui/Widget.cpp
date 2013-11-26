#include <MageApp.h>
#include "Widget.h"
#include "Button.h"

using namespace mage;

//---------------------------------------
// Static
//---------------------------------------
HashMap< Widget* > Widget::sWidgets;
//---------------------------------------
Widget* Widget::LoadWidget( const char* file )
{
	XmlReader reader( file );
	if ( reader.Fail() )
		return 0;
	XmlReader::XmlReaderIterator itr = reader.ReadRoot();
	std::string name = itr.GetAttributeAsString( "name", "Widget" );
	Widget* w = new Widget( name, itr );

	for ( XmlReader::XmlReaderIterator jtr = itr.NextChild();
		  jtr.IsValid(); jtr = jtr.NextSibling() )
	{
		Widget* child = LoadComponent( w, jtr );
		if ( child )
			w->mChildren.push_back( child );
	}

	sWidgets[ w->mName ] = w;
	return w;
}
//---------------------------------------
Widget* Widget::LoadComponent( Widget* parent, const XmlReader::XmlReaderIterator& itr )
{
	Widget* w = 0;
	std::string name = itr.GetAttributeAsString( "name", "Widget" );
	if ( itr.ElementNameEquals( "Widget" ) )
	{
		w = new Widget( name, itr );
	}
	else if ( itr.ElementNameEquals( "Button" ) )
	{
		w = new Button( name, itr );
	}
	else
	{
		WarnFail( "Widget : Unknown element : %s\n", itr.ElementName() );
		return 0;
	}

	if ( w )
	{
		w->mParent = parent;
		sWidgets[ w->mName ] = w;

		for ( XmlReader::XmlReaderIterator jtr = itr.NextChild();
			  jtr.IsValid(); jtr = jtr.NextSibling() )
		{
			Widget* child = LoadComponent( w, jtr );
			if ( child )
				w->mChildren.push_back( child );
		}
	}

	return w;
}
//---------------------------------------
void Widget::DestroyWidget( Widget*& w )
{
	if ( w )
	{
		auto i = sWidgets.find( w->mName );
		if ( i != sWidgets.end() )
			sWidgets.erase( i );
		delete w;
		w = 0;
	}
}
//---------------------------------------
void Widget::DestroyAllWidgets()
{
	for ( auto itr = sWidgets.begin(); itr != sWidgets.end(); ++itr )
	{
		delete itr->second;
	}
	sWidgets.clear();
}
//---------------------------------------
void Widget::UpdateAllWidgets( float dt )
{
	for ( auto itr = sWidgets.begin(); itr != sWidgets.end(); ++itr )
	{
		itr->second->OnUpdate( dt );
	}
}
//---------------------------------------
void Widget::DrawAllWidgets( const Camera& camera )
{
	for ( auto itr = sWidgets.begin(); itr != sWidgets.end(); ++itr )
	{
		itr->second->OnDraw( camera );
	}
}
//---------------------------------------
void Widget::ProcessOnClick( float x, float y )
{
	for ( auto itr = sWidgets.begin(); itr != sWidgets.end(); ++itr )
	{
		itr->second->OnClick( x, y );
	}
}
//---------------------------------------
// Widget
//---------------------------------------
Widget::Widget( const std::string& name, const XmlReader::XmlReaderIterator& itr )
	: Object( name )
	, mSprite( 0 )
	, mParent( 0 )
{
	const char* backgroundAnim = itr.GetAttributeAsCString( "sprite", 0 );

	// Create the background
	if ( backgroundAnim )
	{
		mSprite = SpriteManager::CreateSprite( backgroundAnim, Vec2f::ZERO, "n" );
		if ( mSprite )
		{
			// Ignore camera offset
			mSprite->RelativeToCamera = false;
		}
		else
		{
			WarnFail( "Failed to create widget sprite '%s'\n", backgroundAnim );
		}
	}
}
//---------------------------------------
Widget::~Widget()
{}
//---------------------------------------
void Widget::OnUpdate( float dt )
{
	if ( mSprite )
		mSprite->OnUpdate( dt );
	for ( auto itr = mChildren.begin(); itr != mChildren.end(); ++itr )
	{
		(*itr)->OnUpdate( dt );
	}
}
//---------------------------------------
void Widget::OnDraw( const Camera& camera ) const
{
	if ( mSprite )
		mSprite->OnDraw( camera );
	for ( auto itr = mChildren.begin(); itr != mChildren.end(); ++itr )
	{
		(*itr)->OnDraw( camera );
	}
}
//---------------------------------------
bool Widget::OnClick( float x, float y )
{
	// Check children from top to bottom
	for ( auto itr = mChildren.rbegin(); itr != mChildren.rend(); ++itr )
	{
		if ( (*itr)->OnClick( x, y ) )
			return true;
	}
	return false;
}
//---------------------------------------
