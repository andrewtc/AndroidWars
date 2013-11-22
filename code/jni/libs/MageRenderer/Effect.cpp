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
Effect::Effect()
	: mProgramId( 0 )
{}
//---------------------------------------
Effect::Effect( const Shader& vs, const Shader& fs )
{
	SetShaders( vs, fs );
}
//---------------------------------------
void Effect::SetShaders( const Shader& vs, const Shader& fs )
{
	GLint success = 0;
	GLchar errorlog[1024] = { 0 };
    GLsizei len = 0;

	// Create the program
	mProgramId = glCreateProgram();
    
    if ( mProgramId )
    {
        // Attach shaders
        glAttachShader( mProgramId, vs.GetShaderId() );
        glAttachShader( mProgramId, fs.GetShaderId() );

        // Link program
        glLinkProgram( mProgramId );

        // Check for errors
        GLint success = GL_TRUE;
        glGetProgramiv( mProgramId, GL_LINK_STATUS, &success );
        if ( success != GL_TRUE )
        {
            WarnCrit( "ShaderError: Link Failed\n", "" );
            DebugErrorLog();
            glDeleteProgram( mProgramId );
			mProgramId = 0;
        }
        
        // Validate program
        glValidateProgram( mProgramId );
        
        // Check for errors
        success = GL_TRUE;
        glGetProgramiv( mProgramId, GL_VALIDATE_STATUS, &success );
        if ( success != GL_TRUE )
        {
           WarnCrit( "ShaderError: Link Failed\n", "" );
           DebugErrorLog();
           glDeleteProgram( mProgramId );
		mProgramId = 0;
        }        
    }
    else
    {
        WarnCrit( "ShaderProgram: Failed to create program\n", "" );
        
    }
}
//---------------------------------------
Effect::~Effect()
{
	if ( mProgramId )
	{
		glDeleteProgram( mProgramId );
		mProgramId = 0;
	}
}
//---------------------------------------
void Effect::Apply() const
{
	glUseProgram( mProgramId );

	for ( auto itr = mUniforms.begin(); itr != mUniforms.end(); ++itr )
	{
		(*itr)->Apply();
	}
}
//---------------------------------------
int Effect::GetUniformLocation( const char* name ) const
{
	GLint location = glGetUniformLocation( mProgramId, name );

	// Warn bad name
	if ( location == INVALID_LOCATION )
	{
		ConsolePrintf( CONSOLE_WARNING, "Warning: Unable to get the location of uniform '%s' in program '%s'.\n", name, "(todo names blah blah)" );
	}

	return location;
}
//---------------------------------------
int Effect::GetAttributeLocation( const char* name ) const
{
	int loc = glGetAttribLocation( mProgramId, name );
	// Warn bad name
	/*if ( loc == INVALID_LOCATION )
	{
		ConsolePrintf( CONSOLE_WARNING, "Warning: Unable to get the location of attrib '%s' in program '%s'.\n", name, "(todo names blah blah)" );
	}*/
	return loc;
}
//---------------------------------------
void Effect::AddUniform( Uniform* uniform )
{
	mUniforms.push_back( uniform );
}
//---------------------------------------
void Effect::DebugErrorLog()
{
    GLint infoLen = 0;
    glGetProgramiv( mProgramId, GL_INFO_LOG_LENGTH, &infoLen );
    if ( infoLen )
    {
        char* infoLog = (char*) malloc( infoLen );
        if ( infoLog )
        {
            glGetProgramInfoLog( mProgramId, infoLen, NULL, infoLog );
            FatalError( "%s\n", infoLog );
            free( infoLog );
        }
        else
        {
            FatalError( "Failed to allocate buffer for error message size=%d\n", infoLen );
        }
    }
    else
    {
        FatalError( "No info available\n", "" );
    }
}
//---------------------------------------