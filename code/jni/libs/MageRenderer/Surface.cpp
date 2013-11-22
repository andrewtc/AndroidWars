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
Surface::Surface( uint32 width, uint32 hieght, bool useDepth, bool linear, bool isSceneTarget )
	: Width( width )
	, Height( hieght )
	, DepthBuffer( 0 )
	, mFBO( 0 )
{
	GLenum internalFormat = GL_RGBA;
	GLenum type = GL_UNSIGNED_BYTE;
	GLenum filter = linear ? GL_LINEAR : GL_NEAREST;

	ViewPort.x = 0;
	ViewPort.y = 0;
	ViewPort.width = Width;
	ViewPort.height = Height;

	ClearColor = Color::BLACK;

	if ( !isSceneTarget )
	{
		// Create color texture
		IRenderer::TextureHandle hTexture;
		CreateTexture( &hTexture, NULL, Width, Height, IRenderer::PF_RGBA, linear );
		/*
		glGenTextures( 1, &hTexture );

		glBindTexture( GL_TEXTURE_2D, hTexture );
		glTexImage2D( GL_TEXTURE_2D, 0, internalFormat, Width, Height, 0, GL_RGBA, type, NULL );
		glAssertError;
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		*/
		TextureColor = new Texture2D( hTexture, Width, Height );

		// Create depth buffer
		if ( useDepth )
		{
			glGenRenderbuffers( 1, &DepthBuffer );
			glBindRenderbuffer( GL_RENDERBUFFER, DepthBuffer );
#ifdef ANDROID
            glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Width, Height );
#else
			glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, Width, Height );
#endif
		}
		
		// Create FBO
		glGenFramebuffers( 1, &mFBO );
		glBindFramebuffer( GL_FRAMEBUFFER, mFBO );
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hTexture, 0 );
		
		if ( useDepth )
			glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DepthBuffer );
		
		// Check errors
		GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
		if ( status != GL_FRAMEBUFFER_COMPLETE )
		{
#ifdef ANDROID
    WarnCrit( "Failed to create FBO!", "" );
#else
			switch ( status )
			{
				case GL_FRAMEBUFFER_UNDEFINED:
					assertion( false, "FrameBuffer Error: 0x%X (undefined fbo)\n", status );
					break;
				case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
					assertion( false, "FrameBuffer Error: 0x%X (incomplete attachment)\n", status );
					break;
				case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
					assertion( false, "FrameBuffer Error: 0x%X (incomplete missing attachment)\n", status );
					break;
				case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
					assertion( false, "FrameBuffer Error: 0x%X (incomplete draw buffer)\n", status );
					break;
				case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
					assertion( false, "FrameBuffer Error: 0x%X (incomplete read buffer)\n", status );
					break;
				case GL_FRAMEBUFFER_UNSUPPORTED:
					assertion( false, "FrameBuffer Error: 0x%X (unsupported)\n", status );
					break;
				case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
					assertion( false, "FrameBuffer Error: 0x%X (incomplete multisample)\n", status );
					break;
				case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
					assertion( false, "FrameBuffer Error: 0x%X (incomplete layer targets)\n", status );
					break;
				default:
					assertion( false, "FrameBuffer Error: 0x%X\n", status );
			}
#endif
		}

		glBindTexture( GL_TEXTURE_2D, 0 );
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}
}
//---------------------------------------
Surface::~Surface()
{
	if ( mFBO )
		glDeleteFramebuffers( 1, &mFBO );
	if ( DepthBuffer )
		glDeleteRenderbuffers( 1, &DepthBuffer );
}
//---------------------------------------
void Surface::Bind()
{
	glBindFramebuffer( GL_FRAMEBUFFER, mFBO );
	glViewport( ViewPort.x, ViewPort.y, ViewPort.width, ViewPort.height );
	SetOrthoView( 0, (float) Width, (float) Height, 0, 0.0, 1.0 );
	SetClearColor( ClearColor.r / 255.0f, ClearColor.g / 255.0f, ClearColor.b / 255.0f, ClearColor.a / 255.0f );
}
//---------------------------------------
void Surface::Clear()
{
	glClear( GL_COLOR_BUFFER_BIT | ( DepthBuffer ? GL_DEPTH_BUFFER_BIT : 0 ) );
}
//---------------------------------------