LOCAL_PATH:= $(call my-dir)

magemath_includes := $(LOCAL_PATH)/include

magemath_sources := \
  src/Frustum.cpp \
  src/Integrators.cpp \
  src/Intersection.cpp \
  src/Matrix2.cpp \
  src/Matrix3.cpp \
  src/Matrix4.cpp \
  src/Quaternion.cpp \
  src/RNG.cpp \
  src/Vector2.cpp \
  src/Vector3.cpp \
  src/Vector4.cpp \
  src/MathUtil.cpp \

include $(CLEAR_VARS)

LOCAL_MODULE := magemath
LOCAL_CPPFLAGS += -include MathUtil.h
LOCAL_SRC_FILES := $(magemath_sources)
LOCAL_C_INCLUDES := $(magemath_includes)
LOCAL_EXPORT_C_INCLUDES := $(magemath_includes)

include $(BUILD_STATIC_LIBRARY)