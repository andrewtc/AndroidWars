#include "RendererLib.h"

// Pass through shader
static const char* vs_pass =
STRINGIFY(
	inz vec2 aPosition;										\n
	in vec4 aColor;											\n
	in vec2 aTexCoord0;										\n
	out vec2 vTexCoord0;									\n
	out vec4 vColor;										\n
	uniform mat4 uView;										\n
	void main()												\n
	{														\n
		vTexCoord0 = aTexCoord0;							\n
		vColor = aColor;									\n
		gl_Position = uView * vec4( aPosition, 0, 1 );		\n
	}
);

// Extract the brightness from an image
static const char* fs_bloomExtract =
STRINGIFY(
															\n
	uniform sampler2D uSource;								\n
	uniform float uThreshold;								\n
															\n
	in vec2 vTexCoord0;										\n
	out vec4 fFragColor;									\n
															\n
	void main()												\n
	{														\n
		vec4 c = texture2D( uSource, vTexCoord0 );			\n
		fFragColor = clamp( ( c -  uThreshold ) / ( 1 - uThreshold ), 0.0, 1.0 ); \n
	}
);

// Blur an image
static const char* fs_gaussianBlur =
STRINGIFY(
	const int SAMPLE_COUNT = 15;
	uniform sampler2D uSource;
	uniform vec2 uSampleOffsets[SAMPLE_COUNT];
	uniform float uSampleWeights[SAMPLE_COUNT];
	in vec2 vTexCoord0;
	out vec4 fFragColor;
	void main()
	{
		vec4 c = vec4( 0 );
		for ( int i = 0; i < SAMPLE_COUNT; ++i )
		{
			c += texture2D( uSource, vTexCoord0 + uSampleOffsets[i] ) * uSampleWeights[i];
		}
		fFragColor = c;
	}
);

// Bloom combine
static const char* fs_bloomCombine =
STRINGIFY(
	uniform sampler2D uSource;														\n
	uniform sampler2D uBloom;														\n
																					\n
	uniform float uBloomIntensity;													\n
	uniform float uSourceIntensity;													\n
	uniform float uBloomSaturation;													\n
	uniform float uSourceSaturation;												\n
																					\n
	in vec2 vTexCoord0;																\n
	out vec4 fFragColor;															\n
																					\n
	vec4 AdjustSaturation( vec4 color, float saturation )							\n
	{																				\n
		// Luminescence																\n
		float grey = dot( color, vec4( 0.3, 0.59, 0.11, 1 ) );						\n
		return mix( vec4( grey ), color, saturation );								\n
	}																				\n
																					\n
	void main()																		\n
	{																				\n
		vec4 bloom = texture2D( uBloom, vTexCoord0 );								\n
		vec4 source = texture2D( uSource, vTexCoord0 );								\n
																					\n
		bloom = AdjustSaturation( bloom, uBloomSaturation ) * uBloomIntensity;		\n
		source = AdjustSaturation( source, uSourceSaturation ) * uSourceIntensity;	\n
																					\n
		// Prevent burnout by darkening the source image							\n
		source *= ( 1 - clamp( bloom, 0.0, 1.0 ) );									\n
																					\n
		fFragColor = source + bloom;												\n
	}
);


using namespace mage;

BloomSettings const BloomSettings ::Presets[] =
{
	BloomSettings( "Default",        0.25f, 4.0f, 1.25f, 1.0f, 1.0f, 1.0f ),
	BloomSettings( "Soft",           0.0f,  3.0f, 2.0f,  1.0f, 1.0f, 1.0f ),
	//BloomSettings( "Desaturated",    0.5f,  8.0f, 2.0f,  1.0f, 0.0f, 1.0f ),
	//BloomSettings( "Saturated",      0.25f, 4.0f, 2.0f,  1.0f, 2.0f, 0.0f ),
	BloomSettings( "Blurry",         0.0f,  2.0f, 1.0f,  0.1f, 1.0f, 1.0f ),
	//BloomSettings( "Subtle",         0.5f,  2.0f, 1.0f,  1.0f, 1.0f, 1.0f )
};

