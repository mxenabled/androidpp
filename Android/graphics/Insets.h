//
//  Insets.h
//  cocos2dx
//
//  Created by Saul Howard on 11/22/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef cocos2dx_Insets_h
#define cocos2dx_Insets_h

#include "AndroidMacros.h"
#include "Android/graphics/Rect.h"

ANDROID_BEGIN

class Insets {
    
public:
    static const Insets NONE;
    
    int left = 0;
    int top = 0;
    int right = 0;
    int bottom = 0;
    
    // Factory methods
    
    /**
     * Return an Insets instance with the appropriate values.
     *
     * @param left the left inset
     * @param top the top inset
     * @param right the right inset
     * @param bottom the bottom inset
     *
     * @return Insets instance with the appropriate values
     */
    inline static Insets of(int left, int top, int right, int bottom) {
        if (left == 0 && top == 0 && right == 0 && bottom == 0) {
            return NONE;
        }
        return Insets(left, top, right, bottom);
    }
    
    /**
     * Return an Insets instance with the appropriate values.
     *
     * @param r the Rectangle from which to take the values
     *
     * @return an Insets instance with the appropriate values
     */
    inline static Insets of(Rect r) {
        return (r.isEmpty()) ? NONE : of(r.left(), r.top(), r.right(), r.bottom());
    }
    
    friend bool operator== (Insets &lhs, Insets &rhs);
    
private:
    
    Insets(int left, int top, int right, int bottom) {
        this->left = left;
        this->top = top;
        this->right = right;
        this->bottom = bottom;
    }
};

/**
 * Two Insets instances are equal iff they belong to the same class and their fields are
 * pairwise equal.
 *
 * @param o the object to compare this instance with.
 *
 * @return true iff this object is equal {@code o}
 */
inline bool operator== (Insets &lhs, Insets &rhs) {
    
    if (lhs.bottom != rhs.bottom) return false;
    if (lhs.left != rhs.left) return false;
    if (lhs.right != rhs.right) return false;
    if (lhs.top != rhs.top) return false;
    
    return true;
}

ANDROID_END

#endif
