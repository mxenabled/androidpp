//
//  ColorFilter.h
//  cocos2dx
//
//  Created by Saul Howard on 11/21/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef __cocos2dx__ColorFilter__
#define __cocos2dx__ColorFilter__

#include "AndroidMacros.h"

#include <SkiaColorFilter.h>

ANDROID_BEGIN

class ColorFilter {
public:
//    SkiaColorFilter *colorFilter = NULL;
    
    //    static inline SkiaColorFilter *createPorterDuffFilter(SkColorFilter *skFilter, int srcColor, SkPorterDuff::Mode mode) {
    //        return new SkiaBlendFilter(skFilter, srcColor, SkPorterDuff::ToXfermodeMode(mode));
    //    }
    //
    //    static inline SkiaColorFilter* glCreateLightingFilter(SkColorFilter *skFilter, int mul, int add) {
    //        return new SkiaLightingFilter(skFilter, mul, add);
    //    }
    //
    //    static SkiaColorFilter* glCreateColorMatrixFilter(SkColorFilter *skFilter, float *src) {
    //
    //        float* colorMatrix = new float[16];
    //        memcpy(colorMatrix, src, 4 * sizeof(float));
    //        memcpy(&colorMatrix[4], &src[5], 4 * sizeof(float));
    //        memcpy(&colorMatrix[8], &src[10], 4 * sizeof(float));
    //        memcpy(&colorMatrix[12], &src[15], 4 * sizeof(float));
    //
    //        float* colorVector = new float[4];
    //        colorVector[0] = src[4];
    //        colorVector[1] = src[9];
    //        colorVector[2] = src[14];
    //        colorVector[3] = src[19];
    //
    //        return new SkiaColorMatrixFilter(skFilter, colorMatrix, colorVector);
    //    }
    //
    //    static SkColorFilter* CreatePorterDuffFilter(int srcColor, SkPorterDuff::Mode mode) {
    //        return SkColorFilter::CreateModeFilter(srcColor, SkPorterDuff::ToXfermodeMode(mode));
    //    }
    //
    //    static SkColorFilter* CreateLightingFilter(int mul, int add) {
    //        return SkColorFilter::CreateLightingFilter(mul, add);
    //    }
    
    //    static SkColorFilter* CreateColorMatrixFilter(float* src) {
    //
    //        SkFixed array[20];
    //        for (int i = 0; i < 20; i++) {
    //            array[i] = SkFloatToScalar(src[i]);
    //        }
    //        return new SkColorMatrixFilter(array);
    //    }
};

ANDROID_END

#endif /* defined(__cocos2dx__ColorFilter__) */
