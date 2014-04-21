/*
 * Author      : Matthew Johnson
 * Date        : 3/Jan/2014
 * Description :
 *   
 */
 
#pragma once

namespace mage
{

	class GLContext
		: public RenderContext
	{
	public:
		GLContext();
		virtual ~GLContext();

		virtual bool Init( void** hWindow );
		virtual int SwapBuffers();
		virtual bool Invalidate();
		virtual void Suspend();
		virtual int Resume( void** hWindow );

		bool CheckExtension( const char* extension ) const;

		static GLContext* GetContext();
	};

}