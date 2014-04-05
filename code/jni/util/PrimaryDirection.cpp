#include "androidwars.h"

namespace mage
{
	const PrimaryDirection PrimaryDirection::NONE      ( DIRECTION_NONE );
	const PrimaryDirection PrimaryDirection::EAST      ( DIRECTION_EAST );
	const PrimaryDirection PrimaryDirection::NORTHEAST ( DIRECTION_NORTHEAST );
	const PrimaryDirection PrimaryDirection::NORTH     ( DIRECTION_NORTH );
	const PrimaryDirection PrimaryDirection::NORTHWEST ( DIRECTION_NORTHWEST );
	const PrimaryDirection PrimaryDirection::WEST      ( DIRECTION_WEST );
	const PrimaryDirection PrimaryDirection::SOUTHWEST ( DIRECTION_SOUTHWEST );
	const PrimaryDirection PrimaryDirection::SOUTH     ( DIRECTION_SOUTH );
	const PrimaryDirection PrimaryDirection::SOUTHEAST ( DIRECTION_SOUTHEAST );

	const PrimaryDirection::DirectionInfo PrimaryDirection::DIRECTION_INFO[ DIRECTION_COUNT ] =
	{
		DirectionInfo( "None",      Vec2s(  0, 0 ), DIRECTION_NONE ),
		DirectionInfo( "East",      Vec2s(  1, 0 ), DIRECTION_WEST ),
		DirectionInfo( "Northeast", Vec2s(  1,-1 ), DIRECTION_SOUTHWEST ),
		DirectionInfo( "North",     Vec2s(  0,-1 ), DIRECTION_SOUTH ),
		DirectionInfo( "Northwest", Vec2s( -1,-1 ), DIRECTION_SOUTHEAST ),
		DirectionInfo( "West",      Vec2s( -1, 0 ), DIRECTION_EAST ),
		DirectionInfo( "Southwest", Vec2s( -1, 1 ), DIRECTION_NORTHEAST ),
		DirectionInfo( "South",     Vec2s(  0, 1 ), DIRECTION_NORTH ),
		DirectionInfo( "Southeast", Vec2s(  1, 1 ), DIRECTION_NORTHWEST )
	};

	const PrimaryDirection PRIMARY_DIRECTIONS[ PRIMARY_DIRECTION_COUNT ] =
	{
		PrimaryDirection::EAST,
		PrimaryDirection::NORTHEAST,
		PrimaryDirection::NORTH,
		PrimaryDirection::NORTHWEST,
		PrimaryDirection::WEST,
		PrimaryDirection::SOUTHWEST,
		PrimaryDirection::SOUTH,
		PrimaryDirection::SOUTHEAST
	};

	const PrimaryDirection CARDINAL_DIRECTIONS[ CARDINAL_DIRECTION_COUNT ] =
	{
		PrimaryDirection::EAST,
		PrimaryDirection::NORTH,
		PrimaryDirection::WEST,
		PrimaryDirection::SOUTH,
	};

	const PrimaryDirection ORDINAL_DIRECTIONS[ ORDINAL_DIRECTION_COUNT ] =
	{
		PrimaryDirection::NORTHEAST,
		PrimaryDirection::NORTHWEST,
		PrimaryDirection::SOUTHWEST,
		PrimaryDirection::SOUTHEAST
	};
}
