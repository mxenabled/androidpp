//
//  DrawableContainerState.cpp
//  cocos2dx
//
//  Created by Saul Howard on 11/22/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#include "DrawableContainerState.h"

#include <ui/PixelFormat.h>

ANDROID_BEGIN
    
DrawableContainerState::DrawableContainerState(DrawableContainerState *orig, DrawableContainer *owner, Resources *res) {
    mOwner = owner;
    
    if (orig) {
        mChangingConfigurations = orig->mChangingConfigurations;
        mChildrenChangingConfigurations = orig->mChildrenChangingConfigurations;
        
        vector<shared_ptr<Drawable>> origDr = orig->mDrawables;
        
        mDrawables = vector<shared_ptr<Drawable>>(origDr.size());
        mNumChildren = orig->mNumChildren;
        
        const int N = mNumChildren;
        for (int i = 0; i < N; i++) {
            if (res) {
                mDrawables[i] = origDr[i]->getConstantState()->newDrawable(res);
            } else {
                mDrawables[i] = origDr[i]->getConstantState()->newDrawable();
            }
            mDrawables[i]->setCallback(owner);
            mDrawables[i]->setLayoutDirection(origDr[i]->getLayoutDirection());
        }
        
        mCheckedConstantState = mCanConstantState = true;
        mVariablePadding = orig->mVariablePadding;
        if (!orig->mConstantPadding.isEmpty()) {
            mConstantPadding = Rect(orig->mConstantPadding);
        }
        mConstantSize = orig->mConstantSize;
        mComputedConstantSize = orig->mComputedConstantSize;
        mConstantWidth = orig->mConstantWidth;
        mConstantHeight = orig->mConstantHeight;
        mConstantMinimumWidth = orig->mConstantMinimumWidth;
        mConstantMinimumHeight = orig->mConstantMinimumHeight;
        
        mOpacity = orig->mOpacity;
        mHaveStateful = orig->mHaveStateful;
        mStateful = orig->mStateful;
        
        mDither = orig->mDither;
        
        mEnterFadeDuration = orig->mEnterFadeDuration;
        mExitFadeDuration = orig->mExitFadeDuration;
        
    } else {
        mDrawables = vector<shared_ptr<Drawable>>(10);
        mNumChildren = 0;
        mCheckedConstantState = mCanConstantState = false;
    }
}

int DrawableContainerState::getChangingConfigurations() {
    return mChangingConfigurations | mChildrenChangingConfigurations;
}

const int DrawableContainerState::addChild(shared_ptr<Drawable> dr) {
    const int pos = mNumChildren;
    
    if (pos >= mDrawables.size()) {
        growArray(pos, pos+10);
    }
    
    dr->setVisible(false, true);
    dr->setCallback(mOwner);
    
    mDrawables[pos] = dr;
    mNumChildren++;
    mChildrenChangingConfigurations |= dr->getChangingConfigurations();
    mHaveStateful = false;
    
    mConstantPadding.setEmpty();
    mPaddingChecked = false;
    mComputedConstantSize = false;
    
    return pos;
}

const int DrawableContainerState::getChildCount() {
    return mNumChildren;
}

const vector<shared_ptr<Drawable>> DrawableContainerState::getChildren() {
    return mDrawables;
}

/** A bool value indicating whether to use the maximum padding value of
 * all frames in the set (false), or to use the padding value of the frame
 * being shown (true). Default value is false.
 */
const void DrawableContainerState::setVariablePadding(bool variable) {
    mVariablePadding = variable;
}

const Rect DrawableContainerState::getConstantPadding() {
    
    if (mVariablePadding) {
        return Rect();
    }
    
    if (!mConstantPadding.isEmpty() || mPaddingChecked) {
        return mConstantPadding;
    }
    
    Rect r;
    Rect t = Rect();
    const int N = getChildCount();
    const vector<shared_ptr<Drawable>> &drawables = mDrawables;
    for (int i = 0; i < N; i++) {
        if (drawables[i]->getPadding(t)) {
            if (r.isEmpty()) r = Rect(0, 0, 0, 0);
            if (t.left() > r.left()) r.fLeft = t.left();
            if (t.top() > r.top()) r.fTop = t.top();
            if (t.right() > r.right()) r.fRight = t.right();
            if (t.bottom() > r.bottom()) r.fBottom = t.bottom();
        }
    }
    mPaddingChecked = true;
    return (mConstantPadding = r);
}

