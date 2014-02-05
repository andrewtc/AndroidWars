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

		bool CanMoveAcrossTerrain( TerrainType* terrainType ) const;
		int GetMovementCostAcrossTerrain( TerrainType* terrainType ) const;

	protected:
		void AddMovementCost( const HashString& movementTypeName, int movementCost );

		std::string mDisplayName;
		HashMap< int > mMovementCostsByTerrainTypeID;

		friend class MovementTypesTable;
	};


	inline void MovementType::AddMovementCost( const HashString& movementTypeName, int movementCost )
	{
		mMovementCostsByTerrainTypeID[ movementTypeName ] = movementCost;
	}


	inline bool MovementType::CanMoveAcrossTerrain( TerrainType* terrainType ) const
	{
		return ( GetMovementCostAcrossTerrain( terrainType ) > -1 );
	}


	inline int MovementType::GetMovementCostAcrossTerrain( TerrainType* terrainType ) const
	{
		// By default, don't allow movement across this type of terrain.
		int result = -1;

		// Get the hash value for this TerrainType (if it exists).
		auto it = mMovementCostsByTerrainTypeID.find( terrainType->GetName() );

		if( it != mMovementCostsByTerrainTypeID.end() )
		{
			result = it->second;
		}

		return result;
	}
}
