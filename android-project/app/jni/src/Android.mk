LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include $(LOCAL_PATH)/..

# Add your application source files here...
SRC_PATH := $(LOCAL_PATH)/../../../../src
LOCAL_SRC_FILES := $(SRC_PATH)/board.hpp \
	$(SRC_PATH)/configs.cpp \
	$(SRC_PATH)/configs.hpp \
	$(SRC_PATH)/game_data.cpp \
	$(SRC_PATH)/game_data.hpp \
	$(SRC_PATH)/game_instance.cpp \
	$(SRC_PATH)/game_instance.hpp \
	$(SRC_PATH)/main.cpp \
	$(SRC_PATH)/state.hpp \
	$(SRC_PATH)/drawing/api.cpp \
	$(SRC_PATH)/drawing/api.hpp \
	$(SRC_PATH)/drawing/gl_core_funcs.h \
	$(SRC_PATH)/drawing/gl_es2_funcs.h \
	$(SRC_PATH)/drawing/gl_include_defines.h \
	$(SRC_PATH)/drawing/gl_include.cpp \
	$(SRC_PATH)/drawing/gl_include.hpp \
	$(SRC_PATH)/drawing/primitive.hpp \
	$(SRC_PATH)/drawing/texture.hpp \
	$(SRC_PATH)/drawing/types.hpp \
	$(SRC_PATH)/drawing/dummy/primitive.cpp \
	$(SRC_PATH)/drawing/dummy/primitive.hpp \
	$(SRC_PATH)/drawing/dummy/texture.cpp \
	$(SRC_PATH)/drawing/dummy/texture.hpp \
	$(SRC_PATH)/drawing/gl_core/primitive.cpp \
	$(SRC_PATH)/drawing/gl_core/primitive.hpp \
	$(SRC_PATH)/drawing/gl_es/primitive.cpp \
	$(SRC_PATH)/drawing/gl_es/primitive.hpp \
	$(SRC_PATH)/drawing/gl_shared/common.hpp \
	$(SRC_PATH)/drawing/gl_shared/program.cpp \
	$(SRC_PATH)/drawing/gl_shared/program.hpp \
	$(SRC_PATH)/drawing/gl_shared/shader.cpp \
	$(SRC_PATH)/drawing/gl_shared/shader.hpp \
	$(SRC_PATH)/drawing/gl_shared/texture.cpp \
	$(SRC_PATH)/drawing/gl_shared/texture.hpp \
	$(SRC_PATH)/states/loading.cpp \
	$(SRC_PATH)/states/loading.hpp \
	$(SRC_PATH)/states/menu.cpp \
	$(SRC_PATH)/states/menu.hpp

LOCAL_CPPFLAGS := -std=c++17 -fexceptions -DFMT_HEADER_ONLY

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_image protobuf ygopen ocgcore-proto

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog

include $(BUILD_SHARED_LIBRARY)
