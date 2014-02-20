APP_STL := gnustl_static
APP_CPPFLAGS += -frtti

# The ARMv7 is significanly faster due to the use of the hardware FPU
APP_ABI := armeabi
APP_PLATFORM := android-9
APP_OPTIM := debug

NDK_TOOLCHAIN_VERSION := clang3.3