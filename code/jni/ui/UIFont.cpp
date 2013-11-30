#include <MageApp.h>
#include "UIFont.h"

namespace mage
{
namespace UIFont
{
	//---------------------------------------
	static HashMap< BitmapFont* > gFonts;
	//---------------------------------------
	bool LoadFont( const char* fntFile, const char* name )
	{
		BitmapFont* fnt = new BitmapFont( fntFile );
		if ( fnt )
		{

		}
	}
	//---------------------------------------
	void UnloadAllFonts()
	{
		DestroyMapByValue( gFonts );
	}
	//---------------------------------------
	BitmapFont* GetFontByName( const char* name )
	{

	}
	//---------------------------------------
}
}
