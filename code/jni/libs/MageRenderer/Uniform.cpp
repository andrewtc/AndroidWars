#include "RendererLib.h"

#ifdef ANDROID
#   include <EGL/egl.h>
#   include <GLES/gl.h>
#   include <GLES2/gl2.h>
#   include <GLES2/gl2ext.h>
#else
#   include <glew.h>
#endif

using namespace mage;

//---------------------------------------
Uniform::Uniform( int location )
	: mLocation( location )
{}
//---------------------------------------
Uniform::~Uniform()
{}
//---------------------------------------




//---------------------------------------
Uniform1i::Uniform1i( int location )
	: Uniform( location )
	, mValue( 0 )
{}
//---------------------------------------
Uniform1i::~Uniform1i()
{}
//---------------------------------------
void Uniform1i::Apply()
{
	glUniform1i( mLocation, mValue );
}
//---------------------------------------
void Uniform1i::SetValue( int value )
{
	mValue = value;
}
//---------------------------------------




//---------------------------------------
Uniform1f::Uniform1f( int location )
	: Uniform( location )
	, mValue( 0 )
{}
//---------------------------------------
Uniform1f::~Uniform1f()
{}
//---------------------------------------
void Uniform1f::Apply()
{
	glUniform1f( mLocation, mValue );
}
//---------------------------------------
void Uniform1f::SetValue( float value )
{
	mValue = value;
}
//---------------------------------------




//---------------------------------------
Uniform1fv::Uniform1fv( int location )
	: Uniform( location )
	, mValue( 0 )
	, mCount( 0 )
{}
//---------------------------------------
Uniform1fv::~Uniform1fv()
{}
//---------------------------------------
void Uniform1fv::Apply()
{
	if ( mValue )
		glUniform1fv( mLocation, mCount, mValue );
}
//---------------------------------------
void Uniform1fv::SetValue( float* value, int count )
{
	mValue = value;
	mCount = count;
}
//---------------------------------------



//---------------------------------------
Uniform2fv::Uniform2fv( int location )
	: Uniform( location )
	, mValue( 0 )
	, mCount( 0 )
{}
//---------------------------------------
Uniform2fv::~Uniform2fv()
{}
//---------------------------------------
void Uniform2fv::Apply()
{
	if ( mValue )
		glUniform2fv( mLocation, mCount, &mValue[0].x );
}
//---------------------------------------
void Uniform2fv::SetValue( Vec2f* value, int count )
{
	mValue = value;
	mCount = count;
}
//---------------------------------------