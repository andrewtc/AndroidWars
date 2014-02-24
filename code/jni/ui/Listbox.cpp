#include "androidwars.h"

using namespace mage;


MAGE_IMPLEMENT_RTTI( Widget, Listbox );


Listbox::Listbox( WidgetManager* manager, const HashString& name ) :
	Widget( manager, name )
{ }


Listbox::~Listbox() { }


void Listbox::InsertItem( Widget* item )
{
	assertion( item->GetParent() == nullptr, "Cannot add Widget to Listbox that is already the child of another Widget!" );

}


void Listbox::RemoveItem( Widget* item )
{
	assertion( item->GetParent() == this, "Cannot add Widget to Listbox that is not a child of this Listbox!" );
}


void Listbox::RemoveAllItems()
{

}
