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
	public:
		Widget( const char* ui_file );
		virtual ~Widget();

		void OnUpdate( float dt );
		void OnDraw( const Camera& camera ) const;
		void OnClick( float x, float y );

	private:
		struct Button
		{
			static const int READY = 0;
			static const int DISABLED = -1;
			static const int ACTIVATED = 1;
			Sprite* ButtonSprite;
			HashString OnClickEvent;
			HashString OnClickAnim;
			int State;
		};

		ArrayList< Button > mButtons;
		Sprite* mSprite;
	};

	/*namespace WidgetManager
	{
		void ProcessInput( float x, float y );
		void UpdateAllWidgets( float dt );
		void DrawAllWidgets( const Camera& camera );
	};*/

}
