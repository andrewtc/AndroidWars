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
		: public Object
	{
	protected:
		Widget( const std::string& name, const XmlReader::XmlReaderIterator& itr );
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

		HashString mName;
		Sprite* mSprite;

		ArrayList< Widget* > mChildren;
		Widget* mParent;

		static HashMap< Widget* > sWidgets;
	};

}
