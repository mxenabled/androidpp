//
//  Rect.h
//  cocos2dx
//
//  Created by Saul Howard on 11/25/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef cocos2dx_Rect_h
#define cocos2dx_Rect_h

#include "AndroidMacros.h"

#include <SkRect.h>

ANDROID_BEGIN

struct Rect : SkIRect {
    
    Rect() {
        setEmpty();
    }
    
    Rect(int left, int top, int right, int bottom) {
        fLeft = left;
        fTop = top;
        fRight = right;
        fBottom = bottom;
    }
    
    void set(int left, int top, int right, int bottom) {
        SkIRect::set((int32_t) left, (int32_t) top, (int32_t) right, (int32_t) bottom);
    }
    
    void set(const Rect& src) {
        fLeft   = src.fLeft;
        fTop    = src.fTop;
        fRight  = src.fRight;
        fBottom = src.fBottom;
    }
    
    void unionWith(const Rect &rect) {
        unionWith(rect.left(), rect.top(), rect.right(), rect.bottom());
    }
    
    void unionWith(int left, int top, int right, int bottom) {
        if ((left < right) && (top < bottom)) {
            if ((this->fLeft < this->fRight) && (this->fTop < this->fBottom)) {
                if (this->fLeft > left) this->fLeft = left;
                if (this->fTop > top) this->fTop = top;
                if (this->fRight < right) this->fRight = right;
                if (this->fBottom < bottom) this->fBottom = bottom;
            } else {
                this->fLeft = left;
                this->fTop = top;
                this->fRight = right;
                this->fBottom = bottom;
            }
        }
    }
    
    int hashCode() {
        int result = fLeft;
        result = 31 * result + fTop;
        result = 31 * result + fRight;
        result = 31 * result + fBottom;
        return result;
    }
};

struct RectF : SkRect {
    
    RectF() {
        setEmpty();
    }
    
    void unionWith(float left, float top, float right, float bottom) {
        
        SkScalar fLeft = SkFloatToScalar(left);
        SkScalar fTop = SkFloatToScalar(top);
        SkScalar fRight = SkFloatToScalar(right);
        SkScalar fBottom = SkFloatToScalar(bottom);
        
        if ((fLeft < fRight) && (fTop < fBottom)) {
            if ((this->fLeft < this->fRight) && (this->fTop < this->fBottom)) {
                if (this->fLeft > fLeft)
                    this->fLeft = fLeft;
                if (this->fTop > fTop)
                    this->fTop = fTop;
                if (this->fRight < fRight)
                    this->fRight = fRight;
                if (this->fBottom < fBottom)
                    this->fBottom = fBottom;
            } else {
                this->fLeft = fLeft;
                this->fTop = fTop;
                this->fRight = fRight;
                this->fBottom = fBottom;
            }
        }
    }
    
    void set(const Rect& src) {
        fLeft   = SkIntToScalar(src.fLeft);
        fTop    = SkIntToScalar(src.fTop);
        fRight  = SkIntToScalar(src.fRight);
        fBottom = SkIntToScalar(src.fBottom);
    }
    
    void set(float left, float top, float right, float bottom) {
        fLeft   = SkFloatToScalar(left);
        fTop    = SkFloatToScalar(top);
        fRight  = SkFloatToScalar(right);
        fBottom = SkFloatToScalar(bottom);
    }
    
    int hashCode() {
        int result = (fLeft != +0.0f ? floatToIntBits(fLeft) : 0);
        result = 31 * result + (fTop != +0.0f ? floatToIntBits(fTop) : 0);
        result = 31 * result + (fRight != +0.0f ? floatToIntBits(fRight) : 0);
        result = 31 * result + (fBottom != +0.0f ? floatToIntBits(fBottom) : 0);
        return result;
    }
    
    int floatToIntBits(float floatVar) {
        int fl = *(int*)&floatVar; //assuming sizeof(int) = sizeof(float)
        
        int binaryRepresentation[sizeof(float) * 8];
        
        for (int i = 0; i < sizeof(float) * 8; ++i) {
            binaryRepresentation[i] = ((1 << i) & fl) != 0 ? 1 : 0;
        }
        
        return fl;
    }
};

ANDROID_END

#endif
