#include "CoreLib.h"

using namespace mage;

//---------------------------------------
// Constant Color Definitions
//---------------------------------------
const Color Color::RED           ( 0xFFFF0000 );
const Color Color::GREEN         ( 0xFF00FF00 );
const Color Color::BLUE          ( 0xFF0000FF );
const Color Color::DARK_RED      ( 0xFF800000 );
const Color Color::DARK_GREEN    ( 0xFF008000 );
const Color Color::DARK_BLUE     ( 0xFF0000A0 );
const Color Color::LIGHT_BLUE    ( 0xFFADD8E6 );
const Color Color::YELLOW        ( 0xFFFFFF00 );
const Color Color::DARK_YELLOW   ( 0xFF808000 );
const Color Color::CYAN          ( 0xFF00FFFF );
const Color Color::ORANGE        ( 0xFFFFA500 );
const Color Color::PURPLE        ( 0xFF800080 );
const Color Color::PINK          ( 0xFFFF00FF );
const Color Color::GREY          ( 0xFF808080 );
const Color Color::LIGHT_GREY    ( 0xFFE0E0E0 );
const Color Color::WHITE         ( 0xFFFFFFFF );
const Color Color::BLACK         ( 0xFF000000 );
const Color Color::TRANSPARENCY  ( 0x00000000 );

//---------------------------------------
Color::Color()
	: bgra( 0 )
{}
//---------------------------------------
Color::Color( uint32 argb )
	: bgra( argb )
{}
//---------------------------------------
Color::Color( uint8 a, uint8 r, uint8 g, uint8 b )
	: a( a )
	, r( r )
	, g( g )
	, b( b )
{}
//---------------------------------------
Color::Color( float r, float g, float b, float a )
	: a( (uint8) ( a * 255 ) )
	, r( (uint8) ( r * 255 ) )
	, g( (uint8) ( g * 255 ) )
	, b( (uint8) ( b * 255 ) )
{}
//---------------------------------------
Color Color::ColorFromFloat3( float rgb[3] )
{
	return Color( rgb[0], rgb[1], rgb[2] );
}
//---------------------------------------
void Color::ColorToFloat3( const Color& color, float rgb[3] )
{
	rgb[0] = color.r / 255.0f;
	rgb[1] = color.g / 255.0f;
	rgb[2] = color.b / 255.0f;
}
//---------------------------------------
Color Color::HSVToColor( float h, float s, float v )
{
	if ( h == 0 && s == 0 )
		return Color( v, v, v );

	float c = s * v;
	float x = c * ( 1 - std::fabs( std::fmod( h, 2.0f ) - 1 ) );
	float m = v - c;

	if ( h < 1 )
		return Color( c + m, x + m, m );
	if ( h < 2 )
		return Color( x + m, c + m, m );
	if ( h < 3 )
		return Color( m, c + m, x + m );
	if ( h < 4 )
		return Color( m, x + m, c + m );
	if ( h < 5 )
		return Color( x + m, m , c + m );
	return Color( c + m, m, x + m );
}
//---------------------------------------
// Adapted from:
// http://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb
HSV Color::ColorToHSV( const Color& color )
{
	HSV hsv;
	float rgb[3];
	float min, max, delta;

	ColorToFloat3( color, rgb );

	min = rgb[0] < rgb[1] ? rgb[0] : rgb[1];
	min = min < rgb[2] ? min : rgb[2];

	max = rgb[0] > rgb[1] ? rgb[0] : rgb[1];
	max = max > rgb[2] ? max : rgb[2];

	hsv.value = max;
	delta = max - min;
	if ( max > 0.0f )
	{
		hsv.saturation = ( delta / max );
	}
	else
	{
		hsv.saturation = 0.0f;
		hsv.hue = 0.0f;
		return hsv;
	}

	if ( rgb[0] >= max )
	{
		hsv.hue = ( rgb[1] - rgb[2] ) / delta;
	}
	else
	{
		if ( rgb[1] >= max )
		{
			hsv.hue = 2.0f + ( rgb[2] - rgb[0] ) / delta;
		}
		else
		{
			hsv.hue = 4.0f + ( rgb[0] - rgb[1] ) / delta;
		}
	}

	hsv.hue *= 60.0f;

	if ( hsv.hue < 0.0f )
		hsv.hue += 360.0f;
	return hsv;
}
//---------------------------------------
Color Color::Lerp( const Color& A, const Color& B, float alpha )
{
	const float a = 1.0f - alpha;
	Color c;
	c.r = (uint8) ( A.r * a + B.r * alpha );
	c.b = (uint8) ( A.g * a + B.g * alpha );
	c.g = (uint8) ( A.b * a + B.b * alpha );
	c.a = (uint8) ( A.a * a + B.a * alpha );
	return c;
}
//---------------------------------------