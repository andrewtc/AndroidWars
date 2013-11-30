/*
 * Author      : Matthew Johnson
 * Date        : 26/Nov/2013
 * Description :
 *   
 */
 
#pragma once

namespace mage
{

	namespace UIFont
	{
		bool LoadFont( const char* fntFile, const char* name );
		void UnloadAllFonts();
		BitmapFont* GetFontByName( const char* name );
	}

}
