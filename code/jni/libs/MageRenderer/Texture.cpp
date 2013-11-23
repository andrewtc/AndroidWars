#include "RendererLib.h"

#ifdef ANDROID
#   include <png.h>
#else
#   include <SDL.h>
#   include <SDL_image.h>
#endif


using namespace mage;

static void callback_read_png( png_structp pStruct, png_bytep pData, png_size_t pSize );

HashMap< Texture2D* > Texture2D::mTextureRegistry;

//---------------------------------------
void Texture2D::RegisterTexture( const char* key, Texture2D* tx )
{
	mTextureRegistry[ key ] = tx;
}
//---------------------------------------
void Texture2D::UnregisterTexture( const char* key )
{
	auto i = mTextureRegistry.find( key );
	if ( i != mTextureRegistry.end() )
	{
		mTextureRegistry.erase( i );
	}
}
//---------------------------------------
void Texture2D::ReloadAllTextures()
{
	for ( auto itr = mTextureRegistry.begin(); itr != mTextureRegistry.end(); ++itr )
	{
		Texture2D* tx = itr->second;
		//tx->Unload();
		tx->mIsLoaded = false;
		tx->Load();
	}
}
//---------------------------------------
Texture2D* Texture2D::CreateTexture( const char* filename )
{
	Texture2D*& tx = mTextureRegistry[ filename ];
	if ( !tx )
	{
		tx = new Texture2D( filename );
	}
	return tx;
}
//---------------------------------------
void Texture2D::DestroyAllTextures()
{
	for ( auto itr = mTextureRegistry.begin(); itr != mTextureRegistry.end(); ++itr )
	{
		Texture2D* tx = itr->second;
		UnregisterTexture( tx->mFilename.c_str() );
		delete tx;
	}
}
//---------------------------------------
void Texture2D::DestroyTexture( Texture2D*& tx )
{
	Texture2D*& tx2 = mTextureRegistry[ tx->mFilename ];
	if ( tx2 )
	{
		UnregisterTexture( tx->mFilename.c_str() );
		delete tx2;
		tx = 0;
	}
}
//---------------------------------------




