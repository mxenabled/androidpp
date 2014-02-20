/*
 * File:   FrameLayoutParams.h
 * Author: saulhoward
 *
 * Created on October 18, 2013, 12:00 AM
 */

#ifndef FRAMELAYOUTPARAMS_H
#define	FRAMELAYOUTPARAMS_H

#include "AndroidMacros.h"

#include "Android/view/MarginLayoutParams.h"

ANDROID_BEGIN

class FrameLayoutParams : public MarginLayoutParams {

    public:

        int gravity = -1;

        FrameLayoutParams(int width, int height) : MarginLayoutParams(width, height) {
        }

        FrameLayoutParams(int width, int height, int gravity) : MarginLayoutParams(width, height) {
            this->gravity = gravity;
        }

        FrameLayoutParams(Context *c, AttributeSet *attrs) : MarginLayoutParams(c, attrs) {
            gravity = attrs->getAttributeIntValue(R::styleable::layout_gravity, -1);
        }

        FrameLayoutParams(LayoutParams *source) : MarginLayoutParams(source) {
        }

        FrameLayoutParams(MarginLayoutParams *source) : MarginLayoutParams(source) {
        }
};

ANDROID_END

#endif	/* FRAMELAYOUTPARAMS_H */

