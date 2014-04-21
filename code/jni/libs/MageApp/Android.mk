LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := mageapp
LOCAL_SRC_FILES := MageApp.cpp
LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)
LOCAL_STATIC_LIBRARIES := android_native_app_glue magemath magecore magerenderer png
LOCAL_CFLAGS += -std=c++11

include $(BUILD_STATIC_LIBRARY)