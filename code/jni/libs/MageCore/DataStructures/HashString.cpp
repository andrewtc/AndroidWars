#include "CoreLib.h"

using namespace mage;

//---------------------------------------
HashString::HashString()
	: mHash( 0U )
{}
//---------------------------------------
HashString::HashString( const char* str )
{
	Set( str );
}
//---------------------------------------
HashString::HashString( const std::string& str )
{
	Set( str.c_str() );
}
//---------------------------------------
HashString::~HashString()
{}
//---------------------------------------
int HashString::Compare( const HashString& A, const HashString& B )
{
	return A.mHash - B.mHash;
}
//---------------------------------------
void HashString::Set( const char* str )
{
	mString = str;
	mHash = GenerateHash( StringUtil::StringToLower( str ).c_str() );
}
//---------------------------------------