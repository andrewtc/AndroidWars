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
		Graphic( WidgetManager* manager, const HashString& name );
		virtual ~Graphic();

		void SetSprite( const HashString& animationSetName, const HashString& initialAnimationName );
		void ClearSprite();
		Sprite* GetSprite() const;

	protected:
		virtual void OnLoadFromXML( const XmlReader::XmlReaderIterator& xml );
		virtual void OnLoadFromDictionary( const Dictionary& dictionary );

		virtual void OnUpdate( float elapsedTime );
		virtual void OnDraw( const Camera& camera );

		void RecalculateSize();

	private:
		Sprite* mSprite;
	};
}