//---------------------------------------
Texture2D::Texture2D( uint32 textureId, uint32 w, uint32 h )
	: mIsLoaded( true )
	, mFilename( 0 )
	, mId( textureId )
	, mWidth( w )
	, mHeight( h )
{}
//---------------------------------------
Texture2D::Texture2D( const char* filename )
	: mIsLoaded( false )
	, mFilename( filename )
	, mId( 0 )
	, mWidth( 0 )
	, mHeight( 0 )
{
	RegisterTexture( filename, this );
}
//---------------------------------------
#ifdef ANDROID
#else
Texture2D::Texture2D( SDL_Surface* surf )
	: mIsLoaded( true )
{
	LoadFromSurface( surf );
}
#endif
//---------------------------------------
Texture2D::~Texture2D()
{
	FreeTexture( &mId );
	mId = 0;
}
//---------------------------------------
bool Texture2D::Load()
{
	if ( !mIsLoaded )
	{
#ifdef ANDROID
		mIsLoaded = LoadPng();
#else
		SDL_Surface* surf = IMG_Load( mFilename );

		if ( LoadFromSurface( surf ) )
		{
			SDL_FreeSurface( surf );

			mIsLoaded = true;
		}
#endif
	}
	return mIsLoaded;
}
//---------------------------------------
void Texture2D::Unload()
{
	FreeTexture( &mId );
	mId = 0;
	mIsLoaded = false;
	mWidth = 0;
	mHeight = 0;
}
//---------------------------------------
#ifdef ANDROID
bool Texture2D::LoadPng()
{
	ConsolePrintf( "Loading texture %s", mFilename.c_str() );

	png_byte lHeader[8];
	png_structp lPngPtr = NULL;
	png_infop lInfoPtr = NULL;
	png_byte* lImageBuffer = NULL;
	png_bytep* lRowPtrs = NULL;
	png_int_32 lRowSize;
	bool lTransparency;
	Resource* resource = NULL;
	IRenderer::PixelFormat format;

	resource = CreateResourceHandle( mFilename.c_str() );
	if ( !resource )
		goto ERROR;

	// Opens and checks image signature (first 8 bytes).
	if( resource->Open() != Resource::RS_OK )
		goto ERROR;

	DebugPrintf( "Checking signature.", "" );

	if( resource->Read( lHeader, sizeof(lHeader) ) != Resource::RS_OK )
		goto ERROR;

	if( png_sig_cmp( lHeader, 0, 8 ) != 0 )
		goto ERROR;

	// Creates required structures.
	DebugPrintf( "Creating required structures.", "" );
	lPngPtr = png_create_read_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );

	if( !lPngPtr )
		goto ERROR;

	lInfoPtr = png_create_info_struct( lPngPtr );

	if( !lInfoPtr )
		goto ERROR;

	// Prepares reading operation by setting-up a read callback.
	png_set_read_fn( lPngPtr, resource, callback_read_png );

	// Set-up error management. If an error occurs while reading,
	// code will come back here and jump
	if( setjmp( png_jmpbuf(lPngPtr) ) )
		goto ERROR;

	// Ignores first 8 bytes already read and processes header.
	png_set_sig_bytes( lPngPtr, 8 );
	png_read_info( lPngPtr, lInfoPtr );

	// Retrieves PNG info and updates PNG struct accordingly.
	png_int_32 lDepth, lColorType;
	png_uint_32 lWidth, lHeight;
	png_get_IHDR( lPngPtr, lInfoPtr, &lWidth, &lHeight, &lDepth, &lColorType,
			NULL, NULL, NULL );
	mWidth = lWidth;
	mHeight = lHeight;

	// Creates a full alpha channel if transparency is encoded as
	// an array of palate entries or a single transparent color.
	lTransparency = false;

	if( png_get_valid( lPngPtr, lInfoPtr, PNG_INFO_tRNS ) )
	{
		png_set_tRNS_to_alpha( lPngPtr );
		lTransparency = true;
		goto ERROR;
	}

	// Expands PNG with less than 8bits per channel to 8bits.
	if( lDepth < 8 )
	{
		png_set_packing( lPngPtr );
		// Shrinks PNG with 16bits per color channel down to 8bits.
	}
	else if( lDepth == 16 )
	{
		png_set_strip_16( lPngPtr );
	}

	// Indicates that image needs conversion to RGBA if needed.
	switch( lColorType )
	{
		case PNG_COLOR_TYPE_PALETTE:
			png_set_palette_to_rgb( lPngPtr );
			format = lTransparency ? IRenderer::PF_RGBA : IRenderer::PF_RGB;
			break;

		case PNG_COLOR_TYPE_RGB:
			format = lTransparency ? IRenderer::PF_RGBA : IRenderer::PF_RGB;
			break;

		case PNG_COLOR_TYPE_RGBA:
			format = IRenderer::PF_RGBA;
			break;

		case PNG_COLOR_TYPE_GRAY:
			png_set_expand_gray_1_2_4_to_8( lPngPtr );
			format = lTransparency ? IRenderer::PF_LUMINANCE_ALPHA : IRenderer::PF_LUMINANCE;
			break;

		case PNG_COLOR_TYPE_GA:
			png_set_expand_gray_1_2_4_to_8( lPngPtr );
			format = IRenderer::PF_LUMINANCE_ALPHA;
			break;
	}

	// Validates all transformations.
	png_read_update_info( lPngPtr, lInfoPtr );

	// Get row size in bytes.
	lRowSize = png_get_rowbytes( lPngPtr, lInfoPtr );
	if( lRowSize <= 0 )
		goto ERROR;

	// Creates the image buffer that will be sent to OpenGL.
	lImageBuffer = new png_byte[lRowSize * lHeight];
	if( !lImageBuffer )
		goto ERROR;

	// Pointers to each row of the image buffer. Row order is
	// inverted because different coordinate systems are used by
	// OpenGL (1st pixel is at bottom left) and PNGs (top-left).
	lRowPtrs = new png_bytep[lHeight];
	if( !lRowPtrs )
		goto ERROR;

	for( int32_t i = 0; i < lHeight; ++i )
	{
		lRowPtrs[lHeight - (i + 1)] = lImageBuffer + i * lRowSize;
	}

	// Reads image content.
	png_read_image( lPngPtr, lRowPtrs );

	// Frees memory and resources.
	delete resource;
	png_destroy_read_struct( &lPngPtr, &lInfoPtr, NULL );
	delete[] lRowPtrs;

	// Create the gl texture
	::CreateTexture( &mId, lImageBuffer, mWidth, mHeight, format );

	// Free pixels
	delete lImageBuffer;

	// OK
	return true;

	ERROR:
	WarnFail( "Error while reading PNG file", "" );
	if ( resource ) delete resource;
	delete[] lRowPtrs;
	delete[] lImageBuffer;

	if( lPngPtr != NULL )
	{
		png_infop* lInfoPtrP = lInfoPtr != NULL ? &lInfoPtr : NULL;
		png_destroy_read_struct( &lPngPtr, lInfoPtrP, NULL );
	}

	// Fail
	return false;
}
//---------------------------------------
void callback_read_png( png_structp pStruct, png_bytep pData, png_size_t pSize )
{
	Resource* resource = ((Resource*) png_get_io_ptr( pStruct ));

	resource->Read( pData, pSize );
}
//---------------------------------------
#else
bool Texture2D::LoadFromSurface( SDL_Surface* surf )
{
	if ( surf )
	{
		IRenderer::PixelFormat format;
		if ( surf->format->BytesPerPixel == 4 )
		{
			format = IRenderer::PF_RGBA;
			//if ( surf->format->Rmask == 0x000000ff )
			//	format = IRenderer::PF_RGBA;
			//else
			//	format = IRenderer::PF_BGRA;
		}
		else
		{
			format = IRenderer::PF_RGB;
			//if ( surf->format->Rmask == 0x000000ff )
			//	format = IRenderer::PF_RGB;
			//else
			//	format = IRenderer::PF_BGR;
		}

		mWidth = surf->w;
		mHeight = surf->h;

		CreateTexture( &mId, surf->pixels, mWidth, mHeight, format );

		return true;
	}
	return false;
}
//---------------------------------------
#endif
