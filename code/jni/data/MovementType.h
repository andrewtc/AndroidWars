#pragma once

namespace mage
{
	/**
	 * Stores movement cost information over varying types of terrain.
	 */
	class MovementType : public MovementTypesTable::Record
	{
	public:
		MovementType( const HashString& name );
		virtual ~MovementType();

		int GetSuppliesConsumedPerTurn() const;
		bool RequiresSuppliesToSurvive() const;

		bool CanMoveAcrossTerrain( TerrainType* terrainType ) const;
		int GetMovementCostAcrossTerrain( TerrainType* terrainType ) const;

		std::string GetDisplayName() const;

	protected:
		void AddMovementCost( const HashString& movementTypeName, int movementCost );

		bool mRequiresSuppliesToSurvive;
		int mSuppliesConsumedPerTurn;
		std::string mDisplayName;
		HashMap< int > mMovementCostsByTerrainTypeID;

		friend class MovementTypesTable;
	};
}
