/*
 * Author      : Matthew Johnson
 * Date        : 26/May/2013
 * Description :
 *  Bitmap font rendered from a TTF.
 *  Line spacing and kerning are derived from the font file.
 *
 *  TODO:
 *   Save/Load bitmap font so we don't need the TTF for distribution.
 */
 
#pragma once

namespace mage
{

	class Font
	{
	public:
		static Font* DefaultFont;
		static const int CHARACTER_SET_SIZE = 96;	// standard ASCII
		static int TabSize;							// Number of spaces to use for tabs. Default is 4

		static bool InitFont();
		static void TermFont();

		Font( const char* filename, int fontSize );
		~Font();

		enum FontQuality
		{
			Font_FAST,	// 8-bit fast quality
			Font_NICE,	// 8-bit high quality, boxed (use bg for box color) - you can colorkey it if you want.
			Font_ULTRA	// 32-bit high quality
		};
		
		// Render text that is frequently changing
		// Use this for text that changes frequently
		// Quality defaults to FAST
		void RenderTextDynamic( int x, int y, const char* text,
			const Color& color, int fontQuality=Font_FAST, const Color& bgColor=Color::WHITE );

		// Render text to a surface, which is return for you to render
		// Use this for text that changes rarely
		// YOU are responsible for freeing the returned surface
		// Quality defaults to ULTRA
		// Returns NULL if Font failed to load or text is NULL
		Texture2D* RenderTextStatic( const char* text, const Color& color,
			int fontQuality=Font_ULTRA, const Color& bgColor=Color::WHITE );

		// Font info queries
		int GetRecomendedLineSpacing() const;
		int GetFontHeight() const;
		int GetTextWidth( const char* text ) const;
		void GetTextSize( const char* text, int& width, int& height ) const;

	protected:
		bool LoadFont( const char* filename, int fontSize );

	private:
		static const int sCharacterIndexOffset;
		int mFontSize;
		int mFontHeight;
		int mKerning;
		int mLineskip;
		int mMaxLineWidth;
		Texture2D* mGlyphs[ CHARACTER_SET_SIZE ];
	};

}