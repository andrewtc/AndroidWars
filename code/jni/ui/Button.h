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
		: public Widget
	{
	public:
		Button( const std::string& name, const XmlReader::XmlReaderIterator& itr );
	protected:
		virtual ~Button();

		virtual bool OnClick( float x, float y );

	private:
		HashString mOnClickEvent;
		HashString mOnClickAnim;
	};

}
