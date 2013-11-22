/*
 * Author      : Matthew Johnson
 * Date        : 15/Sep/2013
 * Description :
 *   
 */
 
#pragma once

namespace mage
{

	class Uniform;
	class Effect
	{
	public:
		static const int INVALID_LOCATION = 0xFFFFFFFF;

		Effect();
		Effect( const Shader& vs, const Shader& fs );
		~Effect();

		void SetShaders( const Shader& vs, const Shader& fs );

		void Apply() const;

		int GetUniformLocation( const char* name ) const;
		int GetAttributeLocation( const char* name ) const;

		void AddUniform( Uniform* uniform );

	private:
        void DebugErrorLog();
		uint32 mProgramId;
		std::vector< Uniform* > mUniforms;
	};

}