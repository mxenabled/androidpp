/*
 * File:   LayoutParams.h
 * Author: saulhoward
 *
 * Created on October 15, 2013, 3:02 PM
 */

#ifndef LAYOUTPARAMS_H
#define	LAYOUTPARAMS_H

#include "AndroidMacros.h"

#include "Android/utils/AttributeSet.h"
#include "Android/content/Context.h"
#include "Android/internal/R.h"

ANDROID_BEGIN

class LayoutParams {
    
public:
    /**
     * Horizontal layout direction of this view is from Left to Right.
     * Use with {@link #setLayoutDirection}.
     */
    static const int LAYOUT_DIRECTION_LTR = 0;
    
    /**
     * Horizontal layout direction of this view is from Right to Left.
     * Use with {@link #setLayoutDirection}.
     */
    static const int LAYOUT_DIRECTION_RTL = 1;
    
    const static int MATCH_PARENT = -1;
    const static int WRAP_CONTENT = -2;
    
    int width = 0;
    int height = 0;
    
    LayoutParams() {
    }
    
    LayoutParams(int width, int height) {
        this->width = width;
        this->height = height;
    }
    
    LayoutParams(LayoutParams *source) {
        width = source->width;
        height = source->height;
    }
    
    LayoutParams(Context *context, AttributeSet *attrs) {
        setBaseAttributes(context, attrs);
    }
    
    virtual void resolveLayoutDirection(int layoutDirection) {
    }
    
protected:
    
    void setBaseAttributes(Context *context, AttributeSet *attrs) {
        width = attrs->getDimensionValue(context->getResources(), R::layout::layout_width, MATCH_PARENT);
        height = attrs->getDimensionValue(context->getResources(), R::layout::layout_height, MATCH_PARENT);
    }
};

ANDROID_END

#endif	/* LAYOUTPARAMS_H */

