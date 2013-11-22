#include <MageApp.h>
#include "Widget.h"

using namespace mage;

//---------------------------------------
Widget::Widget( const char* ui_file )
	: Object( "Widget" )
	, mSprite( 0 )
{
	XmlReader reader( ui_file );
	XmlReader::XmlReaderIterator itr = reader.ReadRoot();

	const char* backgroundAnim = itr.GetAttributeAsCString( "sprite" );

	// Create the background
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

	// Create Buttons
	for ( XmlReader::XmlReaderIterator btnItr = itr.NextChild( "Button" );
		  btnItr.IsValid(); btnItr = btnItr.NextSibling( "Button" ) )
	{
		Button b;

		const char* btnSprite = btnItr.GetAttributeAsCString( "sprite" );
		Vec2f pos = btnItr.GetAttributeAsVec2f( "offset", Vec2f::ZERO );

		b.ButtonSprite = SpriteManager::CreateSprite( btnSprite, pos );
		b.OnClickEvent = btnItr.GetAttributeAsString( "onClickEvent", "__DUMMY_EVENT__" );
		b.OnClickAnim = btnItr.GetAttributeAsString( "onClickAnim", "" );
		b.State = Button::READY;

		mButtons.push_back( b );
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
	for ( auto itr = mButtons.begin(); itr != mButtons.end(); ++itr )
	{
		itr->ButtonSprite->OnUpdate( dt );
	}
}
//---------------------------------------
void Widget::OnDraw( const Camera& camera ) const
{
	if ( mSprite )
		mSprite->OnDraw( camera );
	for ( auto itr = mButtons.begin(); itr != mButtons.end(); ++itr )
	{
		itr->ButtonSprite->OnDraw( camera );
	}
}
//---------------------------------------
void Widget::OnClick( float x, float y )
{
	for ( auto itr = mButtons.begin(); itr != mButtons.end(); ++itr )
	{
		Button& b = *itr;
		if ( b.ButtonSprite->GetClippingRectForCurrentAnimation().Contains( (int) x, (int) y ) )
		{
			b.ButtonSprite->PlayAnimation( b.OnClickAnim );
			EventManager::FireEvent( b.OnClickEvent );
		}
	}
}
//---------------------------------------
