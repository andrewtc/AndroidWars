#include "RendererLib.h"

#ifndef ANDROID
#   include <SDL_ttf.h>
#endif

using namespace mage;

Font* Font::DefaultFont = NULL;
const int Font::sCharacterIndexOffset = 32;	// ASCII printable characters start at 32
int Font::TabSize = 4;

//---------------------------------------
bool Font::InitFont()
{
#ifndef ANDROID
	if ( TTF_Init() < 0 )
	{
		assertion( false, "Failed to init TTF_Font:\n%s", TTF_GetError() );
		return false;
	}
#endif
	return true;
}
//---------------------------------------
void Font::TermFont()
{
#ifndef ANDROID
	TTF_Quit();
#endif
}
//---------------------------------------


//---------------------------------------
Font::Font( const char* filename, int fontSize )
	: mFontSize( fontSize )
{
	mGlyphs[0] = NULL;
	LoadFont( filename, fontSize );
}
//---------------------------------------
Font::~Font()
{
	if ( mGlyphs[0] != NULL )
	{
		for( int i = 0; i < 96; ++i )
		{
			delete mGlyphs[i];
		}
	}
}
//---------------------------------------
bool Font::LoadFont( const char* filename, int fontSize )
{
#ifndef ANDROID
	TTF_Font* ttfFont = TTF_OpenFont( filename, fontSize );
	
	if ( !ttfFont )
	{
		printf( "Failed to load font '%s'\n", filename );
		printf( " %s\n", TTF_GetError() );
		return false;
	}

	// Generate glyphs for the character set
	SDL_Surface* surf;
	SDL_Color color = { 255, 255, 255 };
	char s[] = " ";
	for( char i = 0; i < 96; ++i )
	{
		s[0] = i + ' ';

		// Render the TTF font in software
		surf = TTF_RenderText_Blended( ttfFont, s, color );

		// This field is set incorrectly for some reason (bug?)
		surf->format->BytesPerPixel = 4;

		// SDL_TTF is not up to date with SDL2 surfaces
#if defined( SDL2 ) && SDL_TTF_MAJOR_VERSION <= 2
		surf->refcount++; // SDL2: 
#endif
		// Create a texture for the glyph
		mGlyphs[i] = new Texture2D( surf );
		SDL_FreeSurface( surf );
	}

	mKerning = TTF_GetFontKerning( ttfFont );
	mLineskip = TTF_FontLineSkip( ttfFont );
	mFontHeight = TTF_FontHeight( ttfFont );
	mMaxLineWidth = 800;

	TTF_CloseFont( ttfFont );
#endif
	return true;
}
//---------------------------------------
void Font::RenderTextDynamic( int x, int y, const char* text,
	const Color& color, int fontQuality, const Color& bgColor )
{
	if ( mGlyphs[0] == NULL ) return; // font wasn't loaded?
	if(!mGlyphs[0]->GetHeight()) return;   // texture has no size?

	char c;
	int tx = x;
	int ty = y;
	for ( int i = 0; text[i] != '\0'; ++i )
	{
		// Newline
		if ( text[i]=='\n' )
		{
			tx  = x;
			ty += mLineskip;
			continue;
		}

		// Tab
		if ( text[i] == '\t' )
		{
			tx += mGlyphs[ 0 ]->GetWidth() * TabSize;
			continue;
		}

		// Use space for non-printable ASCII characters
		c = text[i] < ' ' ? ' ' : text[i];

		// Line wrap
		/*if ( c != ' ' && abs( (int)( tx ) ) + x + mGlyphs[c-' ']->GetWidth() >= mMaxLineWidth )
		{
			tx  = x;
			ty += mLineskip;
		}*/

		c -= sCharacterIndexOffset;

		float ratio = (float) mFontSize / mGlyphs[c]->GetHeight();

		DrawRect( mGlyphs[c], (float) tx, (float) ty, mGlyphs[c]->GetWidth() * ratio, (float) mFontSize, color );
		tx += (int)( mGlyphs[c]->GetWidth() * ratio * mKerning );
	}
}
//---------------------------------------
Texture2D* Font::RenderTextStatic( const char* text, const Color& color,
	int fontQuality, const Color& bgColor )
{
	return NULL;
}
//---------------------------------------
int Font::GetRecomendedLineSpacing() const
{
	return mLineskip;
}
//---------------------------------------
int Font::GetFontHeight() const
{
	return mFontHeight;
}
//---------------------------------------
int Font::GetTextWidth( const char* text ) const
{
	int w, h;
	GetTextSize( text, w, h );
	return w;
}
//---------------------------------------
void Font::GetTextSize( const char* text, int& width, int& height ) const
{
	height = mFontHeight;
	width = 0;

	char c;
	int tx = 0;
	int ty = 0;
	for ( int i = 0; text[i] != '\0'; ++i )
	{
		// Newline
		if ( text[i]=='\n' )
		{
			if ( tx > width )
				width = tx;
			tx  = 0;
			ty += mLineskip;
			continue;
		}

		// Tab
		if ( text[i] == '\t' )
		{
			tx += mGlyphs[ 0 ]->GetWidth() * TabSize;
			continue;
		}

		// Use space for non-printable ASCII characters
		c = text[i] < ' ' ? ' ' : text[i];

		// Line wrap
		/*if ( c != ' ' && abs( (int)( tx ) ) + x + mGlyphs[c-' ']->GetWidth() >= mMaxLineWidth )
		{
			tx  = x;
			ty += mLineskip;
		}*/

		c -= sCharacterIndexOffset;

		float ratio = (float) mFontSize / mGlyphs[c]->GetHeight();
		tx += (int)( mGlyphs[c]->GetWidth() * ratio * mKerning );
	}

	if ( tx > width )
		width = tx;
}
//---------------------------------------