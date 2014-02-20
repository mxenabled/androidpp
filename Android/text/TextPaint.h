//
//  TextPaint.h
//  Androidpp
//
//  Created by Saul Howard on 1/20/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef __Androidpp__TextPaint__
#define __Androidpp__TextPaint__

#include "AndroidMacros.h"

#include "Android/graphics/Paint.h"

#include <vector>

using namespace std;

ANDROID_BEGIN

/**
 * TextPaint is an extension of Paint that leaves room for some extra
 * data used during text measuring and drawing.
 */
class TextPaint : public Paint {
    
public:
    
    // Special value 0 means no background paint
    int bgColor;
    int baselineShift;
    int linkColor;
    vector<int> drawableState;
    float density = 1.0f;
    /**
     * Special value 0 means no custom underline
     * @hide
     */
    int underlineColor = 0;
    /**
     * Defined as a multiplier of the default underline thickness. Use 1.0f for default thickness.
     * @hide
     */
    float underlineThickness;
    
    TextPaint();
    
    TextPaint(Paint::Flags flags);
    
//    TextPaint(Paint *p);
    
    /**
     * Copy the fields from tp into this TextPaint, including the
     * fields inherited from Paint.
     */
//    void set(TextPaint *tp);
    
    /**
     * Defines a custom underline for this Paint.
     * @param color underline solid color
     * @param thickness underline thickness
     * @hide
     */
    void setUnderlineText(int color, float thickness);
};

ANDROID_END

#endif /* defined(__Androidpp__TextPaint__) */
