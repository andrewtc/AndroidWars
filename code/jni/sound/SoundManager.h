/*
 * Author      : Matthew Johnson
 * Date        : 7/Dec/2013
 * Description :
 *   Mostly copied from NativeSound example
 */
 
#pragma once

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <SLES/OpenSLES_AndroidConfiguration.h>

namespace mage
{
	class SoundManager;

	class SoundClip
	{
	public:
		SoundClip( const char* path );
		~SoundClip();

		const char* GetPath() { return mResource->GetPath(); }

		bool Load();
		void Unload();
	private:
		friend class SoundManager;

		Resource* mResource;
		uint8* mBuffer;
		off_t mLength;
	};

	class SoundManager
	{
	public:
		SoundManager( android_app* pApp );
		~SoundManager();

		bool Start();
		void Stop();

		SoundClip* LoadSoundClip( const char* path, const HashString& name );
		SoundClip* GetSoundClip( const HashString& name );
		void PlaySound( SoundClip* clip );
		bool PlayMusic( const char* path );
		void StopMusic();

		void SetVolume( float volume );

	private:
		bool StartSoundPlayer();

		android_app* mApp;

		// OpenSL
		SLObjectItf mEngineObj;
		SLEngineItf mEngine;
		// Output
		SLObjectItf mOutputMixObj;
		SLVolumeItf mOutputMixVolume;
		// Music player
		SLObjectItf mAudioTrackPlayerObj;
		SLPlayItf mAudioTrackPlayer;
		SLSeekItf mAudioTrackPlayerSeek;
		// Sound player
		SLObjectItf mPlayerObj;
		SLPlayItf mPlayer;
		SLVolumeItf mPlayerVolume;
		SLBufferQueueItf mPlayerQueue;
		// SoundClips
		HashMap< SoundClip* > mSoundClips;
	};

}
