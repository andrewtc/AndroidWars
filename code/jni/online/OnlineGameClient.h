#pragma once

#include <functional>


namespace mage
{
	/**
	 * Sends and receives requests from the AndroidWarsOnline web service.
	 */
	class OnlineGameClient
	{
	public:
		typedef void (*RequestCallback)( long, int, const std::string& );

		enum GameType
		{
			GAME_TYPE_DUEL,
			GAME_TYPE_ALLIANCES
		};

		OnlineGameClient();
		~OnlineGameClient();

		void Init( android_app* app );
		void Destroy();
		void Update();
		bool IsInitialized() const;

		long CallCloudFunction( const std::string& functionName, const std::string& parameters,
								RequestCallback onSuccess = nullptr,
								RequestCallback onFailure = nullptr,
								RequestCallback onComplete = nullptr );

		long LogIn( const std::string& userName, const std::string& password,
					RequestCallback onSuccess = nullptr,
					RequestCallback onFailure = nullptr,
					RequestCallback onComplete = nullptr );

		long RequestMatchmakingGame( GameType gameType,
									 RequestCallback onSuccess = nullptr,
									 RequestCallback onFailure = nullptr,
									 RequestCallback onComplete = nullptr );

	private:
		struct RequestInfo
		{
			RequestInfo();

			RequestCallback onSuccess;
			RequestCallback onFailure;
			RequestCallback onComplete;
		};

		jobject FetchNextResponse( JNIEnv* env );

		JavaVM* mJavaVM;
		jclass mJavaClass;
		jobject mJavaObject;
		jmethodID mJavaFetchNextResponse;
		jmethodID mJavaCallCloudFunction;

		jclass mJavaResponseClass;
		jfieldID mJavaResponseRequestID;
		jfieldID mJavaResponseStatusCode;
		jfieldID mJavaResponseResult;
		jmethodID mJavaResponseIsError;

		std::map< long, RequestInfo > mOpenRequestsByID;
	};


	inline OnlineGameClient::RequestInfo::RequestInfo() :
		onSuccess( nullptr ), onFailure( nullptr ), onComplete( nullptr )
	{ }


	inline bool OnlineGameClient::IsInitialized() const
	{
		return ( mJavaObject != nullptr );
	}
}
