#include "androidwars.h"

using namespace mage;


OnlineGameService::OnlineGameService( android_app* app ) :
	mJavaVM( nullptr ), mJavaObject( nullptr ),
	mMethodStartRequest( nullptr ),
	mMethodFetchResult( nullptr ),
	mResultJavaClass( nullptr ),
	mResultRequestID( nullptr )
{
	DebugPrintf( "Initializing OnlineGameService..." );

	// Get the JNI environment.
	JNIEnv* env;
	mJavaVM = app->activity->vm;
	mJavaVM->AttachCurrentThread( &env, nullptr );
	DebugPrintf( "Attached to Java thread." );

	// Get the activity class.
	jobject activityObject = app->activity->clazz;
	jclass activityClass = env->GetObjectClass( activityObject );

	// Create a new OnlineGameService object using the activity.
	DebugPrintf( "Creating OnlineGameService Java object..." );

	const char* accessorMethodName = "getOnlineGameService";
	jmethodID accessorMethod = env->GetMethodID( activityClass, accessorMethodName, "()Lcom/timechildgames/androidwars/OnlineGameService;" );
	assertion( accessorMethod, "Java factory method \"%s\" could not be found!", accessorMethodName );

	// Create a new Java object.
	jobject javaObject = env->CallObjectMethod( activityObject, accessorMethod );
	mJavaObject = env->NewGlobalRef( javaObject );
	assertion( mJavaObject, "Could not create Java OnlineGameService object!" );

	// Grab relevant methods from the object class.
	jclass javaClass = env->GetObjectClass( mJavaObject );
	mJavaClass = (jclass) env->NewGlobalRef( javaClass );

	mMethodStartRequest = env->GetMethodID( javaClass, "startRequest", "(Ljava/lang/String;Ljava/lang/String;)I" );
	mMethodFetchResult  = env->GetMethodID( javaClass, "fetchRequestResult", "()Lcom/timechildgames/androidwars/OnlineRequestResult;" );

	// Detach from the Java thread.
	mJavaVM->DetachCurrentThread();
}


OnlineGameService::~OnlineGameService() { }


void OnlineGameService::startRequest( const char* methodName, const std::string& parameters )
{
	JNIEnv* env;
	mJavaVM->AttachCurrentThread( &env, nullptr );

	// Convert the passed parameters to Java strings.
	DebugPrintf( "Converting strings for new request..." );
	jstring methodNameString = env->NewStringUTF( methodName );
	jstring parametersString = env->NewStringUTF( parameters.c_str() );

	// Start a new request.
	DebugPrintf( "Starting request..." );
	int requestID = env->CallIntMethod( mJavaObject, mMethodStartRequest, methodNameString, parametersString );
	DebugPrintf( "Got back request ID %d.", requestID );

	mJavaVM->DetachCurrentThread();
}


void OnlineGameService::pollRequestResults()
{
	assertion( mJavaObject != nullptr, "Cannot poll requests for uninitialized OnlineGameService!" );

	JNIEnv* env;
	mJavaVM->AttachCurrentThread( &env, nullptr );

	while( true )
	{
		// Poll for another request result.
		//DebugPrintf( "Fetching next result..." );
		jobject result = env->CallObjectMethod( mJavaObject, mMethodFetchResult );

		if( !result )
		{
			// If no object was returned, exit the loop.
			break;
		}

		if( mResultJavaClass == nullptr )
		{
			// Grab relevant fields from the OnlineRequestResult class (as necessary).
			jclass resultJavaClass = env->GetObjectClass( result );
			mResultJavaClass = (jclass) env->NewGlobalRef( resultJavaClass );

			mResultRequestID    = env->GetFieldID( mResultJavaClass, "requestID", "I" );
			mResultHasError     = env->GetFieldID( mResultJavaClass, "hasError", "Z" );
			mResultJSON         = env->GetFieldID( mResultJavaClass, "result", "Ljava/lang/String;" );
			mResultErrorMessage = env->GetFieldID( mResultJavaClass, "errorMessage", "Ljava/lang/String;" );
		}

		// If a request result came in, handle it.
		int requestID = env->GetIntField( result, mResultRequestID );
		bool hasError = env->GetBooleanField( result, mResultHasError );
		jstring resultString = (jstring) env->GetObjectField( result, mResultJSON );
		jstring errorMessageString = (jstring) env->GetObjectField( result, mResultErrorMessage );

		std::string resultJSON = env->GetStringUTFChars( resultString, NULL );
		std::string errorMessage = env->GetStringUTFChars( errorMessageString, NULL );

		if( !hasError )
		{
			DebugPrintf( "Received result for request %d: %s", requestID, resultJSON.c_str() );
		}
		else
		{
			DebugPrintf( "Received error for request %d: %s", requestID, errorMessage.c_str() );
		}
	}

	mJavaVM->DetachCurrentThread();
}

