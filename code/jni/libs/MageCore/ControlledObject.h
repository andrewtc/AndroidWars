/*
 * Author      : Matthew Johnson
 * Date        : 11/Oct/2013
 * Description :
 *   An object that can be controlled through one or more Controllers.
 */
 
#pragma once

namespace mage
{

	class ControlledObject
		: public Object
	{
		DECLARE_RTTI;
	protected:
		ControlledObject();
		ControlledObject( const std::string& name );
	public:
		virtual ~ControlledObject();

		uint32 GetNumControllers() const				{ return mControllers.size(); }
		Controller* GetController( uint32 i ) const;
		void Attach( Controller* cont );
		void Detach( Controller* cont );
		void DetachAll();
		bool UpdateControllers( float currentTime );

	private:
		ArrayList< Controller* > mControllers;
	};

}