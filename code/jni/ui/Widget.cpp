#include <MageApp.h>
#include "androidwars.h"
#include "Widget.h"
#include "Label.h"
#include "Button.h"

using namespace mage;

const RTTI Widget::TYPE( "mage.Widget", 0 );

//---------------------------------------
// Static
//---------------------------------------
HashMap< Widget* > Widget::sWidgets;
HashMap< BitmapFont* > Widget::sFonts;
//---------------------------------------
Widget* Widget::LoadWidget( const char* file )
{
	XmlReader reader( file );
	if ( reader.Fail() )
		return 0;
	XmlReader::XmlReaderIterator itr = reader.ReadRoot();
	std::string name = itr.GetAttributeAsString( "name", "Widget" );
	Widget* w = new Widget( name, 0 );
	w->LoadFromXML( itr );

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
		w = new Widget( name, parent );
	}
	else if ( itr.ElementNameEquals( "Button" ) )
	{
		w = new Button( name, parent );
	}
	else if ( itr.ElementNameEquals( "Label" ) )
	{
		w = new Label( name, parent );
	}
	else if ( itr.ElementNameEquals( "TextField" ) )
	{
		w = new TextField( name, parent );
	}
	else
	{
		WarnFail( "Widget : Unknown element : %s\n", itr.ElementName() );
		return 0;
	}

	// Load the component from XML.
	w->LoadFromXML( itr );

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
		Widget* w = itr->second;
