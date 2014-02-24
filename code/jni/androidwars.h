#pragma once

namespace mage
{
	class WidgetManager;
	class InputState;
	class GameState;
	class GameStateManager;
	class SoundManager;
	class OnlineGameClient;
	class Game;
	class Unit;
}

#include <MageTypes.h>

extern mage::GameStateManager* gGameStateManager;
extern mage::WidgetManager* gWidgetManager;
extern mage::SoundManager* gSoundManager;
extern mage::OnlineGameClient* gOnlineGameClient;
extern mage::int32 gWindowWidth;
extern mage::int32 gWindowHeight;

#include <MageApp.h>

#include "util/JNI.h"

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"

#include "states/InputState.h"
#include "states/GameState.h"
#include "states/GameStateManager.h"

#include "sound/SoundManager.h"

#include "online/OnlineGameClient.h"

#include "ui/Widget.h"
#include "ui/Label.h"
#include "ui/Graphic.h"
#include "ui/Button.h"
#include "ui/TextField.h"
#include "ui/Listbox.h"
#include "ui/WidgetManager.h"

#include "mainmenu/MainMenuState.h"

#include "data/Table.h"
#include "data/TerrainTypesTable.h"
#include "data/TerrainType.h"
#include "data/MovementTypesTable.h"
#include "data/MovementType.h"
#include "data/UnitTypesTable.h"
#include "data/UnitType.h"
#include "data/Weapon.h"
#include "data/Database.h"

#include "game/GameplayState.h"
#include "game/Player.h"
#include "game/Unit.h"
#include "game/Path.h"
#include "game/Game.h"
