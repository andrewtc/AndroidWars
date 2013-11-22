LOCAL_PATH:= $(call my-dir)
  
magerenderer_export_includes := \
  $(LOCAL_PATH)/ \
  $(LOCAL_PATH)/Scene \
  $(LOCAL_PATH)/GLRenderer \
  
magerenderer_sources := \
 ./GLRenderer/GLRenderer.cpp \
 ./Scene/CameraNode.cpp \
 ./Scene/DrawAxesNode.cpp \
 ./Scene/DrawBoneNode.cpp \
 ./Scene/SpriteNode.cpp \
 ./Effect.cpp \
 ./BloomEffect.cpp \
 ./Camera.cpp \
 ./Font.cpp \
 ./Renderer.cpp \
 ./Shader.cpp \
 ./Surface.cpp \
 ./Texture.cpp \
 ./Uniform.cpp \
 ./Sprite.cpp \
 ./SpriteManager.cpp \
 ./TileMap.cpp \

include $(CLEAR_VARS)

LOCAL_MODULE := magerenderer
LOCAL_CFLAGS += -std=c++11
LOCAL_SRC_FILES := $(magerenderer_sources)
LOCAL_C_INCLUDES := $(magerenderer_export_includes)
LOCAL_EXPORT_C_INCLUDES := $(magerenderer_export_includes)
LOCAL_STATIC_LIBRARIES := magemath magecore png

include $(BUILD_STATIC_LIBRARY)

#$(call import-module,MageMath)
#$(call import-module,MageCore)
#$(call import-module,libpng)