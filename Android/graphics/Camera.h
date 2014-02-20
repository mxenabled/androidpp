//
//  Camera.h
//  cocos2dx
//
//  Created by Saul Howard on 11/25/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef cocos2dx_Camera_h
#define cocos2dx_Camera_h

#include "AndroidMacros.h"

#include <SkCamera.h>

ANDROID_BEGIN

class Camera : public Sk3DView {

public:
    
    void rotate(float x, float y, float z) {
        SkScalar sx = SkFloatToScalar(x);
        SkScalar sy = SkFloatToScalar(y);
        SkScalar sz = SkFloatToScalar(z);
        
        Sk3DView::rotateX(sx);
        Sk3DView::rotateY(sy);
        Sk3DView::rotateZ(sz);
    }
};

ANDROID_END

#endif
