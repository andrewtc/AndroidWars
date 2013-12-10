#pragma once

namespace mage
{
	class MovementType
	{
	public:
		MovementType();
		~MovementType();

		int GetMovementCostOverTerrain( TerrainType terrainType ) const;

	protected:
		HashMap< int > m_movementCostsByTerrainTypeID;
	};


	inline int MovementType::GetMovementCostOverTerrain( TerrainType terrainType ) const
	{
		int result = -1;

		// Get the hash value for this TerrainType (if it exists).
		auto it = m_movementCostsByTerrainTypeID.find( terrainType.GetName() );

		if( it != m_movementCostsByTerrainTypeID.end() )
		{
			result = it->second;
		}

		return result;
	}
}
