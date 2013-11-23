/*
 * Author      : Matthew Johnson
 * Date        : 22/Nov/2013
 * Description :
 *   
 */
 
#pragma once

namespace mage
{

	class BitmapFont
	{
	public:
		// Number of spaces to use for tabs. Default is 4
		static int TabSize;

		BitmapFont( const char* fntFile );
		~BitmapFont();

		void RenderText( float x, float y, const char* text, const Color& color=Color::WHITE, float scale=1.0f, int maxLineLength=-1 ) const;

		inline const float GetLineHeight() const { return mLineHeight; }

	private:
		struct Glyph
		{
			int x, y;
			int w, h;
			int xoff, yoff;
			int a;
			int page;
		};

		int mSize;
		int mLineHeight;
		int mSpaceWidth;
		std::map< int, Glyph > mGlyphs;
		ArrayList< Texture2D* > mPages;
	};

}
