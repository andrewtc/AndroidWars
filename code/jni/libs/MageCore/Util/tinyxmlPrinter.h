/*
 * Author      : Matthew Johnson
 * Date        : 10/Aug/2013
 * Description :
 *   tinyxml2 XmlPrinter
 */
 
#pragma once

#include <tinyxml2.h>

namespace mage
{
	class XmlPrinter
	{
	public:
		XmlPrinter( const char* filename, bool compact=false )
			: mFileName( filename )
			, mCompact( compact )
			, txml2Printer( new tinyxml2::XMLPrinter( 0, compact ) )
		{}
		//---------------------------------------
		void BeginElement( const char* name )
		{
			txml2Printer->OpenElement( name );
		}
		//---------------------------------------
		void EndElement()
		{
			txml2Printer->CloseElement();
		}
		//---------------------------------------
		void AddAttribute( const char* name, const char* value )
		{
			txml2Printer->PushAttribute( name, value );
		}
		//---------------------------------------
		void AddAttribute( const char* name, int value )
		{
			txml2Printer->PushAttribute( name, value );
		}
		//---------------------------------------
		void AddAttribute( const char* name, unsigned value )
		{
			txml2Printer->PushAttribute( name, value );
		}
		//---------------------------------------
		void AddAttribute( const char* name, double value )
		{
			txml2Printer->PushAttribute( name, value );
		}
		//---------------------------------------
		void AddAttribute( const char* name, bool value )
		{
			txml2Printer->PushAttribute( name, value );
		}
		//---------------------------------------
		void AddCData( const char* text )
		{
			txml2Printer->PushText( text, false );
		}
		//---------------------------------------
		void AddComment( const char* comment )
		{
			txml2Printer->PushComment( comment );
		}
		//---------------------------------------
		// Write the data to disk
		void Flush()
		{
			WriteDataFile( mFileName.c_str(), txml2Printer->CStr(), txml2Printer->CStrSize()-1 );
		}
		//---------------------------------------

		tinyxml2::XMLPrinter* txml2Printer;
		std::string mFileName;
		bool mCompact;
	};
}