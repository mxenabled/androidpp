//
//  TransitionDrawable.cpp
//  Androidpp
//
//  Created by Saul Howard on 2/10/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#include "TransitionDrawable.h"

#include <mindroid/os/Clock.h>

#include <algorithm>

ANDROID_BEGIN

TransitionDrawable::TransitionDrawable(vector<shared_ptr<Drawable>> layers) : TransitionDrawable(TransitionState(NULL, NULL, NULL), layers) {
}

TransitionDrawable::TransitionDrawable() : TransitionDrawable(TransitionState(NULL, NULL, NULL), (Resources*) NULL) {
}

LayerDrawable::LayerState TransitionDrawable::createConstantState(LayerState state, Resources *res) {
    return TransitionState((TransitionState*) &state, this, res);
}

/**
 * Begin the second layer on top of the first layer.
 *
 * @param durationMillis The length of the transition in milliseconds
 */
void TransitionDrawable::startTransition(int durationMillis) {
    mFrom = 0;
    mTo = 255;
    mAlpha = 0;
    mDuration = mOriginalDuration = durationMillis;
    mReverse = false;
    mTransitionState = TRANSITION_STARTING;
    invalidateSelf();
}

/**
 * Show only the first layer.
 */
void TransitionDrawable::resetTransition() {
    mAlpha = 0;
    mTransitionState = TRANSITION_NONE;
    invalidateSelf();
}

/**
 * Reverses the transition, picking up where the transition currently is.
 * If the transition is not currently running, this will start the transition
 * with the specified duration. If the transition is already running, the last
 * known duration will be used.
 *
 * @param duration The duration to use if no transition is running.
 */
void TransitionDrawable::reverseTransition(int duration) {
    const uint64_t time = Clock::monotonicTime();
    // Animation is over
    if (time - mStartTimeMillis > mDuration) {
        if (mTo == 0) {
            mFrom = 0;
            mTo = 255;
            mAlpha = 0;
            mReverse = false;
        } else {
            mFrom = 255;
            mTo = 0;
            mAlpha = 255;
            mReverse = true;
        }
        mDuration = mOriginalDuration = duration;
        mTransitionState = TRANSITION_STARTING;
        invalidateSelf();
        return;
    }
    
    mReverse = !mReverse;
    mFrom = mAlpha;
    mTo = mReverse ? 0 : 255;
    mDuration = (int) (mReverse ? time - mStartTimeMillis :
                       mOriginalDuration - (time - mStartTimeMillis));
    mTransitionState = TRANSITION_STARTING;
}

void TransitionDrawable::draw(shared_ptr<Canvas> canvas) {
    bool done = true;
    
    switch (mTransitionState) {
        case TRANSITION_STARTING:
            mStartTimeMillis = Clock::monotonicTime();
            done = false;
            mTransitionState = TRANSITION_RUNNING;
            break;
            
        case TRANSITION_RUNNING:
            if (mStartTimeMillis >= 0) {
                float normalized = (float)
                (Clock::monotonicTime() - mStartTimeMillis) / mDuration;
                done = normalized >= 1.0f;
                normalized = min(normalized, 1.0f);
                mAlpha = (int) (mFrom  + (mTo - mFrom) * normalized);
            }
            break;
    }
    
    const int alpha = mAlpha;
    const bool crossFade = mCrossFade;
    const  vector<shared_ptr<ChildDrawable>> &array = mLayerState.mChildren;
    
    if (done) {
        // the setAlpha() calls below trigger invalidation and redraw. If we're done, just draw
        // the appropriate drawable[s] and return
        if (!crossFade || alpha == 0) {
            array[0]->mDrawable->draw(canvas);
        }
        if (alpha == 0xFF) {
            array[1]->mDrawable->draw(canvas);
        }
        return;
    }
    
    shared_ptr<Drawable> d;
    d = array[0]->mDrawable;
    if (crossFade) {
        d->setAlpha(255 - alpha);
    }
    d->draw(canvas);
    if (crossFade) {
        d->setAlpha(0xFF);
    }
    
    if (alpha > 0) {
        d = array[1]->mDrawable;
        d->setAlpha(alpha);
        d->draw(canvas);
        d->setAlpha(0xFF);
    }
    
    if (!done) {
        invalidateSelf();
    }
}

shared_ptr<Drawable> TransitionDrawable::TransitionState::newDrawable() {
    return make_shared<TransitionDrawable>(*this, (Resources*) NULL);
}

shared_ptr<Drawable> TransitionDrawable::TransitionState::newDrawable(Resources *res) {
    return make_shared<TransitionDrawable>(*this, res);
}

ANDROID_END