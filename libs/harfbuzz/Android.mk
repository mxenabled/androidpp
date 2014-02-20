##
## Copyright (C) 2010 The Android Open Source Project
##
## Licensed under the Apache License, Version 2.0 (the "License");
## you may not use this file except in compliance with the License.
## You may obtain a copy of the License at
##
##      http://www.apache.org/licenses/LICENSE-2.0
##
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS,
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
## See the License for the specific language governing permissions and
## limitations under the License.
##

BASE_PATH := $(call my-dir)
LOCAL_PATH:= $(call my-dir)

#############################################################
#   build the harfbuzz library
#

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES:= \
        contrib/harfbuzz-freetype.c \
        contrib/harfbuzz-unicode-icu.c \
        contrib/harfbuzz-unicode.c \
        src/harfbuzz-buffer.c \
        src/harfbuzz-stream.c \
        src/harfbuzz-dump.c \
        src/harfbuzz-gdef.c \
        src/harfbuzz-gpos.c \
        src/harfbuzz-gsub.c \
        src/harfbuzz-impl.c \
        src/harfbuzz-open.c \
        src/harfbuzz-shaper.cpp \
        src/harfbuzz-tibetan.c \
        src/harfbuzz-khmer.c \
        src/harfbuzz-indic.cpp \
        src/harfbuzz-hebrew.c \
        src/harfbuzz-arabic.c \
        src/harfbuzz-hangul.c \
        src/harfbuzz-myanmar.c \
        src/harfbuzz-thai.c \
        src/harfbuzz-greek.c \
        src/harfbuzz-debug.c

LOCAL_STATIC_LIBRARIES := \
	cutils_static \
	utils_static \
	libft2 \
	libicuuc \
	libicui18n

#LOCAL_SHARED_LIBRARIES := \
#        libicuuc \
#        libicui18n

LOCAL_C_INCLUDES += \
        $(LOCAL_PATH)/src \
        $(LOCAL_PATH)/src/contrib \
        $(LOCAL_PATH)/../icu4c/common \
        $(LOCAL_PATH)/../freetype/include

ifeq ($(NO_FALLBACK_FONT),true)
        LOCAL_CFLAGS += -DNO_FALLBACK_FONT
endif

LOCAL_MODULE:= libharfbuzz

include $(BUILD_STATIC_LIBRARY)

$(call import-module,libs/Andrender/libs/utils)
$(call import-module,libs/Andrender/libs/cutils)
$(call import-module,libs/icu4c)
$(call import-module,libs/freetype)

