LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := android_static

LOCAL_MODULE_FILENAME := libandroid

LOCAL_SRC_FILES := \
	app/Activity.cpp \
	app/DecorView.cpp \
	app/Window.cpp \
	content/Context.cpp \
	content/res/ColorStateList.cpp \
	content/res/Configuration.cpp \
	content/res/Resources.cpp \
	graphics/Bitmap.cpp \
	graphics/Canvas.cpp \
	graphics/Color.cpp \
	graphics/ColorFilter.cpp \
	graphics/HardwareRenderer.cpp \
	graphics/HarfbuzzSkia.cpp \
	graphics/Insets.cpp \
	graphics/NinePatch.cpp \
	graphics/Paint.cpp \
	graphics/Path.cpp \
	graphics/TextLayout.cpp \
	graphics/TextLayoutCache.cpp \
	graphics/Typeface.cpp \
	graphics/drawable/BitmapDrawable.cpp \
	graphics/drawable/ColorDrawable.cpp \
	graphics/drawable/Drawable.cpp \
	graphics/drawable/DrawableContainer.cpp \
	graphics/drawable/DrawableContainerState.cpp \
	graphics/drawable/GradientDrawable.cpp \
	graphics/drawable/LayerDrawable.cpp \
	graphics/drawable/StateListDrawable.cpp \
	graphics/drawable/TransitionDrawable.cpp \
	internal/R.cpp \
	text/BoringLayout.cpp \
	text/Directions.cpp \
	text/Editable.cpp \
	text/InputFilter.cpp \
	text/Layout.cpp \
	text/MeasuredText.cpp \
	text/Selection.cpp \
	text/Spannable.cpp \
	text/SpannableString.cpp \
	text/SpannableStringBuilder.cpp \
	text/SpannableStringInternal.cpp \
	text/SpannedString.cpp \
	text/StaticLayout.cpp \
	text/String.cpp \
	text/TextDirectionHeuristics.cpp \
	text/TextPaint.cpp \
	text/TextUtils.cpp \
	text/method/ReplacementTransformationMethod.cpp \
	text/style/CharacterStyle.cpp \
	text/style/SuggestionSpan.cpp \
	utils/CCPullParser.cpp \
	utils/DisplayMetrics.cpp \
	utils/Object.cpp \
	utils/StateSet.cpp \
	view/AttachInfo.cpp \
	view/GLES20DisplayList.cpp \
	view/Gravity.cpp \
	view/KeyEvent.cpp \
	view/LayoutInflater.cpp \
	view/MotionEvent.cpp \
	view/TouchTarget.cpp \
	view/View.cpp \
	view/ViewGroup.cpp \
	view/ViewRootImpl.cpp \
	view/ViewTreeObserver.cpp \
	view/VelocityTracker.cpp \
	view/animation/Animation.cpp \
	widget/AbsListView.cpp \
	widget/Filter.cpp \
	widget/FrameLayout.cpp \
	widget/HeaderViewListAdapter.cpp \
	widget/LinearLayout.cpp \
	widget/ImageView.cpp \
	widget/RelativeLayout.cpp \
	widget/TextView.cpp

#LOCAL_WHOLE_STATIC_LIBRARIES += cocos2dx_static
LOCAL_STATIC_LIBRARIES := hwui_static \
						cocos2dx_static \
						mindroid_static \
						libharfbuzz \
						libicuuc \
						libicui18n
						
#LOCAL_SHARED_LIBRARIES := \
#        libharfbuzz \
#        libicuuc \
#        libicui18n

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH) \
	$(LOCAL_PATH)/.. \
	$(LOCAL_PATH)/../libs/icu4c/common \
	$(LOCAL_PATH)/../libs/harfbuzz/src \
	$(LOCAL_PATH)/../libs/harfbuzz/contrib

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
	$(LOCAL_PATH)/.. \
	$(LOCAL_PATH)/../libs/icu4c/common \
	$(LOCAL_PATH)/../libs/harfbuzz/src \
	$(LOCAL_PATH)/../libs/harfbuzz/contrib

LOCAL_CFLAGS := -fexceptions -DBUILD_FOR_ANDROID -DCOCOS2D_DEBUG
LOCAL_CPPFLAGS += -std=c++11

include $(BUILD_STATIC_LIBRARY)

$(call import-module,libs/cocos2d-x/cocos2dx)
$(call import-module,libs/Andrender/libs/hwui)
$(call import-module,libs/mindroid)
$(call import-module,libs/harfbuzz)
$(call import-module,libs/icu4c)
