#include "androidwars.h"

#include "tinyxml2.h"

using namespace mage;


//---------------------------------------
// WidgetManager
//---------------------------------------
WidgetManager::WidgetManager() :
	mRootWidget( nullptr )
{ }
//---------------------------------------
WidgetManager::~WidgetManager() { }
//---------------------------------------
void WidgetManager::Init()
{
	assertion( !IsInitialized(), "Cannot initialize WidgetManager that has already been initialized!" );

	// Initialize the manager.
	mIsInitialized = true;

	// Load Widget definitions
	LoadTheme( "ui/definitions.xml" );

	// Register Widget factories.
	RegisterFactory< Widget >( "Widget" );
	RegisterFactory< Label >( "Label" );
	RegisterFactory< Graphic >( "Graphic" );
	RegisterFactory< Button >( "Button" );
	RegisterFactory< TextField >( "TextField" );
	RegisterFactory< ListLayout >( "ListLayout" );

	// Create the root Widget.
	mRootWidget = new Widget( this, "root" );
	mRootWidget->Init();
}
//---------------------------------------
void WidgetManager::Destroy()
{
	assertion( IsInitialized(), "Cannot destroy WidgetManager that hasn't been initialized!" );

	// Destroy the manager.
	mIsInitialized = false;

	// Destroy all Widget templates.
	DestroyAllTemplates();

	// Destroy all Widget factories.
	for( auto it = mFactoriesByType.begin(); it != mFactoriesByType.end(); ++it )
	{
		delete it->second;
	}

	mFactoriesByType.clear();
}
//---------------------------------------
AbstractWidgetFactory* WidgetManager::GetFactory( const HashString& type ) const
{
	AbstractWidgetFactory* factory = nullptr;

	auto it = mFactoriesByType.find( type );

	if( it != mFactoriesByType.end() )
	{
		factory = it->second;
	}

	return factory;
}
//---------------------------------------
void WidgetManager::DestroyWidget( Widget* widget )
{
	assertion( widget->GetManager() == this, "Cannot destroy a Widget that was created by another WidgetManager!" );

	// See if the Widget is the child of some other Widget.
	Widget* parent = widget->GetParent();

	if( parent != nullptr )
	{
		// Remove the Widget from its parent Widget (if any).
		parent->RemoveChild( widget );
	}

	// Destroy the Widget.
	delete widget;
}
//---------------------------------------
void WidgetManager::Update( float elapsedTime )
{
	// Update the root Widget.
	mRootWidget->Update( elapsedTime );
}
//---------------------------------------
void WidgetManager::Draw( const Camera& camera )
{
	// Draw the root Widget.
	mRootWidget->Draw( camera );
}
//---------------------------------------
bool WidgetManager::PointerDown( float x, float y, size_t which )
{
	bool wasHandled = false;

	for( Widget* widget = FindWidgetUnderPointer( x, y );
		 widget != nullptr && widget != mRootWidget && wasHandled == false;
		 widget = widget->GetParent() )
	{
		// If a widget was found under the pointer, let the topmost widget process the event.
		// If the widget didn't handle the event, let each ancestor process the event until
		// the event is handled or the last ancestor is reached.
		wasHandled = widget->PointerDown( x, y );
	}

	return wasHandled;
}
//---------------------------------------
bool WidgetManager::PointerUp( float x, float y, size_t which )
{
	bool wasHandled = false;

	for( Widget* widget = FindWidgetUnderPointer( x, y );
		 widget != nullptr && widget != mRootWidget && wasHandled == false;
		 widget = widget->GetParent() )
	{
		// If a widget was found under the pointer, let the topmost widget process the event.
		// If the widget didn't handle the event, let each ancestor process the event until
		// the event is handled or the last ancestor is reached.
		wasHandled = widget->PointerUp( x, y );
	}

	return wasHandled;
}
//---------------------------------------
bool WidgetManager::PointerMotion( float x, float y, float dx, float dy, size_t which )
{
	//TODO return mRootWidget->OnPointerMotion( x, y, dx, dy );
	return false;
}
//---------------------------------------
void WidgetManager::LoadTheme( const char* file )
{
	XmlReader reader( file );
	XmlReader::XmlReaderIterator root = reader.ReadRoot();

	for ( XmlReader::XmlReaderIterator itr = root.NextChild();
		  itr.IsValid(); itr = itr.NextSibling() )
	{
		// Fonts to load
		if( itr.ElementNameEquals( "Font" ) )
		{
			const char* file = itr.GetAttributeAsCString( "file" );
			const char* name = itr.GetAttributeAsCString( "name" );
			BitmapFont* fnt = new BitmapFont( file );
			mFonts[ name ] = fnt;
		}
		// Sprites to load
		else if( itr.ElementNameEquals( "Sprite" ) )
		{
			const char* file = itr.GetAttributeAsCString( "file" );
			SpriteManager::LoadSpriteAnimations( file, false );
		}
		// Sounds
		else if( itr.ElementNameEquals( "Sound" ) )
		{
			const char* file = itr.GetAttributeAsCString( "file" );
			const char* name = itr.GetAttributeAsCString( "name" );
			SoundClip* clip = gSoundManager->LoadSoundClip( file, name );
			if ( !clip )
			{
				WarnFail( "Failed to load sound %s as %s\n", file, name );
			}
		}
		else if( itr.ElementNameEquals( "Template" ) )
		{
			const char* name = itr.GetAttributeAsCString( "name" );
			const char* file = itr.GetAttributeAsCString( "file" );

			// Load the template from the XML file.
			LoadTemplateFromFile( name, file );
		}
	}

	DebugPrintf( "Loaded Widget theme \"%s\".", file );
}
//---------------------------------------
WidgetTemplate* WidgetManager::CreateTemplate( const HashString& name )
{
	WidgetTemplate* result = nullptr;

	if( !name.GetString().empty() )
	{
		if( GetTemplate( name ) == nullptr )
		{
			// If there isn't already a template with this name, create it and return it.
			result = new WidgetTemplate();

			// Store the template by its name.
			mTemplatesByName[ name ] = result;
		}
		else
		{
			WarnFail( "Failed to create Widget template \"%s\" because a template with the same name already exists!", name.GetCString() );
		}
	}
	else
	{
		WarnFail( "Cannot create WidgetTemplate without a valid name!" );
	}

	return result;
}
//---------------------------------------
WidgetTemplate* WidgetManager::GetTemplate( const HashString& name ) const
{
	WidgetTemplate* result = nullptr;

	// Look up the template by its name (if it exists).
	auto it = mTemplatesByName.find( name );

	if( it != mTemplatesByName.end() )
	{
		// If the template was found, return it.
		result = it->second;
	}

	return result;
}
//---------------------------------------
bool WidgetManager::HasTemplate( const HashString& name ) const
{
	return ( GetTemplate( name ) != nullptr );
}
//---------------------------------------
WidgetTemplate* WidgetManager::LoadTemplateFromFile( const HashString& name, const std::string& file )
{
	WidgetTemplate* widgetTemplate = nullptr;

	// Open the XML file.
	XmlReader reader;
	reader.LoadFile( file.c_str() );

	// Get the root XML element.
	XmlReader::XmlReaderIterator xml = reader.ReadRoot();

	if( !reader.Fail() )
	{
		if( xml.IsValid() )
		{
			// Load the WidgetTemplate from the XML file.
			widgetTemplate = LoadTemplateFromXML( name, xml );

			if( widgetTemplate )
			{
				//DebugPrintf( "Loaded Widget template \"%s\":\n%s", name.GetCString(), widgetTemplate->ToString().c_str() );
			}
			else
			{
				WarnFail( "Error loading Widget template from file \"%s\"!", file.c_str() );
			}
		}
		else
		{
			WarnFail( "No root XML element found in Widget template file \"%s\"!", file.c_str() );
		}
	}
	else
	{
		WarnFail( "Could not open Widget template file \"%s\"!", file.c_str() );
	}

	return widgetTemplate;
}
//---------------------------------------
WidgetTemplate* WidgetManager::LoadTemplateFromXML( const HashString& name, const XmlReader::XmlReaderIterator& xml )
{
	// Create a Widget template with the specified name.
	WidgetTemplate* widgetTemplate = CreateTemplate( name );

	if( widgetTemplate != nullptr )
	{
		// Load the Widget properties from the XML file.
		BuildWidgetTemplateFromXML( xml, *widgetTemplate );
	}
	else
	{
		WarnFail( "Could not create WidgetTemplate \"%s\"!", name.GetCString() );
	}

	return widgetTemplate;
}
//---------------------------------------
void WidgetManager::DestroyTemplate( const HashString& name )
{
	// Find the template by its name.
	auto it = mTemplatesByName.find( name );

	if( it != mTemplatesByName.end() )
	{
		WidgetTemplate* widgetTemplate = it->second;

		// Remove the template from the list of templates.
		mTemplatesByName.erase( it );

		// Destroy the template.
		delete widgetTemplate;
	}
	else
	{
		WarnFail( "Could not destroy template \"%s\" because no template with that name exists!", name.GetCString() );
	}
}
//---------------------------------------
void WidgetManager::DestroyAllTemplates()
{
	for( auto it = mTemplatesByName.begin(); it != mTemplatesByName.end(); ++it )
	{
		delete it->second;
	}

	mTemplatesByName.clear();
}
//---------------------------------------
void WidgetManager::BuildWidgetTemplateFromXML( const XmlReader::XmlReaderIterator& xml, WidgetTemplate& widgetTemplate )
{
	// Clear all properties in the result object.
	widgetTemplate.Clear();

	// Use the name of the element as the proposed type for this Widget template.
	widgetTemplate.SetProperty( WidgetTemplate::PROPERTY_TYPE, xml.ElementName() );

	// Get the first attribute from the XML element.
	const tinyxml2::XMLElement* element = xml.GetCurrentElement();

	for( const tinyxml2::XMLAttribute* currentAttribute = element->FirstAttribute();
		 currentAttribute != nullptr; currentAttribute = currentAttribute->Next() )
	{
		// Load each attribute of the root XML element as a string property.
		widgetTemplate.SetProperty( currentAttribute->Name(), currentAttribute->Value() );
	}

	for( XmlReader::XmlReaderIterator childXML = xml.NextChild(); childXML.IsValid(); childXML = childXML.NextSibling() )
	{
		// Get the name of the child element.
		HashString childName = childXML.GetAttributeAsCString( "name" );

		// Parse the element as a child template.
		WidgetTemplate child;
		BuildWidgetTemplateFromXML( childXML, child );

		// Add the child template to this one.
		widgetTemplate.SetChild( childName, child );
	}
}
//---------------------------------------
BitmapFont* WidgetManager::GetFontByName( const HashString& name )
{
	BitmapFont* font = nullptr;

	// Look up the font by its name.
	auto itr = mFonts.find( name );

	if( itr != mFonts.end() )
	{
		// If a font was found, return it.
		font = itr->second;
	}

	return font;
}
