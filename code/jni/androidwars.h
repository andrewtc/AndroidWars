#pragma once

namespace mage
{
	class Game;
	class Database;
	class SoundManager;
	class OnlineGameClient;
}

extern mage::Game* gGame;
extern mage::Database* gDatabase;
extern mage::SoundManager* gSoundManager;
extern mage::OnlineGameClient* gOnlineGameClient;

#include <MageApp.h>

#include "sound/SoundManager.h"
#include "online/OnlineGameClient.h"

#include "ui/Widget.h"
#include "ui/Label.h"
#include "ui/Button.h"

#include "data/Table.h"
#include "data/TerrainType.h"
#include "data/TerrainTypesTable.h"
#include "data/MovementType.h"
#include "data/MovementTypesTable.h"
#include "data/UnitType.h"
#include "data/Weapon.h"
#include "data/UnitTypesTable.h"
#include "data/Database.h"

#include "game/Player.h"
#include "game/Unit.h"
#include "game/Path.h"
#include "game/Game.h"
