#include "androidwars.h"

using namespace mage;


MAGE_IMPLEMENT_RTTI( Widget, ListLayout );

const char* const ListLayout::DIRECTION_HORIZONTAL_NAME = "horizontal";
const char* const ListLayout::DIRECTION_VERTICAL_NAME   = "vertical";

const std::string ListLayout::ITEM_PREFIX = "item";


ListLayout::Direction ListLayout::GetDirectionByName( const HashString& name )
{
	// Use a vertical layout, by default.
	Direction direction = DIRECTION_VERTICAL;

	if( name == DIRECTION_HORIZONTAL_NAME )
	{
		direction = DIRECTION_HORIZONTAL;
	}
	else if( name == DIRECTION_VERTICAL_NAME )
	{
		direction = DIRECTION_VERTICAL;
	}
	else
	{
		WarnFail( "Invalid ListLayout direction name \"%s\"!", name.GetCString() );
	}

	return direction;
}


ListLayout::ListLayout( WidgetManager* manager, const HashString& name ) :
	Widget( manager, name ),
	mNextItemID( 0 ),
	mDirection( DIRECTION_VERTICAL )
{ }


ListLayout::~ListLayout() { }


void ListLayout::DestroyItem( Widget* item )
{
	assertion( item, "Cannot remove null Widget from ListLayout \"%s\" items!", GetFullName().c_str() );
	assertion( item->GetParent() == this, "Cannot remove Widget \"%s\" from ListLayout \"%s\" items because it is not a child of this ListLayout!", item->GetFullName().c_str(), GetFullName().c_str() );

	// Get the index of the item.
	int index = GetIndexOfItem( item );
	assertion( index >= 0, "Could not remove Widget \"%s\" from ListLayout \"%s\" items because it is not a list item!", item->GetFullName().c_str(), GetFullName().c_str() );

	// If the item was found in the list, remove it.
	DestroyItemAtIndex( index );
}


void ListLayout::DestroyItemAtIndex( int index )
{
	assertion( index >= 0 && index < mItems.size(), "Cannot remove item %d from ListLayout \"%s\" items because the index is out of bounds!", index, GetFullName().c_str() );

	// Get the item at the passed index.
	auto it = ( mItems.begin() + index );
	HashString name = ( *it );

	// Get the item to remove.
	Widget* itemToRemove = GetChildByName( name );
	assertion( itemToRemove, "Cannot remove item %d from ListLayout \"%s\" items because no child Widget with the name \"%s\" exists!", index, GetFullName().c_str(), name.GetCString() );

	// Remove the item from the list.
	mItems.erase( it );

	// Remove and destroy the item.
	RemoveChildByName( name );
	GetManager()->DestroyWidget( itemToRemove );

	// Invalidate the list.
	InvalidateListItems();
}


void ListLayout::DestroyAllItems()
{
	for( int i = ( mItems.size() - 1 ); i >= 0; --i )
	{
		// Remove each item from the list.
		DestroyItemAtIndex( i );
	}
}


int ListLayout::GetIndexOfItem( const Widget* item ) const
{
	assertion( item, "Cannot get list item index of null Widget from ListLayout \"%s\" items!", GetFullName().c_str() );
	assertion( item->GetParent() == this, "Cannot get list item index of Widget \"%s\" from ListLayout \"%s\" because it is not a child of this ListLayout!", item->GetFullName().c_str(), GetFullName().c_str() );

	int index = GetIndexOfItemWithName( item->GetName() );
	assertion( index >= 0, "Cannot get list item index of Widget \"%s\" from ListLayout \"%s\" because it is not an item of this ListLayout!", item->GetFullName().c_str(), GetFullName().c_str() );

	return index;
}


int ListLayout::GetIndexOfItemWithName( const HashString& name ) const
{
	int result = -1;

	for( auto it = mItems.begin(); it != mItems.end(); ++it )
	{
		if( ( *it ) == name )
		{
			// Find the index of the item in the list.
			result = ( it - mItems.begin() );
			break;
		}
	}

	// If the element wasn't found, the result will be -1.
	return result;
}


bool ListLayout::HasItem( const Widget* item ) const
{
	return ( item && item->GetParent() == this && HasItemWithName( item->GetName() ) );
}


bool ListLayout::HasItemWithName( const HashString& item ) const
{
	return ( GetIndexOfItemWithName( item ) >= 0 );
}


size_t ListLayout::GetItemCount() const
{
	return mItems.size();
}


void ListLayout::OnLoadFromTemplate( const WidgetTemplate& widgetTemplate )
{
	// Allow the Widget to load itself.
	Widget::OnLoadFromTemplate( widgetTemplate );

	// Load the direction for the layout.
	HashString directionName = widgetTemplate.GetProperty( "direction", "vertical", false );
	mDirection = GetDirectionByName( directionName );
}


void ListLayout::OnDraw( const Camera& camera )
{
	// Update the list item positions.
	UpdateListItems();

	// Draw the Widget.
	Widget::OnDraw( camera );
}


void ListLayout::InvalidateListItems()
{
	mNeedsListUpdate = true;
}


void ListLayout::UpdateListItems()
{
	if( mNeedsListUpdate )
	{
		Vec2f nextListItemOffset;

		for( int i = 0; i < mItems.size(); ++i )
		{
			// Get the next list item.
			Widget* item = GetItemAtIndex( i );

			// Reposition the item.
			item->SetOffset( nextListItemOffset );

			if( mDirection == DIRECTION_HORIZONTAL )
			{
				// Add the width of the item to the next position.
				nextListItemOffset.x += item->GetWidth();
			}
			else if( mDirection == DIRECTION_VERTICAL )
			{
				// Add the height of the item to the next position.
				nextListItemOffset.y += item->GetHeight();
			}
		}
	}
}


size_t ListLayout::ReserveNextItemID()
{
	size_t result = mNextItemID;
	mNextItemID++;
	return result;
}
