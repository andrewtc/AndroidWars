#pragma once

namespace mage
{
	class AbstractWidgetFactory;
	class Widget;


	class WidgetManager
	{
	public:
		WidgetManager();
		virtual ~WidgetManager();

		void Init();
		void Destroy();

		BitmapFont* GetFontByName( const HashString& name );

		template< class WidgetSubclass >
		void RegisterFactory( const HashString& className );
		AbstractWidgetFactory* GetFactory( const HashString& className ) const;

		template< class WidgetSubclass = Widget >
		WidgetSubclass* CreateWidgetByClassName( const HashString& className, const HashString& name );
		template< class WidgetSubclass = Widget >
		WidgetSubclass* CreateWidgetFromDictionary( const HashString& name, const Dictionary& dictionary );

		template< class WidgetSubclass = Widget >
		WidgetSubclass* LoadWidgetFromFile( const char* file );
		template< class WidgetSubclass = Widget >
		WidgetSubclass* LoadWidgetFromXML( const XmlReader::XmlReaderIterator& xml );

		void DestroyWidget( Widget* widget );

		void Update( float elapsedTime );
		void Draw( const Camera& camera );
		bool OnPointerDown( float x, float y, size_t which );
		bool OnPointerUp( float x, float y, size_t which );
		bool OnPointerMotion( float x, float y, float dx, float dy, size_t which );

		void LoadTheme( const char* file );
		WidgetTemplate* CreateTemplate( const HashString& name );
		WidgetTemplate* GetTemplate( const HashString& name ) const;
		WidgetTemplate* LoadTemplateFromFile( const HashString& name, const std::string& file );
		WidgetTemplate* LoadTemplateFromXML( const HashString& name, const XmlReader::XmlReaderIterator& xml );
		void DestroyTemplate( const HashString& name );
		void DestroyAllTemplates();

		Widget* GetRootWidget() const;

		bool IsInitialized() const;

	private:
		void BuildWidgetTemplateFromXML( const XmlReader::XmlReaderIterator& xml, WidgetTemplate& widgetTemplate );

		bool mIsInitialized;
		Widget* mRootWidget;
		HashMap< WidgetTemplate* > mTemplatesByName;
		HashMap< AbstractWidgetFactory* > mFactoriesByClassName;
		HashMap< BitmapFont* > mFonts;
	};
	//---------------------------------------
	template< typename WidgetSubclass >
	void WidgetManager::RegisterFactory( const HashString& className )
	{
		assertion( IsInitialized(), "Cannot register factory for WidgetManager that is not initialized!" );
		assertion( !className.GetString().empty(), "Cannot register factory with empty class name!" );

		// Make sure there isn't already a factory with the passed name.
		assertion( GetFactory( className ) == nullptr, "A Widget factory with the class name \"%s\" already exists!", className.GetCString() );

		// Create a new factory that services Widgets of the specified type.
		mFactoriesByClassName[ className ] = new WidgetFactory< WidgetSubclass >();
		DebugPrintf( "Registered Widget factory: \"%s\" = %s", className.GetCString(), WidgetSubclass::TYPE.GetName() );
	}
	//---------------------------------------
	template< typename WidgetSubclass >
	WidgetSubclass* WidgetManager::CreateWidgetByClassName( const HashString& className, const HashString& name )
	{
		assertion( IsInitialized(), "Cannot create Widget for WidgetManager that is not initialized!" );

		Widget* base = nullptr;
		WidgetSubclass* derived = nullptr;

		// Look up the factory by its class name.
		AbstractWidgetFactory* factory = GetFactory( className );

		if( factory )
		{
			// If a factory was found, create a new Widget instance.
			base = factory->CreateWidget( this, name );

			// Cast the new Widget to the derived class.
			derived = dynamic_cast< WidgetSubclass* >( base );

			if( !derived )
			{
				// If the Widget could not be cast to the proper class type, return nullptr and post a warning.
				WarnFail( "Could not load Widget \"%s\" because the Widget type \"%s\" could not be cast to the required type (\"%s\")!\n", name.GetCString(), base->GetType().GetName(), WidgetSubclass::TYPE.GetName() );

				// Destroy the Widget.
				delete base;
			}
		}
		else
		{
			// If no factory was found, return nullptr and post a warning.
			WarnFail( "Cannot instantiate unknown widget type \"%s\"\n", className.GetCString() );
		}

		// Return the new Widget.
		return derived;
	}
	//---------------------------------------
	template< class WidgetSubclass >
	WidgetSubclass* WidgetManager::CreateWidgetFromDictionary( const HashString& name, const Dictionary& dictionary )
	{
		assertion( IsInitialized(), "Cannot create Widget for WidgetManager that is not initialized!" );

		WidgetSubclass* widget = nullptr;

		// Determine if a base class was specified in the Dictionary.
		const char* baseClass = "";
		dictionary.Get( "baseClass", baseClass );

		HashString baseClassHash( baseClass );

		if( baseClassHash.GetString().empty() )
		{
			// If no base class was specified, create a new Widget of the specified class.
			widget = new WidgetSubclass( this, name );
		}
		else
		{
			// Otherwise, create a new Widget with the specified class name.
			widget = CreateWidgetByClassName< WidgetSubclass >( baseClassHash, name );
		}

		if( widget )
		{
			// Load the widget from the dictionary.
			widget->OnLoadFromDictionary( dictionary );
		}

		return widget;
	}
	//---------------------------------------
	template< class WidgetSubclass >
	WidgetSubclass* WidgetManager::LoadWidgetFromFile( const char* file )
	{
		assertion( IsInitialized(), "Cannot load Widget for WidgetManager that is not initialized!" );

		WidgetSubclass* widget = nullptr;

		// Open the file as an XML document.
		XmlReader reader( file );

		if ( !reader.Fail() )
		{
			// If the file was opened successfully, parse the Widget subclass from XML.
			XmlReader::XmlReaderIterator itr = reader.ReadRoot();
			widget = LoadWidgetFromXML< WidgetSubclass >( itr );
		}

		return widget;
	}
	//---------------------------------------
	template< class WidgetSubclass >
	WidgetSubclass* WidgetManager::LoadWidgetFromXML( const XmlReader::XmlReaderIterator& xml )
	{
		assertion( IsInitialized(), "Cannot load Widget for WidgetManager that is not initialized!" );

		WidgetSubclass* widget = nullptr;

		// Get the class name and name of the Widget to create.
		HashString className = xml.ElementName();
		std::string name = xml.GetAttributeAsString( "name", "Widget" );

		// Create a new Widget with the specified class and name and load it from XML.
		widget = CreateWidgetByClassName< WidgetSubclass >( className, name );

		if( widget )
		{
			// Load the widget from XML.
			widget->OnLoadFromXML( xml );

			for( XmlReader::XmlReaderIterator childXML = xml.NextChild(); childXML.IsValid(); childXML = childXML.NextSibling() )
			{
				// Load all children.
				Widget* child = LoadWidgetFromXML( childXML );

				if( child )
				{
					// If the child was loaded successfully, add it to its parent.
					widget->AddChild( child );
				}
			}
		}

		return widget;
	}
	//---------------------------------------
	inline bool WidgetManager::IsInitialized() const
	{
		return mIsInitialized;
	}
	//---------------------------------------
	inline Widget* WidgetManager::GetRootWidget() const
	{
		return mRootWidget;
	}
}
