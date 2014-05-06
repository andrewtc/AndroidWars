#pragma once

namespace mage
{
	/**
	 * Represents an ability that can be performed on a Tile.
	 */
	class TileAbility : public Ability
	{
	public:
		class Action;

		TileAbility( Map* map );
		~TileAbility();

		virtual void DetermineActionsForTile( const Vec2s& tilePos, const Actions& actions ) = 0;
	};


	class TileAbility::Action
	{
	public:
		Action();
		~Action();

		Vec2s& TilePos;
	};
}
