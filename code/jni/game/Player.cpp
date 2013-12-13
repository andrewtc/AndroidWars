#include "androidwars.h"

using namespace mage;


Player::Player()
	: mIndex( -1 )
	, mGame( nullptr )
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
