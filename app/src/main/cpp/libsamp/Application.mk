APP_ABI := armeabi-v7a
APP_MODULES := samp
APP_STL := c++_shared
LOCAL_CPP_FEATURES := exceptions
ifeq ($(NDK_DEBUG),1)
  APP_STRIP_MODE := none
endif