#include "androidwars.h"

using namespace mage;

OnlineRequestResult::OnlineRequestResult( long int requestID, bool isError, int statusCode, const std::string& result ) :
	requestID( requestID ),
	isError( isError ),
	statusCode( statusCode ),
	string( result )
{
	// Parse the result as JSON (if possible).
	json.Parse< 0 >( result.c_str() );
	resultIsJSON = ( !json.HasParseError() );
}


OnlineGameClient::OnlineGameClient() :
	mJavaClass( nullptr ),
	mJavaObject( nullptr ),
	mJavaCreateRequestParams( 0 ),
	mJavaFetchNextResponse( 0 ),
	mJavaSendGetRequest( 0 ),
	mJavaSendPostRequest( 0 ),
	mJavaResponseClass( nullptr ),
	mJavaResponseRequestID( 0 ),
	mJavaResponseStatusCode( 0 ),
	mJavaResponseResult( 0 ),
	mJavaResponseIsError( 0 ),
	mJavaRequestParamsClass( nullptr ),
	mJavaRequestParamsPut( 0 )
{ }


OnlineGameClient::~OnlineGameClient()
{
	if( IsInitialized() )
	{
		Destroy();
	}
}


void OnlineGameClient::Init( android_app* app )
{
	assertion( !IsInitialized(), "Cannot initialize OnlineGameClient that has already been initialized!" );

	// Initialize the JNI wrapper.
	assertion( app, "Cannot create OnlineGameClient without a reference to the Java app instance!" );
	JNI::Init( app->activity->vm );

	// Get a reference to the OnlineGameClient Java instance.
	JNI env;

	jclass activityClass = env->GetObjectClass( app->activity->clazz );
	jmethodID getOnlineGameClient = env->GetMethodID( activityClass, "getOnlineGameClient", "()Lcom/timechildgames/androidwars/OnlineGameClient;" );
	assertion( getOnlineGameClient, "Could not find getOnlineGameClient() method in activity class!" );

	jobject instance = env->CallObjectMethod( app->activity->clazz, getOnlineGameClient );
	assertion( instance, "Java OnlineGameClient instance not initialized!" );

	// Store global references to the Java instance and class.
	mJavaObject = env->NewGlobalRef( instance );
	mJavaClass  = (jclass) env->NewGlobalRef( env->GetObjectClass( mJavaObject ) );

	// Store references to important Java methods.
	mJavaCreateRequestParams = env->GetStaticMethodID( mJavaClass, "createRequestParams", "()Lcom/loopj/android/http/RequestParams;" );
	assertion( mJavaCreateRequestParams, "Java createRequestParams() function not found!" );

	mJavaFetchNextResponse = env->GetMethodID( mJavaClass, "fetchNextResponse", "()Lcom/timechildgames/androidwars/OnlineRequestResponse;" );
	assertion( mJavaFetchNextResponse, "Java fetchNextResponse() function not found!" );

	mJavaSendGetRequest = env->GetMethodID( mJavaClass, "sendGetRequest", "(Ljava/lang/String;Lcom/loopj/android/http/RequestParams;Ljava/lang/String;)J" );
	assertion( mJavaSendGetRequest, "Java sendGetRequest() function not found!" );

	mJavaSendPostRequest = env->GetMethodID( mJavaClass, "sendPostRequest", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)J" );
	assertion( mJavaSendPostRequest, "Java sendPostRequest() function not found!" );
}


void OnlineGameClient::Destroy()
{
	assertion( IsInitialized(), "Cannot destroy OnlineGameClient that hasn't been initialized!" );

	{
		JNI env;

		// Destroy the references to the Java instance and class.
		env->DeleteGlobalRef( mJavaClass );
		env->DeleteGlobalRef( mJavaObject );

		// Destroy the references to the Java response class.
		if( mJavaResponseClass != nullptr )
		{
			env->DeleteGlobalRef( mJavaResponseClass );
		}
	}

	// Clear the list of open requests.
	mCallbacksByID.clear();

	// Destroy JNI wrapper.
	JNI::Destroy();
}


