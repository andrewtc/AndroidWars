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
		ListLayout( WidgetManager* manager, const HashString& name );
		~ListLayout();

		void InsertItem( Widget* item );
		void InsertItemAt( Widget* item, int index );
		void RemoveItem( Widget* item );
		void RemoveItemAt( int index );
		void RemoveItemByName( const HashString& name );
		void RemoveAllItems();
		Widget* GetItem( int index ) const;
		int GetIndexOfItem( const Widget* item ) const;
		int GetIndexOfItemWithName( const HashString& name ) const;
		bool HasItem( const Widget* item ) const;
		bool HasItemWithName( const HashString& name ) const;
		size_t GetItemCount() const;

	protected:
		virtual void OnDraw( const Camera& camera );

		void InvalidateListItems();
		void UpdateListItems();

		bool mNeedsListUpdate;
		std::vector< HashString > mItems;
	};
}
