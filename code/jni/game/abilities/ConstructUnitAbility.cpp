#include "androidwars.h"

using namespace mage;


MAGE_IMPLEMENT_ABILITY( ConstructUnitAbility, "ConstructUnit" );


ConstructUnitAbility::ConstructUnitAbility( Map* map ) :
	Ability( map )
{ }


ConstructUnitAbility::~ConstructUnitAbility() { }


void ConstructUnitAbility::DetermineAvailableActionsForTile( const Vec2s& tilePos, Actions& result )
{
	// Get the tile.
	Map::Iterator tile = mMap->GetTile( tilePos );

	if( tile.IsValid() )
	{
		// If the Tile is valid, get the owner of the Tile (if any).
		Faction* owner = tile->GetOwner();

		if( owner )
		{
			// If the Tile has an owner, get the TerrainType for the Tile.
			TerrainType* terrainType = tile->GetTerrainType();

			// Get the list of Units that can be created by this Tile (if any).
			std::vector< UnitType* > constructedUnitTypes;
			terrainType->GetConstructedUnitTypes( constructedUnitTypes );

			if( constructedUnitTypes.size() > 0 )
			{
				// If this Tile can construct Units, get the current funds for the Tile owner.
				int currentFunds = owner->GetFunds();

				for( auto it = constructedUnitTypes.begin(); it != constructedUnitTypes.end(); ++it )
				{
					// Get the build cost of each UnitType that can be constructed.
					UnitType* unitType = *it;
					int buildCost = unitType->GetBuildCost();

					if( buildCost <= currentFunds )
					{
						// If the Unit can be built, create an Action for it.
						ConstructUnitAbility::Action* constructUnitAction = new ConstructUnitAbility::Action();

						constructUnitAction->FactionID = GetMap()->
					}
				}
			}
		}
	}
}


void ConstructUnitAbility::ProcessAction( Ability::Action* action )
{
	// Cast the action to the proper type.
	ConstructUnitAbility::Action* constructUnitAction = dynamic_cast< ConstructUnitAbility::Action* >( action );
	assertion( constructUnitAction, "Could not cast Ability::Action to proper type for ConstructUnitAbility!" );

	// Get the UnitType.
	UnitType* unitType = GetMap()->GetScenario()->UnitTypes.FindByName( constructUnitAction->UnitTypeID );
	assertion( unitType, "Invalid UnitType ID (\"%s\") specified for ConstructUnitAbility Action!", constructUnitAction->UnitTypeID.GetCString() );

	// Get the target Tile.
	Map::Iterator tile = GetMap()->GetTile( constructUnitAction->TilePos );
	assertion( tile.IsValid(), "Invalid Tile position (%d,%d) specified for ConstructUnitAbility Action!", constructUnitAction->TilePos.x, constructUnitAction->TilePos.y );

	// Get the Faction.
	Faction* faction = GetMap()->GetFactionByID( constructUnitAction->FactionID );
	assertion( faction, "Invalid Faction ID (%d) specified for ConstructUnitAbility Action!", constructUnitAction->FactionID );

	// Make sure a valid Unit ID was specified.
	int unitID = constructUnitAction->UnitID;
	assertion( unitID > 0, "Invalid Unit ID (%d) specified for ConstructUnitAbility Action!", unitID );

	// Create and initialize a new, inactive Unit at the specified location.
	Unit* unit = GetMap()->CreateUnit( unitID, unitType, faction, tile.GetPosition() );
	assertion( unit, "Could not create Unit for ConstructUnitAbility Action!" );
	unit->Deactivate();
}


ConstructUnitAbility::Action::Action() :
	UnitID( -1 ),
	FactionID( -1 ),
	BuildCost( 0 )
{ }


ConstructUnitAbility::Action::~Action() { }


void ConstructUnitAbility::Action::SaveToJSON( rapidjson::Document& document, rapidjson::Value& object ) const
{
	JSON::SaveInt( document, object, "unitID", UnitID );
	JSON::SaveInt( document, object, "faction", FactionID );
	JSON::SaveString( document, object, "unitType", UnitTypeID.GetString() );
	JSON::SaveVec2i( document, object, "tilePos", TilePos );
	JSON::SaveInt( document, object, "buildCost", BuildCost );
}


void ConstructUnitAbility::Action::LoadFromJSON( const rapidjson::Value& object )
{
	// TODO
}
