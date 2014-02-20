//
//  DisplayMetrics.cpp
//  cocos2dx
//
//  Created by Saul Howard on 11/4/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#include "DisplayMetrics.h"

ANDROID_BEGIN

const float DisplayMetrics::DENSITY_DEFAULT_SCALE = 1.0f / DENSITY_DEFAULT;

DisplayMetrics::DisplayMetrics() {
    setToDefaults();
}

DisplayMetrics::~DisplayMetrics() {
    
}

void DisplayMetrics::setToDefaults() {
    widthPixels = 0;
    heightPixels = 0;
    scaledDensity = density;
}

ANDROID_END