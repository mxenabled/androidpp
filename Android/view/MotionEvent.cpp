//
//  MotionEvent.cpp
//  cocos2dx
//
//  Created by Saul Howard on 11/4/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#include "MotionEvent.h"

ANDROID_BEGIN

MotionEvent::~MotionEvent() {
    
}

int MotionEvent::getX() const {
    return getDefault()->getLocation().x;
}

int MotionEvent::getY() const {
    return getDefault()->getLocation().y;
}

int MotionEvent::getPointerId() const {
    
    if (getPointerCount() == 0) return INVALID_POINTER_ID;
    
    return getDefault()->getID();
}

int MotionEvent::getPointerId(int pointerIndex) const {
    return getTouchAt(pointerIndex)->getID();
}

int MotionEvent::getPointerIdBits() const {
    int idBits = 0;
    const int pointerCount = getPointerCount();
    for (int i = 0; i < pointerCount; i++) {
        idBits |= 1 << getPointerId(i);
    }
    return idBits;
}

int MotionEvent::findPointerIndex(int pointerId) const {
    
    int count = 0;
    
    for (auto it = m_touches->begin(); it != m_touches->end(); it++) {
        
        auto touch = dynamic_cast<CCTouch*>(*it);
        
        if(touch == nullptr)
            return count;
        
        count++;
    }
    
    return -1;
}

int MotionEvent::getX(int pointerIndex) const {
    return getTouchAt(pointerIndex)->getLocation().x;
}

int MotionEvent::getY(int pointerIndex) const {
    return getTouchAt(pointerIndex)->getLocation().y;
}

CCTouch *MotionEvent::getDefault() const {
    return (CCTouch *)( *m_touches->begin());
}

CCTouch *MotionEvent::getTouchAt(int index) const {
    
    if (getPointerCount() <= index) return NULL;
    
    auto it = m_touches->begin();
    std::advance(it, index);
    
    return (CCTouch *) (*it);
}

void MotionEvent::offsetLocation(float xOffset, float yOffset) {
    CCTouch *touch = getTouchAt(0);
    touch->setTouchInfo(touch->getID(), touch->getLocation().x + xOffset, touch->getLocation().y + yOffset);
}

ANDROID_END