//
//  DrawableContainer.h
//  cocos2dx
//
//  Created by Saul Howard on 11/22/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef __cocos2dx__DrawableContainer__
#define __cocos2dx__DrawableContainer__

#include "AndroidMacros.h"

#include "Android/graphics/drawable/Drawable.h"

#include <memory>

using namespace std;

ANDROID_BEGIN

class DrawableContainerState;

class DrawableContainer : public Drawable, public Drawable::Callback {
    
    friend class DrawableContainerState;
    friend class AnimationRunnable;
    
public:
    
    static const bool DEFAULT_DITHER = true;
    
    virtual void draw(shared_ptr<Canvas> canvas);
    
    virtual int getChangingConfigurations();
    virtual ConstantState *getConstantState();
    virtual shared_ptr<Drawable> getCurrent();
    virtual int getIntrinsicHeight();
    virtual int getIntrinsicWidth();
    virtual int getMinimumHeight();
    virtual int getMinimumWidth();
    virtual int getOpacity();
    virtual Insets getOpticalInsets();
    virtual bool getPadding(Rect padding);
    
    virtual void invalidateDrawable(shared_ptr<Drawable> who);
    virtual void scheduleDrawable(Drawable *who, const mindroid::sp<Runnable> &what, uint32_t when);
    virtual void unscheduleDrawable(Drawable *who, const sp<Runnable> &what);
    
    virtual bool isStateful();
    virtual void jumpToCurrentState();
    virtual shared_ptr<Drawable> mutate();
    bool selectDrawable(int idx);
    
    virtual void setAlpha(int alpha);
    virtual void setColorFilter(ColorFilter *cf);
    virtual void setDither(bool dither);
    void setEnterFadeDuration(uint32_t ms);
    void setExitFadeDuration(uint32_t ms);
    virtual bool setVisible(bool visible, bool restart);
    
protected:
    
    virtual void onBoundsChange(Rect bounds);
    virtual bool onLevelChange(int level);
    virtual bool onStateChange(vector<int> state);
    void setConstantState(DrawableContainerState *state);
    
private:
    
    DrawableContainerState *mDrawableContainerState = NULL;
    shared_ptr<Drawable> mCurrDrawable;
    int mAlpha = 0xFF;
    ColorFilter *mColorFilter = NULL;
    
    int mCurIndex = -1;
    bool m_mutated = false;
    
    // Animations.
    sp<Runnable> mAnimationRunnable = NULL;
    uint32_t mEnterAnimationEnd = 0;
    uint32_t mExitAnimationEnd = 0;
    shared_ptr<Drawable> mLastDrawable = NULL;
    
    void animate(bool schedule);
    
    class AnimationRunnable : public Runnable {
    public:
        
        AnimationRunnable(DrawableContainer *container) {
            m_container = container;
        }
        
        virtual void run() {
            m_container->animate(true);
            m_container->invalidateSelf();
        }
        
    private:
        DrawableContainer *m_container = NULL;
    };
};

ANDROID_END

#endif /* defined(__cocos2dx__DrawableContainer__) */
