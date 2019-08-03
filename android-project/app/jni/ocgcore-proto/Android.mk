LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := ocgcore-proto

LOCAL_C_INCLUDES := $(LOCAL_PATH)/.. $(LOCAL_PATH)

# Add your application source files here...
SRC_PATH := $(LOCAL_PATH)/../../../../lib/ocgcore-proto
LOCAL_SRC_FILES := \
	$(LOCAL_PATH)/core_data.pb.cc \
	$(LOCAL_PATH)/core_msg_request.pb.cc \
	$(LOCAL_PATH)/core_msg_information.pb.cc \
	$(LOCAL_PATH)/core_msg.pb.cc \
	$(SRC_PATH)/msg_encoder.cpp

LOCAL_CPPFLAGS := -std=c++17 -fexceptions
LOCAL_SHARED_LIBRARIES := protobuf

include $(BUILD_STATIC_LIBRARY)
