LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := sdl_mixer

LOCAL_CFLAGS := -I$(LOCAL_PATH) \
                -I$(LOCAL_PATH)/../sdl/include \
                -I$(LOCAL_PATH)/../tremor \
                -DOGG_USE_TREMOR -DOGG_MUSIC -Os

LOCAL_CPP_EXTENSION := .cpp

# Note this simple makefile var substitution, you can find even simpler examples in different Android projects
LOCAL_SRC_FILES := $(notdir $(wildcard $(LOCAL_PATH)/*.c))

LOCAL_STATIC_LIBRARIES := tremor
LOCAL_SHARED_LIBRARIES := sdl
include $(BUILD_SHARED_LIBRARY)
