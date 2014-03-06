#pragma once

#include "util/Callback.h"


namespace mage
{
	struct OnlineRequestResult
	{
		OnlineRequestResult( long requestID, bool isError, int statusCode, const std::string& result );

		bool isError;
		bool resultIsJSON;
		long requestID;
		int statusCode;
		std::string string;
		rapidjson::Document json;
	};


	struct OnlineGameListData
	{
		std::string id;
		std::string name;
	};


	struct OnlineGameData
	{
		std::string id;
		std::string name;
	};


	/**
	 * Accepts any function or lambda with the signature: void (*)( const OnlineRequestResult& result )
	 */
	typedef Callback< void, const OnlineRequestResult& > OnlineRequestCallback;

	/**
	 * Accepts any function or lambda with the signature: void (*)( bool success )
	 */
	typedef Callback< void, bool > OnlineLoginCallback;

	/**
	 * Accepts any function or lambda with the signature: void (*)( bool success, const std::vector< OnlineGameListData >& onlineGameList )
	 */
	typedef Callback< void, bool, const std::vector< OnlineGameListData >& > OnlineGameListCallback;

	/**
	 * Accepts any function or lambda with the signature: void (*)( bool success, OnlineGameData )
	 */
	typedef Callback< void, bool, OnlineGameData > OnlineGameCallback;

	/**
	 * Holds a single parameter to send with a GET request.
	 */
	typedef std::pair< std::string, std::string > OnlineRequestParameter;

	/**
	 * Holds a list of parameters to send with a GET request.
	 */
	typedef std::vector< OnlineRequestParameter > OnlineRequestParameters;


	/**
	 * Sends and receives requests from the AndroidWarsOnline web service.
	 */
	class OnlineGameClient
	{
	public:
		static const char* const SESSION_TOKEN_KEY = "sessionToken";

		OnlineGameClient();
		~OnlineGameClient();

		void Init( android_app* app );
		void Destroy();
		void Update();
		bool IsInitialized() const;

		std::string GetUserSessionToken() const;
		bool IsAuthenticated() const;

		long SendGetRequest( const std::string& resource, const OnlineRequestParameters& parameters = OnlineRequestParameters(), OnlineRequestCallback = OnlineRequestCallback() );
		long SendPostRequest( const std::string& resource, const std::string& data, OnlineRequestCallback = OnlineRequestCallback() );

		long CallCloudFunction( const std::string& functionName, const std::string& data, OnlineRequestCallback callback = OnlineRequestCallback() );

		long LogIn( const std::string& userName, const std::string& password, OnlineLoginCallback callback = OnlineLoginCallback() );
		void LogOut();

		void RequestCurrentGamesList( OnlineGameListCallback callback = OnlineGameListCallback() );
		void RequestGameData( const std::string& gameID, OnlineGameCallback callback = OnlineGameCallback() );

	private:
		static const char* const PARSE_FUNCTION_PREFIX = "functions/";

		jobject CreateJavaRequestParams( const OnlineRequestParameters& parameters );
		jobject FetchNextResponse();

		jclass mJavaClass;
		jobject mJavaObject;
		jmethodID mJavaCreateRequestParams;
		jmethodID mJavaFetchNextResponse;
		jmethodID mJavaSendGetRequest;
		jmethodID mJavaSendPostRequest;

		jclass mJavaResponseClass;
		jfieldID mJavaResponseRequestID;
		jfieldID mJavaResponseStatusCode;
		jfieldID mJavaResponseResult;
		jmethodID mJavaResponseIsError;

		jclass mJavaRequestParamsClass;
		jmethodID mJavaRequestParamsPut;

		std::string mUserSessionToken;
		std::map< long, OnlineRequestCallback > mCallbacksByID;
	};


	inline long OnlineGameClient::CallCloudFunction( const std::string& functionName, const std::string& data, OnlineRequestCallback callback )
	{
		// Fire off a POST request to the AndroidWarsOnline cloud function URL.
		return SendPostRequest( PARSE_FUNCTION_PREFIX + functionName, data, callback );
	}


	inline std::string OnlineGameClient::GetUserSessionToken() const
	{
		return mUserSessionToken;
	}


	inline bool OnlineGameClient::IsAuthenticated() const
	{
		return !( mUserSessionToken.empty() );
	}


	inline bool OnlineGameClient::IsInitialized() const
	{
		return ( mJavaObject != nullptr );
	}


#define MAGE_IMPLEMENT_GET_JSON_VALUE( type, rapidjsonType ) \
	inline type GetJSON ## rapidjsonType ## Value( const rapidjson::Value& object, const char* name, type const & defaultValue ) \
	{ \
		type result = defaultValue; \
		\
		if( object.HasMember( name ) ) \
		{ \
			const rapidjson::Value& member = object[ name ]; \
			\
			if( member.Is ## rapidjsonType () ) \
			{ \
				result = member.Get ## rapidjsonType (); \
			} \
		} \
		\
		return result; \
	}


	MAGE_IMPLEMENT_GET_JSON_VALUE( bool, Bool );
	MAGE_IMPLEMENT_GET_JSON_VALUE( double, Double );
	MAGE_IMPLEMENT_GET_JSON_VALUE( int, Int );
	MAGE_IMPLEMENT_GET_JSON_VALUE( int64_t, Int64 );
	MAGE_IMPLEMENT_GET_JSON_VALUE( unsigned int, Uint );
	MAGE_IMPLEMENT_GET_JSON_VALUE( uint64_t, Uint64 );
	MAGE_IMPLEMENT_GET_JSON_VALUE( const char*, String );


	inline std::string ConvertJSONToString( const rapidjson::Value& object )
	{
		// Create a buffer and a JSON writer.
		rapidjson::GenericStringBuffer< rapidjson::UTF8<> > buffer;
		rapidjson::Writer< rapidjson::GenericStringBuffer< rapidjson::UTF8<> > > writer( buffer );

		// Write the JSON object to the buffer.
		object.Accept( writer );

		// Return the result;
		return std::string( buffer.GetString(), buffer.Size() );
	}
}
