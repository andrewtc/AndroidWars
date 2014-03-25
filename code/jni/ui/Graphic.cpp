#include "androidwars.h"

using namespace mage;


IMPLEMENT_RTTI( mage, Widget, Graphic );


const HashString Graphic::DRAW_MODE_CLIP_STRING  = "clip";
const HashString Graphic::DRAW_MODE_SCALE_STRING = "scale";
const HashString Graphic::DRAW_MODE_TILE_STRING  = "tile";


Graphic::Graphic( WidgetManager* manager, const HashString& name ) :
	Widget( manager, name ),
	mSprite( nullptr ),
	mDrawColor( Color::WHITE ),
	mDrawMode( DRAW_MODE_CLIP )
{ }


Graphic::~Graphic()
{
	// Destroy the Sprite for this Graphic (if any).
	ClearSprite();
}


void Graphic::OnLoadFromTemplate( const WidgetTemplate& widgetTemplate )
{
	Widget::OnLoadFromTemplate( widgetTemplate );

	// Get the name of the sprite to use.
	HashString animationSetName = widgetTemplate.GetProperty( "animationSet", "", true );
	HashString initialAnimationName = widgetTemplate.GetProperty( "initialAnimation", "default" );

	// Set the Sprite.
	SetSprite( animationSetName, initialAnimationName );

	if( mSprite && !widgetTemplate.HasProperty( "size" ) )
	{
		// If no size was specified, use the size of the current sprite frame.
		RectI animationBounds = mSprite->GetClippingRectForCurrentAnimation();
		SetSize( (float) animationBounds.Width(), (float) animationBounds.Height() );
	}

	// Set the draw mode.
	HashString drawModeString = widgetTemplate.GetProperty( "drawMode", DRAW_MODE_SCALE_STRING.GetCString() );
	DrawMode drawMode = DRAW_MODE_CLIP;

	if( drawModeString == DRAW_MODE_CLIP_STRING )
	{
		drawMode = DRAW_MODE_CLIP;
	}
	else if( drawModeString == DRAW_MODE_SCALE_STRING )
	{
		drawMode = DRAW_MODE_SCALE;
	}
	else if( drawModeString == DRAW_MODE_TILE_STRING )
	{
		drawMode = DRAW_MODE_TILE;
	}
	else
	{
		WarnFail( "Invalid drawMode \"%s\" specified for Graphic \"%s\"!", drawModeString.GetCString(), GetName().GetCString() );
	}

	SetDrawMode( drawMode );

	// Set the draw offset.
	mDrawOffset = widgetTemplate.GetPropertyAsVec2f( "drawOffset", Vec2f::ZERO );
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
		// Make sure the sprite is in the right position and is the right size.
		mSprite->Position = CalculatePosition();

		if( mDrawMode == DRAW_MODE_SCALE )
		{
			// If the sprite should scale to fit the Graphic, scale it.
			mSprite->Size.Set( GetSize() );
		}
		else
		{
			// Get the bounds for the current animation.
			RectI animationBounds = mSprite->GetClippingRectForCurrentAnimation();

			// Set the size of the sprite to the size of the current animation.
			mSprite->Size.Set( (float) animationBounds.Width(), (float) animationBounds.Height() );
		}

		if( mDrawMode == DRAW_MODE_TILE )
		{
			// Save the previous position of the Sprite.
			Vec2f previousPosition = mSprite->Position;

			// Find the bounds of the Widget.
			RectF bounds = CalculateBounds();

			// Find the bounds of the sprite.
			RectI animationBounds = mSprite->GetClippingRectForCurrentAnimation();

			// Wrap the draw offset to the size of the sprite.
			Vec2f drawOffset;
			drawOffset.x = Mathf::Wrap( mDrawOffset.x, 0.0f, animationBounds.Width() );
			drawOffset.y = Mathf::Wrap( mDrawOffset.y, 0.0f, animationBounds.Height() );

			for( float top = ( bounds.Top - drawOffset.x ); top < bounds.Bottom; top += animationBounds.Height() )
			{
				for( float left = ( bounds.Left - drawOffset.y ); left < bounds.Right; left += animationBounds.Width() )
				{
					// Draw the sprite, tiling the whole area of the Widget.
					mSprite->Position = Vec2f( left, top );
					mSprite->OnDraw( camera );
				}
			}

			// Restore the previous position.
			mSprite->Position = previousPosition;
		}
		else
		{
			// Draw the sprite once.
			mSprite->OnDraw( camera );
		}
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
			// Ignore the camera offset.
			mSprite->RelativeToCamera = false;

			// Update the Sprite draw options.
			UpdateSprite();
		}
	}
	else
	{
		WarnFail( "Failed to create sprite \"%s\" for Graphic \"%s\"!", animationSetName.GetString().c_str(), GetFullName().c_str() );
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


Sprite* Graphic::GetSprite() const
{
	return mSprite;
}


bool Graphic::HasSprite() const
{
	return ( mSprite != nullptr );
}


void Graphic::SetDrawColor( const Color& color )
{
	mDrawColor = color;
	UpdateSprite();
}


Color Graphic::GetDrawColor() const
{
	return mDrawColor;
}


void Graphic::SetDrawMode( DrawMode drawMode )
{
	mDrawMode = drawMode;
	UpdateSprite();
}


Graphic::DrawMode Graphic::GetDrawMode() const
{
	return mDrawMode;
}


void Graphic::SetDrawOffset( const Vec2f& drawOffset )
{
	mDrawOffset = drawOffset;
}


Vec2f Graphic::GetDrawOffset() const
{
	return mDrawOffset;
}


void Graphic::UpdateSprite()
{
	if( mSprite )
	{
		// If the sprite should scale to fit, set the fixed size property.
		mSprite->FixedSize = ( mDrawMode == DRAW_MODE_SCALE );

		// Set the draw color of the Sprite.
		mSprite->DrawColor = mDrawColor;
	}
}
