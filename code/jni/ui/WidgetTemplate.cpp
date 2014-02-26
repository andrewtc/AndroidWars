#include "androidwars.h"

using namespace mage;

//---------------------------------------
// WidgetProperties
//---------------------------------------
WidgetProperties::WidgetProperties() { }
//---------------------------------------
WidgetProperties::~WidgetProperties() { }
//---------------------------------------
// WidgetTemplate
//---------------------------------------
WidgetTemplate::WidgetTemplate( WidgetManager* manager, const HashString& name ) :
	mManager( manager ), mName( name )
{
	assertion( manager, "Cannot create WidgetTemplate without a WidgetManager!" );
	assertion( !name.GetString().empty(), "Cannot create WidgetTemplate without a valid name" );
}
//---------------------------------------
WidgetTemplate::~WidgetTemplate() { }
//---------------------------------------
WidgetManager* WidgetTemplate::GetManager() const
{
	return mManager;
}
//---------------------------------------
HashString WidgetTemplate::GetName() const
{
	return mName;
}
//---------------------------------------
WidgetProperties& WidgetTemplate::GetRoot()
{
	return mRoot;
}
//---------------------------------------
const WidgetProperties& WidgetTemplate::GetRoot() const
{
	return mRoot;
}
//---------------------------------------
