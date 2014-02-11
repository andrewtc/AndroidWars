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
}
