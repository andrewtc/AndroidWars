#pragma once

namespace mage
{
	class Widget;
	class WidgetManager;
	class InputState;
	class GameState;
	class GameStateManager;
	class SoundManager;
	class OnlineGameClient;
	class Game;
	class Map;
	class Unit;
	class Faction;
	class Player;
}

#include <MageTypes.h>

extern mage::GameStateManager* gGameStateManager;
extern mage::WidgetManager* gWidgetManager;
extern mage::SoundManager* gSoundManager;
extern mage::OnlineGameClient* gOnlineGameClient;
extern mage::int32 gWindowWidth;
extern mage::int32 gWindowHeight;

const size_t MAP_SIZE_POWER_OF_TWO = 10;

#include <MageApp.h>

#include "util/JNI.h"
#include "util/Delegate.h"
#include "util/PrimaryDirection.h"
#include "util/Grid.h"
#include "util/MinHeap.h"

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

#include "states/InputState.h"
#include "states/DialogInputState.h"
#include "states/ProgressInputState.h"
#include "states/GameState.h"
#include "states/GameStateManager.h"

#include "sound/SoundManager.h"

#include "online/OnlineGameClient.h"

#include "ui/WidgetTemplate.h"
#include "ui/Widget.h"
#include "ui/WidgetManager.h"
#include "ui/Label.h"
#include "ui/Graphic.h"
#include "ui/Button.h"
#include "ui/TextField.h"
#include "ui/ListLayout.h"

#include "mainmenu/MainMenuState.h"

#include "data/Table.h"
#include "data/TerrainTypesTable.h"
#include "data/TerrainType.h"
#include "data/MovementTypesTable.h"
#include "data/MovementType.h"
#include "data/UnitTypesTable.h"
#include "data/UnitType.h"
#include "data/Weapon.h"
#include "data/Scenario.h"

#include "game/Path.h"
#include "game/Action.h"
#include "game/UnitAbility.h"
#include "game/Map.h"
#include "game/Faction.h"
#include "game/TileSprite.h"
#include "game/TargetSprite.h"
#include "game/UnitSprite.h"
#include "game/ArrowSprite.h"
#include "game/MapView.h"
#include "game/Player.h"
#include "game/Unit.h"
#include "game/Game.h"
#include "game/GameplayState.h"
#include "game/GameplayInputStates.h"

#include "editor/EditorState.h"
#include "editor/EditorInputStates.h"