//		if ( w->Visible )
			w->OnUpdate( dt );
	}
}
//---------------------------------------
void Widget::DrawAllWidgets( const Camera& camera )
{
	for ( auto itr = sWidgets.begin(); itr != sWidgets.end(); ++itr )
	{
		Widget* w = itr->second;
		if ( w->Visible )
			w->OnDraw( camera );
	}
}
//---------------------------------------
bool Widget::ProcessOnPointerDown( float x, float y )
{
	bool ret = false;
	for ( auto itr = sWidgets.begin(); itr != sWidgets.end(); ++itr )
	{
		Widget* w = itr->second;
		if ( w->Visible && ( w->OnPointerDown( x, y ) || w->Contains( x, y ) ) )
			ret = true;
	}
	return ret;
}
//---------------------------------------
bool Widget::ProcessOnPointerUp( float x, float y )
{
	bool ret = false;
	for ( auto itr = sWidgets.begin(); itr != sWidgets.end(); ++itr )
	{
		Widget* w = itr->second;
		if ( w->Visible && w->OnPointerUp( x, y ) )
			ret = true;
	}
	return ret;
}
//---------------------------------------
void Widget::LoadDefinitions( const char* file )
{
	XmlReader reader( file );
	XmlReader::XmlReaderIterator root = reader.ReadRoot();

	for ( XmlReader::XmlReaderIterator itr = root.NextChild();
		  itr.IsValid(); itr = itr.NextSibling() )
	{
		// Fonts to load
		if ( itr.ElementNameEquals( "Font" ) )
		{
			const char* file = itr.GetAttributeAsCString( "file" );
			const char* name = itr.GetAttributeAsCString( "name" );
			BitmapFont* fnt = new BitmapFont( file );
			sFonts[ name ] = fnt;
		}
		// Sprites to load
		else if ( itr.ElementNameEquals( "Sprite" ) )
		{
			const char* file = itr.GetAttributeAsCString( "file" );
			SpriteManager::LoadSpriteAnimations( file, false );
		}
		// Button styles
		else if ( itr.ElementNameEquals( "ButtonStyle" ) )
		{
			HashString name = itr.GetAttributeAsString( "name" );
			Button::ButtonStyle* style = Button::CreateButtonStyle( name );

			style->SpriteName      = itr.GetAttributeAsString( "sprite" );
			style->DefaultAnimName = itr.GetAttributeAsString( "default", "d" );
			style->PressedAnimName = itr.GetAttributeAsString( "pressed", "p" );
			style->SelectedAnimName= itr.GetAttributeAsString( "selected", "s" );
			style->WrapText        = itr.GetAttributeAsBool( "wrapText", true );
			style->PressedColor    = itr.GetAttributeAsColor( "pressedColor", Color::WHITE );
			style->SelectedSFXName = itr.GetAttributeAsString( "clickSFX", "" );
			style->DisabledColor   = itr.GetAttributeAsColor( "disabledColor", Color::GREY );
		}
		// Sounds
		else if ( itr.ElementNameEquals( "Sound" ) )
		{
			const char* file = itr.GetAttributeAsCString( "file" );
			const char* name = itr.GetAttributeAsCString( "name" );
			SoundClip* clip = gSoundManager->LoadSoundClip( file, name );
			if ( !clip )
			{
				WarnFail( "Failed to load sound %s as %s\n", file, name );
			}
		}
	}

	DebugPrintf( "Widget : Loaded UI definitions" );
}
//---------------------------------------
BitmapFont* Widget::GetFontByName( const HashString& name )
{
	BitmapFont* fnt = 0;
	auto itr = sFonts.find( name );
	if ( itr != sFonts.end() )
	{
		fnt = itr->second;
	}
	return fnt;
}
//---------------------------------------
// Widget
//---------------------------------------
Widget::Widget( const std::string& name, Widget* parent )
	: mName( name )
	, mSprite( 0 )
	, mParent( parent )
	, mBelow( 0 )
	, mAbove( 0 )
	, mToLeftOf( 0 )
	, mToRightOf( 0 )
	, mHeight( 0 )
	, mWidth( 0 )
	, Visible( true )
	, DebugLayout( false )
{

}
//---------------------------------------
Widget::~Widget()
{
	DestroyMapByValue( mChildren );
}
//---------------------------------------
void Widget::LoadFromXML( const XmlReader::XmlReaderIterator& xml )
{
	// Load the widget from XML.
	OnLoadFromXML( xml );

	// Initialize the widget.
	OnInit();
}
//---------------------------------------
void Widget::LoadFromDictionary( const Dictionary& dictionary )
{
	// Load the widget from the dictionary.
	OnLoadFromDictionary( dictionary );

	// Initialize the widget.
	OnInit();
}
//---------------------------------------
void Widget::OnLoadFromXML( const XmlReader::XmlReaderIterator& xml )
{
	const char* backgroundAnim = xml.GetAttributeAsCString( "sprite", 0 );

	mMargins = xml.GetAttributeAsVec4f( "margins", Vec4f::ZERO );
	mPosition = xml.GetAttributeAsVec2f( "position", Vec2f::ZERO );
	mCenterInParent = xml.GetAttributeAsBool( "layout_centerParent", false );
	mDrawColor = xml.GetAttributeAsColor( "color", Color::WHITE );

	Vec2f size = xml.GetAttributeAsVec2f( "size", Vec2f::ZERO );

	// If size is non-zero, the Widget is a fixed size and so should the sprite be
	mFixedSizeSprite = size.LengthSqr() != 0;
	if ( mFixedSizeSprite )
	{
		mWidth = size.x;
		mHeight = size.y;
	}

	// Create the background
	if ( backgroundAnim )
	{
		SetSprite( backgroundAnim );
	}

	DebugPrintf( "Widget : Created '%s' w=%.3f h=%.3f\n", mName.GetString().c_str(), mWidth, mHeight );

	// Layout
	if ( mParent )
	{
		mBelow     = GetLayoutWidget( xml.GetAttributeAsCString( "layout_below", nullptr ) );
		mAbove     = GetLayoutWidget( xml.GetAttributeAsCString( "layout_above", nullptr ) );
		mToLeftOf  = GetLayoutWidget( xml.GetAttributeAsCString( "layout_toLeftOf", nullptr ) );
		mToRightOf = GetLayoutWidget( xml.GetAttributeAsCString( "layout_toRightOf", nullptr ) );
	}
}
//---------------------------------------
void Widget::OnLoadFromDictionary( const Dictionary& dictionary )
{
	const char* backgroundAnim = dictionary.Get( "sprite", (const char*) 0 );

	mMargins = dictionary.Get( "margins", Vec4f::ZERO );
	mPosition = dictionary.Get( "position", Vec2f::ZERO );
	mCenterInParent = dictionary.Get( "layout_centerParent", false );
	mDrawColor = dictionary.Get( "color", Color::WHITE );

	Vec2f size = dictionary.Get( "size", Vec2f::ZERO );

	// If size is non-zero, the Widget is a fixed size and so should the sprite be
	mFixedSizeSprite = size.LengthSqr() != 0;
	if ( mFixedSizeSprite )
	{
		mWidth = size.x;
		mHeight = size.y;
	}

	// Create the background
	if ( backgroundAnim )
	{
		SetSprite( backgroundAnim );
	}

	DebugPrintf( "Widget : Created '%s' w=%.3f h=%.3f\n", mName.GetString().c_str(), mWidth, mHeight );

	// Layout
	if ( mParent )
	{
		mBelow     = GetLayoutWidget( dictionary.Get< const char* >( "layout_below", nullptr ) );
		mAbove     = GetLayoutWidget( dictionary.Get< const char* >( "layout_above", nullptr ) );
		mToLeftOf  = GetLayoutWidget( dictionary.Get< const char* >( "layout_toLeftOf", nullptr ) );
		mToRightOf = GetLayoutWidget( dictionary.Get< const char* >( "layout_toRightOf", nullptr ) );
	}
}
//---------------------------------------
void Widget::OnInit() { }
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
	// If not visible, do not draw this Widget or its children
	if ( !Visible )
		return;

	// Sprite background
	if ( mSprite )
		mSprite->OnDraw( camera );

	// Layout debug visual
	if ( DebugLayout )
	{
		Vec2f pos = GetPosition();
		DrawRectOutline( pos.x, pos.y, mWidth, mHeight, Color::PINK, 1.5f );
	}

	// Draw children
	for ( auto itr = mChildren.begin(); itr != mChildren.end(); ++itr )
	{
		itr->second->OnDraw( camera );
	}
}
//---------------------------------------
bool Widget::OnPointerDown( float x, float y )
{
	// If not visible, do not process this Widget or its children
	if ( !Visible )
		return false;

	// Check children from top to bottom
	for ( auto itr = mChildren.rbegin(); itr != mChildren.rend(); ++itr )
	{
		if ( itr->second->OnPointerDown( x, y ) )
			return true;
	}
	return false;
}
//---------------------------------------
bool Widget::OnPointerUp( float x, float y )
{
	// If not visible, do not process this Widget or its children
	if ( !Visible )
		return false;

	// Check children from top to bottom
	for ( auto itr = mChildren.rbegin(); itr != mChildren.rend(); ++itr )
	{
		if ( itr->second->OnPointerUp( x, y ) )
			return true;
	}
	return false;
}
//---------------------------------------
Widget* Widget::GetChildByName( const HashString& name )
{
	Widget* child = 0;
	Widget* p = mParent ? mParent : this;
	auto i = p->mChildren.find( name );
	if ( i != p->mChildren.end() )
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
	Vec2f pos = mPosition + Vec2f( mMargins[MARGIN_LEFT], mMargins[MARGIN_TOP] );

	// Parent offset
	if ( mParent )
	{
		pos += mParent->GetPosition();
	}

	// Layout adjustments
	if ( mBelow )
	{
		pos.y = mBelow->GetPosition().y + mBelow->mHeight + mBelow->mMargins[MARGIN_BOTTOM] + mMargins[MARGIN_TOP];
	}
	else if ( mAbove )
	{
		pos.y = mAbove->GetPosition().y - mHeight - mAbove->mMargins[MARGIN_TOP] - mMargins[MARGIN_BOTTOM];
	}

	if ( mToLeftOf )
	{
		pos.x = mToLeftOf->GetPosition().x - mWidth - mToLeftOf->mMargins[MARGIN_LEFT] - mMargins[MARGIN_RIGHT];
	}
	else if ( mToRightOf )
	{
		pos.x = mToRightOf->GetPosition().x + mToRightOf->mWidth + mToRightOf->mMargins[MARGIN_RIGHT] + mMargins[MARGIN_LEFT];
	}

	return pos;
}
//---------------------------------------
Widget* Widget::GetLayoutWidget( const char* name )
{
	Widget* result = nullptr;

	if ( name )
	{
		result = GetChildByName( name );
	}
	/*else
	{
		WarnInfo( "Widget %s : no such param '%s'\n", mName.GetString().c_str(), paramName );
	}*/

	return result;
}
//---------------------------------------
float Widget::GetWidth() const
{
	return mWidth + mMargins[0] + mMargins[2];
}
//---------------------------------------
float Widget::GetHeight() const
{
	return mHeight + mMargins[1] + mMargins[3];
}
//---------------------------------------
void Widget::SetSprite( const HashString& spriteName )
{
	mSprite = SpriteManager::CreateSprite( spriteName, Vec2f::ZERO, "n" );
	if ( mSprite )
	{
		// Ignore camera offset
		mSprite->RelativeToCamera = false;
		mSprite->FixedSize = mFixedSizeSprite;
		mSprite->DrawColor = mDrawColor;

		if ( !mFixedSizeSprite )
		{
			const RectI& r = mSprite->GetClippingRectForCurrentAnimation();
			mHeight = r.Height();
			mWidth = r.Width();
		}
		else
		{
			mSprite->Size.Set( mWidth, mHeight );
		}

		// Update the layout offsets
		UpdateLayout();
	}
	else
	{
		WarnFail( "Failed to create widget sprite '%s'\n", spriteName.GetString().c_str() );
	}
}
//---------------------------------------
void Widget::UpdateLayout()
{
	if ( mCenterInParent )
	{
		if ( mParent )
		{
			mPosition.x = ( mParent->mWidth - mWidth ) / 2.0f;
			mPosition.y = ( mParent->mHeight - mHeight ) / 2.0f;
		}
		// TODO should center in screen if no parent
	}
}
//---------------------------------------
Widget* Widget::GetRootWidget()
{
	Widget* root = this;
	while ( root->mParent )
		root = root->mParent;
	return root;
}
//---------------------------------------
bool Widget::Contains( float x, float y )
{
	Widget* root = GetRootWidget();
	Vec2f pos = root->GetPosition();
	RectF r( pos.x, pos.y, pos.x + root->mWidth, pos.y + root->mHeight );
	return r.Contains( (int) x, (int) y );
}
//---------------------------------------
