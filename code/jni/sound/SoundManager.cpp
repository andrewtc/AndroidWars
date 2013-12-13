#include <MageApp.h>
#include "SoundManager.h"

using namespace mage;

//---------------------------------------
// SoundClip
SoundClip::SoundClip( const char* path )
	: mBuffer( NULL )
	, mLength( 0 )
{
	mResource = CreateResourceHandle( path );
}
//---------------------------------------
SoundClip::~SoundClip()
{
	delete mResource;
}
//---------------------------------------
bool SoundClip::Load()
{
	DebugPrintf( "Loading sound %s", mResource->GetPath() );
	Resource::ResourceStatus lRes;

	// Open the sound file.
	if( mResource->Open() != Resource::RS_OK )
		return false;

	// Read the sound file.
	mLength = mResource->GetLength();
	mBuffer = new uint8[mLength];
	lRes = mResource->Read( mBuffer, mLength );
	mResource->Close();

	if( lRes != Resource::RS_OK )
	{
		FatalError( "Error while reading WAV or PCM sound file." );
		return false;
	}
	return true;
}
//---------------------------------------
void SoundClip::Unload()
{
	delete[] mBuffer;
	mBuffer = NULL;
	mLength = 0;
}
//---------------------------------------

//---------------------------------------
// SoundManager
SoundManager::SoundManager( android_app* pApp )
	: mApp( pApp)
	, mEngineObj( NULL )
	, mEngine( NULL )
	, mOutputMixObj(NULL )
	, mAudioTrackPlayerObj( NULL )
	, mAudioTrackPlayer( NULL )
	, mAudioTrackPlayerSeek(NULL )
	, mPlayerObj( NULL )
	, mPlayer( NULL )
	, mPlayerQueue(NULL )
{
}
//---------------------------------------
SoundManager::~SoundManager()
{
	DestroyMapByValue( mSoundClips );
}
//---------------------------------------
bool SoundManager::Start()
{
	DebugPrintf( "Starting SoundService." );

	SLresult lRes;
	const SLuint32 lEngineMixIIDCount = 1;
	const SLInterfaceID lEngineMixIIDs[] = { SL_IID_ENGINE };
	const SLboolean lEngineMixReqs[] = { SL_BOOLEAN_TRUE };
	const SLuint32 lOutputMixIIDCount = 1;
	const SLInterfaceID lOutputMixIIDs[] = { SL_IID_VOLUME };
	const SLboolean lOutputMixReqs[] = { SL_BOOLEAN_FALSE };

	// Creates OpenSL ES engine and dumps its capabilities.
	lRes = slCreateEngine( &mEngineObj, 0, NULL, lEngineMixIIDCount, lEngineMixIIDs, lEngineMixReqs );

	if( lRes != SL_RESULT_SUCCESS )
		goto ERROR;

	lRes = (*mEngineObj)->Realize( mEngineObj, SL_BOOLEAN_FALSE );

	if( lRes != SL_RESULT_SUCCESS )
		goto ERROR;

	lRes = (*mEngineObj)->GetInterface( mEngineObj, SL_IID_ENGINE, &mEngine );

	if( lRes != SL_RESULT_SUCCESS )
		goto ERROR;

	// Creates audio output.
	lRes = (*mEngine)->CreateOutputMix( mEngine, &mOutputMixObj, lOutputMixIIDCount, lOutputMixIIDs, lOutputMixReqs );
	lRes = (*mOutputMixObj)->Realize( mOutputMixObj, SL_BOOLEAN_FALSE );
	lRes = (*mOutputMixObj)->GetInterface( mOutputMixObj, SL_IID_VOLUME, &mOutputMixVolume );
	if( lRes != SL_RESULT_SUCCESS )
	{
		mOutputMixVolume = 0;
	}

	// Set-up sound player.
	if( !StartSoundPlayer() )
		goto ERROR;

	// Load registered sounds.
	for( auto itr = mSoundClips.begin(); itr != mSoundClips.end(); ++itr )
	{
		SoundClip* clip = itr->second;
		if( !clip->Load() )
			goto ERROR;
	}

	return true;

	ERROR:
		FatalError( "Error while starting SoundService" );
	Stop();
	return false;
}
//---------------------------------------
void SoundManager::Stop()
{
	DebugPrintf( "Stopping SoundService." );

	// Stops and destroys Audio Track player.
	StopMusic();

	// Destroy sound player.
	if( mPlayerObj != NULL )
	{
		(*mPlayerObj)->Destroy( mPlayerObj );
		mPlayerObj = NULL;
		mPlayer = NULL;
		mPlayerQueue = NULL;
	}

	// Destroy audio output and engine.
	if( mOutputMixObj != NULL )
	{
		(*mOutputMixObj)->Destroy( mOutputMixObj );
		mOutputMixObj = NULL;
	}

	if( mEngineObj != NULL )
	{
		(*mEngineObj)->Destroy( mEngineObj );
		mEngineObj = NULL;
		mEngine = NULL;
	}

	// Unload or free sound resources.
	for( auto itr = mSoundClips.begin(); itr != mSoundClips.end(); ++itr )
	{
		SoundClip* clip = itr->second;
		clip->Unload();
	}
}
//---------------------------------------
SoundClip* SoundManager::LoadSoundClip( const char* path, const HashString& name )
{
	SoundClip* clip = GetSoundClip( name );
	if ( !clip )
	{
		clip = new SoundClip( path );
		mSoundClips[ name ] = clip;
	}
	return clip;
}
//---------------------------------------
SoundClip* SoundManager::GetSoundClip( const HashString& name )
{
	SoundClip* clip = 0;
	auto itr = mSoundClips.find( name );
	if ( itr != mSoundClips.end() )
		clip = itr->second;
	return clip;
}
//---------------------------------------
void SoundManager::PlaySound( SoundClip* clip )
{
	SLresult lRes;
	SLuint32 lPlayerState;

	if ( !clip )
	{
		WarnCrit( "SoundManager::PlaySound() : Passed NULL clip!" );
		return;
	}

	(*mPlayerObj)->GetState( mPlayerObj, &lPlayerState );

	if( lPlayerState == SL_OBJECT_STATE_REALIZED )
	{
		int16_t* lBuffer = (int16_t*) clip->mBuffer;
		off_t lLength = clip->mLength;

		// Remove any sound from the queue.
		lRes = (*mPlayerQueue)->Clear( mPlayerQueue );

		if( lRes != SL_RESULT_SUCCESS )
			goto ERROR;

		// Play the new sound.
		lRes = (*mPlayerQueue)->Enqueue( mPlayerQueue, lBuffer, lLength );

		if( lRes != SL_RESULT_SUCCESS )
			goto ERROR;
	}

	return;

	ERROR:
		FatalError( "Error trying to play sound" );
}
//---------------------------------------
bool SoundManager::PlayMusic( const char* path )
{
	DebugPrintf( "Opening Audio Track %s", path );

	SLresult lRes;

	// Set-up Audio Track audio source.
	Resource* lResource = CreateResourceHandle( path );
	ResourceDescriptor lDescriptor = lResource->GetDescriptor();

	if( lDescriptor.mDescriptor < 0 )
	{
		WarnFail( "Could not open Audio Track file" );
		return false;
	}

	SLDataLocator_AndroidFD lDataLocatorIn;
	lDataLocatorIn.locatorType = SL_DATALOCATOR_ANDROIDFD;
	lDataLocatorIn.fd = lDescriptor.mDescriptor;
	lDataLocatorIn.offset = lDescriptor.mStart;
	lDataLocatorIn.length = lDescriptor.mLength;

	SLDataFormat_MIME lDataFormat;
	lDataFormat.formatType = SL_DATAFORMAT_MIME;
	lDataFormat.mimeType = NULL;
	lDataFormat.containerType = SL_CONTAINERTYPE_UNSPECIFIED;

	SLDataSource lDataSource;
	lDataSource.pLocator = &lDataLocatorIn;
	lDataSource.pFormat = &lDataFormat;

	SLDataLocator_OutputMix lDataLocatorOut;
	lDataLocatorOut.locatorType = SL_DATALOCATOR_OUTPUTMIX;
	lDataLocatorOut.outputMix = mOutputMixObj;

	SLDataSink lDataSink;
	lDataSink.pLocator = &lDataLocatorOut;
	lDataSink.pFormat = NULL;

	// Create Audio Track player and retrieves its interfaces.
	const SLuint32 lAudioTrackPlayerIIDCount = 2;
	const SLInterfaceID lAudioTrackPlayerIIDs[] = { SL_IID_PLAY, SL_IID_SEEK };
	const SLboolean lAudioTrackPlayerReqs[] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };

	lRes = (*mEngine)->CreateAudioPlayer( mEngine, &mAudioTrackPlayerObj, &lDataSource, &lDataSink,
										   lAudioTrackPlayerIIDCount, lAudioTrackPlayerIIDs, lAudioTrackPlayerReqs );

	if( lRes != SL_RESULT_SUCCESS )
		goto ERROR;

	lRes = (*mAudioTrackPlayerObj)->Realize( mAudioTrackPlayerObj, SL_BOOLEAN_FALSE );

	if( lRes != SL_RESULT_SUCCESS )
		goto ERROR;

	lRes = (*mAudioTrackPlayerObj)->GetInterface( mAudioTrackPlayerObj, SL_IID_PLAY, &mAudioTrackPlayer );

	if( lRes != SL_RESULT_SUCCESS )
		goto ERROR;

	lRes = (*mAudioTrackPlayerObj)->GetInterface( mAudioTrackPlayerObj, SL_IID_SEEK, &mAudioTrackPlayerSeek );

	if( lRes != SL_RESULT_SUCCESS )
		goto ERROR;

	// Enable looping and start playing!
	lRes = (*mAudioTrackPlayerSeek)->SetLoop( mAudioTrackPlayerSeek, SL_BOOLEAN_TRUE, 0, SL_TIME_UNKNOWN );

	if( lRes != SL_RESULT_SUCCESS )
		goto ERROR;

	lRes = (*mAudioTrackPlayer)->SetPlayState( mAudioTrackPlayer, SL_PLAYSTATE_PLAYING );

	if( lRes != SL_RESULT_SUCCESS )
		goto ERROR;

	return true;

	ERROR: return false;
}
//---------------------------------------
bool SoundManager::StartSoundPlayer()
{
	DebugPrintf( "Starting sound player." );
	SLresult lRes;

	// Set-up sound audio source.
	SLDataLocator_AndroidSimpleBufferQueue lDataLocatorIn;
	lDataLocatorIn.locatorType = SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE;
	// At most one buffer in the queue.
	lDataLocatorIn.numBuffers = 1;

	SLDataFormat_PCM lDataFormat;
	lDataFormat.formatType    = SL_DATAFORMAT_PCM;
	lDataFormat.numChannels   = 1; // Mono sound.
	lDataFormat.samplesPerSec = SL_SAMPLINGRATE_44_1;
	lDataFormat.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
	lDataFormat.containerSize = SL_PCMSAMPLEFORMAT_FIXED_16;
	lDataFormat.channelMask   = SL_SPEAKER_FRONT_CENTER;
	lDataFormat.endianness    = SL_BYTEORDER_LITTLEENDIAN;

	SLDataSource lDataSource;
	lDataSource.pLocator = &lDataLocatorIn;
	lDataSource.pFormat = &lDataFormat;

	SLDataLocator_OutputMix lDataLocatorOut;
	lDataLocatorOut.locatorType = SL_DATALOCATOR_OUTPUTMIX;
	lDataLocatorOut.outputMix = mOutputMixObj;

	SLDataSink lDataSink;
	lDataSink.pLocator = &lDataLocatorOut;
	lDataSink.pFormat = NULL;

	// Create the sounds player and retrieves its interfaces.
	const SLuint32 lSoundPlayerIIDCount = 3;
	const SLInterfaceID lSoundPlayerIIDs[] = { SL_IID_PLAY, SL_IID_BUFFERQUEUE, SL_IID_VOLUME };
	const SLboolean lSoundPlayerReqs[] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };

	lRes = (*mEngine)->CreateAudioPlayer( mEngine, &mPlayerObj, &lDataSource, &lDataSink,
										   lSoundPlayerIIDCount, lSoundPlayerIIDs, lSoundPlayerReqs );

	if( lRes != SL_RESULT_SUCCESS )
		goto ERROR;

	lRes = (*mPlayerObj)->Realize( mPlayerObj, SL_BOOLEAN_FALSE );

	if( lRes != SL_RESULT_SUCCESS )
		goto ERROR;

	lRes = (*mPlayerObj)->GetInterface( mPlayerObj, SL_IID_PLAY, &mPlayer );

	if( lRes != SL_RESULT_SUCCESS )
		goto ERROR;

	lRes = (*mPlayerObj)->GetInterface( mPlayerObj, SL_IID_BUFFERQUEUE, &mPlayerQueue );

	if( lRes != SL_RESULT_SUCCESS )
		goto ERROR;

	lRes = (*mPlayerObj)->GetInterface( mPlayerObj, SL_IID_VOLUME, &mPlayerVolume );

	if( lRes != SL_RESULT_SUCCESS )
	{
		FatalError( "SoundManager : Failed to get volume interface" );
		goto ERROR;
	}

	// Start the sound player. Nothing can be heard while the sound queue remains empty.
	lRes = (*mPlayer)->SetPlayState( mPlayer, SL_PLAYSTATE_PLAYING );

	if( lRes != SL_RESULT_SUCCESS )
		goto ERROR;

	return true;

	ERROR:
		FatalError( "Error while starting SoundPlayer" );
	return false;
}
//---------------------------------------
void SoundManager::StopMusic()
{
	if( mAudioTrackPlayer != NULL )
	{
		SLuint32 lAudioTrackPlayerState;
		(*mAudioTrackPlayerObj)->GetState( mAudioTrackPlayerObj, &lAudioTrackPlayerState );

		if( lAudioTrackPlayerState == SL_OBJECT_STATE_REALIZED )
		{
			(*mAudioTrackPlayer)->SetPlayState( mAudioTrackPlayer, SL_PLAYSTATE_PAUSED );

			(*mAudioTrackPlayerObj)->Destroy( mAudioTrackPlayerObj );
			mAudioTrackPlayerObj = NULL;
			mAudioTrackPlayer = NULL;
			mAudioTrackPlayerSeek = NULL;
		}
	}
}
//---------------------------------------
float gain_to_attenuation( float gain )
{
    return gain < 0.01F ? -96.0F : 20 * log10( gain );
}
void SoundManager::SetVolume( float volume )
{
	if ( !mOutputMixVolume )
	{
		(*mPlayerVolume)->SetVolumeLevel( mPlayerVolume, (SLmillibel)(gain_to_attenuation( volume * volume ) * 100 ) );
	}
}
//---------------------------------------
