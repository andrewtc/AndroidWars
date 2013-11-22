/*
 * Author      : Matthew Johnson
 * Date        : 25/Feb/2013
 * Description :
 *   
 */
 
#pragma once

namespace mage
{

	template< typename TReal >
	class Integrators
	{
	public:
		
		typedef TReal (*Function)( TReal, void* );

		static TReal RombergIntegral( int order, TReal a, TReal b, Function func,
			void* userData=0 );
	};

}