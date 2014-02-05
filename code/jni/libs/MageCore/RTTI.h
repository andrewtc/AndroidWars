/*
 * Author      : Matthew Johnson
 * Date        : 11/Oct/2013
 * Description :
 *   
 */
 
#pragma once

namespace mage
{

	class RTTI
	{
	public:
		RTTI( const char* name, const RTTI* base );
		~RTTI();

		const char* GetName() const					{ return mName; }
		bool IsExactly( const RTTI& type ) const	{ return this == &type; }
		bool IsDerived( const RTTI& type ) const;

	private:
		const char* mName;
		const RTTI* mBase;
	};

}

// Put this in classes derived from Object
#define DECLARE_RTTI																\
	public:																			\
		static const RTTI TYPE;														\
		virtual const RTTI& GetType() const { return TYPE; }

#define IMPLEMENT_RTTI( NAMESPACE, BASECALSSNAME, CLASSNAME )						\
	const RTTI CLASSNAME::TYPE( #NAMESPACE"."#CLASSNAME, &BASECALSSNAME::TYPE )

#define IMPLEMENT_RTTI_BASE( NAMESPACE, CLASSNAME )						\
	const RTTI CLASSNAME::TYPE( #NAMESPACE"."#CLASSNAME, 0 )

#define MAGE_IMPLEMENT_RTTI( BASECALSSNAME, CLASSNAME ) IMPLEMENT_RTTI( mage, BASECALSSNAME, CLASSNAME )

#define MAGE_IMPLEMENT_RTTI_BASE( CLASSNAME ) IMPLEMENT_RTTI_BASE( mage, CLASSNAME )
