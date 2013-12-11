#pragma once

namespace mage
{
	/**
	 * Stores all game data.
	 */
	class Database
	{
	public:
		Database();
		~Database();

		void LoadGameData();

		TerrainTypesTable TerrainTypes;
		UnitTypesTable UnitTypes;
		MovementTypesTable MovementTypes;
	};
}
