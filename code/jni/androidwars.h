#pragma once

namespace mage
{
	class Game;
	class Database;
}

extern mage::Game* gGame;
extern mage::Database* gDatabase;

#include <MageApp.h>

#include "data/Table.h"
#include "data/TerrainType.h"
#include "data/TerrainTypesTable.h"
#include "data/UnitType.h"
#include "data/UnitTypesTable.h"
#include "data/Database.h"

#include "game/Player.h"
#include "game/Unit.h"
#include "game/Game.h"
