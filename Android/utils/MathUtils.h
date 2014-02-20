/*
 * File:   MathUtils.h
 * Author: saulhoward
 *
 * Created on October 22, 2013, 10:01 AM
 */

#ifndef MATHUTILS_H
#define	MATHUTILS_H

#include "AndroidMacros.h"

ANDROID_BEGIN

class MathUtils {
public:
    static float constrain(float amount, float low, float high) {
        return amount < low ? low : (amount > high ? high : amount);
    }
};


ANDROID_END

#endif	/* MATHUTILS_H */

