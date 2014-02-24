#include "androidwars.h"

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
	RegisterFactory< Listbox >( "Listbox" );

	// Create the root Widget.
	mRootWidget = new Widget( this, "root" );
	mRootWidget->Init();
}
//---------------------------------------
void WidgetManager::Destroy()
{
	assertion( !IsInitialized(), "Cannot destroy WidgetManager that hasn't been initialized!" );

	// Destroy the manager.
	mIsInitialized = false;

	// Destroy all Widget factories.
	for( auto it = mFactoriesByClassName.begin(); it != mFactoriesByClassName.end(); ++it )
	{
		delete it->second;
	}

	mFactoriesByClassName.clear();
}
//---------------------------------------
AbstractWidgetFactory* WidgetManager::GetFactory( const HashString& className ) const
{
	AbstractWidgetFactory* factory = nullptr;

	auto it = mFactoriesByClassName.find( className );

	if( it != mFactoriesByClassName.end() )
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
bool WidgetManager::OnPointerDown( float x, float y, size_t which )
{
	return mRootWidget->OnPointerDown( x, y );
}
//---------------------------------------
bool WidgetManager::OnPointerUp( float x, float y, size_t which )
{
	return mRootWidget->OnPointerUp( x, y );
}
//---------------------------------------
bool WidgetManager::OnPointerMotion( float x, float y, float dx, float dy, size_t which )
{
	//return mRootWidget->OnPointerMotion( x, y, dx, dy );
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
		// Button styles
		else if( itr.ElementNameEquals( "ButtonStyle" ) )
		{
			HashString name = itr.GetAttributeAsString( "name" );
			Button::ButtonStyle* style = Button::CreateButtonStyle( name );

			style->SpriteName      = itr.GetAttributeAsString( "sprite" );
			style->DefaultAnimName = itr.GetAttributeAsString( "default", "d" );
			style->PressedAnimName = itr.GetAttributeAsString( "pressed", "p" );
			style->SelectedAnimName= itr.GetAttributeAsString( "selected", "s" );
			style->WrapText        = itr.GetAttributeAsBool( "wrapText", true );
			style->PressedColor    = itr.GetAttributeAsColor( "pressedColor", Color::WHITE );
			style->SelectedSFXName = itr.GetAttributeAsString( "clickSFX", "" );
			style->DisabledColor   = itr.GetAttributeAsColor( "disabledColor", Color::GREY );
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
	}

	DebugPrintf( "Widget : Loaded UI definitions" );
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
