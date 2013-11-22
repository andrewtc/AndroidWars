/*
 * Author      : Matthew Johnson
 * Date        : 11/Oct/2013
 * Description :
 *   
 */
 
#pragma once

namespace mage
{

	class ControlledObject;

	class Controller
		: public Object
	{
		DECLARE_RTTI;
	protected:
		friend class ControlledObject;

		Controller();
		Controller( const std::string& name );
	public:
		virtual ~Controller();

		ControlledObject* GetObject() const			{ return mObject; }

		virtual bool OnUpdate( float currentTime ) = 0;

	protected:
		void Bind( ControlledObject* obj )			{ mObject = obj; }
		
		ControlledObject* mObject;
	};

}