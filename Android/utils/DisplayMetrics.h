//
//  DisplayMetrics.h
//  cocos2dx
//
//  Created by Saul Howard on 11/4/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef __cocos2dx__DisplayMetrics__
#define __cocos2dx__DisplayMetrics__

#include "AndroidMacros.h"

ANDROID_BEGIN

class DisplayMetrics {
public:
    DisplayMetrics();
    virtual ~DisplayMetrics();
    
    static const int DENSITY_MEDIUM = 160;
    static const int DENSITY_DEFAULT = DENSITY_MEDIUM;
    static const float DENSITY_DEFAULT_SCALE;
    
    int widthPixels = 0;
    int heightPixels = 0;
    
    /**
     * The screen density expressed as dots-per-inch.  May be either
     * {@link #DENSITY_LOW}, {@link #DENSITY_MEDIUM}, or {@link #DENSITY_HIGH}.
     */
    int densityDpi = 0;
    
    /**
     * The logical density of the display.  This is a scaling factor for the
     * Density Independent Pixel unit, where one DIP is one pixel on an
     * approximately 160 dpi screen (for example a 240x320, 1.5"x2" screen),
     * providing the baseline of the system's display. Thus on a 160dpi screen
     * this density value will be 1; on a 120 dpi screen it would be .75; etc.
     *
     * <p>This value does not exactly follow the real screen size (as given by
     * {@link #xdpi} and {@link #ydpi}, but rather is used to scale the size of
     * the overall UI in steps based on gross changes in the display dpi.  For
     * example, a 240x320 screen will have a density of 1 even if its width is
     * 1.8", 1.3", etc. However, if the screen resolution is increased to
     * 320x480 but the screen size remained 1.5"x2" then the density would be
     * increased (probably to 1.5).
     *
     * @see #DENSITY_DEFAULT
     */
    float density = 0;
    
    /**
     * A scaling factor for fonts displayed on the display.  This is the same
     * as {@link #density}, except that it may be adjusted in smaller
     * increments at runtime based on a user preference for the font size.
     */
    float scaledDensity = 0;
    
    void setToDefaults();
    
    /**
     * The exact physical pixels per inch of the screen in the X dimension.
     */
    float xdpi;
    /**
     * The exact physical pixels per inch of the screen in the Y dimension.
     */
    float ydpi;
};

ANDROID_END

#endif /* defined(__cocos2dx__DisplayMetrics__) */
