#pragma once

#include "IRenderer.h"

namespace mage
{

	class GLRenderer
		: public IRenderer
	{
	public:
		GLRenderer();
		virtual ~GLRenderer();

		void RenderVerticies( RenderMode mode, IRenderer::TextureHandle texture, const VertexList& verts );
		void FlushRenderer();
		void SetViewMatrix( const float* view );
		void ClearScreen();
		void SetClearColor( float r, float g, float b, float a );
		void SetViewport( int x, int y, int w, int h );
		void CreateTexture( TextureHandle* hTexture, void* pixels, unsigned int w, unsigned int h, PixelFormat format, bool linearFilter=true );
		void FreeTexture( TextureHandle* hTexture );

		void SetActiveEffect( Effect* effect );
		void ClearActiveEffect();
		void BindTexture( IRenderer::TextureHandle hTexture, int channel );
		void SetBlendFunc( IRenderer::BlendFunc sFactor, IRenderer::BlendFunc dFactor );

	protected:
		void CopyVertexListToBuffer( const VertexList& verts );
	
		static const size_t MAX_VERTEX_BATCH = 1024;
		Vertex2D mVertexBuffer[ MAX_VERTEX_BATCH ];
		IRenderer::TextureHandle mCurrentTexture;		// Texture to bind for current batch
		IRenderer::TextureHandle mActiveTexture;		// Texture currently bound
		RenderMode mCurrentRenderMode;					// Render mode to use for current batch
		size_t mCurrentBufferCount;						// Vertex Batch usage

		// Experimental
		//ShaderProgram* mCurrentProgram;
		float mView[16];
		Effect* mActiveEffect;

		// List of all the textures
		std::list< IRenderer::TextureHandle > mTextures;
	};

}
