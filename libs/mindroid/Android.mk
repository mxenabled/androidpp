LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := mindroid_static

LOCAL_MODULE_FILENAME := libmindroid

LOCAL_SRC_FILES := \
	mindroid/os/Thread.cpp \
	mindroid/os/Semaphore.cpp \
	mindroid/os/MessageQueue.cpp \
	mindroid/os/Message.cpp \
	mindroid/os/Looper.cpp \
	mindroid/os/Lock.cpp \
	mindroid/os/CondVar.cpp \
	mindroid/os/Handler.cpp \
	mindroid/os/Clock.cpp \
	mindroid/os/AsyncTask.cpp \
	mindroid/os/SerialExecutor.cpp \
	mindroid/os/ThreadPoolExecutor.cpp \
	mindroid/os/AtomicInteger.cpp.arm \
	mindroid/os/Ref.cpp \
	mindroid/os/Bundle.cpp \
	mindroid/util/Buffer.cpp \
	mindroid/util/Log.cpp \
	mindroid/util/Logger.cpp \
	mindroid/lang/StringM.cpp \
	mindroid/lang/StringWrapper.cpp \
	mindroid/net/SocketAddress.cpp \
	mindroid/net/ServerSocket.cpp \
	mindroid/net/Socket.cpp \
	mindroid/net/DatagramSocket.cpp

LOCAL_CFLAGS := -D__ANDROID__
LOCAL_CPPFLAGS += -std=c++11

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)
LOCAL_C_INCLUDES := $(LOCAL_PATH)

LOCAL_STATIC_LIBRARIES := \
	cutils_static \
	utils_static

include $(BUILD_STATIC_LIBRARY)

$(call import-module,libs/Andrender/libs/cutils)
$(call import-module,libs/Andrender/libs/utils)

