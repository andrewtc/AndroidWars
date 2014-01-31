#include "androidwars.h"

using namespace mage;


OnlineGameClient::OnlineGameClient() :
	mJavaVM( nullptr ),
	mJavaClass( nullptr ),
	mJavaObject( nullptr ),
	mJavaCallCloudFunction( 0 ),
	mJavaFetchNextResponse( 0 ),
	mJavaResponseClass( nullptr ),
	mJavaResponseRequestID( 0 ),
	mJavaResponseStatusCode( 0 ),
	mJavaResponseResult( 0 ),
	mJavaResponseIsError( 0 )
{ }


OnlineGameClient::~OnlineGameClient()
{
	if( isInitialized() )
	{
		destroy();
	}
}


void OnlineGameClient::init( android_app* app )
{
	assertion( !isInitialized(), "Cannot initialize OnlineGameClient that has already been initialized!" );

	// Keep a reference to the Java virtual machine.
	assertion( app, "Cannot create OnlineGameClient without a reference to the Java app instance!" );
	mJavaVM = app->activity->vm;

	// Get a reference to the OnlineGameClient Java instance.
	JNIEnv* env;
	mJavaVM->AttachCurrentThread( &env, nullptr );

	jclass activityClass = env->GetObjectClass( app->activity->clazz );
	jmethodID getOnlineGameClient = env->GetMethodID( activityClass, "getOnlineGameClient", "()Lcom/timechildgames/androidwars/OnlineGameClient;" );
	assertion( getOnlineGameClient, "Could not find getOnlineGameClient() method in activity class!" );

	jobject instance = env->CallObjectMethod( app->activity->clazz, getOnlineGameClient );
	assertion( instance, "Java OnlineGameClient instance not initialized!" );

	// Store global references to the Java instance and class.
	mJavaObject = env->NewGlobalRef( instance );
	mJavaClass  = (jclass) env->NewGlobalRef( env->GetObjectClass( mJavaObject ) );

	// Store references to important Java methods.
	mJavaFetchNextResponse = env->GetMethodID( mJavaClass, "fetchNextResponse", "()Lcom/timechildgames/androidwars/OnlineRequestResponse;" );
	assertion( mJavaFetchNextResponse, "Java fetchNextResponse() function not found!" );

	mJavaCallCloudFunction = env->GetMethodID( mJavaClass, "callCloudFunction", "(Ljava/lang/String;Ljava/lang/String;)J" );
	assertion( mJavaFetchNextResponse, "Java callCloudFunction() function not found!" );
}


void OnlineGameClient::destroy()
{
	assertion( isInitialized(), "Cannot destroy OnlineGameClient that hasn't been initialized!" );

	JNIEnv* env;
	mJavaVM->AttachCurrentThread( &env, nullptr );

	// Destroy the references to the Java instance and class.
	env->DeleteGlobalRef( mJavaClass );
	env->DeleteGlobalRef( mJavaObject );

	// Destroy the references to the Java response class.
	if( mJavaResponseClass != nullptr )
	{
		env->DeleteGlobalRef( mJavaResponseClass );
	}

	mJavaVM->DetachCurrentThread();

	// Clear the list of open requests.
	mOpenRequestsByID.clear();
}


void OnlineGameClient::update()
{
	assertion( isInitialized(), "Cannot update OnlineGameClient that hasn't been initialized!" );

	if( mOpenRequestsByID.size() > 0 )
	{
		JNIEnv* env;
		mJavaVM->AttachCurrentThread( &env, nullptr );

		while( true )
		{
			//DebugPrintf( "Checking for responses..." );

			// Fetch the next response (if any).
			jobject response = fetchNextResponse( env );

			if( response == nullptr )
			{
				// If no response was returned, exit the loop.
				break;
			}

			DebugPrintf( "Got response!" );

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

			DebugPrintf( "RESPONSE %d: Status Code %d : \"%s\"", requestID, statusCode, resultText.c_str() );

			// Determine whether an error occurred.
			bool isError = env->CallBooleanMethod( response, mJavaResponseIsError );
			DebugPrintf( "Response %s an error.", ( isError ? "is" : "is not" ) );

			// Look up the response info.
			auto it = mOpenRequestsByID.find( requestID );
			assertion( it != mOpenRequestsByID.end(), "No RequestInfo found for response %d!", requestID );

			const RequestInfo& requestInfo = it->second;

			if( isError )
			{
				// If an error occurred, call the error callback (if any).
				if( requestInfo.onFailure != nullptr )
				{
					requestInfo.onFailure( statusCode, resultText );
				}
			}
			else
			{
				// If the response was successful, call the success callback (if any).
				if( requestInfo.onSuccess != nullptr )
				{
					requestInfo.onSuccess( statusCode, resultText );
				}
			}

			// Call the completion callback (if any).
			if( requestInfo.onComplete != nullptr )
			{
				requestInfo.onComplete( statusCode, resultText );
			}

			// Remove the request from the list of open requests.
			mOpenRequestsByID.erase( it );
		}

		mJavaVM->DetachCurrentThread();
	}
}


long OnlineGameClient::callCloudFunction( const std::string& functionName, const std::string& parameters,
										  RequestCallback onSuccess, RequestCallback onFailure, RequestCallback onComplete )
{
	assertion( isInitialized(), "Cannot call cloud function \"%s\" from OnlineGameClient that hasn't been initialized!", functionName.c_str() );

	long requestID = -1;

	JNIEnv* env;
	mJavaVM->AttachCurrentThread( &env, nullptr );

	// Convert the function name and parameters into Java strings.
	jstring jstringFunctionName = env->NewStringUTF( functionName.c_str() );
	jstring jstringParameters   = env->NewStringUTF( parameters.c_str() );

	// Pass the cloud function call to the Java instance.
	requestID = env->CallLongMethod( mJavaObject, mJavaCallCloudFunction, jstringFunctionName, jstringParameters );

	mJavaVM->DetachCurrentThread();

	// Listen for responses to this request.
	RequestInfo requestInfo;

	requestInfo.onSuccess  = onSuccess;
	requestInfo.onFailure  = onFailure;
	requestInfo.onComplete = onComplete;

	mOpenRequestsByID[ requestID ] = requestInfo;

	// Return the ID of the new request.
	return requestID;
}


jobject OnlineGameClient::fetchNextResponse( JNIEnv* env )
{
	return env->CallObjectMethod( mJavaObject, mJavaFetchNextResponse );
}
