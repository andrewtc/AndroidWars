/*
 * Author      : Matthew Johnson
 * Date        : 15/Oct/2013
 * Description :
 *   
 */
 
#pragma once

namespace mage
{

	class CameraNode
		: public Node
	{
		DECLARE_RTTI;
	public:
		CameraNode( Camera* camera );
		virtual ~CameraNode();

		void OnUpdate( float currentTime );

	private:
		Camera* mCamera;
	};

}