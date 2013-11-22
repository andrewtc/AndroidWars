#include "CoreLib.h"

using namespace mage;

//---------------------------------------
Dictionary::Dictionary()
{}
//---------------------------------------
Dictionary::Dictionary( const Dictionary& other )
{
	*this = other;
}
Dictionary::~Dictionary()
{
	for ( HashMap< DictionaryValueBase* >::iterator itr = mDictionary.begin(); itr != mDictionary.end(); ++itr )
	{
		Delete0( itr->second );
	}
}
//---------------------------------------