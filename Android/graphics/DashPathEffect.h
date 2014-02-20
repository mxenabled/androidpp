//
//  DashPathEffect.h
//  cocos2dx
//
//  Created by Saul Howard on 11/19/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef cocos2dx_DashPathEffect_h
#define cocos2dx_DashPathEffect_h

#include "AndroidMacros.h"

#include <SkDashPathEffect.h>

ANDROID_BEGIN

class DashPathEffect : public SkDashPathEffect {
    
public:
    DashPathEffect(const float intervals[], int count, float phase) : SkDashPathEffect(intervals, count, SkFloatToScalar(phase)) {}
    
};

ANDROID_END

#endif
