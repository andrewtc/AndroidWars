#pragma once


namespace mage
{
	/**
	 * Accepts any function or lambda with the signature: void (*)( long, bool, int, const std::string& )
	 */
	typedef Callback< void, long, bool, int, const std::string& > RequestCallback;


	/**
	 * Sends and receives requests from the AndroidWarsOnline web service.
	 */
	class OnlineGameClient
	{
	public:
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

		long CallCloudFunction( const std::string& functionName, const std::string& parameters, RequestCallback callback = RequestCallback() );

		long LogIn( const std::string& userName, const std::string& password, RequestCallback callback = RequestCallback() );
		long RequestMatchmakingGame( GameType gameType, RequestCallback callback = RequestCallback() );

	private:
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

		std::map< long, RequestCallback > mCallbacksByID;
	};


	inline bool OnlineGameClient::IsInitialized() const
	{
		return ( mJavaObject != nullptr );
	}
}
