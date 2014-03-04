#include <MageApp.h>
#include "androidwars.h"

using namespace mage;

const RTTI Widget::TYPE( "mage.Widget", 0 );

//---------------------------------------
// Widget
//---------------------------------------
Widget::Widget( WidgetManager* manager, const HashString& name )
	: mManager( manager )
	, mIsInitialized( false )
	, mPositionNeedsUpdate( true )
	, mDrawOrderNeedsUpdate( true )
	, mName( name )
	, mParent( nullptr )
	, mIsVisible( true )
	, mDebugLayout( true )
	, mDrawLayer( 0 )
{
	assertion( manager, "Cannot create Widget without a WidgetManager reference!" );
}
//---------------------------------------
Widget::Widget( const Widget& other )
	: mManager( other.mManager )
	, mIsInitialized( false )
	, mPositionNeedsUpdate( true )
	, mDrawOrderNeedsUpdate( true )
	, mName( other.mName )
	, mParent( nullptr )
	, mOffset( other.mOffset )
	, mSize( other.mSize )
	, mIsVisible( other.mIsVisible )
	, mDebugLayout( other.mDebugLayout )
	, mDrawLayer( other.mDrawLayer )
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
void Widget::LoadFromTemplate( const WidgetTemplate& widgetTemplate )
{
	assertion( mIsInitialized == false, "Cannot load Widget from template because it has already been initialized!" );
	OnLoadFromTemplate( widgetTemplate );
}
//---------------------------------------
void Widget::OnLoadFromTemplate( const WidgetTemplate& widgetTemplate )
{
	// Parse all valid properties.
	mOffset = widgetTemplate.GetPropertyAsVec2f( "position", Vec2f::ZERO );
	mIsVisible = widgetTemplate.GetPropertyAsBool( "isVisible", true );
	SetSize( widgetTemplate.GetPropertyAsVec2f( "size", Vec2f::ZERO ) );
	SetDrawLayer( widgetTemplate.GetPropertyAsInt( "layer", 0 ) );
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
void Widget::OnInit() { }
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
void Widget::Update( float elapsedTime )
{
	// Run the update event.
	OnUpdate( elapsedTime );

	for( auto it = mChildren.begin(); it != mChildren.end(); ++it )
	{
		// Update all children.
		Widget* child = it->second;
		child->Update( elapsedTime );
	}
}
//---------------------------------------
void Widget::OnUpdate( float elapsedTime ) { }
//---------------------------------------
void Widget::Draw( const Camera& camera )
{
	// If not visible, do not draw this Widget or its children.
	if( mIsVisible )
	{
		// Draw the Widget.
		OnDraw( camera );

		if( mDebugLayout )
		{
			// Draw debug visual.
			RectF bounds = CalculateBounds();
			DrawRectOutline( bounds.Left, bounds.Top, bounds.Width(), bounds.Height(), Color::PINK, 1.5f );
		}
	}
}
//---------------------------------------
void Widget::OnDraw( const Camera& camera )
{
	// Update the draw order of all children (if necessary).
	UpdateDrawOrder();

	for( auto layerIt = mChildrenByDrawLayer.begin(); layerIt != mChildrenByDrawLayer.end(); ++layerIt )
	{
		const std::vector< Widget* > layer = layerIt->second;

		for( auto childIt = layer.begin(); childIt != layer.end(); ++childIt )
		{
			// Draw all children in order of their draw layer.
			Widget* child = ( *childIt );
			child->Draw( camera );
		}
	}
}
//---------------------------------------
bool Widget::PointerDown( float x, float y )
{
	DebugPrintf( "Firing PointerDown event for \"%s\".", GetFullName().c_str() );
	return OnPointerDown( x, y );
}
//---------------------------------------
bool Widget::OnPointerDown( float x, float y )
{
	return false;
}
//---------------------------------------
bool Widget::PointerUp( float x, float y )
{
	DebugPrintf( "Firing PointerUp event for \"%s\".", GetFullName().c_str() );
	return OnPointerUp( x, y );
}
//---------------------------------------
bool Widget::OnPointerUp( float x, float y )
{
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

	// Resort all children by their draw order.
	InvalidateDrawOrder();
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

		// Resort all children by their draw order.
		InvalidateDrawOrder();
	}
	else
	{
		// If the child wasn't found, post a warning.
		WarnFail( "Could not remove child \"%s\" from Widget \"%s\" because no child with that name was found!", name.GetCString(), mName.GetCString() );
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
	InvalidatePosition();
}
//---------------------------------------
Vec2f Widget::GetOffset() const
{
	return mOffset;
}
//---------------------------------------
Vec2f Widget::CalculatePosition()
{
	UpdatePosition();
	return mCalculatedPosition;
}
//---------------------------------------
void Widget::UpdatePosition()
{
	if( mPositionNeedsUpdate )
	{
		//DebugPrintf( "Updating position of \"%s\"...", mName.GetCString() );

		// Reset the calculated position of the Widget.
		mCalculatedPosition = mOffset;

		if( mParent != nullptr )
		{
			// If this Widget has a parent, add its calculated position as an offset.
			mCalculatedPosition += mParent->CalculatePosition();
		}

		// Mark that the position is up-to-date.
		mPositionNeedsUpdate = false;
	}
}
//---------------------------------------
void Widget::InvalidatePosition()
{
	// Invalidate the position for this Widget.
	mPositionNeedsUpdate = true;

	for( auto it = mChildren.begin(); it != mChildren.end(); ++it )
	{
		// Invalidate the position of all children.
		Widget* child = it->second;
		child->InvalidatePosition();
	}
}
//---------------------------------------
RectF Widget::CalculateBounds()
{
	UpdatePosition();
	return RectF( mCalculatedPosition.x,
				  mCalculatedPosition.y,
				  mCalculatedPosition.x + mSize.x,
				  mCalculatedPosition.y + mSize.y );
}
//---------------------------------------
RectF Widget::GetBoundsInParent() const
{
	return RectF( mOffset.x,
				  mOffset.y,
				  mOffset.x + mSize.x,
				  mOffset.y + mSize.y );
}
//---------------------------------------
void Widget::SetWidth( float width )
{
	// Make sure the width of the Widget is always positive.
	mSize.x = std::max( 0.0f, width );
}
//---------------------------------------
float Widget::GetWidth() const
{
	return mSize.x;
}
//---------------------------------------
void Widget::SetHeight( float height )
{
	// Make sure the height of the Widget is always positive.
	mSize.y = std::max( 0.0f, height );
}
//---------------------------------------
float Widget::GetHeight() const
{
	return mSize.y;
}
//---------------------------------------
void Widget::SetSize( float width, float height )
{
	SetWidth( width );
	SetHeight( height );
}
//---------------------------------------
void Widget::SetSize( const Vec2f& size )
{
	SetWidth( size.x );
	SetHeight( size.y );
}
//---------------------------------------
Vec2f Widget::GetSize() const
{
	return mSize;
}
//---------------------------------------
bool Widget::Contains( float x, float y )
{
	return CalculateBounds().Contains( (int) x, (int) y );
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
void Widget::SetVisible( bool isVisible )
{
	mIsVisible = isVisible;
}
//---------------------------------------
bool Widget::IsVisible() const
{
	return mIsVisible;
}
//---------------------------------------
void Widget::SetDrawLayer( int drawLayer )
{
	mDrawLayer = drawLayer;

	if( mParent )
	{
		mParent->InvalidateDrawOrder();
	}
}
//---------------------------------------
int Widget::GetDrawLayer() const
{
	return mDrawLayer;
}
//---------------------------------------
void Widget::InvalidateDrawOrder()
{
	mDrawOrderNeedsUpdate = true;
}
//---------------------------------------
void Widget::UpdateDrawOrder()
{
	if( mDrawOrderNeedsUpdate )
	{
		//DebugPrintf( "Updating draw order for \"%s\"...", mName.GetCString() );

		// Clear the draw order list.
		mChildrenByDrawLayer.clear();

		for( auto it = mChildren.begin(); it != mChildren.end(); ++it )
		{
			// Add each child to the draw order list, sorted by its draw layer.
			Widget* child = it->second;
			mChildrenByDrawLayer[ child->mDrawLayer ].push_back( child );
		}

		// Mark that the draw order list is up-to-date.
		mDrawOrderNeedsUpdate = false;
	}
}
//---------------------------------------
