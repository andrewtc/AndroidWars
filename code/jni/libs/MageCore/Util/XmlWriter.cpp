#include "CoreLib.h"

// Use the tinyxml printer
#include "tinyxmlPrinter.h"

using namespace mage;

//---------------------------------------
// XmlWriter
//---------------------------------------
XmlWriter::XmlWriter( const char* filename, bool compact )
{
	mPrinter = new XmlPrinter( filename, compact );
}
//---------------------------------------
XmlWriter::~XmlWriter()
{
	Flush();
	delete mPrinter;
}
//---------------------------------------
void XmlWriter::BeginElement( const char* name )
{
	mPrinter->BeginElement( name );
}
//---------------------------------------
void XmlWriter::EndElement()
{
	mPrinter->EndElement();
}
//---------------------------------------
void XmlWriter::AddAttribute( const char* name, const char* value )
{
	mPrinter->AddAttribute( name, value );
}
//---------------------------------------
void XmlWriter::AddAttribute( const char* name, const std::string& value )
{
	mPrinter->AddAttribute( name, value.c_str() );
}
//---------------------------------------
void XmlWriter::AddAttribute( const char* name, int value )
{
	mPrinter->AddAttribute( name, value );
}
//---------------------------------------
void XmlWriter::AddAttribute( const char* name, unsigned value )
{
	mPrinter->AddAttribute( name, value );
}
//---------------------------------------
void XmlWriter::AddAttribute( const char* name, float value )
{
	mPrinter->AddAttribute( name, (double) value );
}
//---------------------------------------
void XmlWriter::AddAttribute( const char* name, double value )
{
	mPrinter->AddAttribute( name, value );
}
//---------------------------------------
void XmlWriter::AddAttribute( const char* name, bool value )
{
	if ( value )
		mPrinter->AddAttribute( name, "true" );
	else
		mPrinter->AddAttribute( name, "false" );
	//mPrinter->AddAttribute( name, value );
}
//---------------------------------------
void XmlWriter::AddAttribute( const char* name, const Vec2f& value )
{
	mPrinter->AddAttribute( name, StringUtil::ToString( value ).c_str() );
}
//---------------------------------------
void XmlWriter::AddAttribute( const char* name, const Vec3f& value )
{
	mPrinter->AddAttribute( name, StringUtil::ToString( value ).c_str() );
}
//---------------------------------------
void XmlWriter::AddAttribute( const char* name, const Vec4f& value )
{
	mPrinter->AddAttribute( name, StringUtil::ToString( value ).c_str() );
}
//---------------------------------------
void XmlWriter::AddAttribute( const char* name, const Vec2i& value )
{
	mPrinter->AddAttribute( name, StringUtil::ToString( value ).c_str() );
}
//---------------------------------------
void XmlWriter::AddAttribute( const char* name, const Vec3i& value )
{
	mPrinter->AddAttribute( name, StringUtil::ToString( value ).c_str() );
}
//---------------------------------------
void XmlWriter::AddAttribute( const char* name, const Vec4i& value )
{
	mPrinter->AddAttribute( name, StringUtil::ToString( value ).c_str() );
}
//---------------------------------------
void XmlWriter::AddAttribute( const char* name, const Color& value )
{
	mPrinter->AddAttribute( name, StringUtil::ToString( value ).c_str() );
}
//---------------------------------------
void XmlWriter::AddAttribute( const char* name, const IntRange& value )
{
	//@TODO
}
//---------------------------------------
void XmlWriter::AddAttribute( const char* name, const FloatRange& value )
{
	//@TODO
}
//---------------------------------------
void XmlWriter::AddCData( const char* text )
{
	mPrinter->AddCData( text );
}
//---------------------------------------
void XmlWriter::AddComment( const char* comment )
{
	mPrinter->AddComment( comment );
}
//---------------------------------------
void XmlWriter::Flush()
{
	mPrinter->Flush();
}
//---------------------------------------