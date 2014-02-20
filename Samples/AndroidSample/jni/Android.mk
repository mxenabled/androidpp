LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := androidsample_shared

LOCAL_MODULE_FILENAME := libandroidsample

LOCAL_SRC_FILES := main/main.cpp

#LOCAL_CFLAGS := -g -ffast-math -O3 -funroll-loops
LOCAL_CFLAGS += -DBUILD_FOR_ANDROID -DCOCOS2D_DEBUG=1
LOCAL_EXPORT_CFLAGS := -DCOCOS2D_DEBUG=1

LOCAL_CPPFLAGS += -std=c++11

LOCAL_WHOLE_STATIC_LIBRARIES += sample_static

LOCAL_LDLIBS := -llog -ljnigraphics

include $(BUILD_SHARED_LIBRARY)

$(call import-module,Samples/iOSSample/iOSSample/Classes)