void OnlineGameClient::Update()
{
	assertion( IsInitialized(), "Cannot update OnlineGameClient that hasn't been initialized!" );

	if( mCallbacksByID.size() > 0 )
	{
		JNI env;

		while( mCallbacksByID.size() > 0 )
		{
			//DebugPrintf( "Checking for responses..." );

			// Fetch the next response (if any).
			jobject response = FetchNextResponse();

			if( response != nullptr )
			{
				//DebugPrintf( "Got response!" );

				if( mJavaResponseClass == nullptr )
				{
					// Store a global reference to the Java OnlineRequestResponse class (as necessary).
					mJavaResponseClass = (jclass) env->NewGlobalRef( env->GetObjectClass( response ) );

					mJavaResponseRequestID  = env->GetFieldID( mJavaResponseClass, "requestID", "J" );
					mJavaResponseStatusCode = env->GetFieldID( mJavaResponseClass, "statusCode", "I" );
					mJavaResponseResult     = env->GetFieldID( mJavaResponseClass, "result", "Ljava/lang/String;" );

					mJavaResponseIsError = env->GetMethodID( mJavaResponseClass, "isError", "()Z" );
				}

				// If a response has come in, fetch all relevant info from the response.
				long requestID = env->GetLongField( response, mJavaResponseRequestID );
				int statusCode = env->GetIntField( response, mJavaResponseStatusCode );
				jstring result = (jstring) env->GetObjectField( response, mJavaResponseResult );

				std::string resultText = env->GetStringUTFChars( result, nullptr );

				//DebugPrintf( "RESPONSE %d: Status Code %d : \"%s\"", requestID, statusCode, resultText.c_str() );

				// Determine whether an error occurred.
				bool isError = env->CallBooleanMethod( response, mJavaResponseIsError );
				//DebugPrintf( "Response %s an error.", ( isError ? "is" : "is not" ) );

				// Look up the response info.
				auto it = mCallbacksByID.find( requestID );
				//assertion( it != mCallbacksByID.end(), "No RequestInfo found for response %d!", requestID );

				if( it != mCallbacksByID.end() )
				{
					// Combine the request result information into an object.
					OnlineRequestResult resultObject( requestID, isError, statusCode, resultText );

					// Call the request callback.
					OnlineRequestCallback callback = it->second;
					callback.Invoke( resultObject );

					// Remove the request from the list of open requests.
					mCallbacksByID.erase( it );
				}
			}
			else
			{
				// If no responses were queued, stop checking for responses.
				break;
			}
		}
	}
}


long OnlineGameClient::SendGetRequest( const std::string& resource, const OnlineRequestParameters& parameters, OnlineRequestCallback callback )
{
	assertion( IsInitialized(), "Cannot send request \"%s\" from OnlineGameClient that hasn't been initialized!", resource.c_str() );

	long requestID = -1;

	{
		JNI env;

		// Convert the resource name and session token into Java strings.
		jstring jstringResource     = env->NewStringUTF( resource.c_str() );
		jstring jstringSessionToken = env->NewStringUTF( mUserSessionToken.c_str() );

		// Convert request parameters into a Java object.
		jobject javaRequestParams = CreateJavaRequestParams( parameters );

		// Pass the cloud function call to the Java instance.
		requestID = env->CallLongMethod( mJavaObject, mJavaSendGetRequest, jstringResource, javaRequestParams, jstringSessionToken );
	}

	if( callback.IsValid() )
	{
		// If a Callback was specified, listen for responses to this request.
		mCallbacksByID[ requestID ] = callback;
	}

	// Return the ID of the new request.
	return requestID;
}


