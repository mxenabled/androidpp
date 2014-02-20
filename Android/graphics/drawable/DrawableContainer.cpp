//
//  DrawableContainer.cpp
//  cocos2dx
//
//  Created by Saul Howard on 11/22/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved->
//

#include "DrawableContainer.h"
#include "Android/graphics/drawable/DrawableContainerState.h"

#include <mindroid/os/Clock.h>

ANDROID_BEGIN

void DrawableContainer::draw(shared_ptr<Canvas> canvas) {
    if (mCurrDrawable != NULL) {
        mCurrDrawable->draw(canvas);
    }
    if (mLastDrawable != NULL) {
        mLastDrawable->draw(canvas);
    }
}

int DrawableContainer::getChangingConfigurations() {
    return Drawable::getChangingConfigurations()
    | mDrawableContainerState->mChangingConfigurations
    | mDrawableContainerState->mChildrenChangingConfigurations;
}

bool DrawableContainer::getPadding(Rect padding) {
    const Rect &r = mDrawableContainerState->getConstantPadding();
    if (!r.isEmpty()) {
        padding.set(r);
        return true;
    }
    if (mCurrDrawable != NULL) {
        return mCurrDrawable->getPadding(padding);
    } else {
        return Drawable::getPadding(padding);
    }
}

Insets DrawableContainer::getOpticalInsets() {
    return (mCurrDrawable == NULL) ? Insets::NONE : mCurrDrawable->getOpticalInsets();
}

void DrawableContainer::setAlpha(int alpha) {
    if (mAlpha != alpha) {
        mAlpha = alpha;
        if (mCurrDrawable != NULL) {
            if (mEnterAnimationEnd == 0) {
                mCurrDrawable->mutate()->setAlpha(alpha);
            } else {
                animate(false);
            }
        }
    }
}

void DrawableContainer::setDither(bool dither) {
    if (mDrawableContainerState->mDither != dither) {
        mDrawableContainerState->mDither = dither;
        if (mCurrDrawable != NULL) {
            mCurrDrawable->mutate()->setDither(mDrawableContainerState->mDither);
        }
    }
}

void DrawableContainer::setColorFilter(ColorFilter *cf) {
    if (mColorFilter != cf) {
        mColorFilter = cf;
        if (mCurrDrawable != NULL) {
            mCurrDrawable->mutate()->setColorFilter(cf);
        }
    }
}

/**
 * Change the global fade duration when a new drawable is entering
 * the scene.
 * @param ms The amount of time to fade in milliseconds.
 */
void DrawableContainer::setEnterFadeDuration(uint32_t ms) {
    mDrawableContainerState->mEnterFadeDuration = ms * 1000000LL;
}

/**
 * Change the global fade duration when a new drawable is leaving
 * the scene.
 * @param ms The amount of time to fade in milliseconds.
 */
void DrawableContainer::setExitFadeDuration(uint32_t ms) {
    mDrawableContainerState->mExitFadeDuration = ms * 1000000LL;
}

void DrawableContainer::onBoundsChange(Rect bounds) {
    if (mLastDrawable != NULL) {
        mLastDrawable->setBounds(bounds);
    }
    if (mCurrDrawable != NULL) {
        mCurrDrawable->setBounds(bounds);
    }
}

bool DrawableContainer::isStateful() {
    return mDrawableContainerState->isStateful();
}

void DrawableContainer::jumpToCurrentState() {
    bool changed = false;
    if (mLastDrawable != NULL) {
        mLastDrawable->jumpToCurrentState();
        mLastDrawable = NULL;
        changed = true;
    }
    if (mCurrDrawable != NULL) {
        mCurrDrawable->jumpToCurrentState();
        mCurrDrawable->mutate()->setAlpha(mAlpha);
    }
    if (mExitAnimationEnd != 0) {
        mExitAnimationEnd = 0;
        changed = true;
    }
    if (mEnterAnimationEnd != 0) {
        mEnterAnimationEnd = 0;
        changed = true;
    }
    if (changed) {
        invalidateSelf();
    }
}

bool DrawableContainer::onStateChange(std::vector<int> state) {
    if (mLastDrawable != NULL) {
        return mLastDrawable->setState(state);
    }
    if (mCurrDrawable != NULL) {
        return mCurrDrawable->setState(state);
    }
    return false;
}

bool DrawableContainer::onLevelChange(int level) {
    if (mLastDrawable != NULL) {
        return mLastDrawable->setLevel(level);
    }
    if (mCurrDrawable != NULL) {
        return mCurrDrawable->setLevel(level);
    }
    return false;
}

int DrawableContainer::getIntrinsicWidth() {
    if (mDrawableContainerState->isConstantSize()) {
        return mDrawableContainerState->getConstantWidth();
    }
    return mCurrDrawable != NULL ? mCurrDrawable->getIntrinsicWidth() : -1;
}

int DrawableContainer::getIntrinsicHeight() {
    if (mDrawableContainerState->isConstantSize()) {
        return mDrawableContainerState->getConstantHeight();
    }
    return mCurrDrawable != NULL ? mCurrDrawable->getIntrinsicHeight() : -1;
}

int DrawableContainer::getMinimumWidth() {
    if (mDrawableContainerState->isConstantSize()) {
        return mDrawableContainerState->getConstantMinimumWidth();
    }
    return mCurrDrawable != NULL ? mCurrDrawable->getMinimumWidth() : 0;
}

int DrawableContainer::getMinimumHeight() {
    if (mDrawableContainerState->isConstantSize()) {
        return mDrawableContainerState->getConstantMinimumHeight();
    }
    return mCurrDrawable != NULL ? mCurrDrawable->getMinimumHeight() : 0;
}

