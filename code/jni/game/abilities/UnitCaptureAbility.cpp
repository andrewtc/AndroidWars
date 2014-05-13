#include "androidwars.h"

using namespace mage;


MAGE_IMPLEMENT_ABILITY( UnitCaptureAbility, "Capture" )


UnitCaptureAbility::UnitCaptureAbility( Map* map ) :
	UnitAbility( map )
{ }


UnitCaptureAbility::~UnitCaptureAbility() { }


void UnitCaptureAbility::DetermineAvailableActions( const Unit* unit, const Path& movementPath, Actions& result ) const
{
	// Get the destination Tile.
	Map::Iterator destinationTile = GetMap()->GetTile( movementPath.GetDestination() );

	if( destinationTile.IsValid() && destinationTile->HasTerrainType() )
	{
		// If the tile is valid and has a valid TerrainType, determine whether the Unit can capture this tile.
		Faction* unitOwner = unit->GetOwner();
		Faction* tileOwner = destinationTile->GetOwner();

		if( destinationTile->IsCapturable() && GetMap()->AreEnemies( unitOwner, tileOwner ) )
		{
			// If the tile can be captured by the Unit, add a capture action.
			UnitCaptureAbility::Action* captureAction = new UnitCaptureAbility::Action();

			captureAction->UnitID = unit->GetID();
			captureAction->MovementPath = movementPath;

			result.push_back( captureAction );
		}
	}
}


void UnitCaptureAbility::ProcessUnitAction( UnitAbility::Action* unitAction )
{
	// Try to cast the Action to the proper type.
	UnitCaptureAbility::Action* captureAction = dynamic_cast< UnitCaptureAbility::Action* >( unitAction );
	assertion( captureAction, "Could not convert UnitAbility::Action to UnitCaptureAbility::Action!" );

	// Get the Unit.
	Unit* unit = GetMap()->GetUnitByID( captureAction->UnitID );
	assertion( unit, "Invalid Unit ID (%d) specified for UnitCaptureAbility::Action!", captureAction->UnitID );

	// Get the destination Tile.
	Map::Iterator destinationTile = GetMap()->GetTile( unitAction->MovementPath.GetDestination() );
	assertion( destinationTile.IsValid() && destinationTile->HasTerrainType(), "Invalid Tile (%d,%d) specified for UnitCaptureAbility::Action!", destinationTile.GetX(), destinationTile.GetY() );

	// Capture the Tile.
	// TODO: Support partial capturing, using capture points.
	destinationTile->SetOwner( unit->GetOwner() );
}


UnitCaptureAbility::Action::Action() :
	CapturePointsRemaining( 0 ),
	CapturePointsRemoved( 0 )
{ }


UnitCaptureAbility::Action::~Action() { }
