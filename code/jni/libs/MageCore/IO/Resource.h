/*
 * Author      : Matthew Johnson
 * Date        : 15/Nov/2013
 * Description :
 *   Manages a file system resource.
 */
 
#pragma once

#ifdef ANDROID
#   include <android/asset_manager.h>
#endif

namespace mage
{

	struct ResourceDescriptor
	{
	    int mDescriptor;
	    off_t mStart;
	    off_t mLength;
	};

	class Resource
	{
	public:
        enum ResourceStatus
        {
            RS_OK,
            RS_FAIL
        };

		Resource( AAssetManager* pAssetManager, const char* path );
		~Resource();
        
        ResourceStatus Open();
        void Close();
        ResourceStatus Read( void* pBuffer, size_t count );
        ResourceDescriptor GetDescriptor() const;
        
        inline const char* GetPath() const { return mPath.c_str(); }
        off_t GetLength() const { return AAsset_getLength( mAsset ); }
        
    private:
        const std::string mPath;
        AAssetManager* mAssetManager;
        AAsset* mAsset;
	};

}
