//
//  Bitmap.h
//  cocos2dx
//
//  Created by Saul Howard on 11/14/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef __cocos2dx__Bitmap__
#define __cocos2dx__Bitmap__

#include "AndroidMacros.h"

#include "Android/graphics/Canvas.h"
#include "Android/utils/DisplayMetrics.h"

#include <SkBitmap.h>

#include <vector>
#include <memory>

using namespace std;

ANDROID_BEGIN

class Bitmap : public SkBitmap {
    
public:
    static const int DENSITY_NONE = 0;
    typedef void (*FromColorProc)(void* dst, const SkColor src[], int width,
    int x, int y);
    
    int getScaledWidth(int targetDensity);
    int getScaledHeight(int targetDensity);
    int getScaledWidth(shared_ptr<Canvas> canvas);
    int getScaledHeight(shared_ptr<Canvas> canvas);
    int getScaledWidth(DisplayMetrics *metrics);
    int getScaledHeight(DisplayMetrics *metrics);
    bool hasAlpha() { return !isOpaque(); }
    
    static Bitmap *createBitmap(int width, int height, Config config);
    
    static bool setPixels(const SkColor* src, int srcStride,
                                  int x, int y, int width, int height,
                                  const Bitmap& dstBitmap);
    static FromColorProc ChooseFromColorProc(Config config);
    static void FromColor_D32(void* dst, const SkColor src[], int width,
                              int, int);
    static void FromColor_D565(void* dst, const SkColor src[], int width,
                               int x, int y);
    static void FromColor_D4444(void* dst, const SkColor src[], int width,
                                int x, int y);
    
    static int scaleFromDensity(int size, int sdensity, int tdensity);
    
    /** Returns the bitmap's width */
    int getWidth() {
        return width();
    }
    
    /** Returns the bitmap's height */
    int getHeight() {
        return height();
    }
    
private:
    
    int mDensity = 0;
    
    static Bitmap *createBitmap(int width, int height, Config config, bool hasAlpha);
    static Bitmap *createBitmap(DisplayMetrics *display, int width, int height,
                                Config config, bool hasAlpha);
};

ANDROID_END

#endif /* defined(__cocos2dx__Bitmap__) */
