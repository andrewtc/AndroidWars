LOCAL_PATH := $(call my-dir)

#mage app
include $(CLEAR_VARS)

mageapp_path := libs/MageApp

LOCAL_MODULE    := _mageapp
LOCAL_SRC_FILES := $(mageapp_path)/MageApp.cpp
LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(mageapp_path)
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/$(mageapp_path)
LOCAL_STATIC_LIBRARIES := android_native_app_glue _magemath _magecore _magerenderer png
LOCAL_CFLAGS += -std=c++11

include $(BUILD_STATIC_LIBRARY)

#mage core
include $(CLEAR_VARS)

magecore_path := libs/MageCore
magecore_fullpath := $(LOCAL_PATH)/$(magecore_path)
magecore_includes := $(LOCAL_PATH)/$(magecore_path)/CoreLib.h
  
magecore_export_includes := \
  $(magecore_fullpath)/ \
  $(magecore_fullpath)/IO \
  $(magecore_fullpath)/DataStructures \
  $(magecore_fullpath)/IK \
  $(magecore_fullpath)/Threads \
  $(magecore_fullpath)/Util \
  $(magecore_fullpath)/Threads \
  $(magecore_fullpath)/External

magecore_sources := \
 $(magecore_path)/External/tinyxml2.cpp \
 $(magecore_path)/IO/Console.cpp \
 $(magecore_path)/IO/DebugIO.cpp \
 $(magecore_path)/IO/FileSystem.cpp \
 $(magecore_path)/IO/Resource.cpp \
 $(magecore_path)/Threads/Mutex_Unix.cpp \
 $(magecore_path)/DataStructures/HashString.cpp \
 $(magecore_path)/DataStructures/Dictionary.cpp \
 $(magecore_path)/Util/StringUtil.cpp \
 $(magecore_path)/Util/HashUtil.cpp \
 $(magecore_path)/Util/XmlReader.cpp \
 $(magecore_path)/Util/base64.cpp \
 $(magecore_path)/Event.cpp \
 $(magecore_path)/Assertion.cpp \
 $(magecore_path)/Color.cpp \
 $(magecore_path)/Clock.cpp \
 $(magecore_path)/Object.cpp \
 $(magecore_path)/RTTI.cpp \

LOCAL_MODULE := _magecore
LOCAL_CFLAGS += -std=c++11
LOCAL_SRC_FILES := $(magecore_sources)
LOCAL_C_INCLUDES := $(magecore_export_includes)
LOCAL_EXPORT_C_INCLUDES := $(magecore_export_includes)
LOCAL_STATIC_LIBRARIES += _magemath

include $(BUILD_STATIC_LIBRARY)

#mage math
include $(CLEAR_VARS)

magemath_path := libs/MageMath
magemath_includes := $(LOCAL_PATH)/$(magemath_path)/include

magemath_sources := \
  $(magemath_path)/src/Frustum.cpp \
  $(magemath_path)/src/Integrators.cpp \
  $(magemath_path)/src/Intersection.cpp \
  $(magemath_path)/src/Matrix2.cpp \
  $(magemath_path)/src/Matrix3.cpp \
  $(magemath_path)/src/Matrix4.cpp \
  $(magemath_path)/src/Quaternion.cpp \
  $(magemath_path)/src/RNG.cpp \
  $(magemath_path)/src/Vector2.cpp \
  $(magemath_path)/src/Vector3.cpp \
  $(magemath_path)/src/Vector4.cpp \
  $(magemath_path)/src/MathUtil.cpp \

include $(CLEAR_VARS)

LOCAL_MODULE := _magemath
LOCAL_CPPFLAGS += -include MathUtil.h
LOCAL_SRC_FILES := $(magemath_sources)
LOCAL_C_INCLUDES := $(magemath_includes)
LOCAL_EXPORT_C_INCLUDES := $(magemath_includes)

include $(BUILD_STATIC_LIBRARY)

#mage renderer
include $(CLEAR_VARS)
  
magerenderer_path := libs/MageRenderer
magerenderer_export_includes := \
  $(LOCAL_PATH)/$(magerenderer_path) \
  $(LOCAL_PATH)/$(magerenderer_path)/Scene \
  $(LOCAL_PATH)/$(magerenderer_path)/GLRenderer \
  
