#include "CoreLib.h"

using namespace mage;

MAGE_IMPLEMENT_RTTI( Object, Definition );

//---------------------------------------
Definition::Definition( const XmlReader::XmlReaderIterator& root )
{
	std::string name = root.GetAttributeAsString( "Name" );

	mName.Set( name.c_str() );
	RegisterObject( this );
}
//---------------------------------------
Definition::~Definition()
{}
//---------------------------------------