#include "androidwars.h"

using namespace mage;


namespace mage
{
	const char* const GetDirectionName( CardinalDirection direction )
	{
		const char* result;

		switch( direction )
		{
		case CARDINAL_DIRECTION_EAST:
			result = "East";
			break;

		case CARDINAL_DIRECTION_NORTH:
			result = "North";
			break;

		case CARDINAL_DIRECTION_WEST:
			result ="West";
			break;

		case CARDINAL_DIRECTION_SOUTH:
			result = "South";
			break;

		default:
			result = "None";
			break;
		}

		return result;
	}
}
