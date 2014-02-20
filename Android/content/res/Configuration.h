//
//  Configuration.h
//  Androidpp
//
//  Created by Saul Howard on 2/4/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef Androidpp_Configuration_h
#define Androidpp_Configuration_h

#include "AndroidMacros.h"

ANDROID_BEGIN

class Configuration {
    
public:
    
    /** Constant for {@link #screenLayout}: bits that encode the size. */
    static const int SCREENLAYOUT_SIZE_MASK = 0x0f;
    /** Constant for {@link #screenLayout}: a {@link #SCREENLAYOUT_SIZE_MASK}
     * value indicating that no size has been set. */
    static const int SCREENLAYOUT_SIZE_UNDEFINED = 0x00;
    /** Constant for {@link #screenLayout}: a {@link #SCREENLAYOUT_SIZE_MASK}
     * value indicating the screen is at least approximately 320x426 dp units,
     * corresponds to the
     * <a href="{@docRoot}guide/topics/resources/providing-resources.html#ScreenSizeQualifier">small</a>
     * resource qualifier.
     * See <a href="{@docRoot}guide/practices/screens_support.html">Supporting
     * Multiple Screens</a> for more information. */
    static const int SCREENLAYOUT_SIZE_SMALL = 0x01;
    /** Constant for {@link #screenLayout}: a {@link #SCREENLAYOUT_SIZE_MASK}
     * value indicating the screen is at least approximately 320x470 dp units,
     * corresponds to the
     * <a href="{@docRoot}guide/topics/resources/providing-resources.html#ScreenSizeQualifier">normal</a>
     * resource qualifier.
     * See <a href="{@docRoot}guide/practices/screens_support.html">Supporting
     * Multiple Screens</a> for more information. */
    static const int SCREENLAYOUT_SIZE_NORMAL = 0x02;
    /** Constant for {@link #screenLayout}: a {@link #SCREENLAYOUT_SIZE_MASK}
     * value indicating the screen is at least approximately 480x640 dp units,
     * corresponds to the
     * <a href="{@docRoot}guide/topics/resources/providing-resources.html#ScreenSizeQualifier">large</a>
     * resource qualifier.
     * See <a href="{@docRoot}guide/practices/screens_support.html">Supporting
     * Multiple Screens</a> for more information. */
    static const int SCREENLAYOUT_SIZE_LARGE = 0x03;
    /** Constant for {@link #screenLayout}: a {@link #SCREENLAYOUT_SIZE_MASK}
     * value indicating the screen is at least approximately 720x960 dp units,
     * corresponds to the
     * <a href="{@docRoot}guide/topics/resources/providing-resources.html#ScreenSizeQualifier">xlarge</a>
     * resource qualifier.
     * See <a href="{@docRoot}guide/practices/screens_support.html">Supporting
     * Multiple Screens</a> for more information.*/
    static const int SCREENLAYOUT_SIZE_XLARGE = 0x04;
    
    /** Constant for {@link #screenLayout}: bits that encode the aspect ratio. */
    static const int SCREENLAYOUT_LONG_MASK = 0x30;
    /** Constant for {@link #screenLayout}: a {@link #SCREENLAYOUT_LONG_MASK}
     * value indicating that no size has been set. */
    static const int SCREENLAYOUT_LONG_UNDEFINED = 0x00;
    /** Constant for {@link #screenLayout}: a {@link #SCREENLAYOUT_LONG_MASK}
     * value that corresponds to the
     * <a href="{@docRoot}guide/topics/resources/providing-resources.html#ScreenAspectQualifier">notlong</a>
     * resource qualifier. */
    static const int SCREENLAYOUT_LONG_NO = 0x10;
    /** Constant for {@link #screenLayout}: a {@link #SCREENLAYOUT_LONG_MASK}
     * value that corresponds to the
     * <a href="{@docRoot}guide/topics/resources/providing-resources.html#ScreenAspectQualifier">long</a>
     * resource qualifier. */
    static const int SCREENLAYOUT_LONG_YES = 0x20;
    
    /** Constant for {@link #screenLayout}: bits that encode the layout direction. */
    static const int SCREENLAYOUT_LAYOUTDIR_MASK = 0xC0;
    /** Constant for {@link #screenLayout}: bits shift to get the layout direction. */
    static const int SCREENLAYOUT_LAYOUTDIR_SHIFT = 6;
    /** Constant for {@link #screenLayout}: a {@link #SCREENLAYOUT_LAYOUTDIR_MASK}
     * value indicating that no layout dir has been set. */
    static const int SCREENLAYOUT_LAYOUTDIR_UNDEFINED = 0x00;
    /** Constant for {@link #screenLayout}: a {@link #SCREENLAYOUT_LAYOUTDIR_MASK}
     * value indicating that a layout dir has been set to LTR. */
    static const int SCREENLAYOUT_LAYOUTDIR_LTR = 0x01 << SCREENLAYOUT_LAYOUTDIR_SHIFT;
    /** Constant for {@link #screenLayout}: a {@link #SCREENLAYOUT_LAYOUTDIR_MASK}
     * value indicating that a layout dir has been set to RTL. */
    static const int SCREENLAYOUT_LAYOUTDIR_RTL = 0x02 << SCREENLAYOUT_LAYOUTDIR_SHIFT;
    
