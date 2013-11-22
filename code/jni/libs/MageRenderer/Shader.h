/*
 * Author      : Matthew Johnson
 * Date        : 15/Sep/2013
 * Description :
 *   
 */
 
#pragma once

namespace mage
{

	class Shader
	{
	public:
		enum ShaderType
		{
			ST_VERTEX,
			ST_FRAGMENT,
			ST_COUNT		// ShaderType Count
		};

		Shader( ShaderType type, const char* source );
		~Shader();

		uint32 GetShaderId() const;

	private:
		uint32 mShaderId;
	};

}