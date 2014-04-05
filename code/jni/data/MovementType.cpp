#include "androidwars.h"

using namespace mage;


MovementType::MovementType( const HashString& name ) :
	Record( name ),
	mSuppliesConsumedPerTurn( 0 ),
	mRequiresSuppliesToSurvive( false )
{ }


MovementType::~MovementType() { }


int MovementType::GetSuppliesConsumedPerTurn() const
{
	return mSuppliesConsumedPerTurn;
}


bool MovementType::RequiresSuppliesToSurvive() const
{
	return mRequiresSuppliesToSurvive;
}


void MovementType::AddMovementCost( const HashString& movementTypeName, int movementCost )
{
	mMovementCostsByTerrainTypeID[ movementTypeName ] = movementCost;
}


bool MovementType::CanMoveAcrossTerrain( TerrainType* terrainType ) const
{
	return ( GetMovementCostAcrossTerrain( terrainType ) > -1 );
}


int MovementType::GetMovementCostAcrossTerrain( TerrainType* terrainType ) const
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


std::string MovementType::GetDisplayName() const
{
	return mDisplayName;
}
