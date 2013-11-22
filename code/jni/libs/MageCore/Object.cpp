#include "CoreLib.h"

using namespace mage;

const RTTI Object::TYPE( "mage.Object", 0 );
HashMap< Object* > Object::ObjectRegistry;
std::map< std::string, uint32 > Object::NameRefCount;

//---------------------------------------
Object::Object()
{ /* Unregistered and uninitialized! */ }
//---------------------------------------
Object::Object( const std::string& name )
	: mName( name )
{
	RegisterObject( this );
}
//---------------------------------------
Object::~Object()
{
	UnRegisterObject( this );
}
//---------------------------------------
// RTTI
//---------------------------------------
const RTTI& Object::GetType() const
{
	return TYPE;
}
//---------------------------------------
bool Object::IsExactly( const RTTI& type ) const
{
	return GetType().IsExactly( type );
}
//---------------------------------------
bool Object::IsDerived( const RTTI& type ) const
{
	return GetType().IsDerived( type );
}
//---------------------------------------
bool Object::IsExactSameType( const Object* obj ) const
{
	return obj && GetType().IsExactly( obj->GetType() );
}
//---------------------------------------
bool Object::IsDerivedFromSameType( const Object* obj ) const
{
	return obj && GetType().IsDerived( obj->GetType() );
}
//---------------------------------------
// Naming
//---------------------------------------
Object* Object::GetObjectByName( const HashString& name )
{
	Object* _ret = 0;
	HashMap< Object* >::const_iterator found = ObjectRegistry.find( name );
	if ( found != ObjectRegistry.end() )
	{
		_ret = found->second;
	}
	return _ret;
}
//---------------------------------------
void Object::RegisterObject( Object* obj )
{
	uint32& nameRef = NameRefCount[ obj->GetName() ];
	if ( nameRef != 0 )
	{
		char nameBuf[256];
		const std::string& name = obj->GetName();

		sprintf_s( nameBuf, "%s_%03d", name.c_str(), nameRef + 1 );
		WarnInfo( "Object name '%s' already exists! Using '%s' instead.\n", name.c_str(), nameBuf );
		obj->mName.Set( nameBuf );
	}
	++nameRef;

	ObjectRegistry[ obj->mName ] = obj;
}
//---------------------------------------
void Object::UnRegisterObject( Object* obj )
{
	HashMap< Object* >::iterator found = ObjectRegistry.find( obj->mName);
	if ( found != ObjectRegistry.end() )
	{
		ObjectRegistry.erase( found );
		uint32& nameRef = NameRefCount[ obj->GetName() ];
		if ( nameRef > 0 )
			--nameRef;
	}
}
//---------------------------------------