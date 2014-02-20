//
//  Shader.h
//  cocos2dx
//
//  Created by Saul Howard on 11/19/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef cocos2dx_Shader_h
#define cocos2dx_Shader_h

#include "AndroidMacros.h"

#include "Android/graphics/Bitmap.h"

#include <SkShader.h>
#include <SkiaShader.h>

#include <string>
#include <memory>

using namespace std;

ANDROID_BEGIN

using namespace android::uirenderer;

class Shader : public SkShader {
public:
    
    Shader(SkShader *instance, SkiaShader *shader) {
        this->instance = instance;
        this->shader = shader;
    }
    
    virtual ~Shader() {
        SkSafeUnref(instance);
        if (Caches::hasInstance()) {
            Caches::getInstance().resourceCache.destructor(shader);
        } else {
            delete shader;
        }
    }
    
    SkShader *instance;
    SkiaShader *shader;
    
    // Never used just implemented due to abstract base class
    virtual void shadeSpan(int x, int y, SkPMColor[], int count) {}
    virtual Factory getFactory() { return Factory(); }
    
    static inline TileMode getTileMode(string tileMode, TileMode defaultTileMode) {
        
        if (tileMode.compare("clamp") == 0) {
            return TileMode::kClamp_TileMode;
        } else if (tileMode.compare("repeat") == 0) {
            return TileMode::kRepeat_TileMode;
        } else if (tileMode.compare("mirror") == 0) {
            return TileMode::kMirror_TileMode;
        }
        
        return defaultTileMode;
    }
    
    static inline Shader *createBitmapShader(Bitmap *bitmap, TileMode tileModeX, TileMode tileModeY) {
        
        SkShader *shader = SkShader::CreateBitmapShader(*bitmap, tileModeX, tileModeY);
        
        SkiaShader* skiaShader = new SkiaBitmapShader(bitmap, shader,
                                                      tileModeX, tileModeY,
                                                      NULL, (shader->getFlags() & SkShader::kOpaqueAlpha_Flag) == 0);
        
        return new Shader(shader, skiaShader);
    }
};

ANDROID_END

#endif
