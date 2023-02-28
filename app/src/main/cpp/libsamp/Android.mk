LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := libopus 
LOCAL_SRC_FILES := vendor/opus/libopus.a

include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libenet 
LOCAL_SRC_FILES := vendor/enet/libenet.a

include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE  := GTASA
LOCAL_XOM := false
LOCAL_SRC_FILES  := libGTASA.so
include $(PREBUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := samp
LOCAL_LDLIBS := -llog -lGLESv3
LOCAL_XOM := false
LOCAL_C_INCLUDES += $(wildcard $(LOCAL_PATH)/vendor/)

# samp
FILE_LIST := $(wildcard $(LOCAL_PATH)/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/game/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/game/Animation/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/net/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/java_systems/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/java_systems/casino/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/util/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/game/RW/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/gui/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/cryptors/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../santrope-tea-gtasa/encryption/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../santrope-tea-gtasa/encryption/*.c)

# vendor
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/ini/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/RakNet/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/RakNet/SAMP/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/imgui/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/hash/*.cpp)


LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)
LOCAL_STATIC_LIBRARIES := libopus libenet

LOCAL_CPPFLAGS := -w -s -fvisibility=hidden -pthread -Wall -fpack-struct=1 -O0 -std=c++17 -fexceptions -marm
include $(BUILD_SHARED_LIBRARY)