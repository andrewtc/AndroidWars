#include "CoreLib.h"

using namespace mage;

MAGE_IMPLEMENT_RTTI( Object, ControlledObject );

//---------------------------------------
ControlledObject::ControlledObject()
{}
//---------------------------------------
ControlledObject::ControlledObject( const std::string& name )
	: Object( name )
{}
//---------------------------------------
ControlledObject::~ControlledObject()
{
	DetachAll();
}
//---------------------------------------
Controller* ControlledObject::GetController( uint32 i ) const
{
	Controller* cont = 0;
	if ( i >= mControllers.size() )
	{
		WarnCrit( "GetController( i ) : out-of-bounds : %u\n", i );
	}
	else
	{
		cont = mControllers[i];
	}
	return cont;
}
//---------------------------------------
void ControlledObject::Attach( Controller* cont )
{
	for ( uint32 i = 0; i < mControllers.size(); ++i )
	{
		if ( mControllers[i] == cont )
			return;	// Controller already attached
	}
	// Bind controller to object
	cont->Bind( this );
	mControllers.push_back( cont );
}
//---------------------------------------
void ControlledObject::Detach( Controller* cont )
{
	for ( uint32 i = 0; i < mControllers.size(); ++i )
	{
		if ( mControllers[i] == cont )
		{
			cont->Bind( 0 );
			mControllers.remove_elem( cont );
			break;
		}
	}
}
//---------------------------------------
void ControlledObject::DetachAll()
{
	for ( uint32 i = 0; i < mControllers.size(); ++i )
	{
		mControllers[i]->Bind( 0 );
	}
}
//---------------------------------------
bool ControlledObject::UpdateControllers( float currentTime )
{
	bool update = false;
	for ( uint32 i = 0; i < mControllers.size(); ++i )
	{
		if ( mControllers[i]->OnUpdate( currentTime ) )
		{
			update = true;
		}
	}
	return update;
}
//---------------------------------------