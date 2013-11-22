/*
 * Author      : Matthew Johnson
 * Date        : 17/Sep/2013
 * Description :
 *   
 */
 
#pragma once

namespace mage
{

	struct BloomSettings
	{
		BloomSettings( const std::string& name, float bloomThreshold, float blurAmount,
					   float bloomIntensity, float sourceIntensity,
					   float bloomSaturation, float sourceStaturation )
					   : Name( name )
					   , BloomThreshold( bloomThreshold )
					   , BlurAmount( blurAmount )
					   , BloomIntensity( bloomIntensity )
					   , SourceIntensity( sourceIntensity )
					   , BloomSaturation( bloomSaturation )
					   , SourceSaturation( sourceStaturation )

		{}

		std::string Name;
		float BloomThreshold;
		float BlurAmount;
		float BloomIntensity;
		float SourceIntensity;
		float BloomSaturation;
		float SourceSaturation;

		enum BloomSettingsPreset
		{
			BLOOM_DEFAULT,
			BLOOM_SOFT,
			//BLOOM_DESATURATED,
			//BLOOM_SATURATED,
			BLOOM_BLURRY,
			//BLOOM_SUBTLE,
			BLOOM_COUNT
		};

		static BloomSettings const Presets[];
	};

	class BloomEffect
	{
	public:
		BloomEffect();
		~BloomEffect();

		void Initialize( int width, int height );
		void Destroy();

		// Applies a bloom post-process effect to the source surface
		// and renders it to the target surface
		void Draw( Surface* source, Surface* target );

		BloomSettings Settings;

	private:
		float ComputeGaussian( float n ) const;
		void SetBlurEffectParams( float dx, float dy );

		// Must be same as in shader
		static const int SAMPLE_COUNT = 15;

		Effect mBloomExtractEffect;
		Effect mBloomCombineEffect;
		Effect mGaussianBlurEffect;

		Uniform1f mBloomThreshold;
		Uniform1i mBloomExtractSource;

		Uniform1fv mGaussianBlurSampleWeights;
		Uniform2fv mGaussianBlurSampleOffsets;
		float mSampleWeights[SAMPLE_COUNT];
		Vec2f mSampleOffets[SAMPLE_COUNT];

		Uniform1i mBloomSource;
		Uniform1i mBloomBlur;
		Uniform1f mBloomIntensity;
		Uniform1f mBloomSaturation;
		Uniform1f mSourceIntensity;
		Uniform1f mSourceSaturation;

	public:
		Surface* mExtract;
		Surface* mSuraceA;
		Surface* mSuraceB;
	};

}