const void DrawableContainerState::setConstantSize(bool constant) {
    mConstantSize = constant;
}

const bool DrawableContainerState::isConstantSize() {
    return mConstantSize;
}

const int DrawableContainerState::getConstantWidth() {
    if (!mComputedConstantSize) {
        computeConstantSize();
    }
    
    return mConstantWidth;
}

const int DrawableContainerState::getConstantHeight() {
    if (!mComputedConstantSize) {
        computeConstantSize();
    }
    
    return mConstantHeight;
}

const int DrawableContainerState::getConstantMinimumWidth() {
    if (!mComputedConstantSize) {
        computeConstantSize();
    }
    
    return mConstantMinimumWidth;
}

const int DrawableContainerState::getConstantMinimumHeight() {
    if (!mComputedConstantSize) {
        computeConstantSize();
    }
    
    return mConstantMinimumHeight;
}

void DrawableContainerState::computeConstantSize() {
    mComputedConstantSize = true;
    
    const int N = getChildCount();
    const vector<shared_ptr<Drawable>> &drawables = mDrawables;
    mConstantWidth = mConstantHeight = -1;
    mConstantMinimumWidth = mConstantMinimumHeight = 0;
    for (int i = 0; i < N; i++) {
        shared_ptr<Drawable> dr = drawables[i];
        int s = dr->getIntrinsicWidth();
        if (s > mConstantWidth) mConstantWidth = s;
        s = dr->getIntrinsicHeight();
        if (s > mConstantHeight) mConstantHeight = s;
        s = dr->getMinimumWidth();
        if (s > mConstantMinimumWidth) mConstantMinimumWidth = s;
        s = dr->getMinimumHeight();
        if (s > mConstantMinimumHeight) mConstantMinimumHeight = s;
    }
}

const void DrawableContainerState::setEnterFadeDuration(int duration) {
    mEnterFadeDuration = duration;
}

const int DrawableContainerState::getEnterFadeDuration() {
    return mEnterFadeDuration;
}

const void DrawableContainerState::setExitFadeDuration(int duration) {
    mExitFadeDuration = duration;
}

const int DrawableContainerState::getExitFadeDuration() {
    return mExitFadeDuration;
}

const int DrawableContainerState::getOpacity() {
    const int N = getChildCount();
    const vector<shared_ptr<Drawable>> &drawables = mDrawables;
    int op = N > 0 ? drawables[0]->getOpacity() : PixelFormats::PIXEL_FORMAT_TRANSPARENT;
    for (int i = 1; i < N; i++) {
        op = Drawable::resolveOpacity(op, drawables[i]->getOpacity());
    }
    mOpacity = op;
    return op;
}

const bool DrawableContainerState::isStateful() {
    if (mHaveStateful) {
        return mStateful;
    }
    
    bool stateful = false;
    const int N = getChildCount();
    for (int i = 0; i < N; i++) {
        if (mDrawables[i]->isStateful()) {
            stateful = true;
            break;
        }
    }
    
    mStateful = stateful;
    mHaveStateful = true;
    return stateful;
}

void DrawableContainerState::growArray(int oldSize, int newSize) {
    mDrawables.resize(newSize);
}

bool DrawableContainerState::canConstantState() {
    if (!mCheckedConstantState) {
        mCanConstantState = true;
        const int N = mNumChildren;
        for (int i=0; i<N; i++) {
            if (mDrawables[i]->getConstantState() == NULL) {
                mCanConstantState = false;
                break;
            }
        }
        mCheckedConstantState = true;
    }
    
    return mCanConstantState;
}

ANDROID_END