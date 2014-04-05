#include "androidwars.h"

using namespace mage;


Player::Player( Game* game, Faction* faction ) :
	mGame( game ), mFaction( faction )
{
	assertion( mGame, "Cannot create Player without a valid Game!" );
	assertion( mFaction, "Cannot create Player without a valid Faction!" );
}


Player::~Player() { }


Game* Player::GetGame() const
{
	return mGame;
}


Faction* Player::GetFaction() const
{
	return mFaction;
}
