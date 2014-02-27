#include "androidwars.h"

using namespace mage;

//---------------------------------------
// WidgetTemplate
//---------------------------------------
WidgetTemplate::WidgetTemplate() { }
//---------------------------------------
WidgetTemplate::WidgetTemplate( const WidgetTemplate& other ) :
	mProperties( other.mProperties ), mChildren( other.mChildren )
{ }
//---------------------------------------
WidgetTemplate::~WidgetTemplate() { }
//---------------------------------------
void WidgetTemplate::operator=( const WidgetTemplate& other )
{
	CopyProperties( other );
	CopyChildren( other );
}
//---------------------------------------
void WidgetTemplate::SetProperty( const HashString& name, const std::string& value )
{
	// Set the property, creating it if necessary.
	mProperties[ name ] = value;
}
//---------------------------------------
void WidgetTemplate::CopyProperties( const WidgetTemplate& other )
{
	if( &other != this )
	{
		// Copy all properties from the other object.
		mProperties = other.mProperties;
	}
}
//---------------------------------------
void WidgetTemplate::MergeProperty( const HashString& name, const WidgetTemplate& other )
{
	if( &other != this && other.HasProperty( name ) )
	{
		// If the other object has the requested property, overwrite it in this object.
		SetProperty( name, other.GetProperty( name ) );
	}
}
//---------------------------------------
void WidgetTemplate::MergeProperties( const WidgetTemplate& other )
{
	if( &other != this )
	{
		for( auto it = other.mProperties.begin(); it != other.mProperties.end(); ++it )
		{
			// For every property in the other object, set the corresponding property in this one.
			MergeProperty( it->first, other );
		}
	}
}
//---------------------------------------
std::string WidgetTemplate::GetProperty( const HashString& name ) const
{
	std::string result;

	// Look up the property by name.
	auto it = mProperties.find( name );

	if( it != mProperties.end() )
	{
		// If the property was found, return its value.
		result = it->second;
	}
	else
	{
		WarnFail( "WidgetTemplate property \"%s\" is undefined!", name.GetCString() );
	}

	return result;
}
//---------------------------------------
WidgetTemplate::PropertyList WidgetTemplate::GetProperties() const
{
	PropertyList result;

	for( auto it = mProperties.begin(); it != mProperties.end(); ++it )
	{
		// Build a list of all properties in the object.
		result.push_back( *it );
	}

	return result;
}
//---------------------------------------
bool WidgetTemplate::HasProperty( const HashString& name ) const
{
	bool result = false;

	auto it = mProperties.find( name );

	if( it != mProperties.end() )
	{
		result = true;
	}

	return result;
}
//---------------------------------------
bool WidgetTemplate::HasProperties() const
{
	return ( mProperties.empty() == false );
}
//---------------------------------------
void WidgetTemplate::ClearProperty( const HashString& name )
{
	auto it = mProperties.find( name );

	if( it != mProperties.end() )
	{
		// If the property was found, clear it.
		mProperties.erase( it );
	}
}
//---------------------------------------
void WidgetTemplate::ClearAllProperties()
{
	mProperties.clear();
}
//---------------------------------------
void WidgetTemplate::SetChild( const HashString& name, const WidgetTemplate& child )
{
	// Set the child properties with the given name, overwriting any existing data.
	mChildren[ name ] = child;
}
//---------------------------------------
void WidgetTemplate::CopyChildren( const WidgetTemplate& other )
{
	if( &other != this )
	{
		// Copy all children from the other object.
		mChildren = other.mChildren;
	}
}
//---------------------------------------
void WidgetTemplate::MergeChild( const HashString& name, const WidgetTemplate& other )
{
	if( &other != this && other.HasChild( name ) )
	{
		// Merge the child with the given name, if it exists in the other object.
		SetChild( name, other.GetChild( name ) );
	}
}
//---------------------------------------
void WidgetTemplate::DeepMergeChild( const HashString& name, const WidgetTemplate& other )
{
	if( &other != this && other.HasChild( name ) )
	{
		// Merge the child with the given name, if it exists in the other object.
		mChildren[ name ].DeepMerge( other.GetChild( name ) );
	}
}
//---------------------------------------
void WidgetTemplate::MergeChildren( const WidgetTemplate& other )
{
	if( &other != this )
	{
		for( auto it = other.mChildren.begin(); it != other.mChildren.end(); ++it )
		{
			// Merge each child of the other object into this one.
			SetChild( it->first, it->second );
		}
	}
}
//---------------------------------------
void WidgetTemplate::DeepMergeChildren( const WidgetTemplate& other )
{
	if( &other != this )
	{
		for( auto it = other.mChildren.begin(); it != other.mChildren.end(); ++it )
		{
			// Deep merge each child of the other object into this one.
			mChildren[ it->first ].DeepMerge( it->second );
		}
	}
}
//---------------------------------------
WidgetTemplate& WidgetTemplate::GetChild( const HashString& name )
{
	assertion( HasChild( name ), "WidgetTemplate child with name \"%s\" does not exist!", name.GetCString() );
	return mChildren[ name ];
}
//---------------------------------------
const WidgetTemplate& WidgetTemplate::GetChild( const HashString& name ) const
{
	assertion( HasChild( name ), "WidgetTemplate child with name \"%s\" does not exist!", name.GetCString() );

	auto it = mChildren.find( name );
	return it->second;
}
//---------------------------------------
bool WidgetTemplate::HasChild( const HashString& name ) const
{
	bool result = false;

	auto it = mChildren.find( name );

	if( it != mChildren.end() )
	{
		result = true;
	}

	return result;
}
//---------------------------------------
bool WidgetTemplate::HasChildren() const
{
	return ( mChildren.empty() == false );
}
//---------------------------------------
void WidgetTemplate::ClearChild( const HashString& name )
{
	auto it = mChildren.find( name );

	if( it != mChildren.end() )
	{
		// If a child with the specified name was found, remove it.
		mChildren.erase( it );
	}
}
//---------------------------------------
void WidgetTemplate::ClearAllChildren()
{
	mChildren.clear();
}
//---------------------------------------
void WidgetTemplate::Merge( const WidgetTemplate& other )
{
	MergeProperties( other );
	MergeChildren( other );
}
//---------------------------------------
void WidgetTemplate::DeepMerge( const WidgetTemplate& other )
{
	MergeProperties( other );
	DeepMergeChildren( other );
}
//---------------------------------------
void WidgetTemplate::Clear()
{
	// Clear all properties and children.
	ClearAllProperties();
	ClearAllChildren();
}
//---------------------------------------
std::string WidgetTemplate::ToString( size_t indentLevel ) const
{
	static const size_t CHILD_INDENT_LEVEL = 4;

	std::stringstream formatter;

	// Create a string for the indent level.
	std::string indent( indentLevel, ' ' );

	formatter << indent << "{ ";

	if( HasProperties() )
	{
		// Print a section for properties.
		formatter << "\n" << indent << "  PROPERTIES (" << mProperties.size() << "):\n";

		for( auto it = mProperties.begin(); it != mProperties.end(); ++it )
		{
			// Print all properties.
			formatter << indent << "    " << it->first.GetCString() << ": \"" << it->second << "\"\n";
		}
	}

	if( HasChildren() )
	{
		formatter << indent << "  CHILDREN (" << mChildren.size() << "):\n";

		for( auto it = mChildren.begin(); it != mChildren.end(); ++it )
		{
			// Print the name of the child.
			formatter << indent << "    " << it->first.GetString() << "\n";

			// Print all children with a larger indent.
			formatter << it->second.ToString( indentLevel + CHILD_INDENT_LEVEL ) << "\n";
		}
	}

	if( HasProperties() || HasChildren() )
	{
		formatter << indent;
	}

	formatter << "}";

	return formatter.str();
}
//---------------------------------------
std::ostream& operator<<( std::ostream& stream, const WidgetTemplate& widgetTemplate )
{
	stream << widgetTemplate.ToString();
	return stream;
}
//---------------------------------------
