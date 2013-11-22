/*
 * Author      : Matthew Johnson
 * Date        : 10/Oct/2013
 * Description :
 *   std::map<> with HashString as _Kty
 */
 
#pragma once

namespace mage
{

	template< class _Ty >
	class HashMap
		: public std::map< HashString, _Ty >
	{
	};

}