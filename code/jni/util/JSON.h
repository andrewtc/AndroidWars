#pragma once


#define MAGE_DECLARE_JSON_CONVERSION( type, cppType ) \
	static void Save ## type( rapidjson::Document& document, rapidjson::Value& object, const char* name, cppType const& value ); \
	static bool Load ## type( const rapidjson::Value& object, const char* name, cppType& result ); \
	static bool Load ## type( const rapidjson::Value& object, const char* name, cppType& result, cppType const& defaultValue );

#define MAGE_IMPLEMENT_JSON_CONVERSION_TEMPLATE( type, cppType, defaultVal ) \
	template<> \
	inline bool JSON::Load< cppType >( const rapidjson::Value& object, const char* name, cppType& result, cppType const& defaultValue ) \
	{ \
		assertion( object.IsObject(), "Cannot load JSON value \"%s\" from JSON because the parent JSON value is not an object!", name ); \
		\
		bool success = false; \
		\
		if( object.HasMember( name ) ) \
		{ \
			const rapidjson::Value& value = object[ name ]; \
			\
			if( value.Is ## type() ) \
			{ \
				/* Load the variable. */ \
				result = value.Get ## type(); \
				success = true; \
			} \
			else \
			{ \
				WarnFail( "Could not load value \"%s\" from JSON because the value (\"%s\") could not be converted to type " #cppType "!", name, value.GetString() ); \
				result = defaultValue; \
			} \
		} \
		else \
		{ \
			WarnFail( "Could not load value \"%s\" from JSON because no value with that name was found!", name ); \
			result = defaultValue; \
		} \
		\
		return success; \
	} \
	\
	template<> \
	inline bool JSON::Load< cppType >( const rapidjson::Value& object, const char* name, cppType& result ) \
	{ \
		return Load< cppType >( object, name, result, defaultVal ); \
	} \
	\
	template<> \
	inline void JSON::Save< cppType >( rapidjson::Document& document, rapidjson::Value& object, const char* name, cppType const& value ) \
	{ \
		assertion( object.IsObject(), "Cannot save JSON value \"%s\" to JSON because the parent JSON value is not an object!", name ); \
		\
		rapidjson::Value jsonValue; \
		jsonValue.Set ## type( value ); \
		object.AddMember( name, jsonValue, document.GetAllocator() ); \
	}\

#define MAGE_IMPLEMENT_JSON_PARSER_CONVERSION_TEMPLATE( type, cppType, defaultVal ) \
	template<> \
	inline void JSON::Save< cppType >( rapidjson::Document& document, rapidjson::Value& object, const char* name, cppType const& value ) \
	{ \
		Save< std::string >( document, object, name, StringUtil::ToString( value ) ); \
	} \
	\
	template<> \
	inline bool JSON::Load< cppType >( const rapidjson::Value& object, const char* name, cppType& result, cppType const& defaultValue ) \
	{ \
		std::string string; \
		bool success = Load< std::string >( object, name, string ); \
		\
		if( success ) \
		{ \
			/* If the value was loaded successfully, attempt to parse it. */ \
			success = StringUtil::Parse ## type( string, defaultValue, result ); \
		} \
		\
		return success; \
	} \
	\
	template<> \
	inline bool JSON::Load< cppType >( const rapidjson::Value& object, const char* name, cppType& result ) \
	{ \
		return Load< cppType >( object, name, result, defaultVal ); \
	}

#define MAGE_IMPLEMENT_JSON_VECTOR_CONVERSION_TEMPLATE( type, cppType ) \
	MAGE_IMPLEMENT_JSON_PARSER_CONVERSION_TEMPLATE( type, cppType, cppType::ZERO );

#define MAGE_IMPLEMENT_JSON_CONVERSION( type, cppType ) \
	inline void JSON::Save ## type( rapidjson::Document& document, rapidjson::Value& object, const char* name, cppType const& value ) \
	{ \
		Save< cppType >( document, object, name, value ); \
	} \
	\
	inline bool JSON::Load ## type( const rapidjson::Value& object, const char* name, cppType& result ) \
	{ \
		return Load< cppType >( object, name, result ); \
	} \
	\
	inline bool JSON::Load ## type( const rapidjson::Value& object, const char* name, cppType& result, cppType const& defaultValue ) \
	{ \
		return Load< cppType >( object, name, result, defaultValue ); \
	}


namespace mage
{
	class JSON
	{
	public:
		template< typename T >
		static void Save( rapidjson::Document& document, rapidjson::Value& object, const char* name, const T& value );

		template< typename T >
		static bool Load( const rapidjson::Value& object, const char* name, T& result );

		template< typename T >
		static bool Load( const rapidjson::Value& object, const char* name, T& result, const T& defaultValue );

		MAGE_DECLARE_JSON_CONVERSION( Bool,       bool           )
		MAGE_DECLARE_JSON_CONVERSION( Char,       char           )
		MAGE_DECLARE_JSON_CONVERSION( Uchar,      unsigned char  )
		MAGE_DECLARE_JSON_CONVERSION( Short,      short          )
		MAGE_DECLARE_JSON_CONVERSION( Ushort,     unsigned short )
		MAGE_DECLARE_JSON_CONVERSION( Int,        int            )
		MAGE_DECLARE_JSON_CONVERSION( Uint,       unsigned int   )
		MAGE_DECLARE_JSON_CONVERSION( Long,       long           )
		MAGE_DECLARE_JSON_CONVERSION( Ulong,      unsigned long  )
		MAGE_DECLARE_JSON_CONVERSION( Float,      float          )
		MAGE_DECLARE_JSON_CONVERSION( Double,     double         )
		MAGE_DECLARE_JSON_CONVERSION( String,     const char*    )
		MAGE_DECLARE_JSON_CONVERSION( String,     std::string    )
		MAGE_DECLARE_JSON_CONVERSION( HashString, HashString     )
		MAGE_DECLARE_JSON_CONVERSION( IntRange,   IntRange       )
		MAGE_DECLARE_JSON_CONVERSION( FloatRange, FloatRange     )
		MAGE_DECLARE_JSON_CONVERSION( Vec2i,      Vec2i          )
		MAGE_DECLARE_JSON_CONVERSION( Vec2f,      Vec2f          )
	};


	MAGE_IMPLEMENT_JSON_CONVERSION_TEMPLATE( Bool,   bool,           false );
	MAGE_IMPLEMENT_JSON_CONVERSION_TEMPLATE( Int,    char,           0     );
	MAGE_IMPLEMENT_JSON_CONVERSION_TEMPLATE( Uint,   unsigned char,  0u    );
	MAGE_IMPLEMENT_JSON_CONVERSION_TEMPLATE( Int,    short,          0     );
	MAGE_IMPLEMENT_JSON_CONVERSION_TEMPLATE( Uint,   unsigned short, 0u    );
	MAGE_IMPLEMENT_JSON_CONVERSION_TEMPLATE( Int,    int,            0     );
	MAGE_IMPLEMENT_JSON_CONVERSION_TEMPLATE( Uint,   unsigned int,   0u    );
	MAGE_IMPLEMENT_JSON_CONVERSION_TEMPLATE( Int64,  long,           0l    );
	MAGE_IMPLEMENT_JSON_CONVERSION_TEMPLATE( Uint64, unsigned long,  0ul   );
	MAGE_IMPLEMENT_JSON_CONVERSION_TEMPLATE( Double, float,          0.0f  );
	MAGE_IMPLEMENT_JSON_CONVERSION_TEMPLATE( Double, double,         0.0   );
	MAGE_IMPLEMENT_JSON_CONVERSION_TEMPLATE( String, const char*,    ""    );


	template<>
	inline void JSON::Save< std::string >( rapidjson::Document& document, rapidjson::Value& object, const char* name, std::string const& value )
	{
		Save< const char* >( document, object, name, value.c_str() );
	}


	template<>
	inline bool JSON::Load< std::string >( const rapidjson::Value& object, const char* name, std::string& result, const std::string& defaultValue )
	{
		// Load the value as a C-string and convert it to a std::string.
		const char* string;
		bool success = Load< const char* >( object, name, string, defaultValue.c_str() );
		result = string;

		return success;
	}


	template<>
	inline bool JSON::Load< std::string >( const rapidjson::Value& object, const char* name, std::string& result )
	{
		return JSON::Load( object, name, result, std::string() );
	}


	template<>
	inline void JSON::Save< HashString >( rapidjson::Document& document, rapidjson::Value& object, const char* name, HashString const& value )
	{
		Save< const char* >( document, object, name, value.GetCString() );
	}


	template<>
	inline bool JSON::Load< HashString >( const rapidjson::Value& object, const char* name, HashString& result, const HashString& defaultValue )
	{
		// Load the value as a C-string and convert it to a HashString.
		const char* string;
		bool success = Load< const char* >( object, name, string, defaultValue.GetCString() );
		result = string;

		return success;
	}


	template<>
	inline bool JSON::Load< HashString >( const rapidjson::Value& object, const char* name, HashString& result )
	{
		return JSON::Load( object, name, result, HashString() );
	}


	MAGE_IMPLEMENT_JSON_PARSER_CONVERSION_TEMPLATE( IntRange, IntRange, IntRange( 0, 0 ) );
	MAGE_IMPLEMENT_JSON_PARSER_CONVERSION_TEMPLATE( FloatRange, FloatRange, FloatRange( 0, 0 ) );

	MAGE_IMPLEMENT_JSON_VECTOR_CONVERSION_TEMPLATE( Vec2i, Vec2i );
	MAGE_IMPLEMENT_JSON_VECTOR_CONVERSION_TEMPLATE( Vec2f, Vec2f );


	MAGE_IMPLEMENT_JSON_CONVERSION( Bool,       bool           )
	MAGE_IMPLEMENT_JSON_CONVERSION( Char,       char           )
	MAGE_IMPLEMENT_JSON_CONVERSION( Uchar,      unsigned char  )
	MAGE_IMPLEMENT_JSON_CONVERSION( Short,      short          )
	MAGE_IMPLEMENT_JSON_CONVERSION( Ushort,     unsigned short )
	MAGE_IMPLEMENT_JSON_CONVERSION( Int,        int            )
	MAGE_IMPLEMENT_JSON_CONVERSION( Uint,       unsigned int   )
	MAGE_IMPLEMENT_JSON_CONVERSION( Long,       long           )
	MAGE_IMPLEMENT_JSON_CONVERSION( Ulong,      unsigned long  )
	MAGE_IMPLEMENT_JSON_CONVERSION( Float,      float          )
	MAGE_IMPLEMENT_JSON_CONVERSION( Double,     double         )
	MAGE_IMPLEMENT_JSON_CONVERSION( String,     const char*    )
	MAGE_IMPLEMENT_JSON_CONVERSION( String,     std::string    )
	MAGE_IMPLEMENT_JSON_CONVERSION( HashString, HashString     )
	MAGE_IMPLEMENT_JSON_CONVERSION( IntRange,   IntRange       )
	MAGE_IMPLEMENT_JSON_CONVERSION( FloatRange, FloatRange     )
	MAGE_IMPLEMENT_JSON_CONVERSION( Vec2i,      Vec2i          )
	MAGE_IMPLEMENT_JSON_CONVERSION( Vec2f,      Vec2f          )
}
