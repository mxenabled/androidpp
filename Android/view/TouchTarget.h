//
//  TouchTarget.h
//  cocos2dx
//
//  Created by Saul Howard on 11/4/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef cocos2dx_TouchTarget_h
#define cocos2dx_TouchTarget_h

#include "AndroidMacros.h"

#include "Android/view/View.h"

#include <mutex>
#include <memory>

using namespace std;

ANDROID_BEGIN

class TouchTarget {
public:
    
    static const int ALL_POINTER_IDS = -1; // all ones
    
    shared_ptr<View> child = NULL;
    int pointerIdBits;
    TouchTarget *next;
    
    static TouchTarget *obtain(shared_ptr<View> child, int pointerIdBits);
    void recycle();
    
private:
    
    static const int MAX_RECYCLED;
    static TouchTarget *sRecycleBin;
    static int sRecycledCount;
    
    TouchTarget() {}
    
    static mutex _mutex;
};

ANDROID_END

#endif
