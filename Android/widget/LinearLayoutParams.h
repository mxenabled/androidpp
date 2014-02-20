/*
 * File:   LinearLayoutParams.h
 * Author: saulhoward
 *
 * Created on October 16, 2013, 11:20 PM
 */

#ifndef LINEARLAYOUTPARAMS_H
#define	LINEARLAYOUTPARAMS_H

#include "AndroidMacros.h"

#include "Android/view/MarginLayoutParams.h"

ANDROID_BEGIN

class LinearLayoutParams : public MarginLayoutParams {

    public:
        float weight;
        int gravity = -1;
    
        LinearLayoutParams(Context *context, AttributeSet *attrs) : MarginLayoutParams(context, attrs) {
            weight = attrs->getAttributeFloatValue(R::styleable::layout_weight, 0);
            gravity = attrs->getAttributeIntValue(R::styleable::layout_gravity, -1);
        }

        LinearLayoutParams(int width, int height) : MarginLayoutParams(width, height) {
            weight = 0;
        }

        LinearLayoutParams(int width, int height, float weight) : MarginLayoutParams(width, height) {
            this->weight = weight;
        }

        LinearLayoutParams(LayoutParams *p) : MarginLayoutParams(p) {
        }

        LinearLayoutParams(MarginLayoutParams *source) : MarginLayoutParams(source) {
        }


        virtual ~LinearLayoutParams() {}
};

ANDROID_END

#endif	/* LINEARLAYOUTPARAMS_H */

