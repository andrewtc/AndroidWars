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
	if( IsInitialized() )
	{
		Destroy();
	}
}


void OnlineGameClient::Init( android_app* app )
{
	assertion( !IsInitialized(), "Cannot initialize OnlineGameClient that has already been initialized!" );

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


void OnlineGameClient::Destroy()
{
	assertion( IsInitialized(), "Cannot destroy OnlineGameClient that hasn't been initialized!" );

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
	mCallbacksByID.clear();
}


void OnlineGameClient::Update()
{
	assertion( IsInitialized(), "Cannot update OnlineGameClient that hasn't been initialized!" );

	if( mCallbacksByID.size() > 0 )
	{
		JNIEnv* env;
		mJavaVM->AttachCurrentThread( &env, nullptr );

		while( true )
		{
			//DebugPrintf( "Checking for responses..." );

			// Fetch the next response (if any).
			jobject response = FetchNextResponse( env );

			if( response == nullptr )
			{
				// If no response was returned, exit the loop.
				break;
			}

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
				// Call the request callback.
				RequestCallback callback = it->second;
				callback( requestID, isError, statusCode, resultText );

				// Remove the request from the list of open requests.
				mCallbacksByID.erase( it );
			}
		}

		mJavaVM->DetachCurrentThread();
	}
}


long OnlineGameClient::CallCloudFunction( const std::string& functionName, const std::string& parameters, RequestCallback callback )
{
	assertion( IsInitialized(), "Cannot call cloud function \"%s\" from OnlineGameClient that hasn't been initialized!", functionName.c_str() );

	long requestID = -1;

	JNIEnv* env;
	mJavaVM->AttachCurrentThread( &env, nullptr );

	// Convert the function name and parameters into Java strings.
	jstring jstringFunctionName = env->NewStringUTF( functionName.c_str() );
	jstring jstringParameters   = env->NewStringUTF( parameters.c_str() );

	// Pass the cloud function call to the Java instance.
	requestID = env->CallLongMethod( mJavaObject, mJavaCallCloudFunction, jstringFunctionName, jstringParameters );

	mJavaVM->DetachCurrentThread();

	if( callback.IsValid() )
	{
		// If a Callback was specified, listen for responses to this request.
		mCallbacksByID[ requestID ] = callback;
	}

	// Return the ID of the new request.
	return requestID;
}


jobject OnlineGameClient::FetchNextResponse( JNIEnv* env )
{
	assertion( IsInitialized(), "Cannot fetch response for OnlineGameClient that hasn't been initialized!" );
	return env->CallObjectMethod( mJavaObject, mJavaFetchNextResponse );
}
