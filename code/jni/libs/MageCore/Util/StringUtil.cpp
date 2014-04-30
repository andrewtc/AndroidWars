#include "CoreLib.h"

namespace mage
{
	int GetMappedValue(
		const std::map< std::string, int >& map,
		const std::string& tag,
		const char* errorMsg,
		int defaultValue )
	{
		std::map< std::string, int >::const_iterator itr = map.find( tag );
		if ( itr != map.end() )
		{
			return itr->second;
		}
		ConsolePrintf( CONSOLE_WARNING, "Warning: '%s' : %s\n", tag.c_str(), errorMsg );
		return defaultValue;
	}
	//---------------------------------------
	template<>
	bool StringUtil::StringToType< bool >( const std::string& str, bool* type )
	{
		std::stringstream ss;
		ss << str;
		ss >> std::boolalpha >> *type;
		return !ss.fail();
	}
}

using namespace mage;

//---------------------------------------
// String Utils
//---------------------------------------
std::string StringUtil::StringToLower( const std::string& str )
{
	std::string _ret = str;
	StringToLower( _ret );
	return _ret;
}
//---------------------------------------
void StringUtil::StringToLower( std::string& str )
{
	std::transform( str.begin(), str.end(), str.begin(), ::tolower );
}
//---------------------------------------
std::string StringUtil::StringToUpper( const std::string& str )
{
	std::string _ret = str;
	StringToUpper( _ret );
	return _ret;
}
//---------------------------------------
void StringUtil::StringToUpper( std::string& str )
{
	std::transform( str.begin(), str.end(), str.begin(), ::toupper );
}
//---------------------------------------
void StringUtil::Tokenize( const std::string& str, std::vector< std::string >& tokens, const std::string& pattern )
{
	size_t start = 0;
	size_t end = str.find_first_of( pattern );

	while ( end != std::string::npos )
	{
		tokens.push_back( str.substr( start, end - start ) );
		start = end + 1;
		end = str.find( pattern, start );
	}

	tokens.push_back( str.substr( start, end - start ) );
}
//---------------------------------------
int StringUtil::StringiCmp( const std::string& A, const std::string& B )
{
	return _stricmp( A.data(), B.data() );
}
//---------------------------------------
std::string StringUtil::ExtractFilenameFromPath( const std::string& path, bool keepExtension )
{
	std::string _ret;
	unsigned int lastSlash = path.find_last_of( "/" );
	if ( lastSlash != std::string::npos )
	{
		_ret = path.substr( lastSlash + 1 );
	}
	else
	{
		_ret = path;
	}

	if ( !keepExtension )
	{
		unsigned int lastDot = _ret.find_last_of( "." );
		if ( lastDot != std::string::npos )
		{
			_ret = _ret.substr( 0, lastDot );
		}
	}

	return _ret;
}
//---------------------------------------
std::string StringUtil::StripFilenameFromPath( const std::string& path )
{
	std::string _ret;
	unsigned int lastSlash = path.find_last_of( "/" );
	if ( lastSlash != std::string::npos )
	{
		_ret = path.substr( 0, lastSlash + 1 );
	}
	else
	{
		_ret = path;
	}

	return _ret;
}
//---------------------------------------
std::string StringUtil::LStrip( const std::string& str, const std::string pattern )
{
	std::string _ret = str;

	for ( unsigned int c = 0; c < _ret.length(); ++c )
	{
		std::string sub = _ret.substr( c, pattern.length() );
		if ( sub == pattern )
		{
			_ret = _ret.substr( c + pattern.length() );
		}
		else
		{
			break;
		}
	}

	return _ret;
}
//---------------------------------------
std::string StringUtil::RStrip( const std::string& str, const std::string pattern )
{
	std::string _ret = str;

	for ( int c = _ret.length(); c >= 0; --c )
	{
		if ( c - pattern.length() < 0 ) break;
		std::string sub = _ret.substr( c - pattern.length(), pattern.length() );
		if ( sub == pattern )
		{
			_ret = _ret.substr( 0, c - pattern.length() );
		}
		else
		{
			break;
		}
	}

	return _ret;
}
//---------------------------------------
std::string StringUtil::Strip( const std::string& str, const std::string pattern )
{
	return RStrip( LStrip( str, pattern ), pattern );
}
//---------------------------------------
#define AutoParseType( type ) \
	ParseType< type >( string, defaultValue, result, #type )
//---------------------------------------

#define AutoParseVector( vectorType, size ) \
	ParseVector< vectorType >( string, size, defaultValue, result, #vectorType )
//---------------------------------------
bool StringUtil::ParseInt( const std::string& string, const int& defaultValue, int& result )
{
	return AutoParseType( int );
}
//---------------------------------------
bool StringUtil::ParseUInt( const std::string& string, const unsigned& defaultValue, unsigned& result )
{
	return AutoParseType( unsigned );
}
//---------------------------------------
bool StringUtil::ParseFloat( const std::string& string, const float& defaultValue, float& result )
{
	return AutoParseType( float );
}
//---------------------------------------
bool StringUtil::ParseBool( const std::string& string, const bool& defaultValue, bool& result )
{
	return AutoParseType( bool );
}
//---------------------------------------
bool StringUtil::ParseVec2f( const std::string& string, const Vec2f& defaultValue, Vec2f& result )
{
	return AutoParseVector( Vec2f, 2 );
}
//---------------------------------------
bool StringUtil::ParseVec3f( const std::string& string, const Vec3f& defaultValue, Vec3f& result )
{
	return AutoParseVector( Vec3f, 3 );
}
//---------------------------------------
bool StringUtil::ParseVec4f( const std::string& string, const Vec4f& defaultValue, Vec4f& result )
{
	return AutoParseVector( Vec4f, 4 );
}
//---------------------------------------
bool StringUtil::ParseColor( const std::string& string, const Color& defaultValue, Color& result )
{
	bool success = true;

	if ( string[0] == '#' )
	{
		char* res;
		uint32_t hex = (uint32_t) std::strtoul( &string[0]+1, &res, 16 );

		if ( *res == nullptr )
		{
			result = Color( hex );
		}
		else
		{
			success = false;
			WarnFail( "StringUtil : Could not parse Color: Not a valid hexadecimal number (\"%s\").", string.c_str() );
		}
	}
	else
	{
		success = false;
		WarnFail( "StringUtil : Could not parse Color: Missing '#' at the beginning of color value (\"%s\").", string.c_str() );
	}

	if( !success )
	{
		result = defaultValue;
	}

	return success;
}
//---------------------------------------
bool StringUtil::ParseIntRange( const std::string& string, const IntRange& defaultValue, IntRange& result )
{
	bool success = false;
	IntRange value;

	// Tokenize the string.
	std::vector< std::string > tokens;
	StringUtil::Tokenize( string, tokens, "~" );

	if( tokens.size() > 0 )
	{
		// Parse the first value of the range.
		success = StringUtil::StringToType( tokens[ 0 ], &value.Min );

		if( success )
		{
			if( tokens.size() > 1 )
			{
				// Parse the second value of the range.
				success = StringUtil::StringToType( tokens[ 1 ], &value.Max );
			}
			else
			{
				// If only one number was found, parse it and set the min and max range to the same value.
				result.Max = result.Min;
			}
		}
	}

	// If the parse was successful, return the value in the out parameter.
	// Otherwise, return the default value.
	result = ( success ? value : defaultValue );

	return success;
}
//---------------------------------------
bool StringUtil::ParseFloatRange( const std::string& string, const FloatRange& defaultValue, FloatRange& result )
{
	// TODO
	return false;
}
//---------------------------------------
