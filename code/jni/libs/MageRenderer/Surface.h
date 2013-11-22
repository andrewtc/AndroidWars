/*
 * Author      : Matthew Johnson
 * Date        : 15/Sep/2013
 * Description :
 *   
 */
 
#pragma once

namespace mage
{

	class Surface
	{
	public:
		// Set isSceneTarget to true to create a Surface that renders to the backbuffer
		Surface( uint32 width, uint32 hieght, bool useDepth, bool linear, bool isSceneTarget=false );
		~Surface();

		void Bind();
		void Clear();

		struct ViewPortRect
		{
			int x, y;
			uint32 width, height;
		} ViewPort;

		const uint32 Width;
		const uint32 Height;

		Color ClearColor;
		//Matrix4f ModelView;
		//Matrix4f Projection;

		Texture2D* TextureColor;
		uint32 DepthBuffer;

	private:
		uint32 mFBO;
	};

}