#pragma once


#define MAGE_DECLARE_JSON_CONVERSION( type, cppType ) \
	static void Save ## type( rapidjson::Document& document, rapidjson::Value& object, const char* name, cppType const& value ); \
	static void Load ## type( const rapidjson::Value& object, const char* name, cppType& result );

#define MAGE_IMPLEMENT_JSON_CONVERSION_TEMPLATE( type, cppType, defaultValue ) \
	template<> \
	inline void JSON::Load< cppType >( const rapidjson::Value& object, const char* name, cppType& result ) \
	{ \
		assertion( object.IsObject(), "Cannot load JSON value \"%s\" from JSON because the parent JSON value is not an object!", name ); \
		\
		if( object.HasMember( name ) ) \
		{ \
			const rapidjson::Value& value = object[ name ]; \
			\
			if( value.Is ## type() ) \
			{ \
				/* Load the variable. */ \
				result = value.Get ## type(); \
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
	} \
	\
	template<> \
	inline void JSON::Save< cppType >( rapidjson::Document& document, rapidjson::Value& object, const char* name, cppType const & value ) \
	{ \
		assertion( object.IsObject(), "Cannot save JSON value \"%s\" to JSON because the parent JSON value is not an object!", name ); \
		\
		rapidjson::Value jsonValue; \
		jsonValue.Set ## type( value ); \
		object.AddMember( name, jsonValue, document.GetAllocator() ); \
	}\

#define MAGE_IMPLEMENT_JSON_CONVERSION( type, cppType ) \
	inline void JSON::Save ## type( rapidjson::Document& document, rapidjson::Value& object, const char* name, cppType const& value ) \
	{ \
		Save< cppType >( document, object, name, value ); \
	} \
	\
	inline void JSON::Load ## type( const rapidjson::Value& object, const char* name, cppType& result ) \
	{ \
		Load< cppType >( object, name, result ); \
	}


namespace mage
{
	class JSON
	{
	public:
		template< typename T >
		static void Save( rapidjson::Document& document, rapidjson::Value& object, const char* name, const T& value );

		template< typename T >
		static void Load( const rapidjson::Value& object, const char* name, T& result );

		MAGE_DECLARE_JSON_CONVERSION( Bool,   bool           )
		MAGE_DECLARE_JSON_CONVERSION( Char,   char           )
		MAGE_DECLARE_JSON_CONVERSION( Uchar,  unsigned char  )
		MAGE_DECLARE_JSON_CONVERSION( Short,  short          )
		MAGE_DECLARE_JSON_CONVERSION( Ushort, unsigned short )
		MAGE_DECLARE_JSON_CONVERSION( Int,    int            )
		MAGE_DECLARE_JSON_CONVERSION( Uint,   unsigned int   )
		MAGE_DECLARE_JSON_CONVERSION( Long,   long           )
		MAGE_DECLARE_JSON_CONVERSION( Ulong,  unsigned long  )
		MAGE_DECLARE_JSON_CONVERSION( Float,  float          )
		MAGE_DECLARE_JSON_CONVERSION( Double, double         )
		MAGE_DECLARE_JSON_CONVERSION( String, const char*    )
		MAGE_DECLARE_JSON_CONVERSION( String, std::string    )
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
	inline void JSON::Load< std::string >( const rapidjson::Value& object, const char* name, std::string& result )
	{
		const char* string;
		Load< const char* >( object, name, string );
		result = string;
	}


	MAGE_IMPLEMENT_JSON_CONVERSION( Bool,   bool           )
	MAGE_IMPLEMENT_JSON_CONVERSION( Char,   char           )
	MAGE_IMPLEMENT_JSON_CONVERSION( Uchar,  unsigned char  )
	MAGE_IMPLEMENT_JSON_CONVERSION( Short,  short          )
	MAGE_IMPLEMENT_JSON_CONVERSION( Ushort, unsigned short )
	MAGE_IMPLEMENT_JSON_CONVERSION( Int,    int            )
	MAGE_IMPLEMENT_JSON_CONVERSION( Uint,   unsigned int   )
	MAGE_IMPLEMENT_JSON_CONVERSION( Long,   long           )
	MAGE_IMPLEMENT_JSON_CONVERSION( Ulong,  unsigned long  )
	MAGE_IMPLEMENT_JSON_CONVERSION( Float,  float          )
	MAGE_IMPLEMENT_JSON_CONVERSION( Double, double         )
	MAGE_IMPLEMENT_JSON_CONVERSION( String, const char*    )
	MAGE_IMPLEMENT_JSON_CONVERSION( String, std::string    )
}
