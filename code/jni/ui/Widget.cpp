#include <MageApp.h>
#include "Widget.h"
#include "Label.h"
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
	Widget* w = new Widget( name, itr, 0 );

	for ( XmlReader::XmlReaderIterator jtr = itr.NextChild();
		  jtr.IsValid(); jtr = jtr.NextSibling() )
	{
		Widget* child = LoadComponent( w, jtr );
		if ( child )
			w->mChildren[ child->mName ] = child;
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
		w = new Widget( name, itr, parent );
	}
	else if ( itr.ElementNameEquals( "Button" ) )
	{
		w = new Button( name, itr, parent );
	}
	else if ( itr.ElementNameEquals( "Label" ) )
	{
		w = new Label( name, itr, parent );
	}
	else
	{
		WarnFail( "Widget : Unknown element : %s\n", itr.ElementName() );
		return 0;
	}

	if ( w )
	{
		for ( XmlReader::XmlReaderIterator jtr = itr.NextChild();
			  jtr.IsValid(); jtr = jtr.NextSibling() )
		{
			Widget* child = LoadComponent( w, jtr );
			if ( child )
				w->mChildren[ child->mName ] = child;
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
Widget::Widget( const std::string& name, const XmlReader::XmlReaderIterator& itr, Widget* parent )
	: mName( name )
	, mSprite( 0 )
	, mParent( parent )
	, mBelow( 0 )
	, mAbove( 0 )
	, mToLeftOf( 0 )
	, mToRightOf( 0 )
	, mHeight( 0 )
	, mWidth( 0 )
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

			const RectI& r = mSprite->GetClippingRectForCurrentAnimation();
			mHeight = r.Height();
			mWidth = r.Width();
		}
		else
		{
			WarnFail( "Failed to create widget sprite '%s'\n", backgroundAnim );
		}
	}

	DebugPrintf( "Widget : Created '%s' w=%.3f h=%.3f\n", mName.GetString().c_str(), mWidth, mHeight );

	// Layout
	if ( mParent )
	{
		LoadLayoutParam( mBelow, itr, "layout_below" );
		LoadLayoutParam( mAbove, itr, "layout_above" );
		LoadLayoutParam( mToLeftOf, itr, "layout_toLeftOf" );
		LoadLayoutParam( mToRightOf, itr, "layout_toRightOf" );
	}

	mPosition = itr.GetAttributeAsVec2f( "position", Vec2f::ZERO );
}
//---------------------------------------
Widget::~Widget()
{
	DestroyMapByValue( mChildren );
}
//---------------------------------------
void Widget::OnUpdate( float dt )
{
	if ( mSprite )
	{
		mSprite->OnUpdate( dt );
		// Transform position
		mSprite->Position = GetPosition();
	}
	for ( auto itr = mChildren.begin(); itr != mChildren.end(); ++itr )
	{
		itr->second->OnUpdate( dt );
	}
}
//---------------------------------------
void Widget::OnDraw( const Camera& camera ) const
{
	if ( mSprite )
		mSprite->OnDraw( camera );
	for ( auto itr = mChildren.begin(); itr != mChildren.end(); ++itr )
	{
		itr->second->OnDraw( camera );
	}
}
//---------------------------------------
bool Widget::OnClick( float x, float y )
{
	// Check children from top to bottom
	for ( auto itr = mChildren.rbegin(); itr != mChildren.rend(); ++itr )
	{
		if ( itr->second->OnClick( x, y ) )
			return true;
	}
	return false;
}
//---------------------------------------
Widget* Widget::GetChildByName( const HashString& name )
{
	Widget* child = 0;
	auto i = mParent->mChildren.find( name );
	if ( i != mParent->mChildren.end() )
	{
		child = i->second;
	}
	else
	{
		WarnInfo( "Widget %s : no such child '%s'\n", mName.GetString().c_str(), name.GetString().c_str() );
	}
	return child;
}
//---------------------------------------
Vec2f Widget::GetPosition() const
{
	Vec2f pos = mPosition;

	// Layout adjustments
	if ( mBelow )
	{
		pos.y = mBelow->mPosition.y + mBelow->mHeight;
	}
	else if ( mAbove )
	{
		pos.y = mAbove->mPosition.y - mHeight;
	}

	if ( mToLeftOf )
	{
		pos.x = mToLeftOf->mPosition.x - mWidth;
	}
	else if ( mToRightOf )
	{
		pos.x = mToRightOf->mPosition.x + mToRightOf->mWidth;
	}

	// Parent offset
	if ( mParent )
	{
		pos += mParent->GetPosition();
	}
	return pos;
}
//---------------------------------------
void Widget::LoadLayoutParam( Widget*& target, const XmlReader::XmlReaderIterator& itr, const char* paramName )
{
	const char* param = itr.GetAttributeAsCString( paramName, 0 );
	if ( param )
	{
		target = GetChildByName( param );
	}
	/*else
	{
		WarnInfo( "Widget %s : no such param '%s'\n", mName.GetString().c_str(), paramName );
	}*/
}
//---------------------------------------
