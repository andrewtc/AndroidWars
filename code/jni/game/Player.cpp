#include "androidwars.h"

using namespace mage;


Player::Player( Game* game ) :
	mGame( game )
{
	assertion( mGame, "Cannot create Player without a valid Game!" );
}


Player::~Player() { }


Game* Player::GetGame() const
{
	return mGame;
}
