#pragma once

#ifndef ANDROID
struct SDL_Surface;
#endif

namespace mage
{

	class Texture2D
	{
	public:
		Texture2D( uint32 textureId, uint32 w, uint32 h );
		Texture2D( const char* filename );
#ifndef ANDROID
		Texture2D( SDL_Surface* surf );
#endif
		~Texture2D();

		bool Load();
		void Unload();
	private:
#ifdef ANDROID
        bool LoadPng();
#else
		bool LoadFromSurface( SDL_Surface* surf );
#endif
	public:
		inline unsigned int GetWidth() const { return mWidth; }
		inline unsigned int GetHeight() const { return mHeight; }
		inline unsigned int GetId() const { return mId; }
	
	private:
		unsigned int mWidth, mHeight;
		bool mIsLoaded;
		const char* mFilename;
		unsigned int mId;
	};

}
