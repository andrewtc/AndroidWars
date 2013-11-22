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
        
        inline const char* GetPath() const { return mPath; }
        
    private:
        const char* mPath;
        AAssetManager* mAssetManager;
        AAsset* mAsset;
	};

}