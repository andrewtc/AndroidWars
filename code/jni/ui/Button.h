/*
 * Author      : Matthew Johnson
 * Date        : 26/Nov/2013
 * Description :
 *   
 */
 
#pragma once


namespace mage
{

	class Button
		: public Label
	{
	public:
		Button( const std::string& name, const XmlReader::XmlReaderIterator& itr, Widget* parent );

		struct ButtonStyle
		{
			HashString SpriteName;
			HashString DefaultAnimName;
			HashString PressedAnimName;
			HashString SelectedAnimName;
		};
		static HashMap< ButtonStyle > sButtonStyles;
	protected:
		virtual ~Button();

		virtual bool OnPointerDown( float x, float y );
		virtual bool OnPointerUp( float x, float y );

	private:
		HashString mOnClickEvent;
		HashString mStyleName;
	};

}
