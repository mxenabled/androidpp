//
//  Bitmap.cpp
//  cocos2dx
//
//  Created by Saul Howard on 11/14/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#include "Bitmap.h"

#include "Android/utils/Exceptions.h"

#include <SkDither.h>

ANDROID_BEGIN

/**
 * Returns a mutable bitmap with the specified width and height.  Its
 * initial density is as per {@link #getDensity}.
 *
 * @param width    The width of the bitmap
 * @param height   The height of the bitmap
 * @param config   The bitmap config to create.
 * @throws IllegalArgumentException if the width or height are <= 0
 */
Bitmap *Bitmap::createBitmap(int width, int height, Config config) {
    return createBitmap(width, height, config, true);
}

/**
 * Returns a mutable bitmap with the specified width and height.  Its
 * initial density is as per {@link #getDensity}.
 *
 * @param width    The width of the bitmap
 * @param height   The height of the bitmap
 * @param config   The bitmap config to create.
 * @param hasAlpha If the bitmap is ARGB_8888 this flag can be used to mark the
 *                 bitmap as opaque. Doing so will clear the bitmap in black
 *                 instead of transparent.
 *
 * @throws IllegalArgumentException if the width or height are <= 0
 */
Bitmap *Bitmap::createBitmap(int width, int height, Config config, bool hasAlpha) {
    return createBitmap(NULL, width, height, config, hasAlpha);
}

/**
 * Returns a mutable bitmap with the specified width and height.  Its
 * initial density is determined from the given {@link DisplayMetrics}.
 *
 * @param display  Display metrics for the display this bitmap will be
 *                 drawn on.
 * @param width    The width of the bitmap
 * @param height   The height of the bitmap
 * @param config   The bitmap config to create.
 * @param hasAlpha If the bitmap is ARGB_8888 this flag can be used to mark the
 *                 bitmap as opaque. Doing so will clear the bitmap in black
 *                 instead of transparent.
 *
 * @throws IllegalArgumentException if the width or height are <= 0
 */
Bitmap *Bitmap::createBitmap(DisplayMetrics *display, int width, int height,
                                   Config config, bool hasAlpha) {
    
    if (width <= 0 || height <= 0) {
        throw new IllegalArgumentException("width and height must be > 0");
    }
    
    Bitmap *bm = new Bitmap();
    bm->setConfig(config, width, height);
    
    if (display) {
        bm->mDensity = display->densityDpi;
    }
    
    if (config == Config::kARGB_8888_Config && !hasAlpha) {
        bm->eraseColor(0xff000000);
        bm->setIsOpaque(!hasAlpha);
    } else {
        // No need to initialize it to zeroes; it is backed by a VM byte array
        // which is by definition preinitialized to all zeroes.
        //
        //nativeErase(bm.mNativeBitmap, 0);
    }
    
    return bm;
}

int Bitmap::getScaledHeight(int targetDensity) {
    return scaleFromDensity(getHeight(), mDensity, targetDensity);
}

int Bitmap::getScaledWidth(int targetDensity) {
    return scaleFromDensity(getWidth(), mDensity, targetDensity);
}

int Bitmap::getScaledHeight(shared_ptr<Canvas> canvas) {
    return 0;
}

int Bitmap::getScaledWidth(shared_ptr<Canvas> canvas) {
    return 0;
}

int Bitmap::getScaledHeight(DisplayMetrics *metrics) {
    return scaleFromDensity(getHeight(), mDensity, metrics->densityDpi);
}

int Bitmap::getScaledWidth(DisplayMetrics *metrics) {
    return scaleFromDensity(getWidth(), mDensity, metrics->densityDpi);
}

bool Bitmap::setPixels(const SkColor* src, int srcStride,
                            int x, int y, int width, int height,
                            const Bitmap& dstBitmap) {
    SkAutoLockPixels alp(dstBitmap);
    void* dst = dstBitmap.getPixels();
    FromColorProc proc = ChooseFromColorProc(dstBitmap.config());
    
    if (NULL == dst || NULL == proc) {
        return false;
    }
    
    // reset to to actual choice from caller
    dst = dstBitmap.getAddr(x, y);
    // now copy/convert each scanline
    for (int y = 0; y < height; y++) {
        proc(dst, src, width, x, y);
        src += srcStride;
        dst = (char*)dst + dstBitmap.rowBytes();
    }
    
    dstBitmap.notifyPixelsChanged();
    
    return true;
}

Bitmap::FromColorProc Bitmap::ChooseFromColorProc(Config config) {
    switch (config) {
        case SkBitmap::kARGB_8888_Config:
            return FromColor_D32;
        case SkBitmap::kARGB_4444_Config:
            return FromColor_D4444;
        case SkBitmap::kRGB_565_Config:
            return FromColor_D565;
        default:
            break;
    }
    return NULL;
}

void Bitmap::FromColor_D32(void* dst, const SkColor src[], int width,
                          int, int) {
    SkPMColor* d = (SkPMColor*)dst;
    
    for (int i = 0; i < width; i++) {
        *d++ = SkPreMultiplyColor(*src++);
    }
}

void Bitmap::FromColor_D565(void* dst, const SkColor src[], int width,
                           int x, int y) {
    uint16_t* d = (uint16_t*)dst;
    
    DITHER_565_SCAN(y);
    for (int stop = x + width; x < stop; x++) {
        SkColor c = *src++;
        *d++ = SkDitherRGBTo565(SkColorGetR(c), SkColorGetG(c), SkColorGetB(c),
                                DITHER_VALUE(x));
    }
}

void Bitmap::FromColor_D4444(void* dst, const SkColor src[], int width,
                            int x, int y) {
    SkPMColor16* d = (SkPMColor16*)dst;
    
    DITHER_4444_SCAN(y);
    for (int stop = x + width; x < stop; x++) {
        SkPMColor c = SkPreMultiplyColor(*src++);
        *d++ = SkDitherARGB32To4444(c, DITHER_VALUE(x));
        //        *d++ = SkPixel32ToPixel4444(c);
    }
}

int Bitmap::scaleFromDensity(int size, int sdensity, int tdensity) {
    if (sdensity == DENSITY_NONE || tdensity == DENSITY_NONE || sdensity == tdensity) {
        return size;
    }
    
    // Scale by tdensity / sdensity, rounding up.
    return ((size * tdensity) + (sdensity >> 1)) / sdensity;
}

ANDROID_END