long OnlineGameClient::SendPostRequest( const std::string& resource, const std::string& data, OnlineRequestCallback callback )
{
	assertion( IsInitialized(), "Cannot send request \"%s\" from OnlineGameClient that hasn't been initialized!", resource.c_str() );

	long requestID = -1;

	{
		JNI env;

		// Convert the resource name, data, and session token into Java strings.
		jstring jstringResource     = env->NewStringUTF( resource.c_str() );
		jstring jstringData         = env->NewStringUTF( data.c_str() );
		jstring jstringSessionToken = env->NewStringUTF( mUserSessionToken.c_str() );

		// Pass the cloud function call to the Java instance.
		requestID = env->CallLongMethod( mJavaObject, mJavaSendPostRequest, jstringResource, jstringData, jstringSessionToken );
	}

	assertion( requestID >= 0, "Received bad requestID!" );

	if( callback.IsValid() )
	{
		// If a Callback was specified, listen for responses to this request.
		mCallbacksByID[ requestID ] = callback;
	}

	// Return the ID of the new request.
	return requestID;
}


long OnlineGameClient::LogIn( const std::string& userName, const std::string& password, OnlineLoginCallback callback )
{
	// Log out the previous user (if any).
	LogOut();

	// Format the request parameters.
	OnlineRequestParameters parameters;
	parameters.push_back( OnlineRequestParameter( "username", userName ) );
	parameters.push_back( OnlineRequestParameter( "password", password ) );

	// Log in using the credentials provided.
	return SendGetRequest( "login", parameters, [this, callback]( const OnlineRequestResult& result )
	{
		DebugPrintf( "Received login response: \"%s\"", result.string.c_str() );

		bool success = ( !result.isError );

		if( success )
		{
			// Make sure a valid session token was returned.
			success = result.json.HasMember( SESSION_TOKEN_KEY );

			if( success )
			{
				// Pull the user session token from the result.
				mUserSessionToken = result.json[ SESSION_TOKEN_KEY ].GetString();
				DebugPrintf( "User session token is now \"%s\".", mUserSessionToken.c_str() );
			}
			else
			{
				DebugPrintf( "Login failed: No session token returned with response!" );
				DebugPrintf( "Response: \"%s\"", result.string.c_str() );
			}
		}
		else
		{
			DebugPrintf( "An error occurred." );
		}

		//DebugPrintf( "Checking callback..." );
		if( callback.IsValid() )
		{
			// Fire off the request callback (if any).
			callback.Invoke( success );
		}
	});
}


void OnlineGameClient::LogOut()
{
	// Clear the session token.
	mUserSessionToken = "";
	DebugPrintf( "Logged out." );
}


void OnlineGameClient::RequestCurrentGamesList( OnlineGameListCallback callback )
{
	// Fire off the request.
	CallCloudFunction( "getCurrentGameList", "{}", [ this, callback ]( const OnlineRequestResult& result )
	{
		bool success = false;
		std::vector< OnlineGameListData > onlineGameList;

		if( !result.isError )
		{
			if( result.resultIsJSON )
			{
				//DebugPrintf( "Current games: \"%s\"", result.string.c_str() );

				const rapidjson::Value& list = result.json[ "result" ];
				assertion( list.IsArray(), "Result of game list request is not an array!" );

				success = true;

				for( rapidjson::SizeType i = 0; i < list.Size(); ++i )
				{
					const rapidjson::Value& gameJSON = list[ i ];

					// Construct a new game data entry.
					OnlineGameListData gameData;

					gameData.id   = GetJSONStringValue( gameJSON, "id", "" );
					gameData.name = GetJSONStringValue( gameJSON, "name", "" );

					if( !gameData.id.empty() && !gameData.name.empty() )
					{
						// Add the new entry to the list of current games.
						onlineGameList.push_back( gameData );
					}
					else
					{
						WarnFail( "Received invalid game data! (id=\"%s\", name=\"%s\").", gameData.id.c_str(), gameData.name.c_str() );
					}
				}
			}
			else
			{
				WarnFail( "The games list response was not a JSON object!" );
			}
		}
		else
		{
			WarnFail( "Error retrieving current games list: %s", result.string.c_str() );
		}

		if( callback.IsValid() )
		{
			// Pass the list of games to the success callback.
			callback.Invoke( success, onlineGameList );
		}
	});
}


