#include "androidwars.h"

using namespace mage;


IMPLEMENT_RTTI( mage, Widget, Graphic );


Graphic::Graphic( WidgetManager* manager, const HashString& name ) :
	Widget( manager, name ),
	mSprite( nullptr )
{ }


Graphic::~Graphic()
{
	// Destroy the Sprite for this Graphic (if any).
	ClearSprite();
}


void Graphic::OnLoadFromXML( const XmlReader::XmlReaderIterator& xml )
{
	Widget::OnLoadFromXML( xml );

	// Get the name of the sprite to use.
	HashString animationSetName = xml.GetAttributeAsCString( "animationSet", "" );
	HashString initialAnimationName = xml.GetAttributeAsCString( "initialAnimation", "default" );

	// Set the Sprite.
	SetSprite( animationSetName, initialAnimationName );
}


void Graphic::OnLoadFromDictionary( const Dictionary& dictionary )
{
	Widget::OnLoadFromDictionary( dictionary );

	// Get the name of the sprite to use.
	HashString animationSetName = dictionary.Get( "animationSet", "" );
	HashString initialAnimationName = dictionary.Get( "initialAnimation", "default" );

	// Set the Sprite.
	SetSprite( animationSetName, initialAnimationName );
}


void Graphic::OnUpdate( float elapsedTime )
{
	Widget::OnUpdate( elapsedTime );

	if( mSprite )
	{
		// Update the Sprite.
		mSprite->OnUpdate( elapsedTime );
	}
}


void Graphic::OnDraw( const Camera& camera )
{
	Widget::OnDraw( camera );

	if( mSprite )
	{
		// Make sure the sprite is in the right position.
		mSprite->Position = FindPosition();

		// Draw the sprite.
		mSprite->OnDraw( camera );
	}
}


void Graphic::SetSprite( const HashString& animationSetName, const HashString& initialAnimationName )
{
	// Destroy the existing Sprite (if any).
	ClearSprite();

	// Create a new sprite.
	Sprite* sprite = SpriteManager::CreateSprite( animationSetName, Vec2f::ZERO, initialAnimationName );

	if( sprite )
	{
		// Use the new sprite.
		mSprite = sprite;

		if( mSprite )
		{
			// Ignore camera offset
			mSprite->RelativeToCamera = false;
			mSprite->FixedSize = IsFixedSizeSprite();
			mSprite->DrawColor = mDrawColor;

			// Recalculate the size of the Graphic.
			RecalculateSize();
		}
	}
	else
	{
		WarnFail( "Failed to create widget sprite '%s'\n", animationSetName.GetString().c_str() );
	}
}


void Graphic::ClearSprite()
{
	if( mSprite )
	{
		// If there is already a sprite for this Graphic, destroy it.
		SpriteManager::DestroySprite( mSprite );
	}
}


void Graphic::RecalculateSize()
{
	if ( !IsFixedSizeSprite() )
	{
		// If this Graphic has a fixed-size Sprite, update the Widget dimensions.
		const RectI& bounds = mSprite->GetClippingRectForCurrentAnimation();
		SetSize( bounds.Width(), bounds.Height() );
	}
	else
	{
		// If the Sprite should scale with the Widget, set the width and height of the Sprite.
		mSprite->Size.Set( GetSize() );
	}
}
