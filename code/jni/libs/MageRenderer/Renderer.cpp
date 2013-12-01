#include "RendererLib.h"

//using namespace mage;
namespace mage
{

static IRenderer* pRenderer;
static std::stack< Matrix4f > gMatrixStack;
static Matrix4f gCurrentMatrix;
static bool gYUp = false;
static int gPickingID;
static Font* gDefaultFont;

static Surface* gActiveSurface;

static float width;
static float height;
static float halfWidth;
static float halfHeight;

bool InitRenderer( IRenderer* renderer )
{
	pRenderer = renderer;
	if ( !pRenderer )
		return false;

	if ( !Font::InitFont() )
		return false;

	gDefaultFont = new Font( "C:/windows/fonts/arial.ttf", 14 );

	return true;
}

void DestroyRenderer()
{
	if ( pRenderer )
	{
		delete pRenderer;
		pRenderer = NULL;
	}
}

void SetTextureYUp( bool up )
{
	gYUp = up;
}

void FlushRenderer()
{
	IRenderCall( FlushRenderer() );
}

void DrawRect( float x, float y, float w, float h, const mage::Color& color )
{
	DrawRect( 0, x, y, w, h, color, false );
}

void DrawRectOutline( float x, float y, float w, float h, const mage::Color& color, float lineWidth )
{
	DrawLine( x, y, x + w, y, lineWidth, color );
	DrawLine( x + w, y, x + w, y + h, lineWidth, color );
	DrawLine( x + w, y + h, x, y + h, lineWidth, color );
	DrawLine( x, y + h, x, y, lineWidth, color );
}

void DrawRectOutlined( float x, float y, float w, float h, const mage::Color& color, float outlineWidth, const mage::Color& outlineColor )
{
	DrawRect( x, y, w, h, color );
	DrawRectOutline( x, y, w, h, outlineColor, outlineWidth );
}

void GetColorFormUInt( unsigned char rgba[4], unsigned int color )
{
	rgba[3] = (unsigned char) ( ( color              ) >> 24 );
	rgba[0] = (unsigned char) ( ( color & 0x00FF0000 ) >> 16 );
	rgba[1] = (unsigned char) ( ( color & 0x0000FF00 ) >> 8  );
	rgba[2] = (unsigned char) ( ( color & 0x000000FF )       );
}

void DrawRect( Texture2D* texture, float x, float y, float w, float h, bool yUp )
{
	DrawRect( texture, x, y, w, h, Color::WHITE, yUp );
}

void DrawRect( Texture2D* texture, float x, float y, bool yUp )
{
	DrawRect( texture, x, y, (float) texture->GetWidth(), (float) texture->GetHeight(), Color::WHITE, yUp );
}

void DrawRect( Texture2D* texture, float x, float y, const mage::Color& color )
{
	DrawRect( texture, x, y, (float) texture->GetWidth(), (float) texture->GetHeight(), color );
}

void DrawRect( Texture2D* texture, float x, float y, const mage::RectI& clip, bool yUp )
{
	if ( texture )
	{
		mage::RectF fclip;
		fclip.Left   = clip.Left   / (float) texture->GetWidth();
		fclip.Right  = clip.Right  / (float) texture->GetWidth();
		fclip.Top    = clip.Top    / (float) texture->GetHeight();
		fclip.Bottom = clip.Bottom / (float) texture->GetHeight();

		DrawRect( texture, x, y, (float) texture->GetWidth(), (float) texture->GetHeight(), fclip, 0xFFFFFFFF, yUp );
	}
	else
	{
		DrawRect( x, y, clip.Width(), clip.Height(), Color::PINK );
	}
}

void DrawRect( Texture2D* texture, float x, float y, int clipX, int clipY, int clipW, int clipH )
{
	DrawRect( texture, x, y, Color::WHITE, clipX, clipY, clipW, clipH );
}

void DrawRect( Texture2D* texture, float x, float y, const mage::Color& color, int clipX, int clipY, int clipW, int clipH )
{
	if ( texture )
	{
		mage::RectF fclip;
		fclip.Left   = clipX             / (float) texture->GetWidth();
		fclip.Right  = ( clipX + clipW ) / (float) texture->GetWidth();
		fclip.Top    = 1.0f - ( clipY             / (float) texture->GetHeight() );
		fclip.Bottom = 1.0f - ( ( clipY + clipH ) / (float) texture->GetHeight() );


		DrawRect( texture, x, y, (float) clipW, (float) clipH, fclip, color );
	}
	else
	{
		DrawRect( x, y, clipW, clipH, Color::PINK );
	}
}

void DrawRect( Texture2D* texture, float x, float y, const mage::RectF& uv, bool yUp )
{
	DrawRect( texture, x, y, (float) texture->GetWidth(), (float) texture->GetHeight(), uv, 0xFFFFFFFF, yUp );
}

void DrawRect( Texture2D* texture, float x, float y, float w, float h, const mage::RectF& fclip, const mage::Color& color, bool yUp )
{
	IRenderer::VertexList verts;

	verts.resize( 6 );

	unsigned char rgba[4] =
	{
		color.r, color.g, color.b, color.a
	};

	Vec3f v[6];
	
	v[0].Set( x, y + h, 0 );
	v[0] = gCurrentMatrix * v[0];

	v[1].Set( x, y, 0 );
	v[1] = gCurrentMatrix * v[1];

	v[2].Set( x + w, y, 0 );
	v[2] = gCurrentMatrix * v[2];

	v[3].Set( x, y + h, 0 );
	v[3] = gCurrentMatrix * v[3];

	v[4].Set( x + w, y + h, 0 );
	v[4] = gCurrentMatrix * v[4];

	v[5].Set( x + w, y, 0 );
	v[5] = gCurrentMatrix * v[5];

	for ( int i = 0; i < 6; ++i )
		memcpy( &verts[ i ], &v[ i ], 2 * sizeof( float ) );
	//verts.Verts[ 0 ].x = x;
	//verts.Verts[ 0 ].y = y + h;
	verts[ 0 ].u = fclip.Left;
	verts[ 0 ].v = yUp ? fclip.Top : fclip.Bottom;
	memcpy( verts[ 0 ].rgba, rgba, 4 );

	//verts.Verts[ 1 ].x = x;
	//verts.Verts[ 1 ].y = y;
	verts[ 1 ].u = fclip.Left;
	verts[ 1 ].v = yUp ? fclip.Bottom : fclip.Top;
	memcpy( verts[ 1 ].rgba, rgba, 4 );

	//verts.Verts[ 2 ].x = x + w;
	//verts.Verts[ 2 ].y = y;
	verts[ 2 ].u = fclip.Right;
	verts[ 2 ].v = yUp ? fclip.Bottom : fclip.Top;
	memcpy( verts[ 2 ].rgba, rgba, 4 );

	//verts.Verts[ 3 ].x = x;
	//verts.Verts[ 3 ].y = y + h;
	verts[ 3 ].u = fclip.Left;
	verts[ 3 ].v = yUp ? fclip.Top : fclip.Bottom;
	memcpy( verts[ 3 ].rgba, rgba, 4 );

	//verts.Verts[ 4 ].x = x + w;
	//verts.Verts[ 4 ].y = y + h;
	verts[ 4 ].u = fclip.Right;
	verts[ 4 ].v = yUp ? fclip.Top : fclip.Bottom;
	memcpy( verts[ 4 ].rgba, rgba, 4 );

	//verts.Verts[ 5 ].x = x + w;
	//verts.Verts[ 5 ].y = y;
	verts[ 5 ].u = fclip.Right;
	verts[ 5 ].v = yUp ? fclip.Bottom : fclip.Top;
	memcpy( verts[ 5 ].rgba, rgba, 4 );

	IRenderer::TextureHandle hTexture = 0;
	if ( texture )
		hTexture = texture->GetId();

	IRenderCall( RenderVerticies( IRenderer::Triangles, hTexture, verts ) );
}

void DrawRect( Texture2D* texture, float x, float y, float w, float h, const mage::Color& color, bool yUp )
{
	DrawRect( texture, x, y, w, h, mage::RectF( 0, 0, 1, 1 ), color, yUp );
}

void DrawLine( const mage::Vec2f& A, const mage::Vec2f& B, float width, const mage::Color& color )
{
	DrawLine( A.x, A.y, B.x, B.y, width, color.bgra );
}

void DrawLine( float x0, float y0, float x1, float y1, float width, const mage::Color& color )
{
	DrawGradientLine( x0, y0, x1, y1, width, color, color );
}

void DrawGradientLine( float x0, float y0, float x1, float y1, float width, const mage::Color& colorStart, const mage::Color& colorEnd )
{
	IRenderer::VertexList verts;
#ifdef TRIANGLE_LINES
	const float halfWidth = width / 2.0f;
	Vec3f p1( x0, y0, 0 );
	Vec3f p2( x1, y1, 0 );
	Vec3f dir = p2 - p1;

	dir.Normalize();

	Vec3f vperp( -dir.y, dir.x, 0 );

	Vec3f v[4];

	v[ 0 ] = gCurrentMatrix * ( p1 + halfWidth * vperp );
	v[ 1 ] = gCurrentMatrix * ( p1 - halfWidth * vperp );
	v[ 2 ] = gCurrentMatrix * ( p2 + halfWidth * vperp );
	v[ 3 ] = gCurrentMatrix * ( p2 - halfWidth * vperp );

	unsigned char rgbaBegin[4];
	unsigned char rgbaEnd[4];
	GetColorFormUInt( rgbaBegin, colorStart.bgra );
	GetColorFormUInt( rgbaEnd, colorEnd.bgra );

	verts.Verts[ 0 ].x = v[ 0 ].x;
	verts.Verts[ 0 ].y = v[ 0 ].y;
	memcpy( verts.Verts[ 0 ].rgba, rgbaBegin, 4 );

	verts.Verts[ 1 ].x = v[ 1 ].x;
	verts.Verts[ 1 ].y = v[ 1 ].y;
	memcpy( verts.Verts[ 1 ].rgba, rgbaBegin, 4 );

	verts.Verts[ 2 ].x = v[ 2 ].x;
	verts.Verts[ 2 ].y = v[ 2 ].y;
	memcpy( verts.Verts[ 2 ].rgba, rgbaEnd, 4 );

	verts.Verts[ 3 ].x = v[ 2 ].x;
	verts.Verts[ 3 ].y = v[ 2 ].y;
	memcpy( verts.Verts[ 3 ].rgba, rgbaEnd, 4 );

	verts.Verts[ 4 ].x = v[ 3 ].x;
	verts.Verts[ 4 ].y = v[ 3 ].y;
	memcpy( verts.Verts[ 4 ].rgba, rgbaEnd, 4 );

	verts.Verts[ 5 ].x = v[ 1 ].x;
	verts.Verts[ 5 ].y = v[ 1 ].y;
	memcpy( verts.Verts[ 5 ].rgba, rgbaBegin, 4 );

	verts.NumVerts = 6;

	IRenderCall( RenderVerticies( IRenderer::Triangles, 0, verts ) );
#else
	verts.resize( 2 );
	unsigned char rgbaBegin[4];
	unsigned char rgbaEnd[4];

	GetColorFormUInt( rgbaBegin, colorStart.bgra );
	GetColorFormUInt( rgbaEnd, colorEnd.bgra );

	Vec3f p1( x0, y0, 0 );
	Vec3f p2( x1, y1, 0 );
	Vec3f v[2];

	v[ 0 ] = gCurrentMatrix * p1;
	v[ 1 ] = gCurrentMatrix * p2;

	verts[ 0 ].x = v[0].x;
	verts[ 0 ].y = v[0].y;
	memcpy( verts[ 0 ].rgba, rgbaBegin, 4 );

	verts[ 1 ].x = v[1].x;
	verts[ 1 ].y = v[1].y;
	memcpy( verts[ 1 ].rgba, rgbaEnd, 4 );

	IRenderCall( RenderVerticies( IRenderer::Lines, 0, verts ) );
#endif
}

void DrawAxes( const Matrix3f& tm, float len )
{
	Vec2f x1, x2;
	Vec2f y1, y2;

	x1 = tm * Vec2f::UNIT_X;
	x2 = tm * (len * Vec2f::UNIT_X);
	y1 = tm * Vec2f::UNIT_Y;
	y2 = tm * (len * Vec2f::UNIT_Y);

	DrawLine( x1, x2, 2.0f, Color::RED );
	DrawLine( y1, y2, 2.0f, Color::GREEN );
}

void DrawAxes( float len )
{
	Vec2f x1, x2;
	Vec2f y1, y2;

	x1 = Vec2f::UNIT_X;
	x2 = (len * Vec2f::UNIT_X);
	y1 = Vec2f::UNIT_Y;
	y2 = (len * Vec2f::UNIT_Y);

	DrawLine( x1, x2, 2.0f, Color::RED );
	DrawLine( y1, y2, 2.0f, Color::GREEN );
}

void DrawCurve( const mage::Vec2f& A, const mage::Vec2f& B, const mage::Vec2f& C, const mage::Vec2f& D, float width, const mage::Color& color )
{
	int segments = 25;
	Vec2f points[25];
	float dt = 1.0f / segments;
	float t = 0;
	for ( int i = 0; i < segments; ++i, t += dt )
	{
		float a = 1.0f - t;
		float b = a * a;
		float c = a * b;
		float t2 = t * t;
		float t3 = t * t2;
		points[i] = c * A + 3.0f * t * b * B + 3.0f * t2 * a * C + t3 * D;
	}
	DrawPolyLine( (float*)points, segments, width, color, false );
	DrawPoints( (float*)points, segments, 4.0f, Color::RED );
	DrawPoint( A.x, A.y, 4.0f, Color::BLUE );
	DrawPoint( B.x, B.y, 4.0f, Color::BLUE );
	DrawPoint( C.x, C.y, 4.0f, Color::BLUE );
	DrawPoint( D.x, D.y, 4.0f, Color::BLUE );
}

void DrawThickCurve( const mage::ArrayList< Vec2f >& points, float radius, bool cap )
{
	Vec2f p0, p1, p2, delta;
	Vec2f v0, v1, v2, v3;
	float dist;

	if ( points.empty() )
	{
		return;
	}

	p0 = points[0];

	if ( points.size() == 1 )
	{
		return;
	}

	p1 = points[1];

	delta = p1 - p0;
	delta.Normalize();

	// if cap

	v0 = p0 + radius * delta;
	v1 = p0 - radius * delta;

	if ( points.size() >= 2 )
	{
		v2 = p1 + radius * delta;
		v3 = p1 - radius * delta;

		DrawTriangle( v2, p1, v0, Color::RED );
		DrawTriangle( p1, v3, v0, 0xFFFFFF00 );
		DrawTriangle( v0, p0, v3, Color::BLUE );
		DrawTriangle( p0, v1, v3, 0xFF00FFFF );

		v0 = v2;
		v1 = v3;
	}

	for ( size_t i = 2; i < points.size() - 1; ++i )
	{
		p0 = points[ i - 1 ];
		p1 = points[ i     ];
		p2 = points[ i + 1 ];

		delta = p1 - p0;
		delta.Normalize();

		v2 = p1 + radius * delta;
		v3 = p1 - radius * delta;

		DrawTriangle( v2, v3, v0, Color::RED );
		DrawTriangle( v0, v1, v3, Color::BLUE );

		v0 = v2;
		v1 = v3;
	}

	p0 = points[ points.size() - 2 ];
	p1 = points[ points.size() - 1 ];

	delta = p1 - p0;
	delta.Normalize();

	v2 = p1 + radius * delta;
	v3 = p1 - radius * delta;

	DrawTriangle( v2, p1, v0, Color::RED );
	DrawTriangle( p1, v3, v0, 0xFFFFFF00 );
	DrawTriangle( v0, v3, v1, Color::BLUE );
}

void DrawTriangle( const mage::Vec2f& A, const mage::Vec2f& B, const mage::Vec2f& C, const mage::Color& color )
{
	IRenderer::VertexList verts;
	verts.resize( 3 );
	unsigned char rgba[4];
	GetColorFormUInt( rgba, color.bgra );

	Vec2f v1 = B - A;
	Vec2f v2 = C - A;
	float d = v1.x * v2.y - v1.y * v2.x;

	if ( d >= 0 )
	{
		verts[ 0 ].x = A.x;
		verts[ 0 ].y = A.y;
		memcpy( verts[ 0 ].rgba, rgba, 4 );

		verts[ 1 ].x = B.x;
		verts[ 1 ].y = B.y;
		memcpy( verts[ 1 ].rgba, rgba, 4 );

		verts[ 2 ].x = C.x;
		verts[ 2 ].y = C.y;
		memcpy( verts[ 2 ].rgba, rgba, 4 );
	}
	else
	{
		verts[ 0 ].x = A.x;
		verts[ 0 ].y = A.y;
		memcpy( verts[ 0 ].rgba, rgba, 4 );

		verts[ 1 ].x = C.x;
		verts[ 1 ].y = C.y;
		memcpy( verts[ 1 ].rgba, rgba, 4 );

		verts[ 2 ].x = B.x;
		verts[ 2 ].y = B.y;
		memcpy( verts[ 2 ].rgba, rgba, 4 );
	}

	IRenderCall( RenderVerticies( IRenderer::Triangles, 0, verts ) );
}

void DrawPoint( float x, float y, float pointSize, const mage::Color& color )
{
	IRenderer::VertexList verts;
	verts.resize( 1 );
	unsigned char rgba[4];
	GetColorFormUInt( rgba, color.bgra );

	Vec3f v( x, y, 0 );
	v = gCurrentMatrix * v;

	verts[ 0 ].x = v.x;
	verts[ 0 ].y = v.y;
	verts[ 0 ].pointsize = pointSize;
	memcpy( verts[ 0 ].rgba, rgba, 4 );

	IRenderCall( RenderVerticies( IRenderer::Points, 0, verts ) );
}

void DrawPoint( const mage::Vec2f& pos, float pointSize, const mage::Color& color )
{
	DrawPoint( pos.x, pos.y, pointSize, color );
}

void DrawPoints( float* points, unsigned int nPoints, float pointSize, const mage::Color& color )
{
	IRenderer::VertexList verts;
	unsigned char rgba[4];
	GetColorFormUInt( rgba, color.bgra );

	const size_t N = nPoints * 2;
	verts.resize( nPoints );
	for ( size_t i = 0, j = 0; i < N; i += 2, ++j )
	{
		Vec3f v( points[ i ], points[ i + 1 ], 0 );
		v = gCurrentMatrix * v;
		verts[ j ].x = v.x;
		verts[ j ].y = v.y;
		verts[ j ].pointsize = pointSize;
		memcpy( verts[ j ].rgba, rgba, 4 );
	}

	IRenderCall( RenderVerticies( IRenderer::Points, 0, verts ) );
}

void DrawPolyLine( float* points, size_t nPoints, float width, const Color& color, bool loop )
{
	const size_t N = 2 * nPoints;
	for ( size_t i = 0; i < N; i+=2 )
	{
		if ( i + 2 < N )
		{
			DrawLine( points[i], points[i+1], points[i+2], points[i+3], width, color );
		}
	}

	// Draw edge between first and last point
	if ( loop && nPoints > 2 )
	{
		DrawLine( points[N-2], points[N-1], points[0], points[1], width, color );
	}
}

void DrawPolygon( float* points, size_t nPoints, unsigned int color )
{
	//pRenderer->DrawPolygon( points, nPoints, color );
}

void DrawPolygonOutlined( float* points, size_t nPoints, unsigned int color, float outlineWidth, unsigned int outlineColor )
{
	DrawPolygon( points, nPoints, color );
	DrawPolyLine( points, nPoints, outlineWidth, outlineColor, true );
}

void DrawCircle( float cx, float cy, float r, const mage::Color& color )
{
	const int segments = 20;
	const float inc = Mathf::TWO_PI / segments;
	float theta = 0;
	std::vector< float > polyLine;

	for ( int i = 0; i < segments; ++i )
	{
		theta += inc;
		float px = std::cos( theta ) * r + cx;
		float py = std::sin( theta ) * r + cy;
		polyLine.push_back( px );
		polyLine.push_back( py );
	}
	DrawPolyLine( &polyLine[0], polyLine.size() / 2, 1.0f, color.bgra, true );
}

void DrawPartialCircle( float cx, float cy, float r, float minAngle, float maxAngle, const mage::Color& color )
{
	const int segments = 20;
	const float inc = maxAngle / segments;
	float theta = minAngle;
	std::vector< float > polyLine;

	for ( int i = 0; i < segments; ++i )
	{
		theta += inc;
		float px = std::cos( theta ) * r + cx;
		float py = std::sin( theta ) * r + cy;
		polyLine.push_back( px );
		polyLine.push_back( py );
	}
	DrawPolyLine( &polyLine[0], polyLine.size() / 2, 1.0f, color.bgra, true );
}

void DrawText( float x, float y, mage::BitmapFont* bmpFont, const mage::Color& color, float scale, int maxLineLength, const char* text )
{
	if ( bmpFont )
		bmpFont->RenderText( x, y, text, color, scale, maxLineLength );
}

void DrawText( float x, float y, mage::BitmapFont* bmpFont, const mage::Color& color, float scale, const char* text )
{
	DrawText( x, y, bmpFont, color, scale, -1, text );
}

void DrawText( float x, float y, mage::BitmapFont* bmpFont, const mage::Color& color, const char* text )
{
	DrawText( x, y, bmpFont, color, 1.0f, -1, text );
}

void DrawText( float x, float y, mage::BitmapFont* bmpFont, const char* text )
{
	DrawText( x, y, bmpFont, Color::WHITE, 1.0f, -1, text );
}

void DrawTextFormat( float x, float y, mage::BitmapFont* bmpFont, const mage::Color& color, float scale, int maxLineLength, const char* text, ... )
{
	char textFormatBuffer[ 1024 ];

	// Apply text formating
	va_list vargs;
	va_start( vargs, text );
	vsprintf_s( textFormatBuffer, text, vargs );
	va_end( vargs );

	DrawText( x, y, bmpFont, color, scale, maxLineLength, textFormatBuffer );
}

void DrawTextFormat( float x, float y, mage::BitmapFont* bmpFont, const mage::Color& color, float scale, const char* text, ... )
{
	char textFormatBuffer[ 1024 ];

	// Apply text formating
	va_list vargs;
	va_start( vargs, text );
	vsprintf_s( textFormatBuffer, text, vargs );
	va_end( vargs );

	DrawText( x, y, bmpFont, color, scale, textFormatBuffer );
}

void DrawTextFormat( float x, float y, mage::BitmapFont* bmpFont, const mage::Color& color, const char* text, ... )
{
	char textFormatBuffer[ 1024 ];

	// Apply text formating
	va_list vargs;
	va_start( vargs, text );
	vsprintf_s( textFormatBuffer, text, vargs );
	va_end( vargs );

	DrawText( x, y, bmpFont, color, textFormatBuffer );
}

void DrawTextFormat( float x, float y, mage::BitmapFont* bmpFont, const char* text, ... )
{
	char textFormatBuffer[ 1024 ];

	// Apply text formating
	va_list vargs;
	va_start( vargs, text );
	vsprintf_s( textFormatBuffer, text, vargs );
	va_end( vargs );

	DrawText( x, y, bmpFont, textFormatBuffer );
}

void SetViewMatrix( const float* view )
{
	IRenderCall( SetViewMatrix( view ) );
}

void ClearScreen()
{
	if ( gActiveSurface )
		gActiveSurface->Clear();
	else
		IRenderCall( ClearScreen() );
}

void CreateTexture( IRenderer::TextureHandle* hTexture, void* pixels, unsigned int w, unsigned int h, IRenderer::PixelFormat format, bool linearFilter )
{
	IRenderCall( CreateTexture( hTexture, pixels, w, h, format, linearFilter ) );
}

void FreeTexture( IRenderer::TextureHandle* hTexture )
{
	// Render should cleanup textures when it is destroyed
	// Textures may be freed at other times through this function
	if ( pRenderer )
		IRenderCall( FreeTexture( hTexture ) );
}

void SetClearColor( float r, float g, float b, float a )
{
	IRenderCall( SetClearColor( r, g, b, a ) );
}

void SetViewport( int x, int y, int w, int h )
{
	IRenderCall( SetViewport( x, y, w, h ) );
}

void SetOrthoView( float left, float right, float bottom, float top, float zNear, float zFar )
{
	width = left + right;
	height = top + bottom;
	halfWidth = width / 2.0f;
	halfHeight = height / 2.0f;

	Matrix4f view;
	view.MakeOrtho( left, right, top, bottom, zNear, zFar );
	SetViewMatrix( view.GetArray() );
}

void PushMatrix()
{
	gMatrixStack.push( gCurrentMatrix );
}

void PopMatrix()
{
	if ( !gMatrixStack.empty() )
	{
		gCurrentMatrix = gMatrixStack.top();
		gMatrixStack.pop();
	}
}

void LoadIdentity()
{
	gCurrentMatrix.Identity();
}

void LoadMatrix( float* m )
{
	gCurrentMatrix.Set( m );
}

void LoadTransform2D( const Transform2D& transform2D )
{
	const Matrix3f& mat = transform2D.GetMatrix();
	gCurrentMatrix = Matrix4f(
		mat[0][0], mat[0][1], 0, 0,
		mat[1][0], mat[1][1], 0, 0,
		0, 0, 0, 0,
		mat[0][2], mat[1][2], 0, 1 );
}

void Translate( float x, float y, float z )
{
	Matrix4f m;//( gCurrentMatrix );
	m.Translate( Vec3f( x, y, z ) );
	gCurrentMatrix = m * gCurrentMatrix;
	//gCurrentMatrix *= m;
}

void Rotate( float angleDeg, float x, float y, float z )
{
	//Matrix4f m;( gCurrentMatrix );
	//m.Rotate( angleDeg, Vec3f( x, y, z ) );
	//gCurrentMatrix = m * gCurrentMatrix;

	Quatf q;
	q.FromAxisAngle( Vec3f( x, y, z ), Mathf::DegreesToRadians( angleDeg ) );
	Matrix4f R;
	R.RotationFromQuat( q );
	gCurrentMatrix = R * gCurrentMatrix;

	//gCurrentMatrix *= m;
}

void Scale( float x, float y, float z )
{
	Matrix4f m;//( gCurrentMatrix );
	m.Scale( Vec3f( x, y, z ) );
	gCurrentMatrix = m * gCurrentMatrix;
	//gCurrentMatrix *= m;
}

void ScaleUniform( float scale )
{
	Scale( scale, scale, scale );
}

void Translate2D( const Vec2f& trans )
{
	Translate2D( trans.x, trans.y );
}

void Translate2D( float x, float y )
{
	Translate( x, y, 0.0f );
}

void Rotate2D( float angleDeg )
{
	Rotate( angleDeg, 0.0f, 0.0f, 1.0f );
}

void Scale2D( float x, float y )
{
	Scale( x, y, 1.0f );
}

void ScaleUniform2D( float scale )
{
	Scale2D( scale, scale );
}

void SetActiveEffect( Effect* effect )
{
	IRenderCall( SetActiveEffect( effect ) );
}

void ClearActiveEffect()
{
	IRenderCall( ClearActiveEffect() );
}

void BindTexture( Texture2D* texture, int channel )
{
	IRenderer::TextureHandle hTexture = texture->GetId();
	IRenderCall( BindTexture( hTexture, channel ) );
}

void SetActiveSurface( Surface* surface )
{
	// Flush if we are switching draw surfaces
	if ( gActiveSurface != surface )
	{
		FlushRenderer();
	}
	gActiveSurface = surface;
	gActiveSurface->Bind();
}

void SetPickingID( int id )
{
	gPickingID = id;
}

void ToWorldSpace( Vec2f& pos )
{
	pos.x += halfWidth;
	pos.y = halfHeight - pos.y;
}

}