void OnlineGameClient::RequestGameData( const std::string& gameID, OnlineGameCallback callback )
{
	// Format parameters.
	rapidjson::Document parameters;
	parameters.SetObject();

	rapidjson::Value gameIDValue;
	gameIDValue.SetString( gameID.c_str(), gameID.length() );

	parameters.AddMember( "id", gameIDValue, parameters.GetAllocator() );

	// Fire off the request.
	CallCloudFunction( "getGame", ConvertJSONToString( parameters ), [ this, callback, gameID ]( const OnlineRequestResult& result )
	{
		bool success = false;
		OnlineGameData gameData;

		if( !result.isError )
		{
			if( result.resultIsJSON )
			{
				success = true;

				const rapidjson::Value& gameJSON = result.json[ "result" ];

				// Construct a new game data entry.
				gameData.id        = GetJSONStringValue( gameJSON, "id", "" );
				gameData.name      = GetJSONStringValue( gameJSON, "name", "" );

				// Convert the JSON value to string.
				gameData.gameState = ConvertJSONToString( gameJSON[ "currentState" ] );

				if( !gameData.id.empty() && !gameData.name.empty() && !gameData.gameState.empty() )
				{
				}
				else
				{
					WarnFail( "Received invalid game data! (id=\"%s\", name=\"%s\").", gameData.id.c_str(), gameData.name.c_str() );
				}
			}
			else
			{
				WarnFail( "The games list response was not a JSON object!" );
			}
		}
		else
		{
			WarnFail( "Error retrieving current game \"%s\": %s", gameID.c_str(), result.string.c_str() );
		}

		if( callback.IsValid() )
		{
			// Pass the list of games to the success callback.
			callback.Invoke( success, gameData );
		}
	});
}


void OnlineGameClient::PostTurn( const std::string& gameState )
{
	// Fire off the request.
	CallCloudFunction( "postTurn", gameState );
}


jobject OnlineGameClient::CreateJavaRequestParams( const OnlineRequestParameters& parameters )
{
	assertion( IsInitialized(), "Cannot create Java RequestParams object for OnlineGameClient that hasn't been initialized!" );
	JNI env;

	// Create a new Java object for the params.
	jobject javaRequestParams = env->CallStaticObjectMethod( mJavaClass, mJavaCreateRequestParams );
	assertion( javaRequestParams, "Could not create Java RequestParams object!" );

	if( mJavaRequestParamsClass == nullptr )
	{
		// If the Java RequestParams class hasn't been cached, cache it.
		mJavaRequestParamsClass = (jclass) env->NewGlobalRef( env->GetObjectClass( javaRequestParams ) );

		// Cache the put() method ID.
		mJavaRequestParamsPut = env->GetMethodID( mJavaRequestParamsClass, "put", "(Ljava/lang/String;Ljava/lang/String;)V" );
		assertion( mJavaRequestParamsPut, "No RequestParams.put() method found!" );
	}

	for( auto it = parameters.begin(); it != parameters.end(); ++it )
	{
		// Convert each parameter name and value to a Java string.
		jstring name  = env->NewStringUTF( it->first.c_str() );
		jstring value = env->NewStringUTF( it->second.c_str() );

		// Upload all parameters to the Java object.
		env->CallVoidMethod( javaRequestParams, mJavaRequestParamsPut, name, value );
	}

	// Return the Java object.
	return javaRequestParams;
}


jobject OnlineGameClient::FetchNextResponse()
{
	assertion( IsInitialized(), "Cannot fetch response for OnlineGameClient that hasn't been initialized!" );
	JNI env;

	jobject result = env->CallObjectMethod( mJavaObject, mJavaFetchNextResponse );
	return result;
}
