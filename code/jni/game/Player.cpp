#include "androidwars.h"

using namespace mage;


Player::Player()
	: mIndex( -1 )
	, mGame( nullptr )
	, CitiesOwned( 0 )
	, mFunds( 0 )
	, mUnits( 0 )
{ }


Player::~Player() { }

Color Player::GetPlayerColor() const
{
	switch ( mIndex )
	{
	case 0:
		return Color::RED;
	case 1:
		return Color::BLUE;
	default:
		return Color::WHITE;
	}
}

void Player::GenerateFunds()
{
	int gain = CitiesOwned * 10;
	mFunds += gain;
	//gGame->PostMessageFormat( GetPlayerColor(), "+$%d", gain );
};


bool Player::HasLost() const
{
	return mUnits == 0;
}


void Player::OnGainUnit( Unit* unit )
{
	mUnits++;
}


void Player::OnLoseUnit( Unit* unit )
{
	mUnits--;
}