//---------------------------------------
BloomEffect::BloomEffect()
	: mSuraceA( NULL )
	, mSuraceB( NULL )
	, Settings( BloomSettings::Presets[ BloomSettings::BLOOM_DEFAULT ] )
{}
//---------------------------------------
BloomEffect::~BloomEffect()
{
	Destroy();
}
//---------------------------------------
void BloomEffect::Initialize( int width, int height )
{
	Shader passV( Shader::ST_VERTEX, vs_pass );
	Shader bloomExtractF( Shader::ST_FRAGMENT, fs_bloomExtract );
	Shader gaussianBlurF( Shader::ST_FRAGMENT, fs_gaussianBlur );
	Shader bloomCombineF( Shader::ST_FRAGMENT, fs_bloomCombine );

	mBloomExtractEffect.SetShaders( passV, bloomExtractF );
	mBloomCombineEffect.SetShaders( passV, bloomCombineF );
	mGaussianBlurEffect.SetShaders( passV, gaussianBlurF );

	mBloomExtractEffect.AddUniform( &mBloomThreshold );
	mBloomExtractEffect.AddUniform( &mBloomExtractSource );

	mBloomCombineEffect.AddUniform( &mBloomSource );
	mBloomCombineEffect.AddUniform( &mBloomBlur );
	mBloomCombineEffect.AddUniform( &mBloomIntensity );
	mBloomCombineEffect.AddUniform( &mBloomSaturation );
	mBloomCombineEffect.AddUniform( &mSourceIntensity );
	mBloomCombineEffect.AddUniform( &mSourceSaturation );

	mGaussianBlurEffect.AddUniform( &mGaussianBlurSampleWeights );
	mGaussianBlurEffect.AddUniform( &mGaussianBlurSampleOffsets );

	mBloomThreshold.SetLocation( mBloomExtractEffect.GetUniformLocation( "uThreshold" ) );
	mBloomExtractSource.SetLocation( mBloomExtractEffect.GetUniformLocation( "uSource" ) );

	mBloomSource.SetLocation( mBloomCombineEffect.GetUniformLocation( "uSource" ) );
	mBloomBlur.SetLocation( mBloomCombineEffect.GetUniformLocation( "uBloom" ) );
	mBloomIntensity.SetLocation( mBloomCombineEffect.GetUniformLocation( "uBloomIntensity" ) );
	mBloomSaturation.SetLocation( mBloomCombineEffect.GetUniformLocation( "uBloomSaturation" ) );
	mSourceIntensity.SetLocation( mBloomCombineEffect.GetUniformLocation( "uSourceIntensity" ) );
	mSourceSaturation.SetLocation( mBloomCombineEffect.GetUniformLocation( "uSourceSaturation" ) );

	mGaussianBlurSampleWeights.SetLocation( mGaussianBlurEffect.GetUniformLocation( "uSampleWeights" ) );
	mGaussianBlurSampleOffsets.SetLocation( mGaussianBlurEffect.GetUniformLocation( "uSampleOffsets" ) );

	mGaussianBlurSampleWeights.SetValue( mSampleWeights, SAMPLE_COUNT );
	mGaussianBlurSampleOffsets.SetValue( mSampleOffets, SAMPLE_COUNT );

	mBloomBlur.SetValue( 1 );

	mExtract = new Surface( width, height, false, true );
	mSuraceA = new Surface( width, height, false, true );
	mSuraceB = new Surface( width, height, false, true );
}
//---------------------------------------
void BloomEffect::Destroy()
{
	if ( mExtract )
		delete mExtract;
	if ( mSuraceA )
		delete mSuraceA;
	if ( mSuraceB )
		delete mSuraceB;
}
//---------------------------------------
void BloomEffect::Draw( Surface* source, Surface* target )
{
	mBloomThreshold.SetValue( Settings.BloomThreshold );

	// Extract brightness from the scene
	SetActiveSurface( mExtract );
	ClearScreen();
	SetActiveEffect( &mBloomExtractEffect );
	BindTexture( source->TextureColor, 0 );
	DrawRect( 0, 0, 0, (float) mExtract->Width, (float) mExtract->Height, Color::WHITE, true );
	ClearActiveEffect();

	// Blur horizontal
	SetActiveSurface( mSuraceA );
	ClearScreen();
	SetActiveEffect( &mGaussianBlurEffect );
	SetBlurEffectParams( 1.0f / mSuraceA->Width, 0 );
	BindTexture( mExtract->TextureColor, 0 );
	DrawRect( 0, 0, 0, (float) mSuraceA->Width, (float) mSuraceA->Height, Color::WHITE, true );

	// Blur vertical
	SetActiveSurface( mSuraceB );
	ClearScreen();
	SetBlurEffectParams( 0, 1.0f / mSuraceA->Height );
	BindTexture( mSuraceA->TextureColor, 0 );
	DrawRect( 0, 0, 0, (float) mSuraceA->Width, (float) mSuraceA->Height, Color::WHITE, true );
	ClearActiveEffect();

	mBloomIntensity.SetValue( Settings.BloomIntensity );
	mBloomSaturation.SetValue( Settings.BloomSaturation );
	mSourceIntensity.SetValue( Settings.SourceIntensity );
	mSourceSaturation.SetValue( Settings.SourceSaturation );

	// Final pass
	SetActiveSurface( target );
	ClearScreen();
	SetActiveEffect( &mBloomCombineEffect );
	BindTexture( source->TextureColor, 0 );
	BindTexture( mSuraceB->TextureColor, 1 );
	DrawRect( 0, 0, 0, (float) source->Width, (float) source->Height, Color::WHITE, true );
	ClearActiveEffect();
}
//---------------------------------------
float BloomEffect::ComputeGaussian( float n ) const
{
	const float theta = Settings.BlurAmount;
	// TODO part of this equation could be cached as long as BlurAmount doesn't change often (or ever)
	return ( 1.0f / std::sqrt( 2 * Mathf::PI * theta ) ) *
		std::exp( -( n * n ) / ( 2 * theta * theta ) );
}
//---------------------------------------
void BloomEffect::SetBlurEffectParams( float dx, float dy )
{
	// First sample is always 0
	mSampleWeights[0] = ComputeGaussian( 0 );
	mSampleOffets[0] = Vec2f::ZERO;

	// Sum of all weights
	float totalWeights = mSampleWeights[0];

	for ( int i = 0; i < SAMPLE_COUNT / 2; ++i )
	{
		float weight = ComputeGaussian( i + 1.0f );

		mSampleWeights[ i * 2 + 1 ] = weight;
		mSampleWeights[ i * 2 + 2 ] = weight;

		totalWeights += weight * 2;

		float sampleOffet = i * 2 + 1.5f;
		Vec2f delta = Vec2f( dx, dy ) * sampleOffet;

		mSampleOffets[ i * 2 + 1 ] =  delta;
		mSampleOffets[ i * 2 + 2 ] = -delta;
	}

	for ( int i = 0; i < SAMPLE_COUNT; ++i )
	{
		mSampleWeights[i] /= totalWeights;
	}
}
//---------------------------------------