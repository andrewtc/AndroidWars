/*
 * Author      : Matthew Johnson
 * Date        : 22/Nov/2013
 * Description :
 *   
 */
 
#pragma once

namespace mage
{

	class Widget
	{
	DECLARE_RTTI;
	protected:
		Widget( const std::string& name, const XmlReader::XmlReaderIterator& itr, Widget* parent );
		virtual ~Widget();

		virtual void OnUpdate( float dt );
		virtual void OnDraw( const Camera& camera ) const;
		virtual bool OnPointerDown( float x, float y );
		virtual bool OnPointerUp( float x, float y );

	public:
		// Load a widget from xml definition
		static Widget* LoadWidget( const char* file );
		// Unregister widget and free memory
		static void DestroyWidget( Widget*& w );
		// Clean up all widgets
		static void DestroyAllWidgets();

		static void UpdateAllWidgets( float dt );
		static void DrawAllWidgets( const Camera& camera );
		static bool ProcessOnPointerDown( float x, float y );
		static bool ProcessOnPointerUp( float x, float y );
		static void LoadDefinitions( const char* file );

		float GetWidth() const;
		float GetHeight() const;

		Widget* GetParent() const { return mParent; }
		// Get the Widget that is the base for this widgets hierarchy
		Widget* GetRootWidget();

		bool Contains( float x, float y );

		void Show() { Visible = true; }
		void Hide() { Visible = false; }

		const HashString& GetName() const { return mName; }
		Widget* GetChildByName( const HashString& name );


		bool Visible;
		bool DebugLayout;

	protected:
		static Widget* LoadComponent( Widget* parent, const XmlReader::XmlReaderIterator& itr );
		static BitmapFont* GetFontByName( const HashString& name );

		void LoadLayoutParam( Widget*& target, const XmlReader::XmlReaderIterator& itr, const char* paramName );
		Vec2f GetPosition() const;
		void SetSprite( const HashString& spriteName );
		void UpdateLayout();

		HashString mName;
		Sprite* mSprite;

		HashMap< Widget* > mChildren;
		Widget* mParent;

		// Layout
		Widget* mBelow;
		Widget* mAbove;
		Widget* mToLeftOf;
		Widget* mToRightOf;
		bool mCenterInParent;

		bool mFixedSizeSprite;

		Vec2f mPosition;
		float mHeight;
		float mWidth;
		Vec4f mMargins;
		Color mDrawColor;

		enum MarginIndex
		{
			MARGIN_LEFT,
			MARGIN_TOP,
			MARGIN_RIGHT,
			MARGIN_BOTTOM,
		};

		static HashMap< Widget* > sWidgets;
		static HashMap< BitmapFont* > sFonts;
	};

}
