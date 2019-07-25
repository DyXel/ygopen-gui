LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := ygopen

LOCAL_C_INCLUDES := $(LOCAL_PATH)/..

# Add your application source files here...
SRC_PATH := $(LOCAL_PATH)/../../../../lib/ygopen
LOCAL_SRC_FILES := $(SRC_PATH)/banlist.cpp \
	$(SRC_PATH)/core_auxiliary.cpp \
	$(SRC_PATH)/core_interface.cpp \
	$(SRC_PATH)/database_manager.cpp \
	$(SRC_PATH)/deck.cpp \
	$(SRC_PATH)/sqlite3.c \
	$(SRC_PATH)/sqlite3.h \
	$(SRC_PATH)/banlist.hpp \
	$(SRC_PATH)/core_auxiliary.hpp \
	$(SRC_PATH)/core_interface.hpp \
	$(SRC_PATH)/deck.hpp \
	$(SRC_PATH)/pod_card.hpp

LOCAL_CPPFLAGS := -std=c++17 -fexceptions

include $(BUILD_STATIC_LIBRARY)
