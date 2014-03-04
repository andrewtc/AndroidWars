#pragma once

namespace mage
{
	/**
	 * Widget class that displays an animated sprite on a Widget layout.
	 */
	class Graphic : public Widget
	{
		DECLARE_RTTI;

	public:
		static const HashString DRAW_MODE_CLIP_STRING;
		static const HashString DRAW_MODE_SCALE_STRING;
		static const HashString DRAW_MODE_TILE_STRING;

		enum DrawMode
		{
			DRAW_MODE_CLIP,
			DRAW_MODE_SCALE,
			DRAW_MODE_TILE
		};

		Graphic( WidgetManager* manager, const HashString& name );
		virtual ~Graphic();

		void SetSprite( const HashString& animationSetName, const HashString& initialAnimationName );
		void ClearSprite();
		Sprite* GetSprite() const;

		void SetDrawColor( const Color& color );
		Color GetDrawColor() const;

		void SetDrawMode( DrawMode drawMode );
		DrawMode GetDrawMode() const;

		void SetDrawOffset( const Vec2f& drawOffset );
		Vec2f GetDrawOffset() const;

	protected:
		virtual void OnLoadFromTemplate( const WidgetTemplate& widgetTemplate );

		virtual void OnUpdate( float elapsedTime );
		virtual void OnDraw( const Camera& camera );

		void UpdateSprite();

	private:
		DrawMode mDrawMode;
		Sprite* mSprite;
		Color mDrawColor;
		Vec2f mDrawOffset;
	};
}