void DrawableContainer::invalidateDrawable(shared_ptr<Drawable> who) {
    if (who == mCurrDrawable && getCallback() != NULL) {
        getCallback()->invalidateDrawable(shared_from_this());
    }
}

void DrawableContainer::scheduleDrawable(Drawable *who, const sp<Runnable> &what, uint32_t when) {
    if (who == mCurrDrawable.get() && getCallback() != NULL) {
        getCallback()->scheduleDrawable(this, what, when);
    }
}

void DrawableContainer::unscheduleDrawable(Drawable *who, const sp<Runnable> &what) {
    if (who == mCurrDrawable.get() && getCallback() != NULL) {
        getCallback()->unscheduleDrawable(this, what);
    }
}

bool DrawableContainer::setVisible(bool visible, bool restart) {
    bool changed = Drawable::setVisible(visible, restart);
    if (mLastDrawable != NULL) {
        mLastDrawable->setVisible(visible, restart);
    }
    if (mCurrDrawable != NULL) {
        mCurrDrawable->setVisible(visible, restart);
    }
    return changed;
}

int DrawableContainer::getOpacity() {
    return mCurrDrawable == NULL || !mCurrDrawable->isVisible() ? PixelFormats::PIXEL_FORMAT_TRANSPARENT :
    mDrawableContainerState->getOpacity();
}

bool DrawableContainer::selectDrawable(int idx) {
    if (idx == mCurIndex) {
        return false;
    }
    
    const uint32_t now = Clock::monotonicTime();
    
    if (mDrawableContainerState->mExitFadeDuration > 0) {
        if (mLastDrawable != NULL) {
            mLastDrawable->setVisible(false, false);
        }
        if (mCurrDrawable != NULL) {
            mLastDrawable = mCurrDrawable;
            mExitAnimationEnd = now + mDrawableContainerState->mExitFadeDuration;
        } else {
            mLastDrawable = NULL;
            mExitAnimationEnd = 0;
        }
    } else if (mCurrDrawable != NULL) {
        mCurrDrawable->setVisible(false, false);
    }
    
    if (idx >= 0 && idx < mDrawableContainerState->mNumChildren) {
        shared_ptr<Drawable> d = mDrawableContainerState->mDrawables[idx];
        mCurrDrawable = d;
        mCurIndex = idx;
        if (d != NULL) {
            d->mutate();
            if (mDrawableContainerState->mEnterFadeDuration > 0) {
                mEnterAnimationEnd = now + mDrawableContainerState->mEnterFadeDuration;
            } else {
                d->setAlpha(mAlpha);
            }
            d->setVisible(isVisible(), true);
            d->setDither(mDrawableContainerState->mDither);
            d->setColorFilter(mColorFilter);
            d->setState(getState());
            d->setLevel(getLevel());
            d->setBounds(getBounds());
            d->setLayoutDirection(getLayoutDirection());
        }
    } else {
        mCurrDrawable = NULL;
        mCurIndex = -1;
    }
    
    if (mEnterAnimationEnd != 0 || mExitAnimationEnd != 0) {
        if (mAnimationRunnable == NULL) {
            mAnimationRunnable = new AnimationRunnable(this);
        } else {
            unscheduleSelf(mAnimationRunnable);
        }
        // Compute first frame and schedule next animation.
        animate(true);
    }
    
    invalidateSelf();
    
    return true;
}

void DrawableContainer::animate(bool schedule) {
    const uint32_t now = Clock::monotonicTime();
    bool animating = false;
    if (mCurrDrawable != NULL) {
        if (mEnterAnimationEnd != 0) {
            if (mEnterAnimationEnd <= now) {
                mCurrDrawable->mutate()->setAlpha(mAlpha);
                mEnterAnimationEnd = 0;
            } else {
                int animAlpha = (int)((mEnterAnimationEnd-now)*255)
                / mDrawableContainerState->mEnterFadeDuration;
                mCurrDrawable->mutate()->setAlpha(((255-animAlpha)*mAlpha)/255);
                animating = true;
            }
        }
    } else {
        mEnterAnimationEnd = 0;
    }
    if (mLastDrawable != NULL) {
        if (mExitAnimationEnd != 0) {
            if (mExitAnimationEnd <= now) {
                mLastDrawable->setVisible(false, false);
                mLastDrawable = NULL;
                mExitAnimationEnd = 0;
            } else {
                int animAlpha = (int)((mExitAnimationEnd-now)*255)
                / mDrawableContainerState->mExitFadeDuration;
                mLastDrawable->mutate()->setAlpha((animAlpha*mAlpha)/255);
                animating = true;
            }
        }
    } else {
        mExitAnimationEnd = 0;
    }
    
    if (schedule && animating) {
        scheduleSelf(mAnimationRunnable, now + 1000/60 * 1000000LL);
    }
}

shared_ptr<Drawable> DrawableContainer::getCurrent() {
    return mCurrDrawable;
}

Drawable::ConstantState *DrawableContainer::getConstantState() {
    if (mDrawableContainerState->canConstantState()) {
        mDrawableContainerState->mChangingConfigurations = getChangingConfigurations();
        return mDrawableContainerState;
    }
    return NULL;
}

shared_ptr<Drawable> DrawableContainer::mutate() {
    if (!m_mutated && Drawable::mutate() == shared_from_this()) {
        const int N = mDrawableContainerState->getChildCount();
        const vector<shared_ptr<Drawable>> &drawables = mDrawableContainerState->getChildren();
        for (int i = 0; i < N; i++) {
            if (drawables[i] != NULL) drawables[i]->mutate();
        }
        m_mutated = true;
    }
    return shared_from_this();
}

void DrawableContainer::setConstantState(DrawableContainerState *state) {
    mDrawableContainerState = state;
}

ANDROID_END