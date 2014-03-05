#include "androidwars.h"

using namespace mage;


MAGE_IMPLEMENT_RTTI( Widget, ListLayout );


ListLayout::ListLayout( WidgetManager* manager, const HashString& name ) :
	Widget( manager, name )
{ }


ListLayout::~ListLayout() { }


void ListLayout::InsertItem( Widget* item )
{
	// Insert the item at the back of the list.
	InsertItemAt( item, mItems.size() - 1 );
}


void ListLayout::InsertItemAt( Widget* item, int index )
{
	assertion( item, "Cannot add null Widget to ListLayout \"%s\" items!", GetFullName().c_str() );
	assertion( item->GetParent() == nullptr, "Cannot add Widget \"%s\" to ListLayout \"%s\" items because it is already the child of a Widget!", item->GetFullName().c_str(), GetFullName().c_str() );
	assertion( !HasItem( item ), "Cannot add Widget \"%s\" to ListLayout \"%s\" items because it is already part of the list!", item->GetFullName().c_str(), GetFullName().c_str() );
	assertion( index >= 0 && index <= mItems.size(), "Cannot add Widget \"%s\" to ListLayout \"%s\" items at index %d because the index is out of bounds!", item->GetFullName().c_str(), GetFullName().c_str(), index );

	// Add the Widget as a child of this one.
	AddChild( item );

	// Add the Widget name to the list of list items at the specified index.
	mItems.insert( mItems.begin() + index, item->GetName() );
	InvalidateListItems();
}


void ListLayout::RemoveItem( Widget* item )
{
	assertion( item, "Cannot remove null Widget from ListLayout \"%s\" items!", GetFullName().c_str() );
	assertion( item->GetParent() == this, "Cannot remove Widget \"%s\" from ListLayout \"%s\" items because it is not a child of this ListLayout!", item->GetFullName().c_str(), GetFullName().c_str() );

	// Get the index of the item.
	int index = GetIndexOfItem( item );
	assertion( index >= 0, "Could not remove Widget \"%s\" from ListLayout \"%s\" items because it is not a list item!", item->GetFullName().c_str(), GetFullName().c_str() );

	// If the item was found in the list, remove it.
	RemoveItemAt( index );
}


void ListLayout::RemoveItemAt( int index )
{
	assertion( index >= 0 && index < mItems.size(), "Cannot remove item %d from ListLayout \"%s\" items because the index is out of bounds!", index, GetFullName().c_str() );

	// Get the item at the passed index.
	auto it = ( mItems.begin() + index );
	HashString name = ( *it );

	// Remove the item from the list.
	mItems.erase( it );

	// Remove the item from this Widget.
	RemoveChildByName( name );

	// Invalidate the list.
	InvalidateListItems();
}


void ListLayout::RemoveItemByName( const HashString& name )
{
	// Get the index of the item to remove.
	int index = GetIndexOfItemWithName( name );

	if( index >= 0 )
	{
		// Remove the item from the list.
		RemoveItemAt( index );
	}
	else
	{
		WarnFail( "Could not remove item \"%s\" from ListLayout \"%s\" because no item with that name exists!", name.GetCString(), GetFullName().c_str() );
	}
}


void ListLayout::RemoveAllItems()
{
	for( int i = ( mItems.size() - 1 ); i >= 0; --i )
	{
		// Remove each item from the list.
		RemoveItemAt( i );
	}
}


int ListLayout::GetIndexOfItem( const Widget* item ) const
{
	assertion( item, "Cannot get list item index of null Widget from ListLayout \"%s\" items!", GetFullName().c_str() );
	assertion( item->GetParent() == this, "Cannot get list item index of Widget \"%s\" from ListLayout \"%s\" because it is not a child of this ListLayout!", item->GetFullName().c_str(), GetFullName().c_str() );
	return GetIndexOfItemWithName( item->GetName() );
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
	assertion( item != nullptr, "Cannot test if ListLayout \"%s\" contains null Widget!", GetFullName().c_str() );
	return ( item->GetParent() == this && HasItemWithName( item->GetName() ) );
}


bool ListLayout::HasItemWithName( const HashString& item ) const
{
	return ( GetIndexOfItemWithName( item ) != -1 );
}


size_t ListLayout::GetItemCount() const
{
	return mItems.size();
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
		// TODO
	}
}
