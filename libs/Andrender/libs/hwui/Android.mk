LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

# Only build libhwui when USE_OPENGL_RENDERER is
# defined in the current device/board configuration
LOCAL_SRC_FILES:= \
	utils/SortedListImpl.cpp \
	FontRenderer.cpp \
	GammaFontRenderer.cpp \
	Caches.cpp \
	DisplayListLogBuffer.cpp \
	DisplayListRenderer.cpp \
	FboCache.cpp \
	GradientCache.cpp \
	LayerCache.cpp \
	LayerRenderer.cpp \
	Matrix.cpp \
	OpenGLRenderer.cpp \
	Patch.cpp \
	PatchCache.cpp \
	PathCache.cpp \
	Program.cpp \
	ProgramCache.cpp \
	ResourceCache.cpp \
	ShapeCache.cpp \
	SkiaColorFilter.cpp \
	SkiaShader.cpp \
	Snapshot.cpp \
	TextureCache.cpp \
	TextDropShadowCache.cpp

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/../../..
LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/../../..

LOCAL_CFLAGS += -DUSE_OPENGL_RENDERER -DGL_GLEXT_PROTOTYPES -DHAVE_PTHREADS=1 -DDEBUG_OPENGL=1 -DBUILD_FOR_ANDROID
LOCAL_CPPFLAGS += -std=c++11

LOCAL_STATIC_LIBRARIES := \
	cutils_static \
	utils_static \
	ui_static \
	skia_static

LOCAL_MODULE := hwui_static
LOCAL_MODULE_FILENAME := libhwui

ifndef HWUI_COMPILE_SYMBOLS
	LOCAL_CFLAGS += -fvisibility=hidden
endif

ifdef HWUI_COMPILE_FOR_PERF
	LOCAL_CFLAGS += -fno-omit-frame-pointer -marm -mapcs
endif

include $(BUILD_STATIC_LIBRARY)

$(call import-module,libs/Andrender/libs/utils)
$(call import-module,libs/Andrender/libs/cutils)
$(call import-module,libs/skia)
$(call import-module,libs/Andrender/libs/ui)