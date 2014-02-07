#pragma once

namespace mage
{
	/**
	 * Attaches to the JavaVM upon creation and maintains a reference counter to make sure
	 * that the JNI environment doesn't get detached before all references to have been
	 * destroyed.
	 */
	class JNI
	{
	public:
		static void Init( JavaVM* javaVM );
		static void Destroy();

		JNI();
		~JNI();

		JNIEnv* operator->();
		const JNIEnv* operator->() const;

	private:
		static JavaVM* sJavaVM;
		static JNIEnv* instance;
		static int referenceCount;
	};


	inline void JNI::Init( JavaVM* javaVM )
	{
		assertion( javaVM, "Tried to initialized JNI wrapper without a JavaVM instance!" );
		assertion( !sJavaVM, "Tried to initialize JNI wrapper that has already been initialized!" );
		sJavaVM = javaVM;
	}


	inline void JNI::Destroy()
	{
		assertion( sJavaVM, "Tried to destroy JNI wrapper that has not been initialized!" );
		sJavaVM = nullptr;
	}


	inline JNI::JNI()
	{
		assertion( sJavaVM, "Cannot create JNI reference because JNI wrapper has not been initialized!" );

		if( referenceCount == 0 )
		{
			// If the thread hasn't been attached yet, attach it and save the reference.
			//DebugPrintf( "Attaching to Java thread." );
			sJavaVM->AttachCurrentThread( &instance, nullptr );
		}

		// Increase the reference counter.
		++referenceCount;
		//DebugPrintf( "Incremented JNI reference count to %d.", referenceCount );
	}


	inline JNI::~JNI()
	{
		assertion( sJavaVM, "Cannot destroy JNI reference because JNI wrapper has not been initialized!" );

		// Decrease the reference counter.
		--referenceCount;
		//DebugPrintf( "Decremented JNI reference count to %d.", referenceCount );

		assertion( referenceCount >= 0, "JNI reference counter went negative!" );

		if( referenceCount == 0 )
		{
			// If the last reference has been destroyed, detach from the Java thread.
			//DebugPrintf( "Detaching from Java thread." );
			sJavaVM->DetachCurrentThread();
			instance = nullptr;
		}
	}


	inline JNIEnv* JNI::operator->()
	{
		assertion( instance, "Tried to reference JNI environment that was not yet attached!" );
		return instance;
	}


	inline const JNIEnv* JNI::operator->() const
	{
		assertion( instance, "Tried to reference JNI environment that was not yet attached!" );
		return instance;
	}
}
