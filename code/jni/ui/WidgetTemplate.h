#pragma once

namespace mage
{
/**
	 * Encapsulates a nested list of properties that can be used to construct a
	 * hierarchy of Widgets.
	 */
	class WidgetTemplate
	{
	public:
		typedef std::vector< std::pair< HashString, std::string > > PropertyList;
		typedef std::vector< std::pair< HashString, WidgetTemplate > > DefinitionList;

		WidgetTemplate();
		WidgetTemplate( const WidgetTemplate& other );
		~WidgetTemplate();
		void operator=( const WidgetTemplate& other );

		void SetProperty( const HashString& name, const std::string& value );
		void CopyProperties( const WidgetTemplate& other );
		void MergeProperty( const HashString& name, const WidgetTemplate& other );
		void MergeProperties( const WidgetTemplate& other );
		std::string GetProperty( const HashString& name ) const;
		PropertyList GetProperties() const;
		bool HasProperty( const HashString& name ) const;
		bool HasProperties() const;
		void ClearProperty( const HashString& name );
		void ClearAllProperties();

		void SetChild( const HashString& name, const WidgetTemplate& child );
		void CopyChildren( const WidgetTemplate& other );
		void MergeChild( const HashString& name, const WidgetTemplate& other );
		void DeepMergeChild( const HashString& name, const WidgetTemplate& other );
		void MergeChildren( const WidgetTemplate& other );
		void DeepMergeChildren( const WidgetTemplate& other );
		WidgetTemplate& GetChild( const HashString& name );
		const WidgetTemplate& GetChild( const HashString& name ) const;

		bool HasChild( const HashString& name ) const;
		bool HasChildren() const;
		void ClearChild( const HashString& name );
		void ClearAllChildren();

		void Merge( const WidgetTemplate& other );
		void DeepMerge( const WidgetTemplate& other );
		void Clear();

		std::string ToString( size_t indentLevel = 0 ) const;

	protected:
		HashMap< std::string > mProperties;
		HashMap< WidgetTemplate > mChildren;
	};


	std::ostream& operator<<( std::ostream& stream, const WidgetTemplate& widgetTemplate );
}
