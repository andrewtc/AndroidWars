#pragma once

#ifndef ANDROID
struct SDL_Surface;
#endif

namespace mage
{

	class Texture2D
	{
	public:
		// Constructor for graphics engine to use
		Texture2D( uint32 textureId, uint32 w, uint32 h );
	private:
		// Called by CreateTexture()
		Texture2D( const char* filename );
#ifndef ANDROID
		Texture2D( SDL_Surface* surf );
#endif
		~Texture2D();
	public:

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

		// Creates and registers a new Texture2D
		static Texture2D* CreateTexture( const char* filename );

		// Only call this if the graphics context was destroyed and texture ids have been invalidated
		// Calling this when texture ids are valid will cause duplicate textures to be loaded
		static void ReloadAllTextures();

		static void DestroyAllTextures();

		// Use this to unload a single texture
		// You should make sure no one else is using it
		static void DestroyTexture( Texture2D*& tx );
	
	private:
		static void RegisterTexture( const char* filename, Texture2D* tx );
		static void UnregisterTexture( const char* filename );

		unsigned int mWidth, mHeight;
		bool mIsLoaded;
		std::string mFilename;
		unsigned int mId;

		static HashMap< Texture2D* > mTextureRegistry;
	};

}
