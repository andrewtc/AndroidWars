/*
 * Author      : Matthew Johnson
 * Date        : 19/May/2013
 * Description :
 *   
 */
 
#pragma once

// Helper to make strings
#define STRINGIFY( S ) #S

namespace mage
{

	class StringUtil
	{
	public:
		static std::string StringToLower( const std::string& str );
		static void StringToLower( std::string& str );
		static std::string StringToUpper( const std::string& str );
		static void StringToUpper( std::string& str );
		static void Tokenize( const std::string& str, std::vector< std::string >& tokens, const std::string& pattern=" " );
		static int StringiCmp( const std::string& A, const std::string& B );
		static std::string ExtractFilenameFromPath( const std::string& path, bool keepExtension=true );
		static std::string StripFilenameFromPath( const std::string& path );
		static std::string LStrip( const std::string& str, const std::string pattern=" " );
		static std::string RStrip( const std::string& str, const std::string pattern=" " );
		static std::string Strip( const std::string& str, const std::string pattern=" " );

		static bool HexStringToUnsigned( const std::string& str, unsigned& dst )
		{
			std::stringstream ss;
			ss << std::hex << str;
			ss >> dst;
			return !ss.fail();
		}

		template< typename T >
		static bool StringToType( const std::string& str, T* type );

		template< typename T >
		static std::string ToString( const T& type )
		{
			std::stringstream ss;
			ss << type;
			return ss.str();
		}

		template< typename T >
		static std::string VectorToCSVString( const std::vector< T >& v )
		{
			std::string csv;
			if ( v.size() )
				csv = ToString( v.front() );
			for ( typename std::vector< T >::const_iterator itr = v.begin() + 1; itr != v.end(); ++itr )
			{
				csv += "," + ToString( *itr );
			}
			return csv;
		}

		static bool ParseInt        ( const std::string& string, const int& defaultValue,        int& result );
		static bool ParseUInt       ( const std::string& string, const unsigned& defaultValue,   unsigned& result );
		static bool ParseFloat      ( const std::string& string, const float& defaultValue,      float& result );
		static bool ParseBool       ( const std::string& string, const bool& defaultValue,       bool& result );
		static bool ParseVec2i      ( const std::string& string, const Vec2i& defaultValue,      Vec2i& result );
		static bool ParseVec2f      ( const std::string& string, const Vec2f& defaultValue,      Vec2f& result );
		static bool ParseVec3f      ( const std::string& string, const Vec3f& defaultValue,      Vec3f& result );
		static bool ParseVec4f      ( const std::string& string, const Vec4f& defaultValue,      Vec4f& result );
		static bool ParseColor      ( const std::string& string, const Color& defaultValue,      Color& result );
		static bool ParseIntRange   ( const std::string& string, const IntRange& defaultValue,   IntRange& result );
		static bool ParseFloatRange ( const std::string& string, const FloatRange& defaultValue, FloatRange& result );

	private:
		template< typename T >
		static bool ParseType( const std::string& string, const T& defaultValue, T& result, const char* typeName );

		// Util to convert a n-dim vector encoded as "x,y,...n" to vectorN
		template< typename TVector >
		static bool ParseVector( const std::string& string, unsigned size, const TVector& defaultValue, TVector& result, const char* typeName );
	};

	inline const char* BoolToCString( bool b ) { return b ? "True" : "False"; }
	inline std::string BoolToString( bool b ) { return std::string( BoolToCString( b ) ); }

	template< typename T >
	bool StringUtil::StringToType( const std::string& str, T* type )
	{
		std::stringstream ss;
		ss << str;
		ss >> *type;
		return !ss.fail();
	}

	// IO util functions
	template< typename buffer_t >
	bool TextFileToBuffer( const std::string& fileName, buffer_t& buffer )
	{
		std::ifstream pFile( fileName, std::ios::in | std::ios::binary );
		if ( pFile )
		{
			pFile.seekg( 0, std::ios::end );
			buffer.resize( (size_t) pFile.tellg() );
			pFile.seekg( 0, std::ios::beg );
			pFile.read( &buffer[0], buffer.size() );
			pFile.close();
			return true;
		}
		WarnFail( "Failed to open '%s'\n", fileName.c_str() );
		return false;
	}

	// Check if c is a seperating character
	inline bool IsSeperator( char c )
	{
		return ( c == ' '  ||
			c == '\n' ||
			c == '\r' ||
			c == '\t' ||
			c == '\f' );
	}

