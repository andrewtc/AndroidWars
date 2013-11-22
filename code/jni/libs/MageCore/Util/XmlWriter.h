/*
 * Author      : Matthew Johnson
 * Date        : 10/Aug/2013
 * Description :
 *   
 */
 
#pragma once

namespace mage
{

	class XmlWriter
	{
	public:
		XmlWriter( const char* filename, bool compact=false );
		~XmlWriter();

		void BeginElement( const char* name );
		void EndElement();

		void AddAttribute( const char* name, const char* value );
		void AddAttribute( const char* name, const std::string& value );
		void AddAttribute( const char* name, int value );
		void AddAttribute( const char* name, unsigned value );
		void AddAttribute( const char* name, float value );
		void AddAttribute( const char* name, double value );
		void AddAttribute( const char* name, bool value );

		void AddAttribute( const char* name, const Vec2f& value );
		void AddAttribute( const char* name, const Vec3f& value );
		void AddAttribute( const char* name, const Vec4f& value );
		void AddAttribute( const char* name, const Vec2i& value );
		void AddAttribute( const char* name, const Vec3i& value );
		void AddAttribute( const char* name, const Vec4i& value );

		void AddAttribute( const char* name, const Color& value );

		void AddAttribute( const char* name, const IntRange& value );
		void AddAttribute( const char* name, const FloatRange& value );

		template< typename T >
		void AddAttribute( const char* name, const std::vector< T >& csv )
		{
			AddAttribute( name, StringUtil::VectorToCSVString( csv ) );
		}

		void AddCData( const char* text );

		void AddComment( const char* comment );

		// Write contents to disk
		// Called by destructor
		void Flush();

	private:
		class XmlPrinter* mPrinter;
	};

}