LOCAL_PATH:= $(call my-dir)

magecore_includes := $(LOCAL_PATH)/CoreLib.h
  
magecore_export_includes := \
  $(LOCAL_PATH)/ \
  $(LOCAL_PATH)/IO \
  $(LOCAL_PATH)/DataStructures \
  $(LOCAL_PATH)/IK \
  $(LOCAL_PATH)/Threads \
  $(LOCAL_PATH)/Util \
  $(LOCAL_PATH)/Threads \
  $(LOCAL_PATH)/External

magecore_sources := \
 ./External/tinyxml2.cpp \
 ./IO/Console.cpp \
 ./IO/DebugIO.cpp \
 ./IO/FileSystem.cpp \
 ./IO/Resource.cpp \
 ./Threads/Mutex_Unix.cpp \
 ./DataStructures/HashString.cpp \
 ./DataStructures/Dictionary.cpp \
 ./Util/StringUtil.cpp \
 ./Util/HashUtil.cpp \
 ./Util/XmlReader.cpp \
 ./Util/base64.cpp \
 ./Event.cpp \
 ./Assertion.cpp \
 ./Color.cpp \
 ./Clock.cpp \
 ./Object.cpp \
 ./RTTI.cpp \

include $(CLEAR_VARS)

LOCAL_MODULE := magecore
LOCAL_CFLAGS += -std=c++11
LOCAL_SRC_FILES := $(magecore_sources)
LOCAL_C_INCLUDES := $(magecore_export_includes)
LOCAL_EXPORT_C_INCLUDES := $(magecore_export_includes)
LOCAL_STATIC_LIBRARIES += magemath
#LOCAL_LDLIBS := -llog -landroid

include $(BUILD_STATIC_LIBRARY)

#$(call import-module,MageMath)