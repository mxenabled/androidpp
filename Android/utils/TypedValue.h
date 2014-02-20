//
//  TypedValue.h
//  Androidpp
//
//  Created by Saul Howard on 1/30/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef Androidpp_TypedValue_h
#define Androidpp_TypedValue_h

#include "AndroidMacros.h"

#include "Android/utils/DisplayMetrics.h"

ANDROID_BEGIN

class TypedValue {
    
public:
    
    /** Complex data: bit location of unit information. */
    static const int COMPLEX_UNIT_SHIFT = 0;
    /** Complex data: mask to extract unit information (after shifting by
     *  {@link #COMPLEX_UNIT_SHIFT}). This gives us 16 possible types, as
     *  defined below. */
    static const int COMPLEX_UNIT_MASK = 0xf;
    
    /** {@link #TYPE_DIMENSION} complex unit: Value is raw pixels. */
    static const int COMPLEX_UNIT_PX = 0;
    /** {@link #TYPE_DIMENSION} complex unit: Value is Device Independent
     *  Pixels. */
    static const int COMPLEX_UNIT_DIP = 1;
    /** {@link #TYPE_DIMENSION} complex unit: Value is a scaled pixel. */
    static const int COMPLEX_UNIT_SP = 2;
    /** {@link #TYPE_DIMENSION} complex unit: Value is in points. */
    static const int COMPLEX_UNIT_PT = 3;
    /** {@link #TYPE_DIMENSION} complex unit: Value is in inches. */
    static const int COMPLEX_UNIT_IN = 4;
    /** {@link #TYPE_DIMENSION} complex unit: Value is in millimeters. */
    static const int COMPLEX_UNIT_MM = 5;
    
    /**
     * Converts an unpacked complex data value holding a dimension to its const floating
     * point value. The two parameters <var>unit</var> and <var>value</var>
     * are as in {@link #TYPE_DIMENSION}.
     *
     * @param unit The unit to convert from.
     * @param value The value to apply the unit to.
     * @param metrics Current display metrics to use in the conversion --
     *                supplies display density and scaling information.
     *
     * @return The complex floating point value multiplied by the appropriate
     * metrics depending on its unit.
     */
    static float applyDimension(int unit, float value, DisplayMetrics metrics) {
        
        switch (unit) {
            case COMPLEX_UNIT_PX:
                return value;
            case COMPLEX_UNIT_DIP:
                return value * metrics.density;
            case COMPLEX_UNIT_SP:
                return value * metrics.scaledDensity;
            case COMPLEX_UNIT_PT:
                return value * metrics.xdpi * (1.0f / 72);
            case COMPLEX_UNIT_IN:
                return value * metrics.xdpi;
            case COMPLEX_UNIT_MM:
                return value * metrics.xdpi * (1.0f / 25.4f);
        }
        return 0;
    }
};

ANDROID_END

#endif
