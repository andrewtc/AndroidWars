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

// Mapping of internal enums to OpenGL enums
static GLenum gsOGLShaderType[ Shader::ST_COUNT ] =
{
	GL_VERTEX_SHADER,
	//GL_TESS_CONTROL_SHADER,
	//GL_TESS_EVALUATION_SHADER,
	//GL_GEOMETRY_SHADER,
	GL_FRAGMENT_SHADER
};

//---------------------------------------
Shader::Shader( ShaderType type, const char* source )
{
	mShaderId = glCreateShader( gsOGLShaderType[ type ] );
    
    if ( mShaderId )
    {
        glShaderSource( mShaderId, 1, &source, NULL );
        // Compile the shader
        glCompileShader( mShaderId );
        
        // Check for and report errors
        GLint success = 0;
        glGetShaderiv( mShaderId, GL_COMPILE_STATUS, &success );
        
        if ( !success )
        {
            WarnCrit( "ShaderError: Compile Failed\n", "" );
            
            GLint infoLen = 0;
            glGetShaderiv( mShaderId, GL_INFO_LOG_LENGTH, &infoLen );
            
            if ( infoLen )
            {
                char* infoLog = (char*) malloc( infoLen );
                if ( infoLog )
                {
                    glGetShaderInfoLog( mShaderId, infoLen, NULL, infoLog );
                    FatalError( "%s\n", infoLog );
                    free( infoLog );
                }
                else
                {
                    FatalError( "Failed to allocate buffer for error message size=%d\n", infoLen );
                }
                
                glDeleteShader( mShaderId );
                mShaderId = 0;
            }
            else
            {
                FatalError( "No info available\n", "" );
            }
        }
    }
    else
    {
        WarnCrit( "Shader: Failed to create shader\n", "" );
        
    }
/*
	const GLchar* p[1];
	GLint lengths[1];
	GLchar infolog[1024] = {0};
    GLsizei len = 0;

	p[0] = &source[0];
	lengths[0] = strlen( source );
	glShaderSource( mShaderId, 1, p, lengths );

	// Compile the shader
	glCompileShader( mShaderId );

	// Check for and report errors
	GLint success = 0;
	glGetShaderiv( mShaderId, GL_COMPILE_STATUS, &success );

	// Compiler error
	if ( !success )
	{
		glGetShaderInfoLog( mShaderId, sizeof( infolog ), &len, infolog );
        
		// Extract line number from infolog
		std::string log( infolog );
		std::string shaderText( source );
		int f = log.find( '(' );
		int l = log.find( ')' );
		std::string linenum = log.substr( f+1, l-f-1 );
		std::string errorline = "";

		int i=0, line=0, linenum_int, textsize = shaderText.length();
		StringUtil::StringToType( linenum, &linenum_int );
		--linenum_int;
		for ( ; i < textsize; ++i )
		{
			if ( shaderText[i] == '\n' ) ++line;
			if ( line == linenum_int )
			{
				errorline = shaderText.substr( i+1, shaderText.find( '\n', i+1 ) - i );
				break;
			}
		}

		// file_path(line#): error messages...
		assertion( false, "%s(%s):\n\n%s\nLine(%s):\n>%s\n\nError compiling shader.\n", "(file not found)", linenum.c_str(), infolog, linenum.c_str(), errorline.c_str() );

	}
*/
}
//---------------------------------------
Shader::~Shader()
{
	if ( mShaderId )
	{
		glDeleteShader( mShaderId );
	}
}
//---------------------------------------
uint32 Shader::GetShaderId() const
{
	return mShaderId;
}
//---------------------------------------