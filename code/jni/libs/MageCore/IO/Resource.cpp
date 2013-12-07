#include "CoreLib.h"

using namespace mage;

//---------------------------------------
Resource::Resource( AAssetManager* pAssetManager, const char* path )
    : mPath( path )
    , mAssetManager( pAssetManager )
    , mAsset( 0 )
{}
//---------------------------------------
Resource::~Resource()
{
    Close();
}
//---------------------------------------
Resource::ResourceStatus Resource::Open()
{
    mAsset = AAssetManager_open( mAssetManager, mPath, AASSET_MODE_UNKNOWN );
    return mAsset ? RS_OK : RS_FAIL;
}
//---------------------------------------
void Resource::Close()
{
    if ( mAsset )
    {
        AAsset_close( mAsset );
        mAsset = 0;
    }
}
//---------------------------------------
Resource::ResourceStatus Resource::Read( void* pBuffer, size_t count )
{
    int32 readCount = AAsset_read( mAsset, pBuffer, count );
    return readCount == count ? RS_OK : RS_FAIL;
}
//---------------------------------------
ResourceDescriptor Resource::GetDescriptor() const
{
	 ResourceDescriptor descriptor = { -1, 0, 0 };

	AAsset* asset = AAssetManager_open( mAssetManager, mPath, AASSET_MODE_UNKNOWN );

	if( asset != NULL )
	{
		descriptor.mDescriptor = AAsset_openFileDescriptor( asset, &descriptor.mStart, &descriptor.mLength );
		AAsset_close( asset );
	}

	return descriptor;
}
//---------------------------------------