magerenderer_sources := \
 $(magerenderer_path)/GLRenderer/GLRenderer.cpp \
 $(magerenderer_path)/GLRenderer/GLContext.cpp \
 $(magerenderer_path)/GLRenderer/ESContext.cpp \
 $(magerenderer_path)/GLRenderer/gl3stub.c \
 $(magerenderer_path)/Scene/CameraNode.cpp \
 $(magerenderer_path)/Scene/DrawAxesNode.cpp \
 $(magerenderer_path)/Scene/DrawBoneNode.cpp \
 $(magerenderer_path)/Scene/SpriteNode.cpp \
 $(magerenderer_path)/Effect.cpp \
 $(magerenderer_path)/BloomEffect.cpp \
 $(magerenderer_path)/Camera.cpp \
 $(magerenderer_path)/Font.cpp \
 $(magerenderer_path)/Renderer.cpp \
 $(magerenderer_path)/Shader.cpp \
 $(magerenderer_path)/Surface.cpp \
 $(magerenderer_path)/Texture.cpp \
 $(magerenderer_path)/Uniform.cpp \
 $(magerenderer_path)/Sprite.cpp \
 $(magerenderer_path)/SpriteManager.cpp \
 $(magerenderer_path)/TileMap.cpp \
 $(magerenderer_path)/BitmapFont.cpp \



LOCAL_MODULE := _magerenderer
LOCAL_CFLAGS += -std=c++11
LOCAL_SRC_FILES := $(magerenderer_sources)
LOCAL_C_INCLUDES := $(magerenderer_export_includes)
LOCAL_EXPORT_C_INCLUDES := $(magerenderer_export_includes)
LOCAL_STATIC_LIBRARIES := _magemath _magecore png

include $(BUILD_STATIC_LIBRARY)

#android wars
include $(CLEAR_VARS)

aw_game_path := game
aw_editor_path := editor
aw_mainmenu_path := mainmenu
aw_data_path := data
aw_states_path := states

LOCAL_MODULE    := androidwars
LOCAL_SRC_FILES := androidwars.cpp \
$(aw_states_path)/InputState.cpp \
$(aw_states_path)/DialogInputState.cpp \
$(aw_states_path)/ProgressInputState.cpp \
$(aw_states_path)/GameState.cpp \
$(aw_states_path)/GameStateManager.cpp \
$(aw_mainmenu_path)/MainMenuState.cpp \
$(aw_data_path)/TerrainType.cpp \
$(aw_data_path)/TerrainTypesTable.cpp \
$(aw_data_path)/UnitType.cpp \
$(aw_data_path)/Weapon.cpp \
$(aw_data_path)/UnitTypesTable.cpp \
$(aw_data_path)/MovementType.cpp \
$(aw_data_path)/MovementTypesTable.cpp \
$(aw_data_path)/Scenario.cpp \
$(aw_game_path)/GameplayState.cpp \
$(aw_game_path)/GameplayInputStates.cpp \
$(aw_game_path)/abilities/Ability.cpp \
$(aw_game_path)/abilities/UnitAbility.cpp \
$(aw_game_path)/abilities/UnitWaitAbility.cpp \
$(aw_game_path)/abilities/UnitAttackAbility.cpp \
$(aw_game_path)/abilities/UnitReinforceAbility.cpp \
$(aw_game_path)/abilities/UnitCaptureAbility.cpp \
$(aw_game_path)/abilities/TileAbility.cpp \
$(aw_game_path)/animations/MapAnimation.cpp \
$(aw_game_path)/animations/UnitMoveMapAnimation.cpp \
$(aw_game_path)/Game.cpp \
$(aw_game_path)/Player.cpp \
$(aw_game_path)/Faction.cpp \
$(aw_game_path)/Unit.cpp \
$(aw_game_path)/Map.cpp \
$(aw_game_path)/MapView.cpp \
$(aw_game_path)/TileSprite.cpp \
$(aw_game_path)/UnitSprite.cpp \
$(aw_game_path)/ArrowSprite.cpp \
$(aw_game_path)/TargetSprite.cpp \
$(aw_editor_path)/EditorState.cpp \
$(aw_editor_path)/EditorInputStates.cpp \
 ui/WidgetManager.cpp \
 ui/WidgetTemplate.cpp \
 ui/Widget.cpp \
 ui/Label.cpp \
 ui/Graphic.cpp \
 ui/Button.cpp \
 ui/TextField.cpp \
 ui/ListLayout.cpp \
 sound/SoundManager.cpp \
 online/OnlineGameClient.cpp \
 util/JNI.cpp \
 util/PrimaryDirection.cpp

LOCAL_LDLIBS := -llog -landroid -lEGL -lGLESv2 -lOpenSLES
LOCAL_STATIC_LIBRARIES := android_native_app_glue _magemath _magecore _magerenderer png _mageapp
LOCAL_C_INCLUDES := $(LOCAL_PATH)/libs/rapidjson
LOCAL_CFLAGS += -std=c++11

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
$(call import-module,libpng)
