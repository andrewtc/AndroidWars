#include "androidwars.h"

using namespace mage;

//---------------------------------------
// WidgetTemplate
//---------------------------------------
const HashString WidgetTemplate::TYPE_INCLUDE( "Include" );
const HashString WidgetTemplate::TYPE_OVERRIDE( "Override" );
//---------------------------------------
const HashString WidgetTemplate::PROPERTY_TYPE( "type" );
const HashString WidgetTemplate::PROPERTY_NAME( "name" );
const HashString WidgetTemplate::PROPERTY_TEMPLATE( "template" );
//---------------------------------------
const HashString WidgetTemplate::MERGE_CHILDREN_REPLACE( "replace" );
const HashString WidgetTemplate::MERGE_CHILDREN_SHALLOW( "shallow" );
const HashString WidgetTemplate::MERGE_CHILDREN_DEEP( "deep" );
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
		// Clear all properties in this object and then merge.
		ClearAllProperties();
		MergeProperties( other );
	}
}
//---------------------------------------
void WidgetTemplate::MergeProperty( const HashString& name, const WidgetTemplate& other )
{
	if( &other != this && other.HasProperty( name ) )
	{
		// If the other object has the requested property, overwrite it in this object.
		//DebugPrintf( "\"%s\": \"%s\" <= \"%s\"", name.GetCString(), GetProperty( name ).c_str(), other.GetProperty( name ).c_str() );
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
std::string WidgetTemplate::GetProperty( const HashString& name, const std::string& defaultValue, bool isRequired ) const
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
		// Otherwise, return the default value.
		result = defaultValue;

		if( isRequired )
		{
			WarnFail( "WidgetTemplate property \"%s\" is undefined!", name.GetCString() );
		}
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
#define MAGE_IMPLEMENT_PROPERTY_CONVERTER( type, typeName ) \
type WidgetTemplate::GetPropertyAs ## typeName( const HashString& name, const type& defaultValue, bool isRequired ) const \
{ \
	type result; \
	bool success = false; \
	\
	if( HasProperty( name ) ) \
	{ \
		/* If the property was found, parse the value. */ \
		std::string value = GetProperty( name ); \
		success = StringUtil::Parse ## typeName( value, defaultValue, result ); \
		\
		if( !success ) \
		{ \
			WarnFail( "The value of WidgetTemplate property \"%s\" (\"%s\") could not be converted to " #type "!", name.GetCString(), value.c_str() ); \
			\
			if( isRequired ) \
			{ \
				WarnFail( "Required property \"%s\" was set to an invalid value!", name.GetCString() ); \
			} \
		} \
	} \
	else if( isRequired ) \
	{ \
		WarnFail( "Required property \"%s\" was not found!", name.GetCString() ); \
	} \
	\
	if( !success ) \
	{ \
		/* If the property was NOT found (or failed to parse), return the default value. */ \
		result = defaultValue; \
	} \
	\
	return result; \
}
//---------------------------------------
MAGE_IMPLEMENT_PROPERTY_CONVERTER( int, Int )
MAGE_IMPLEMENT_PROPERTY_CONVERTER( unsigned int, UInt )
MAGE_IMPLEMENT_PROPERTY_CONVERTER( float, Float )
MAGE_IMPLEMENT_PROPERTY_CONVERTER( bool, Bool )
MAGE_IMPLEMENT_PROPERTY_CONVERTER( Vec2f, Vec2f )
MAGE_IMPLEMENT_PROPERTY_CONVERTER( Vec3f, Vec3f )
MAGE_IMPLEMENT_PROPERTY_CONVERTER( Vec4f, Vec4f )
MAGE_IMPLEMENT_PROPERTY_CONVERTER( Color, Color )
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
		mChildren.clear();

		for( auto it = other.mChildren.begin(); it != other.mChildren.end(); ++it )
		{
			// Copy all children from the other object.
			mChildren[ it->first ] = it->second;
		}
		//mChildren = other.mChildren;
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
			MergeChild( it->first, it->second );
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
			DebugPrintf( "Deep merging \"%s\" <= \"%s\"", it->first.GetCString(), it->second.GetProperty( "name" ).c_str() );
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
WidgetTemplate::TemplateList WidgetTemplate::GetChildren() const
{
	TemplateList result;

	for( auto it = mChildren.begin(); it != mChildren.end(); ++it )
	{
		// Build a list of all children in the object.
		result.push_back( *it );
	}

	return result;
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
void WidgetTemplate::Replace( const WidgetTemplate& other )
{
	//DebugPrintf( "Replacing \"%s\" <= \"%s\"", GetProperty( "name" ).c_str(), other.GetProperty( "name" ).c_str() );

	// Clear all properties and children.
	MergeProperties( other );
	CopyChildren( other );
}
//---------------------------------------
void WidgetTemplate::Merge( const WidgetTemplate& other )
{
	//DebugPrintf( "Merging \"%s\" <= \"%s\"", GetProperty( "name" ).c_str(), other.GetProperty( "name" ).c_str() );

	// Clear all properties and children.
	MergeProperties( other );
	MergeChildren( other );
}
//---------------------------------------
void WidgetTemplate::DeepMerge( const WidgetTemplate& other )
{
	//DebugPrintf( "Deep merging \"%s\" <= \"%s\"", GetProperty( "name" ).c_str(), other.GetProperty( "name" ).c_str() );

	// Clear all properties and children.
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
void WidgetTemplate::ResolveIncludes( WidgetManager* manager )
{
	if( HasProperty( PROPERTY_TYPE ) )
	{
		// Get the type of the current template.
		HashString type = GetProperty( PROPERTY_TYPE );

		if( type == TYPE_INCLUDE )
		{
			if( HasProperty( PROPERTY_TEMPLATE ) )
			{
				// Copy only the properties and children to override.
				WidgetTemplate overrides = ( *this );
				overrides.ClearProperty( PROPERTY_TYPE );

				// If this is an include, get the name of the template to load.
				HashString includedTemplateName = GetProperty( PROPERTY_TEMPLATE );
				DebugPrintf( "Including template \"%s\"...", includedTemplateName.GetCString() );

				// Load the properties of the template.
				const WidgetTemplate* includedTemplate = manager->GetTemplate( includedTemplateName );

				if( includedTemplate != nullptr )
				{
					// Replace the include with the included template.
					( *this ) = ( *includedTemplate );

					// Determine how to merge the include.
					HashString mergeChildren = GetProperty( "mergeChildren", MERGE_CHILDREN_SHALLOW.GetString() );
					//DebugPrintf( "Merge: \"%s\"", mergeChildren.GetCString() );

					if( mergeChildren == MERGE_CHILDREN_DEEP )
					{
						// Deep merge overridden properties and children into the template.
						DeepMerge( overrides );
					}
					else if( mergeChildren == MERGE_CHILDREN_SHALLOW )
					{
						// Merge overridden properties and children into the template.
						Merge( overrides );
					}
					else
					{
						// Merge overridden properties and but replace children.
						Replace( overrides );

						if( mergeChildren != MERGE_CHILDREN_REPLACE )
						{
							WarnFail( "Invalid value \"%s\" specified for \"mergeChildren\" property in included template.", mergeChildren.GetCString() );
						}
					}
				}
				else
				{
					WarnFail( "Could not resolve template include because the template \"%s\" was not found!", includedTemplateName.GetCString() );
				}
			}
			else
			{
				WarnFail( "Could not resolve includes for WidgetTemplate because no \"%s\" parameter was found!", PROPERTY_TEMPLATE.GetCString() );
			}
		}

		for( auto it = mChildren.begin(); it != mChildren.end(); ++it )
		{
			// Resolve includes for all children.
			it->second.ResolveIncludes( manager );
		}
	}
	else
	{
		WarnFail( "Could not resolve includes for WidgetTemplate because no \"%s\" parameter was found!", PROPERTY_TYPE.GetCString() );
	}
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
