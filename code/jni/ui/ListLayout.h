#pragma once

namespace mage
{
	/**
	 * Renders a list of sub-widgets in list format and allows scrolling.
	 */
	class ListLayout : public Widget
	{
		DECLARE_RTTI;

	public:
		static const char* const DIRECTION_HORIZONTAL_NAME;
		static const char* const DIRECTION_VERTICAL_NAME;

		enum Direction
		{
			DIRECTION_HORIZONTAL,
			DIRECTION_VERTICAL
		};

		static Direction GetDirectionByName( const HashString& name );

		ListLayout( WidgetManager* manager, const HashString& name );
		virtual ~ListLayout();

		template< class WidgetSubclass = Widget >
		WidgetSubclass* CreateItem( const WidgetTemplate& widgetTemplate );
		template< class WidgetSubclass = Widget >
		WidgetSubclass* CreateItemAtIndex( const WidgetTemplate& widgetTemplate, int index );
		void DestroyItem( Widget* item );
		void DestroyItemAtIndex( int index );
		void DestroyAllItems();
		template< class WidgetSubclass = Widget >
		WidgetSubclass* GetItemAtIndex( int index ) const;
		int GetIndexOfItem( const Widget* item ) const;
		bool HasItem( const Widget* item ) const;
		size_t GetItemCount() const;

	protected:
		static const std::string ITEM_PREFIX;

		virtual void OnLoadFromTemplate( const WidgetTemplate& widgetTemplate );
		virtual void OnDraw( const Camera& camera );

		void InvalidateListItems();
		void UpdateListItems();

		int GetIndexOfItemWithName( const HashString& name ) const;
		bool HasItemWithName( const HashString& name ) const;

	private:
		size_t ReserveNextItemID();

		bool mNeedsListUpdate;
		size_t mNextItemID;
		Direction mDirection;
		std::vector< HashString > mItems;
	};


	template< class WidgetSubclass >
	WidgetSubclass* ListLayout::CreateItem( const WidgetTemplate& widgetTemplate )
	{
		// Create the item at the back of the list.
		return CreateItemAtIndex< WidgetSubclass >( widgetTemplate, mItems.size() );
	}


	template< class WidgetSubclass >
	WidgetSubclass* ListLayout::CreateItemAtIndex( const WidgetTemplate& widgetTemplate, int index )
	{
		assertion( index >= 0 && index <= mItems.size(), "Cannot create item for ListLayout \"%s\" at index %d because the index is out of bounds!", GetFullName().c_str(), index );

		// Construct a unique name for the new item.
		std::stringstream formatter( ITEM_PREFIX );
		formatter << ReserveNextItemID();

		HashString itemName( formatter.str() );
		assertion( !HasChildWithName( itemName ), "Cannot create item for ListLayout \"%s\" because it already has a child with the name \"%s\"!", GetFullName().c_str(), itemName.GetCString() );

		// Create a new Widget for this ListLayout and add it as a child.
		WidgetSubclass* item = GetManager()->CreateWidgetFromTemplate< WidgetSubclass >( widgetTemplate, itemName );

		if( item )
		{
			// Add the new item to the list.
			AddChild( item );

			// Add the Widget name to the list of list items at the specified index.
			mItems.insert( mItems.begin() + index, itemName );
			InvalidateListItems();
		}
		else
		{
			WarnFail( "Could not create item \"%s\" for ListLayout \"%s\"!", itemName.GetCString(), GetFullName().c_str() );
		}

		return item;
	}


	template< class WidgetSubclass >
	WidgetSubclass* ListLayout::GetItemAtIndex( int index ) const
	{
		assertion( index >= 0 && index < mItems.size(), "Cannot get item for ListLayout \"%s\" at index %d because the index is out of bounds!", GetFullName().c_str(), index );

		// Get the name of the item at the passed index.
		HashString itemName = mItems[ index ];

		// Return the child with the same name.
		return GetChildByName< WidgetSubclass >( itemName );
	}
}