    /** Constant for {@link #screenLayout}: a value indicating that screenLayout is undefined */
    static const int SCREENLAYOUT_UNDEFINED = SCREENLAYOUT_SIZE_UNDEFINED |
    SCREENLAYOUT_LONG_UNDEFINED | SCREENLAYOUT_LAYOUTDIR_UNDEFINED;
    
    /**
     * Bit mask of overall layout of the screen.  Currently there are two
     * fields:
     * <p>The {@link #SCREENLAYOUT_SIZE_MASK} bits define the overall size
     * of the screen.  They may be one of
     * {@link #SCREENLAYOUT_SIZE_SMALL}, {@link #SCREENLAYOUT_SIZE_NORMAL},
     * {@link #SCREENLAYOUT_SIZE_LARGE}, or {@link #SCREENLAYOUT_SIZE_XLARGE}.
     *
     * <p>The {@link #SCREENLAYOUT_LONG_MASK} defines whether the screen
     * is wider/taller than normal.  They may be one of
     * {@link #SCREENLAYOUT_LONG_NO} or {@link #SCREENLAYOUT_LONG_YES}.
     *
     * <p>The {@link #SCREENLAYOUT_LAYOUTDIR_MASK} defines whether the screen layout
     * is either LTR or RTL.  They may be one of
     * {@link #SCREENLAYOUT_LAYOUTDIR_LTR} or {@link #SCREENLAYOUT_LAYOUTDIR_RTL}.
     *
     * <p>See <a href="{@docRoot}guide/practices/screens_support.html">Supporting
     * Multiple Screens</a> for more information.
     */
    int screenLayout = 0;
    
    Configuration() {
        setToDefaults();
    }
    
    /**
     * Return the layout direction. Will be either {@link View#LAYOUT_DIRECTION_LTR} or
     * {@link View#LAYOUT_DIRECTION_RTL}.
     *
     * @return Returns {@link View#LAYOUT_DIRECTION_RTL} if the configuration
     * is {@link #SCREENLAYOUT_LAYOUTDIR_RTL}, otherwise {@link View#LAYOUT_DIRECTION_LTR}.
     */
    int getLayoutDirection();
    
    /**
     * Set this object to the system defaults.
     */
    void setToDefaults() {
//        fontScale = 1;
//        mcc = mnc = 0;
//        locale = null;
//        userSetLocale = false;
//        touchscreen = TOUCHSCREEN_UNDEFINED;
//        keyboard = KEYBOARD_UNDEFINED;
//        keyboardHidden = KEYBOARDHIDDEN_UNDEFINED;
//        hardKeyboardHidden = HARDKEYBOARDHIDDEN_UNDEFINED;
//        navigation = NAVIGATION_UNDEFINED;
//        navigationHidden = NAVIGATIONHIDDEN_UNDEFINED;
//        orientation = ORIENTATION_UNDEFINED;
        screenLayout = SCREENLAYOUT_LAYOUTDIR_LTR;
//        uiMode = UI_MODE_TYPE_UNDEFINED;
//        screenWidthDp = compatScreenWidthDp = SCREEN_WIDTH_DP_UNDEFINED;
//        screenHeightDp = compatScreenHeightDp = SCREEN_HEIGHT_DP_UNDEFINED;
//        smallestScreenWidthDp = compatSmallestScreenWidthDp = SMALLEST_SCREEN_WIDTH_DP_UNDEFINED;
//        densityDpi = DENSITY_DPI_UNDEFINED;
//        seq = 0;
    }
    
    void setTo(Configuration o) {
//        fontScale = o.fontScale;
//        mcc = o.mcc;
//        mnc = o.mnc;
//        if (o.locale != null) {
//            locale = (Locale) o.locale.clone();
//        }
//        userSetLocale = o.userSetLocale;
//        touchscreen = o.touchscreen;
//        keyboard = o.keyboard;
//        keyboardHidden = o.keyboardHidden;
//        hardKeyboardHidden = o.hardKeyboardHidden;
//        navigation = o.navigation;
//        navigationHidden = o.navigationHidden;
//        orientation = o.orientation;
        screenLayout = o.screenLayout;
//        uiMode = o.uiMode;
//        screenWidthDp = o.screenWidthDp;
//        screenHeightDp = o.screenHeightDp;
//        smallestScreenWidthDp = o.smallestScreenWidthDp;
//        densityDpi = o.densityDpi;
//        compatScreenWidthDp = o.compatScreenWidthDp;
//        compatScreenHeightDp = o.compatScreenHeightDp;
//        compatSmallestScreenWidthDp = o.compatSmallestScreenWidthDp;
//        seq = o.seq;
    }
};

ANDROID_END

#endif
