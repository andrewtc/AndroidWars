#pragma once

namespace mage
{
	/**
	 * Renders a list of sub-widgets in list format and allows scrolling.
	 */
	class Listbox : public Widget
	{
		DECLARE_RTTI;

	public:
		Listbox( WidgetManager* manager, const HashString& name );
		~Listbox();

		void InsertItem( Widget* item );
		void RemoveItem( Widget* item );
		void RemoveAllItems();

	protected:
		HashMap< Widget* > mItems;
	};
}
