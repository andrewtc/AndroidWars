/*
 * Author      : Matthew Johnson
 * Date        : 10/Oct/2013
 * Description :
 *   A string hashed into an unsigned 32bit int. HashStrings are case insensitive.
 */
 
#pragma once

namespace mage
{

	class HashString
	{
	public:
		HashString();
		HashString( const char* str );
		HashString( const std::string& str );
		~HashString();

		static int Compare( const HashString& A, const HashString& B );

		void Set( const char* str );
		const std::string& GetString() const					{ return mString; }
		uint32 GetHash() const									{ return mHash; }

		HashString& operator=( const HashString& other )		{ mHash = other.mHash; mString = other.mString; return *this; }
		bool operator==( const HashString& other ) const		{ return Compare( *this, other ) == 0; }
		bool operator!=( const HashString& other ) const		{ return !( *this == other ); }
		bool operator< ( const HashString& other ) const		{ return Compare( *this, other ) < 0; }
		bool operator<=( const HashString& other ) const		{ return Compare( *this, other ) <= 0; }
		bool operator> ( const HashString& other ) const		{ return Compare( *this, other ) > 0; }
		bool operator>=( const HashString& other ) const		{ return Compare( *this, other ) >= 0; }

	private:
		uint32 mHash;
		std::string mString;
	};

}