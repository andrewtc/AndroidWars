/*
 * Author      : Matthew Johnson
 * Date        : 10/Oct/2013
 * Description :
 *   A named object. Each object has a unique name and is self managed. This is a base class.
 *   Names are stored in a HashString which in turn is the key for objects stored in a HashMap.
 *   Each object has a RTTI and derived classes should include the RTTI preprocessor tags.
 *   In header: DECLARE_RTTI;
 *   In source: IMPLEMENT_RTTI( namespace, base_object_name, object_name );
 */
 
#pragma once

namespace mage
{

	class Object
	{
	protected:
		Object();
		Object( const std::string& name );
	public:
		virtual ~Object();

		// Type info
		virtual const RTTI& GetType() const;
		bool IsExactly( const RTTI& type ) const;
		bool IsDerived( const RTTI& type ) const;
		bool IsExactSameType( const Object* obj ) const;
		bool IsDerivedFromSameType( const Object* obj ) const;
		static const RTTI TYPE;

		// Naming
		const std::string& GetName() const		{ return mName.GetString(); }
		uint32 GetHash() const					{ return mName.GetHash(); }

	protected:
		HashString mName;

	public:
		// Returns Object or NULL if not found
		static Object* GetObjectByName( const HashString& name );
	protected:
		static void RegisterObject( Object* obj );
		static void UnRegisterObject( Object* obj );

	private:
		static HashMap< Object* > ObjectRegistry;
		static std::map< std::string, uint32 > NameRefCount;
	};

}