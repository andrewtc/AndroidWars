#include "androidwars.h"

using namespace mage;

Color Player::sPlayerColors[] =
{
	Color::RED,
	Color::BLUE
};

Player::Player()
	: mIndex( -1 )
	, mGame( nullptr )
{ }


Player::~Player() { }
