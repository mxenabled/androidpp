//
//  LinearGradient.h
//  cocos2dx
//
//  Created by Saul Howard on 11/19/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef cocos2dx_LinearGradient_h
#define cocos2dx_LinearGradient_h

#include "AndroidMacros.h"

#include "Android/graphics/Shader.h"

#include <SkShader.h>
#include <SkiaShader.h>
#include <SkGradientShader.h>

#include <vector>

ANDROID_BEGIN

class Gradient : public Shader {
    
public:
    
    static inline Gradient *createLinear(float x0, float y0, float x1, float y1, std::vector<int> colorArray, std::vector<float> posArray, Shader::TileMode tileMode) {
        
        SkPoint pts[2];
        pts[0].set(SkFloatToScalar(x0), SkFloatToScalar(y0));
        pts[1].set(SkFloatToScalar(x1), SkFloatToScalar(y1));
        
        float storedBounds[4] = {x0, y0, x1, y1};
        
        size_t count = colorArray.size();
        
        SkScalar* pos = convertPositions(posArray, count);
        SkColor* colors = convertColors(colorArray);
        
        SkShader* shader = SkGradientShader::CreateLinear(pts,
                                                          colors,
                                                          pos, count,
                                                          tileMode);
        
        SkiaShader* skiaShader = new SkiaLinearGradientShader(storedBounds, colors,
                                                              pos, count, shader, tileMode, NULL,
                                                              (shader->getFlags() & SkShader::kOpaqueAlpha_Flag) == 0);
        
        return (Gradient *) new Shader(shader, skiaShader);
    }
    
    static inline Gradient *createLinear(float x0, float y0, float x1, float y1, int color0, int color1, Shader::TileMode tileMode) {
        
        SkPoint pts[2];
        pts[0].set(SkFloatToScalar(x0), SkFloatToScalar(y0));
        pts[1].set(SkFloatToScalar(x1), SkFloatToScalar(y1));
        
        float storedBounds[4] = {x0, y0, x1, y1};
        
        float* storedPositions = new float[2];
        storedPositions[0] = 0.0f;
        storedPositions[1] = 1.0f;
        
        uint32_t* storedColors = new uint32_t[2];
        storedColors[0] = static_cast<uint32_t>(color0);
        storedColors[1] = static_cast<uint32_t>(color1);
        
        SkColor colors[2];
        colors[0] = color0;
        colors[1] = color1;
        
        SkShader* shader = SkGradientShader::CreateLinear(pts, colors, NULL, 2, (SkShader::TileMode)tileMode);
        
        SkiaShader* skiaShader = new SkiaLinearGradientShader(storedBounds, storedColors,
                                                              storedPositions, 2, shader, static_cast<SkShader::TileMode>(tileMode), NULL,
                                                              (shader->getFlags() & SkShader::kOpaqueAlpha_Flag) == 0);
        
        return (Gradient *) new Shader(shader, skiaShader);
    }
    
    static inline Gradient *createRadial(float x, float y, float radius, std::vector<int> colorArray, std::vector<float> posArray, Shader::TileMode tileMode) {
        
        SkPoint center;
        center.set(SkFloatToScalar(x), SkFloatToScalar(y));
        
        size_t count = colorArray.size();
        
        SkScalar* pos = convertPositions(posArray, count);
        SkColor* colors = convertColors(colorArray);
        
        SkShader* shader = SkGradientShader::CreateRadial(center,
                                                          SkFloatToScalar(radius),
                                                          colors,
                                                          pos, count,
                                                          tileMode);
        
        SkiaShader* skiaShader = new SkiaCircularGradientShader(x, y, radius, colors,
                                                                pos, count, shader, tileMode, NULL,
                                                                (shader->getFlags() & SkShader::kOpaqueAlpha_Flag) == 0);
        
        return (Gradient *) new Shader(shader, skiaShader);
    }
    
    static inline Gradient *createRadial(float x, float y, float radius, int color0, int color1, Shader::TileMode tileMode) {
        
        SkPoint center;
        center.set(SkFloatToScalar(x), SkFloatToScalar(y));
        
        SkColor colors[2];
        colors[0] = color0;
        colors[1] = color1;
        
        SkShader* shader = SkGradientShader::CreateRadial(center, SkFloatToScalar(radius), colors, NULL,
                                                     2, (SkShader::TileMode)tileMode);
        
        float* storedPositions = new float[2];
        storedPositions[0] = 0.0f;
        storedPositions[1] = 1.0f;
        
        uint32_t* storedColors = new uint32_t[2];
        storedColors[0] = static_cast<uint32_t>(color0);
        storedColors[1] = static_cast<uint32_t>(color1);
        
        SkiaShader* skiaShader = new SkiaCircularGradientShader(x, y, radius, storedColors,
                                                                storedPositions, 2, shader, (SkShader::TileMode) tileMode, NULL,
                                                                (shader->getFlags() & SkShader::kOpaqueAlpha_Flag) == 0);
        
        return (Gradient *) new Shader(shader, skiaShader);
    }
    
    static inline Gradient *createSweep(float x, float y, std::vector<int> colorArray, std::vector<float> posArray) {
        
        size_t count = colorArray.size();
        
        SkScalar* pos = convertPositions(posArray, count);
        SkColor* colors = convertColors(colorArray);
        
        SkShader* shader = SkGradientShader::CreateSweep(SkFloatToScalar(x),
                                                         SkFloatToScalar(y),
                                                         colors,
                                                         pos, count);
        
        SkiaShader* skiaShader = new SkiaSweepGradientShader(x, y, colors, pos, count,
                                                             shader, NULL, (shader->getFlags() & SkShader::kOpaqueAlpha_Flag) == 0);
        
        return (Gradient *) new Shader(shader, skiaShader);
    }
    
    static inline Gradient *createSweep(float x, float y, int color0, int color1) {
        
        SkColor colors[2];
        colors[0] = color0;
        colors[1] = color1;
        
        float* storedPositions = new float[2];
        storedPositions[0] = 0.0f;
        storedPositions[1] = 1.0f;
        
        uint32_t* storedColors = new uint32_t[2];
        storedColors[0] = static_cast<uint32_t>(color0);
        storedColors[1] = static_cast<uint32_t>(color1);
        
        SkShader* shader = SkGradientShader::CreateSweep(SkFloatToScalar(x), SkFloatToScalar(y),
                                                         colors, NULL, 2);
        
        SkiaShader* skiaShader = new SkiaSweepGradientShader(x, y, storedColors, storedPositions, 2,
                                                             shader, NULL, (shader->getFlags() & SkShader::kOpaqueAlpha_Flag) == 0);
        
        return (Gradient *) new Shader(shader, skiaShader);
    }
    
private:
    
    static inline SkColor *convertColors(std::vector<int> colors) {
        
        SkColor *skColors = new SkColor[colors.size()];
        
        for (int i = 0; i < colors.size(); i++) {
            skColors[i] = (uint32_t) colors[i];
        }
        
        return skColors;
    }
    
    static inline SkScalar *convertPositions(std::vector<float> positions, int count) {
        
        SkScalar *skPositions = new SkScalar[count];
        
        if (positions.empty()) {
            
            skPositions[0] = 0.0f;
            const float step = 1.0f / (count - 1);
            for (size_t i = 1; i < count - 1; i++) {
                skPositions[i] = step * i;
            }
            skPositions[count - 1] = 1.0f;
            
            return skPositions;
        }
        
        for (int i = 0; i < count; i++) {
            skPositions[i] = SkFloatToScalar(positions[i]);
        }
        
        return skPositions;
    }
};

ANDROID_END

#endif
