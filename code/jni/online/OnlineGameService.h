#pragma once


namespace mage
{
	/**
	 * Allows interaction with the AndroidWarsOnline web service.
	 */
	class OnlineGameService
	{
	public:
		OnlineGameService( android_app* app );
		~OnlineGameService();

		void startRequest( const char* methodName, const std::string& parameters );
		void pollRequestResults();

	private:
		JavaVM* mJavaVM;
		jclass mJavaClass;
		jobject mJavaObject;

		jmethodID mMethodStartRequest;
		jmethodID mMethodFetchResult;

		jclass mResultJavaClass;
		jfieldID mResultRequestID;
		jfieldID mResultHasError;
		jfieldID mResultJSON;
		jfieldID mResultErrorMessage;
	};
}
