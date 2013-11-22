/*
 * Author      : Matthew Johnson
 * Date        : 16/Sep/2013
 * Description :
 *   
 */
 
#pragma once

namespace mage
{

	template< typename TState >
	class ParticleManager
	{
	public:

		// Basic particle
		class Particle
		{
		public:
			Particle()
				: mTexture( 0 )
				, mScale( 1.0f, 1.0f )
				, mRotation( 0.0f )
				, mColor( Color::WHITE )
				, mLifetime( 0.0f )
				, mLifePercent( 1.0f )
			{}

			Texture2D* mTexture;
			Vec2f mPosition;
			Vec2f mScale;
			float mRotation;
			Color mColor;
			float mLifetime;
			float mCurrentLifetime;
			float mLifePercent;
			TState mState;
		};

	private:

		// Circular buffer for storing particles
		class ParticlePool
		{
		public:
			ParticlePool( int poolSize )
				: mStart( 0 )
				, mPoolSize( poolSize )
			{
				mParticleList = new Particle*[ mPoolSize ];
				for ( int i = 0; i < mPoolSize; ++i )
					mParticleList[i] = new Particle();
			}
			~ParticlePool()
			{
				for ( int i = 0; i < mPoolSize; ++i )
					delete mParticleList[i];
				delete[] mParticleList;
			}

			inline int GetStart() const						{ return mStart; }
			inline void SetStart( int v )					{ mStart = v % mPoolSize; }
			inline void Rotate()							{ SetStart( mStart + 1 ); }

			inline int GetCapacity() const					{ return mPoolSize; }

			inline Particle*& operator[]( int i ) const		{ return mParticleList[ ( mStart + i ) % mPoolSize ]; }

		private:
			int mStart;
			int mPoolSize;
			Particle** mParticleList;
		};

	public:
		typedef void (*ParticleUpdateFn)( Particle& particle, float dt );
		//---------------------------------------
		ParticleManager( int poolSize, ParticleUpdateFn particleUpdateFn )
			: mParticleUpdateFn( particleUpdateFn )
			, mParticlePool( poolSize )
			, mParticleCount( 0 )
		{}
		//---------------------------------------
		~ParticleManager()
		{}
		//---------------------------------------
		void CreateParticle( Texture2D* texture, const Vec2f& position, const Color& color,
			float lifetime, TState state, const Vec2f& scale=Vec2f::ONE, float angle=0.0f )
		{
			int index;
			if ( mParticleCount == mParticlePool.GetCapacity() )
			{
				index = 0;
				mParticlePool.Rotate();
			}
			else
			{
				index = mParticleCount++;
			}

			Particle* particle = mParticlePool[ index ];

			particle->mTexture = texture;
			particle->mPosition = position;
			particle->mColor = color;
			particle->mLifetime = lifetime;
			particle->mCurrentLifetime = lifetime;
			particle->mLifePercent = 1.0f;
			particle->mScale = scale;
			particle->mRotation = angle;
			particle->mState = state;
		}
		//---------------------------------------
		void OnUpdate( float dt )
		{
			int removed = 0;
			for ( int i = 0; i < mParticleCount; ++i )
			{
				Particle& particle = *mParticlePool[i];

				mParticleUpdateFn( particle, dt );
				particle.mCurrentLifetime -= dt;
				particle.mLifePercent = particle.mCurrentLifetime / particle.mLifetime;

				// Move dead particles to end of list
				SwapParticle( mParticlePool, i - removed, i );

				if ( particle.mLifePercent < 0 )
					++removed;
			}
			mParticleCount -= removed;
		}
		//---------------------------------------
		void OnDraw()
		{
			for ( int i = 0; i < mParticleCount; ++i )
			{
				Particle& particle = *mParticlePool[i];

				Vec2f center = Vec2f( particle.mTexture->GetWidth() / 2.0f, particle.mTexture->GetHeight() / 2.0f );
				PushMatrix();

				Translate2D( particle.mPosition.x, particle.mPosition.y );
				Scale2D( particle.mScale.x, particle.mScale.y );
				Rotate2D( Mathf::RadiansToDegrees( particle.mRotation ) );

				DrawRect( particle.mTexture, -center.x, -center.y, particle.mColor );
				PopMatrix();
			}
		}
		//---------------------------------------

	private:
		static void SwapParticle( ParticlePool& pool, int i0, int i1 )
		{
			Particle* tmp = pool[i0];
			pool[i0] = pool[i1];
			pool[i1] = tmp;
		}

		ParticlePool mParticlePool;
		ParticleUpdateFn mParticleUpdateFn;
		int mParticleCount;
	};

	// TODO managed emitter/system setup that is data driven...
	class ParticleEmitter
	{
	public:
		void OnUpdate( float dt );
	};

	class ParticleSystem
	{
	public:
		void OnUpdate( float dt );

	private:
		std::vector< ParticleEmitter* > mEmitters;
	};

	class ParticleEmitterTemplate
	{
	public:
		ParticleEmitterTemplate( const XmlReader::XmlReaderIterator& emitter );
		ParticleEmitter* Create() const;

		Texture2D* Tex;
		float Duration;
		int Loops;
		float Delay;
		float Rate;
		float Lifetime;
		float InitScale;
		Color InitColor;
	};

	class ParticleSystemTemplate
	{
	public:
		ParticleSystemTemplate( const XmlReader::XmlReaderIterator& emitter );
		ParticleSystem* Copy() const;
	private:
		std::vector< ParticleEmitterTemplate* > mEmitters;
		static std::map< std::string, ParticleEmitterTemplate* > PSTemplates;
	};

}