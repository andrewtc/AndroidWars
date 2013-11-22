/*
 * Author      : Matthew Johnson
 * Date        : 11/Oct/2013
 * Description :
 *   
 */
 
#pragma once

namespace mage
{

	//---------------------------------------
	class Definition
		: public Object
	{
		DECLARE_RTTI;
	public:
		Definition( const XmlReader::XmlReaderIterator& root );
		virtual ~Definition();

		// Create a new object using this definition
		virtual Object* Create() const = 0;
	};
	//---------------------------------------
	template< typename TObject, typename TDefinition >
	class Factory
	{
	public:
		
		static TObject* Create( const HashString& name )
		{
			Object* obj = Object::GetObjectByName( name );
			TDefinition* def;

			if ( !obj->IsDerived( TDefinition::TYPE ) )
			{
				WarnFail( "Failed to create object '%s' : Not an object definition", name.GetString().c_str() );
				return 0;
			}

			def = (TDefinition*) obj;
			obj = def->Create();

			return (TObject*) obj;
		}
	};
	//---------------------------------------

}