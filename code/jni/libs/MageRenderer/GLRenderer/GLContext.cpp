#include "RendererLib.h"

#ifdef ANDROID
#	include <EGL/egl.h>
#	include <GLES2/gl2.h>
#else
#   define GLEW
#   include "glew.h"
#endif

using namespace mage;

//---------------------------------------
GLContext* GLContext::GetContext()
{
	static GLContext context;
	return &context;
}
//---------------------------------------
GLContext::GLContext()
{}
//---------------------------------------
GLContext::~GLContext()
{}
//---------------------------------------
bool GLContext::Init( void** hWindow )
{
	return true;
}
//---------------------------------------
int GLContext::SwapBuffers()
{
	return 1;
}
//---------------------------------------
bool GLContext::Invalidate()
{
	return true;
}
//---------------------------------------
void GLContext::Suspend()
{
}
//---------------------------------------
int GLContext::Resume( void** hWindow )
{
	return 1;
}
//---------------------------------------
bool GLContext::CheckExtension( const char* extension ) const
{
	if ( extension )
	{
		std::string extensions = std::string( (char*) glGetString( GL_EXTENSIONS ) );
		std::string str = std::string( extension );
		str.append( " " );

		size_t pos = 0;
		return extensions.find( extension, pos ) != std::string::npos;
	}
	return false;
}
//---------------------------------------