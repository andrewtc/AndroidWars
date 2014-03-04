#pragma once

namespace mage
{
#define MAGE_DEFINE_PROPERTY_CONVERTER( type, typeName ) \
	type GetPropertyAs ## typeName( const HashString& name, const type& defaultValue, bool isRequired = false ) const

	/**
	 * Encapsulates a nested list of properties that can be used to construct a
	 * hierarchy of Widgets.
	 */
	class WidgetTemplate
	{
	public:
		static const HashString TYPE_INCLUDE;
		static const HashString TYPE_OVERRIDE;

		static const HashString PROPERTY_TYPE;
		static const HashString PROPERTY_NAME;
		static const HashString PROPERTY_TEMPLATE;

		static const HashString MERGE_CHILDREN_REPLACE;
		static const HashString MERGE_CHILDREN_SHALLOW;
		static const HashString MERGE_CHILDREN_DEEP;

		typedef std::vector< std::pair< HashString, std::string > > PropertyList;
		typedef std::vector< std::pair< HashString, WidgetTemplate > > TemplateList;

		WidgetTemplate();
		WidgetTemplate( const WidgetTemplate& other );
		~WidgetTemplate();
		void operator=( const WidgetTemplate& other );

		void SetProperty( const HashString& name, const std::string& value );
		void CopyProperties( const WidgetTemplate& other );
		void MergeProperty( const HashString& name, const WidgetTemplate& other );
		void MergeProperties( const WidgetTemplate& other );
		std::string GetProperty( const HashString& name, const std::string& defaultValue = "", bool isRequired = false ) const;
		PropertyList GetProperties() const;
		bool HasProperty( const HashString& name ) const;
		bool HasProperties() const;
		void ClearProperty( const HashString& name );
		void ClearAllProperties();

		MAGE_DEFINE_PROPERTY_CONVERTER( int, Int );
		MAGE_DEFINE_PROPERTY_CONVERTER( unsigned int, UInt );
		MAGE_DEFINE_PROPERTY_CONVERTER( float, Float );
		MAGE_DEFINE_PROPERTY_CONVERTER( bool, Bool );
		MAGE_DEFINE_PROPERTY_CONVERTER( Vec2f, Vec2f );
		MAGE_DEFINE_PROPERTY_CONVERTER( Vec3f, Vec3f );
		MAGE_DEFINE_PROPERTY_CONVERTER( Vec4f, Vec4f );
		MAGE_DEFINE_PROPERTY_CONVERTER( Color, Color );

		void SetChild( const HashString& name, const WidgetTemplate& child );
		void CopyChildren( const WidgetTemplate& other );
		void MergeChild( const HashString& name, const WidgetTemplate& other );
		void DeepMergeChild( const HashString& name, const WidgetTemplate& other );
		void MergeChildren( const WidgetTemplate& other );
		void DeepMergeChildren( const WidgetTemplate& other );
		WidgetTemplate& GetChild( const HashString& name );
		const WidgetTemplate& GetChild( const HashString& name ) const;
		TemplateList GetChildren() const;

		bool HasChild( const HashString& name ) const;
		bool HasChildren() const;
		void ClearChild( const HashString& name );
		void ClearAllChildren();

		void Replace( const WidgetTemplate& other );
		void Merge( const WidgetTemplate& other );
		void DeepMerge( const WidgetTemplate& other );
		void Clear();

		void ResolveIncludes( WidgetManager* manager );

		std::string ToString( size_t indentLevel = 0 ) const;

	protected:
		HashMap< std::string > mProperties;
		HashMap< WidgetTemplate > mChildren;
	};


	std::ostream& operator<<( std::ostream& stream, const WidgetTemplate& widgetTemplate );
}
