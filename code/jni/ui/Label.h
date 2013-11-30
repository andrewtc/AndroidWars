/*
 * Author      : Matthew Johnson
 * Date        : 26/Nov/2013
 * Description :
 *   
 */
 
#pragma once

namespace mage
{

	class Label
		: public Widget
	{
	public:
		Label( const std::string& name, const XmlReader::XmlReaderIterator& itr, Widget* parent );
		virtual ~Label();

		virtual void OnDraw( const Camera& camera ) const;

		std::string Text;
		float TextScale;
		Color TextColor;
		int MaxLineLength;

	protected:
		BitmapFont* mFont;
	};

}
