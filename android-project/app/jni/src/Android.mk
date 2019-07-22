LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include $(LOCAL_PATH)/..

# Add your application source files here...
LOCAL_SRC_FILES := YourSourceHere.c

LOCAL_CFLAGS := -std=c++17 -fexceptions -DFMT_HEADER_ONLY

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_image protobuf

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog

include $(BUILD_SHARED_LIBRARY)
