#include "CoreLib.h"

using namespace mage;

//---------------------------------------
RTTI::RTTI( const char* name, const RTTI* base )
	: mName( name )
	, mBase( base )
{}
//---------------------------------------
RTTI::~RTTI()
{}
//---------------------------------------
bool RTTI::IsDerived( const RTTI& type ) const
{
	const RTTI* i = this;
	while ( i )
	{
		if ( i == &type )
		{
			return true;
		}
		i = i->mBase;
	}
	return false;
}
//---------------------------------------