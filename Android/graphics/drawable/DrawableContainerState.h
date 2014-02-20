//
//  DrawableContainerState.h
//  cocos2dx
//
//  Created by Saul Howard on 11/22/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef __cocos2dx__DrawableContainerState__
#define __cocos2dx__DrawableContainerState__

#include "AndroidMacros.h"

#include "Android/graphics/drawable/Drawable.h"
#include "Android/graphics/drawable/DrawableContainer.h"

ANDROID_BEGIN

class DrawableContainerState : public Drawable::ConstantState {
    
    friend class DrawableContainer;
    
public:
    
    DrawableContainerState() {}
    
    DrawableContainerState(DrawableContainerState *orig, DrawableContainer *owner, Resources *res);
    
    const int addChild(shared_ptr<Drawable> dr);
    bool canConstantState();
    
    virtual int getChangingConfigurations();
    const int getChildCount();
    const vector<shared_ptr<Drawable>> getChildren();
    const int getConstantHeight();
    const int getConstantMinimumHeight();
    const int getConstantMinimumWidth();
    const int getConstantWidth();
    const Rect getConstantPadding();
    const int getEnterFadeDuration();
    const int getExitFadeDuration();
    const int getOpacity();
    
    virtual void growArray(int oldSize, int newSize);
    
    const bool isConstantSize();
    const bool isStateful();
    
    const void setConstantSize(bool constant);
    const void setEnterFadeDuration(int duration);
    const void setExitFadeDuration(int duration);
    const void setVariablePadding(bool variable);
    
protected:
    
    void computeConstantSize();
    
private:
    
    DrawableContainer *mOwner = NULL;
    
    int mChangingConfigurations;
    int mChildrenChangingConfigurations;
    
    vector<shared_ptr<Drawable>> mDrawables;
    int mNumChildren = 0;
    
    bool mVariablePadding = false;
    Rect mConstantPadding;
    
    bool mConstantSize = false;
    bool mComputedConstantSize = false;
    int mConstantWidth = 0;
    int mConstantHeight = 0;
    int mConstantMinimumWidth = 0;
    int mConstantMinimumHeight = 0;
    
    int mOpacity = 0;
    
    bool mHaveStateful = false;
    bool mStateful = false;
    
    bool mCheckedConstantState = false;
    bool mCanConstantState = false;
    
    bool mPaddingChecked = false;
    
    bool mDither = DrawableContainer::DEFAULT_DITHER;
    
    uint32_t mEnterFadeDuration = 0;
    uint32_t mExitFadeDuration = 0;
};

ANDROID_END

#endif /* defined(__cocos2dx__DrawableContainerState__) */
