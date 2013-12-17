#pragma once

class IRenderer
{
public:
	virtual ~IRenderer() {}
	enum RenderMode
	{
		None,			// No primitives should be rendered
		Triangles,
		Lines,
		Points
	};

	enum BlendFunc
	{
		BF_ZERO,
		BF_ONE,
		BF_SRC_COLOR,
		BF_ONE_MINUS_SRC_COLOR,
		BF_DST_COLOR,
		BF_ONE_MINUS_DST_COLOR,
		BF_SRC_ALPHA,
		BF_ONE_MINUS_SRC_ALPHA,
		BF_DST_ALPHA,
		BF_ONE_MINUS_DST_ALPHA,
		BF_CONSTANT_COLOR,
		BF_ONE_MINUS_CONSTANT_COLOR,
		BF_CONSTANT_ALPHA,
		BF_ONE_MINUS_CONSTANT_ALPHA,
		BF_SRC_ALPHA_SATURATE,
		BF_SRC1_COLOR,
		BF_ONE_MINUS_SRC1_COLOR,
		BF_SRC1_ALPHA,
		BF_ONE_MINUS_SRC1_ALPHA
	};

	struct Vertex2D
	{
		float x, y;
		float u, v;
		unsigned char rgba[4];
		int pickingID;
		float pointsize;
		float padding;		// Make vertex multiple of 32
	};

	typedef std::vector< Vertex2D > VertexList;
	/*struct VertexList
	{
		static const size_t MAX_VLIST_SIZE = 128;
		Vertex2D Verts[ MAX_VLIST_SIZE ];
		size_t NumVerts;
	};*/

	typedef unsigned int TextureHandle;
	enum PixelFormat
	{
		PF_RGB,
		PF_RGBA,
		PF_BGR,
		PF_BGRA,
		PF_LUMINANCE,
		PF_LUMINANCE_ALPHA
	};

	virtual void CreateTexture( IRenderer::TextureHandle* hTexture, void* pixels, unsigned int w, unsigned int h, IRenderer::PixelFormat format, bool linearFilter=true ) = 0;
	virtual void FreeTexture( IRenderer::TextureHandle* hTexture ) = 0;

	// This call is a draw request. Drawing may be delayed until FlushRenderer() is called.
	virtual void RenderVerticies( RenderMode mode, IRenderer::TextureHandle texture, const VertexList& verts ) = 0;
	// Called when drawing must occur.
	virtual void FlushRenderer() = 0;
	// Set the effect program to be used when drawing.
	virtual void SetActiveEffect( mage::Effect* effect ) = 0;
	// Clear the active effect program and use the default one
	virtual void ClearActiveEffect() = 0;
	// Set the current view
	virtual void SetViewMatrix( const float* view ) = 0;
	// Clear the screen to the clear color
	virtual void ClearScreen() = 0;
	// Set the clear color
	virtual void SetClearColor( float r, float g, float b, float a ) = 0;
	virtual void SetViewport( int x, int y, int w, int h ) = 0;
	// Bind a texture to a specific channel
	virtual void BindTexture( IRenderer::TextureHandle hTexture, int channel ) = 0;
	// Set how pixels are blended
	virtual void SetBlendFunc( IRenderer::BlendFunc sFactor, IRenderer::BlendFunc dFactor ) = 0;
};
