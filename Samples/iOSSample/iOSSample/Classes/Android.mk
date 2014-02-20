LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := sample_static

LOCAL_MODULE_FILENAME := libsample

LOCAL_SRC_FILES := \
	AndroidAppDelegate.cpp \
	CustomView.cpp \
	DialActivity.cpp \
	DialView.cpp \
	TestActivity.cpp \

LOCAL_WHOLE_STATIC_LIBRARIES += cocos2dx_static
LOCAL_WHOLE_STATIC_LIBRARIES += android_static

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH) \
		$(LOCAL_PATH)/..

LOCAL_C_INCLUDES := $(LOCAL_PATH)

LOCAL_CFLAGS := -DBUILD_FOR_ANDROID
LOCAL_CPPFLAGS += -std=c++11

include $(BUILD_STATIC_LIBRARY)

$(call import-module,libs/cocos2d-x/cocos2dx) \
$(call import-module,Android)