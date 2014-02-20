//
//  Path.h
//  cocos2dx
//
//  Created by Saul Howard on 11/19/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef __cocos2dx__Path__
#define __cocos2dx__Path__

#include "AndroidMacros.h"

#include <SkPath.h>

#include <vector>

ANDROID_BEGIN

class RectF;

class Path : public SkPath {
    
public:
    
    void addRoundRect(RectF rect, std::vector<float> array, Direction direction);
    void arcTo(const RectF oval, float startAngle, float sweepAngle, bool forceMoveTo);
    void addOval(const RectF oval, Direction dir = kCW_Direction);
    void computeBounds(RectF &rect, bool exact);
};

ANDROID_END

#endif /* defined(__cocos2dx__Path__) */
