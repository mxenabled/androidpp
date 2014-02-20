//
//  Path.cpp
//  cocos2dx
//
//  Created by Saul Howard on 11/19/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#include "Path.h"

#include "Android/graphics/Rect.h"

#include <SkTemplates.h>

ANDROID_BEGIN

void Path::addRoundRect(RectF rect, std::vector<float> array, Direction direction) {
    size_t count = array.size();
    
    SkAutoSTMalloc<8, SkScalar> storage(count);
    SkScalar* pos = NULL;
    
    pos = (SkScalar*) storage.get();
    for (size_t i = 0; i < count; i++) {
        pos[i] = SkFloatToScalar(array[i]);
    }
    
    SkPath::addRoundRect(rect, pos, direction);
}

void Path::arcTo(const RectF oval, float startAngle, float sweepAngle, bool forceMoveTo) {
    SkPath::arcTo(oval, SkFloatToScalar(startAngle), SkFloatToScalar(sweepAngle), forceMoveTo);
}

void Path::addOval(const RectF oval, Direction dir) {
    SkPath::addOval(oval, dir);
}

void Path::computeBounds(RectF &rect, bool exact) {
    SkRect skrect = getBounds();
    rect.setLTRB(skrect.left(), skrect.top(), skrect.right(), skrect.bottom());
}

ANDROID_END