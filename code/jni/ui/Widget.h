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
	protected:
		Widget( const std::string& name, const XmlReader::XmlReaderIterator& itr, Widget* parent );
		virtual ~Widget();

		virtual void OnUpdate( float dt );
		virtual void OnDraw( const Camera& camera ) const;
		virtual bool OnClick( float x, float y );

	public:
		// Load a widget from xml definition
		static Widget* LoadWidget( const char* file );
		// Unregister widget and free memory
		static void DestroyWidget( Widget*& w );
		// Clean up all widgets
		static void DestroyAllWidgets();

		static void UpdateAllWidgets( float dt );
		static void DrawAllWidgets( const Camera& camera );
		static void ProcessOnClick( float x, float y );

	protected:
		static Widget* LoadComponent( Widget* parent, const XmlReader::XmlReaderIterator& itr );

		Widget* GetChildByName( const HashString& name );
		void LoadLayoutParam( Widget*& target, const XmlReader::XmlReaderIterator& itr, const char* paramName );

		Vec2f GetPosition() const;

		HashString mName;
		Sprite* mSprite;

		HashMap< Widget* > mChildren;
		Widget* mParent;

		// Layout
		Widget* mBelow;
		Widget* mAbove;
		Widget* mToLeftOf;
		Widget* mToRightOf;

		Vec2f mPosition;
		float mHeight;
		float mWidth;

		static HashMap< Widget* > sWidgets;
	};

}
