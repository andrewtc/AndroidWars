#include "androidwars.h"

using namespace mage;


const float Faction::INACTIVE_COLOR_VALUE_SHIFT = 0.5f;


Faction::Faction( Map* map ) :
	mMap( map ),
	mFunds( 0 ),
	mColor( Color::WHITE )
{
	assertion( mMap, "Cannot create Faction without a valid Map!" );
}


Faction::~Faction() { }


void Faction::OnTurnStart( int turnIndex )
{
	// Determine how many funds to give the Faction based on the tiles the Faction owns.
	int fundsToAdd = CalculateIncome();

	// Add the funds to the Faction's account.
	AddFunds( fundsToAdd );

	for( auto it = mUnits.begin(); it != mUnits.end(); ++it )
	{
		// Let each Unit process the next turn.
		Unit* unit = *it;
		unit->OnTurnStart( turnIndex );
	}
}


void Faction::OnTurnEnd( int turnIndex )
{
	for( auto it = mUnits.begin(); it != mUnits.end(); ++it )
	{
		// Let each Unit process the next turn.
		Unit* unit = *it;
		unit->OnTurnEnd( turnIndex );
	}
}


Map* Faction::GetMap() const
{
	return mMap;
}


void Faction::SetFunds( int funds )
{
	// Don't allow funds to drop below zero.
	mFunds = std::max( funds, 0 );
}


void Faction::AddFunds( int funds )
{
	SetFunds( mFunds + funds );
}


int Faction::CalculateIncome() const
{
	int income = 0;

	for( auto it = mTiles.begin(); it != mTiles.end(); ++it )
	{
		Map::Iterator tile = *it;

		// Get the income for each Tile from its TerrainType.
		assertion( tile->HasTerrainType(), "Cannot get income for tile (%d,%d) because it does not have a valid TerrainType!", tile.GetX(), tile.GetY() );
		income += tile->GetTerrainType()->GetIncome();
	}

	return income;
}


int Faction::GetFunds() const
{
	return mFunds;
}


const Faction::Units& Faction::GetUnits() const
{
	return mUnits;
}


size_t Faction::GetUnitCount() const
{
	return mUnits.size();
}


bool Faction::HasUnits() const
{
	return ( GetUnitCount() > 0 );
}


const Faction::Tiles& Faction::GetTiles() const
{
	return mTiles;
}


size_t Faction::GetTileCount() const
{
	return mTiles.size();
}


bool Faction::HasTiles() const
{
	return ( GetTileCount() > 0 );
}


void Faction::SetHeadquarters( Map::Iterator tile )
{
	mHeadquarters = tile;
}


void Faction::ClearHeadquarters()
{
	mHeadquarters = Map::Iterator();
}


Map::Iterator Faction::GetHeadquarters() const
{
	return mHeadquarters;
}


bool Faction::HasHeadquarters() const
{
	return mHeadquarters.IsValid();
}


void Faction::SetColor( const Color& color )
{
	// Set the color of the Faction.
	mColor = color;

	// Fire the color changed event.
	OnColorChanged.Invoke( mColor );
}


Color Faction::GetColor() const
{
	return mColor;
}


Color Faction::GetInactiveColor() const
{
	// Return a darker version of the Faction color.
	HSV hsv = Color::ColorToHSV( mColor );
	return Color::HSVToColor( hsv.hue, hsv.saturation, Mathf::Clamp( hsv.value - INACTIVE_COLOR_VALUE_SHIFT, 0.0f, 1.0f ) );
}


void Faction::UnitGained( Unit* unit )
{
	// Keep track of the Units owned by this Faction.
	mUnits.insert( unit );
}


void Faction::UnitLost( Unit* unit )
{
	// Keep track of the Units owned by this Faction.
	mUnits.erase( unit );
}


void Faction::TileGained( const Map::Iterator& tile )
{
	// Keep track of the Units owned by this Faction.
	mTiles.insert( tile );
}


void Faction::TileLost( const Map::Iterator& tile )
{
	// Keep track of the Units owned by this Faction.
	mTiles.insert( tile );
}