	// Check if c is a newline character
	inline bool IsNewLine( char c )
	{
		return ( c == '\n' || c == '\r' || c == '\f' );
	}

	template< typename char_t >
	inline bool EOB( char_t itr, char_t end_itr )
	{
		if ( itr == end_itr ) return true;
		else --end_itr;
		return ( itr == end_itr );
	}

	// Get the next word seperated by a space or newline
	template< typename char_t >
	inline char_t GetNextWord( char_t pBuffer, char_t pEnd )
	{
		while ( !EOB( pBuffer, pEnd ) )
		{
			if ( !IsSeperator( *pBuffer ) || IsNewLine( *pBuffer ) ) break;
			++pBuffer;
		}

		return pBuffer;
	}

	// Skips a line in the buffer
	template< typename char_t >
	inline char_t SkipLine( char_t itr, char_t end_itr, int& lineNum )
	{
		while ( !EOB( itr, end_itr ) && !IsNewLine( *itr ) ) ++itr;

		if ( itr != end_itr )
		{
			++itr;
			++lineNum;
		}

		// Remove extra white space
		while ( itr != end_itr && ( *itr == '\t' || *itr == ' ' ) ) ++itr;
		return itr;
	}

	// Get a pointer to the next token
	template< typename char_t >
	inline char_t GetNextToken( char_t pBuffer, char_t pEnd )
	{
		while ( !EOB( pBuffer, pEnd ) )
		{
			if ( IsSeperator( *pBuffer ) ) break;
			++pBuffer;
		}
		return GetNextWord( pBuffer, pEnd );
	}

	template< typename char_t >
	inline char_t CopyNextWord( char_t itr, char_t end_itr, char* pBuffer, int len )
	{
		int i = 0;
		itr = GetNextWord< char_t >( itr, end_itr );

		while ( !IsSeperator( *itr ) && itr != end_itr )
		{
			pBuffer[ i++ ] = *itr;
			if ( i == len-1 ) break;
			++itr;
		}
		pBuffer[ i ] = '\0';
		return itr;
	}

	// Get next float from a line
	template< typename char_t >
	inline char_t GetNextFloat( char_t itr, char_t end_itr, float& out )
	{
		static const int BUFFERSIZE = 1024;
		char buffer[ BUFFERSIZE ];
		itr = CopyNextWord( itr, end_itr, buffer, BUFFERSIZE );
		out = (float) std::atof( buffer );
		return itr;
	}

	template< typename char_t >
	inline char_t GetString( char_t itr, char_t end_itr, std::string& out )
	{
		out = "";
		itr = GetNextToken< char_t >( itr, end_itr );

		if ( EOB( itr, end_itr ) ) return end_itr;

		char* pCurrent = &( *itr );
		while ( !EOB( itr, end_itr ) && !IsSeperator( *itr ) ) ++ itr;

		std::string str( pCurrent, &( *itr ) );
		if ( str.empty() ) return itr;
		out = str;
		return itr;
	}

	// Helper for getting mapped value
	int GetMappedValue(
		const std::map< std::string, int >& map,
		const std::string& tag,
		const char* errorMsg,
		int defaultValue=0 );


	template< typename T >
	bool StringUtil::ParseType( const std::string& string, const T& defaultValue, T& result, const char* typeName )
	{
		// Try to parse the value.
		bool success = StringToType< T >( string, &result );

		if( !success )
		{
			// If the value could not be parsed, use the default value.
			result = defaultValue;
			WarnFail( "StringUtil : Could not parse %s value from string \"%s\"!", typeName, string.c_str() );
		}

		return success;
	}


	template< typename TVector >
	bool StringUtil::ParseVector( const std::string& string, unsigned size, const TVector& defaultValue, TVector& result, const char* typeName )
	{
		bool success = true;

		std::vector< std::string > tokens;
		Tokenize( string, tokens, "," );

		if ( tokens.size() != size )
		{
			success = false;
			WarnFail( "StringUtil : Could not parse vector: %s", tokens.size() < size ? "Too few elements." : "Too many elements." );
		}
		else
		{
			for ( unsigned i = 0; i < size; ++i )
			{
				// Parse each element of the vector.
				if ( !StringUtil::StringToType( tokens[i], &result[i] ) )
				{
					success = false;
					WarnFail( "StringUtil : Could not parse vector: Element %d (\"%s\") could not be converted to the requested type.",
						i+1, tokens[i].c_str() );
					break;
				}
			}
		}

		if( !success )
		{
			// If the value failed to parse, use the default value.
			result = defaultValue;
		}

		return success;
	}
}
