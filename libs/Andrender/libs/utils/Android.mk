# Copyright (C) 2008 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := utils_static

LOCAL_MODULE_FILENAME := libutils

# libutils is a little unique: It's built twice, once for the host
# and once for the device.

commonSources:= \
	BasicHashtable.cpp \
	BlobCache.cpp \
	BufferedTextOutput.cpp \
	CallStack.cpp \
	Debug.cpp \
	Flattenable.cpp \
	JenkinsHash.cpp \
	LinearAllocator.cpp \
	LinearTransform.cpp \
	Log.cpp \
	PropertyMap.cpp \
	SharedBuffer.cpp \
	Static.cpp \
	StopWatch.cpp \
	String8.cpp \
	String16.cpp \
	StringArray.cpp \
	SystemClock.cpp \
	TextOutput.cpp \
	Threads.cpp \
	Timers.cpp \
	Unicode.cpp \
	VectorImpl.cpp \
	WorkQueue.cpp \
	misc.cpp

LOCAL_SRC_FILES:= $(commonSources)

LOCAL_CFLAGS += -DHAVE_PTHREADS=1 -DBUILD_FOR_ANDROID
LOCAL_CPPFLAGS += -std=c++11

LOCAL_STATIC_LIBRARIES += cutils_static \
						mindroid_static

include $(BUILD_STATIC_LIBRARY)

$(call import-module,libs/Andrender/libs/cutils)
$(call import-module,libs/mindroid)