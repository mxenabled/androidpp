//
//  TouchTarget.c
//  cocos2dx
//
//  Created by Saul Howard on 11/5/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#include "TouchTarget.h"

ANDROID_BEGIN

const int TouchTarget::MAX_RECYCLED = 32;
TouchTarget *TouchTarget::sRecycleBin = NULL;
int TouchTarget::sRecycledCount = 0;
mutex TouchTarget::_mutex;

TouchTarget *TouchTarget::obtain(shared_ptr<View> child, int pointerIdBits) {
    
    TouchTarget *target = NULL;
    unique_lock<mutex> lock(_mutex);
    
    if (!sRecycleBin) {
        target = new TouchTarget();
    } else {
        target = sRecycleBin;
        sRecycleBin = target->next;
        sRecycledCount--;
        target->next = NULL;
    }
    
    target->child = child;
    target->pointerIdBits = pointerIdBits;
    
    return target;
}

void TouchTarget::recycle() {
    
    unique_lock<mutex> lock(_mutex);
    
    if (sRecycledCount < MAX_RECYCLED) {
        next = sRecycleBin;
        sRecycleBin = this;
        sRecycledCount += 1;
    } else {
        next = NULL;
    }
    child = NULL;
}

ANDROID_END