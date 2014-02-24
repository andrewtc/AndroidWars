#include <MageApp.h>
#include "androidwars.h"
#include "Widget.h"
#include "Label.h"
#include "Button.h"

using namespace mage;

const RTTI Widget::TYPE( "mage.Widget", 0 );

//---------------------------------------
// Widget
//---------------------------------------
Widget::Widget( WidgetManager* manager, const HashString& name )
	: mManager( manager )
	, mIsInitialized( false )
	, mNeedsBoundsUpdate( true )
	, mName( name )
	, mSprite( nullptr )
	, mParent( nullptr )
	, mIsVisible( true )
	, mDebugLayout( true )
{
	assertion( manager, "Cannot create Widget without a WidgetManager reference!" );
}
//---------------------------------------
Widget::Widget( const Widget& other )
	: mManager( other.mManager )
	, mIsInitialized( false )
	, mNeedsBoundsUpdate( true )
	, mName( other.mName )
	, mSprite( other.mSprite )
	, mParent( nullptr )
	, mBelowWidgetName( other.mBelowWidgetName )
	, mAboveWidgetName( other.mAboveWidgetName )
	, mToLeftOfWidgetName( other.mToLeftOfWidgetName )
	, mToRightOfWidgetName( other.mToRightOfWidgetName )
	, mOffset( other.mOffset )
	, mSize( other.mSize )
	, mIsVisible( other.mIsVisible )
	, mDebugLayout( other.mDebugLayout )
{ }
//---------------------------------------
Widget::~Widget()
{
	// Destroy all children.
	DestroyMapByValue( mChildren );
}
//---------------------------------------
Widget* Widget::Clone() const
{
	// By default, return a new Widget with the same properties as this one.
	return new Widget( *this );
}
//---------------------------------------
void Widget::Init()
{
	assertion( mIsInitialized == false, "Cannot initialize a Widget that has already been initialized!" );

	// Initialize the widget.
	mIsInitialized = true;
	OnInit();
}
//---------------------------------------
WidgetManager* Widget::GetManager() const
{
	return mManager;
}
//---------------------------------------
std::string Widget::GetFullName() const
{
	std::string result;

	if( mParent )
	{
		// Add the parent's full name to the string.
		result = ( mParent->GetFullName() + "." );
	}

	// Add this Widget's name to the string.
	result += mName.GetString();

	return result;
}
//---------------------------------------
void Widget::SetSprite( Sprite* sprite )
{
	mSprite = sprite;
	InvalidateBounds();
}
//---------------------------------------
Sprite* Widget::GetSprite() const
{
	return mSprite;
}
//---------------------------------------
bool Widget::HasSprite() const
{
	return ( mSprite != nullptr );
}
//---------------------------------------
void Widget::SetFixedSizeSprite( bool isFixedSizeSprite )
{
	mIsFixedSizeSprite = isFixedSizeSprite;
	InvalidateBounds();
}
//---------------------------------------
bool Widget::IsFixedSizeSprite() const
{
	return mIsFixedSizeSprite;
}
//---------------------------------------
void Widget::OnLoadFromXML( const XmlReader::XmlReaderIterator& xml )
{
	const char* backgroundAnim = xml.GetAttributeAsCString( "sprite", 0 );

	mMargins = xml.GetAttributeAsVec4f( "margins", Vec4f::ZERO );
	mOffset = xml.GetAttributeAsVec2f( "position", Vec2f::ZERO );
	mCenterInParent = xml.GetAttributeAsBool( "layout_centerParent", false );
	mDrawColor = xml.GetAttributeAsColor( "color", Color::WHITE );
	mSize = xml.GetAttributeAsVec2f( "size", Vec2f::ZERO );

	// If size is non-zero, the Widget is a fixed size and so should the sprite be
	mIsFixedSizeSprite = ( mSize.LengthSqr() != 0.0f );

	// Create the background
	if ( backgroundAnim )
	{
		SetSprite( backgroundAnim );
	}

	// Load layout dependencies.
	mBelowWidgetName     = xml.GetAttributeAsCString( "layout_below", "" );
	mAboveWidgetName     = xml.GetAttributeAsCString( "layout_above", "" );
	mToLeftOfWidgetName  = xml.GetAttributeAsCString( "layout_toLeftOf", "" );
	mToRightOfWidgetName = xml.GetAttributeAsCString( "layout_toRightOf", "" );
}
//---------------------------------------
void Widget::OnLoadFromDictionary( const Dictionary& dictionary )
{
	const char* backgroundAnim = dictionary.Get( "sprite", (const char*) 0 );

	mMargins = dictionary.Get( "margins", Vec4f::ZERO );
	mOffset = dictionary.Get( "position", Vec2f::ZERO );
	mCenterInParent = dictionary.Get( "layout_centerParent", false );
	mDrawColor = dictionary.Get( "color", Color::WHITE );

	Vec2f size = dictionary.Get( "size", Vec2f::ZERO );

	// If size is non-zero, the Widget is a fixed size and so should the sprite be
	mIsFixedSizeSprite = size.LengthSqr() != 0;

	if( mIsFixedSizeSprite )
	{
		mSize = size;
	}

	// Create the background
	if ( backgroundAnim )
	{
		SetSprite( backgroundAnim );
	}

	// Load layout dependencies.
	mBelowWidgetName     = dictionary.Get< const char* >( "layout_below", "" );
	mAboveWidgetName     = dictionary.Get< const char* >( "layout_above", "" );
	mToLeftOfWidgetName  = dictionary.Get< const char* >( "layout_toLeftOf", "" );
	mToRightOfWidgetName = dictionary.Get< const char* >( "layout_toRightOf", "" );
}
//---------------------------------------
void Widget::OnInit()
{
	// Make sure the bounds of the object get recalculated.
	InvalidateBounds();
}
//---------------------------------------
void Widget::Update( float elapsedTime )
{
	// Run the update code.
	OnUpdate( elapsedTime );

	for( auto it = mChildren.begin(); it != mChildren.end(); ++it )
	{
		// Update all children.
		Widget* child = it->second;
		child->Update( elapsedTime );
	}

	// Update the position of the Widget.
	Vec2f position = FindPosition();

	if( mSprite )
	{
		mSprite->Position = position;
	}
}
//---------------------------------------
void Widget::OnUpdate( float elapsedTime )
{
	if( mSprite )
	{
		// If there is a background sprite, update its animation.
		mSprite->OnUpdate( elapsedTime );
	}
}
//---------------------------------------
void Widget::Draw( const Camera& camera )
{
	// If not visible, do not draw this Widget or its children.
	if( mIsVisible )
	{
		// Draw the Widget.
		OnDraw( camera );

		// Draw debug visual.
		if( mDebugLayout )
		{
			Vec2f position = FindPosition();
			DrawRectOutline( position.x, position.y, mSize.x, mSize.y, Color::PINK, 1.5f );
		}

		// Draw all children.
		for( auto it = mChildren.begin(); it != mChildren.end(); ++it )
		{
			Widget* child = it->second;
			child->Draw( camera );
		}
	}
}
//---------------------------------------
void Widget::OnDraw( const Camera& camera )
{
	// Draw the background sprite (if any).
	if( mSprite )
	{
		mSprite->OnDraw( camera );
	}
}
//---------------------------------------
bool Widget::OnPointerDown( float x, float y )
{
	// If not visible, do not process this Widget or its children
	if ( !mIsVisible )
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
	if ( !mIsVisible )
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
void Widget::AddChild( Widget* child )
{
	assertion( child, "Cannot add null child to Widget \"%s\"!", GetName().GetCString() );
	assertion( child->GetManager() == mManager, "Cannot add child \"%s\" to Widget \"%s\" because it was created by a different WidgetManager!", child->mName.GetCString(), mName.GetCString() );

	HashString parentName = ( child->mParent ? child->mParent->mName : "" );

	assertion( child->mParent == nullptr, "Cannot add child \"%s\" to Widget \"%s\" because it is already a child of Widget \"%s\"!", child->mName.GetCString(), mName.GetCString(), parentName.GetCString() );
	assertion( !HasChildWithName( child->mName ), "Could not add child \"%s\" to Widget \"%s\" because a child of the same name already exists!", child->mName.GetCString(), mName.GetCString() );

	//DebugPrintf( "Adding child \"%s\" to Widget \"%s\"...", child->GetName().GetCString(), GetName().GetCString() );

	// If a child with the specified name does not already exist, add the new child.
	mChildren[ child->mName ] = child;
	child->mParent = this;

	// Invalidate the child's position.
	child->InvalidateBounds();

	// Update dependent siblings.
	UpdateSiblingsOf( child );
}
//---------------------------------------
void Widget::RemoveChild( Widget* child )
{
	if( HasChild( child ) )
	{
		// If this Widget contains the specified child, remove it by name.
		RemoveChildByName( child->mName );
	}
}
//---------------------------------------
void Widget::RemoveChildByName( const HashString& name )
{
	auto it = mChildren.find( name );

	if( it != mChildren.end() )
	{
		Widget* child = it->second;

		// Remove the child with the specified name.
		mChildren.erase( it );

		// Update dependent siblings.
		UpdateSiblingsOf( child );
	}
	else
	{
		// If the child wasn't found, post a warning.
		WarnInfo( "Could not remove child \"%s\" from Widget \"%s\" because no child with that name was found!", name.GetCString(), mName.GetCString() );
	}
}
//---------------------------------------
bool Widget::HasChildWithName( const HashString& name ) const
{
	return ( GetChildByName( name ) != nullptr );
}
//---------------------------------------
bool Widget::HasChild( const Widget* child ) const
{
	bool result = false;

	if( child )
	{
		// Look for a child with the same name as the child passed.
		Widget* matchingChild = GetChildByName( child->mName );

		if( matchingChild && matchingChild == child )
		{
			// If the child Widget was found, return true.
			result = true;
		}
	}

	return result;
}
//---------------------------------------
void Widget::SetOffset( const Vec2f& offset )
{
	mOffset = offset;
	InvalidateBounds();
}
//---------------------------------------
Vec2f Widget::GetOffset() const
{
	return mOffset;
}
//---------------------------------------
Vec2f Widget::FindPosition()
{
	const RectF& bounds = FindBounds();
	return Vec2f( bounds.Left, bounds.Top );
}
//---------------------------------------
const RectF& Widget::FindBounds()
{
	if( mNeedsBoundsUpdate )
	{
		// If the position of this Widget or an ancestor has changed, recalculate the position.
		RecalculateBounds();
	}

	return mCalculatedBounds;
}
//---------------------------------------
void Widget::RecalculateBounds()
{
	// Calculate the position of the Widget after margins.
	Vec2f topLeft = mOffset + Vec2f( mMargins[ MARGIN_LEFT ], mMargins[ MARGIN_TOP ] );

	if( mParent )
	{
		// Apply parent offset.
		topLeft += mParent->FindPosition();

		if( mCenterInParent )
		{
			topLeft = ( mParent->GetSize() - mSize ) * 0.5f;
		}
	}

	// Find all Widgets that this one's position depends on.
	Widget* below = FindLayoutBelow();
	Widget* above = FindLayoutAbove();
	Widget* toLeftOf = FindLayoutToLeftOf();
	Widget* toRightOf = FindLayoutToRightOf();

	// Apply vertical constraints.
	if( below )
	{
		DebugPrintf( "Widget \"%s\" is below \"%s\"", GetFullName().c_str(), below->GetFullName().c_str() );
		topLeft.y = below->FindPosition().y + below->GetHeight() + below->mMargins[ MARGIN_BOTTOM ] + mMargins[ MARGIN_TOP ];
	}
	else if( above )
	{
		DebugPrintf( "Widget \"%s\" is above \"%s\"", GetFullName().c_str(), below->GetFullName().c_str() );
		topLeft.y = above->FindPosition().y - GetHeight() - above->mMargins[ MARGIN_TOP ] - mMargins[ MARGIN_BOTTOM ];
	}

	// Apply horizontal constraints.
	if( toLeftOf )
	{
		DebugPrintf( "Widget \"%s\" is to the left of \"%s\"", GetFullName().c_str(), below->GetFullName().c_str() );
		topLeft.x = toLeftOf->FindPosition().x - GetWidth() - toLeftOf->mMargins[ MARGIN_LEFT ] - mMargins[ MARGIN_RIGHT ];
	}
	else if( toRightOf )
	{
		DebugPrintf( "Widget \"%s\" is to the right of \"%s\"", GetFullName().c_str(), below->GetFullName().c_str() );
		topLeft.x = toRightOf->FindPosition().x + toRightOf->GetWidth() + toRightOf->mMargins[ MARGIN_RIGHT ] + mMargins[ MARGIN_LEFT ];
	}

	// Calculate the final bounds.
	mCalculatedBounds.Left   = topLeft.x;
	mCalculatedBounds.Top    = topLeft.y;
	mCalculatedBounds.Right  = ( topLeft.x + GetWidth() );
	mCalculatedBounds.Bottom = ( topLeft.y + GetHeight() );

	DebugPrintf( "Recalculated bounds for \"%s\": {Left: %.2f, Top: %.2f, Right: %.2f, Bottom: %.2f}",
				 GetFullName().c_str(),
				 mCalculatedBounds.Left,
				 mCalculatedBounds.Top,
				 mCalculatedBounds.Right,
				 mCalculatedBounds.Bottom );

	// Flag the position as updated.
	mNeedsBoundsUpdate = false;
}
//---------------------------------------
void Widget::UpdateSiblingsOf( Widget* child )
{
	assertion( child, "Cannot update siblings of null child Widget!" );
	assertion( HasChild( child ), "Cannot update siblings of Widget that is not a child!" );

	for( auto it = mChildren.begin(); it != mChildren.end(); ++it )
	{
		Widget* sibling = it->second;

		if( sibling != child && sibling->IsRelativeToSiblingWithName( child->mName ) )
		{
			// Invalidate the bounds of all siblings that depend on the child.
			sibling->InvalidateBounds();
		}
	}
}
//---------------------------------------
void Widget::InvalidateBounds()
{
	if( mNeedsBoundsUpdate == false )
	{
		// Flag this object as needing a position update.
		mNeedsBoundsUpdate = true;

		for( auto it = mChildren.begin(); it != mChildren.end(); ++it )
		{
			// Flag each child as needing an update.
			Widget* child = it->second;
			child->InvalidateBounds();
		}
	}
}
//---------------------------------------
void Widget::SetWidth( float width )
{
	mSize.x = width;
	InvalidateBounds();
}
//---------------------------------------
float Widget::GetWidth() const
{
	return mSize.x;
}
//---------------------------------------
void Widget::SetHeight( float height )
{
	mSize.y = height;
	InvalidateBounds();
}
//---------------------------------------
float Widget::GetHeight() const
{
	return mSize.y;
}
//---------------------------------------
void Widget::SetSize( float width, float height )
{
	mSize.x = width;
	mSize.y = height;
	InvalidateBounds();
}
//---------------------------------------
void Widget::SetSize( const Vec2f& size )
{
	mSize = size;
	InvalidateBounds();
}
//---------------------------------------
Vec2f Widget::GetSize() const
{
	return mSize;
}
//---------------------------------------
void Widget::SetSprite( const HashString& spriteName )
{
	mSprite = SpriteManager::CreateSprite( spriteName, Vec2f::ZERO, "n" );

	if ( mSprite )
	{
		// Ignore camera offset
		mSprite->RelativeToCamera = false;
		mSprite->FixedSize = mIsFixedSizeSprite;
		mSprite->DrawColor = mDrawColor;

		if ( !mIsFixedSizeSprite )
		{
			// If this Widget has a fixed-size Sprite, update the Widget dimensions.
			const RectI& r = mSprite->GetClippingRectForCurrentAnimation();
			SetSize( r.Width(), r.Height() );
		}
		else
		{
			// If the Sprite should scale with the Widget, set the width and height of the Sprite.
			mSprite->Size.Set( mSize );
		}
	}
	else
	{
		WarnFail( "Failed to create widget sprite '%s'\n", spriteName.GetString().c_str() );
	}

	InvalidateBounds();
}
//---------------------------------------
bool Widget::Contains( float x, float y )
{
	return FindBounds().Contains( (int) x, (int) y );
}
//---------------------------------------
void Widget::SetLayoutAbove( const HashString& widgetName )
{
	mAboveWidgetName = widgetName;
}
//---------------------------------------
void Widget::SetLayoutBelow( const HashString& widgetName )
{
	mBelowWidgetName = widgetName;
}
//---------------------------------------
void Widget::SetLayoutToLeftOf( const HashString& widgetName )
{
	mToLeftOfWidgetName = widgetName;
}
//---------------------------------------
void Widget::SetLayoutToRightOf( const HashString& widgetName )
{
	mToRightOfWidgetName = widgetName;
}
//---------------------------------------
bool Widget::HasSibling( const Widget* sibling ) const
{
	// Only return true if both Widgets have the same parent.
	return ( sibling->mParent == mParent );
}
//---------------------------------------
bool Widget::HasSiblingWithName( const HashString& siblingName ) const
{
	// Only return true if this Widget has a parent and that parent has a child with a matching name.
	return ( mParent && mParent->HasChildWithName( siblingName ) );
}
//---------------------------------------
bool Widget::IsRelativeToSibling( const Widget* sibling ) const
{
	bool result = false;

	if( HasSibling( sibling ) )
	{
		// Only return true if the Widget is a sibling of this one and this one depends on it.
		result = IsRelativeToSiblingWithName( sibling->mName );
	}

	return result;
}
//---------------------------------------
bool Widget::IsRelativeToSiblingWithName( const HashString& siblingName ) const
{
	return ( mBelowWidgetName == siblingName ) ||
		   ( mAboveWidgetName == siblingName ) ||
		   ( mToLeftOfWidgetName == siblingName ) ||
		   ( mToRightOfWidgetName == siblingName );
}
//---------------------------------------
void Widget::SetVisible( bool isVisible )
{
	mIsVisible = isVisible;
}
//---------------------------------------
bool Widget::IsVisible() const
{
	return mIsVisible;
}
