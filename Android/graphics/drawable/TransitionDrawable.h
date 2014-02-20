//
//  TransitionDrawable.h
//  Androidpp
//
//  Created by Saul Howard on 2/10/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef __Androidpp__TransitionDrawable__
#define __Androidpp__TransitionDrawable__

#include "AndroidMacros.h"

#include "Android/graphics/drawable/Drawable.h"
#include "Android/graphics/drawable/LayerDrawable.h"

ANDROID_BEGIN

/**
 * An extension of LayerDrawables that is intended to cross-fade between
 * the first and second layer. To start the transition, call {@link #startTransition(int)}. To
 * display just the first layer, call {@link #resetTransition()}.
 * <p>
 * It can be defined in an XML file with the <code>&lt;transition></code> element.
 * Each Drawable in the transition is defined in a nested <code>&lt;item></code>. For more
 * information, see the guide to <a
 * href="{@docRoot}guide/topics/resources/drawable-resource.html">Drawable Resources</a>.</p>
 *
 * @attr ref android.R.styleable#LayerDrawableItem_left
 * @attr ref android.R.styleable#LayerDrawableItem_top
 * @attr ref android.R.styleable#LayerDrawableItem_right
 * @attr ref android.R.styleable#LayerDrawableItem_bottom
 * @attr ref android.R.styleable#LayerDrawableItem_drawable
 * @attr ref android.R.styleable#LayerDrawableItem_id
 *
 */
class TransitionDrawable : public LayerDrawable, public Drawable::Callback {
    
protected:
    
    class TransitionState : public LayerState {
        
        friend class TransitionDrawable;
        
    protected:
        
        TransitionState(TransitionState *orig, TransitionDrawable *owner, Resources *res) : LayerState(orig, owner, res) {
        }
        
    public:
        
        virtual shared_ptr<Drawable> newDrawable();
        
        virtual shared_ptr<Drawable> newDrawable(Resources *res);
        
        virtual int getChangingConfigurations() {
            return mChangingConfigurations;
        }
    };
    
private:
    
    /**
     * A transition is about to start.
     */
    static const int TRANSITION_STARTING = 0;
    
    /**
     * The transition has started and the animation is in progress
     */
    static const int TRANSITION_RUNNING = 1;
    
    /**
     * No transition will be applied
     */
    static const int TRANSITION_NONE = 2;
    
    /**
     * The current state of the transition. One of {@link #TRANSITION_STARTING},
     * {@link #TRANSITION_RUNNING} and {@link #TRANSITION_NONE}
     */
    int mTransitionState = TRANSITION_NONE;
    
    bool mReverse = false;
    uint64_t mStartTimeMillis = 0;
    int mFrom = 0;
    int mTo = 0;
    int mDuration = 0;
    int mOriginalDuration = 0;
    int mAlpha = 0;
    bool mCrossFade = false;
    
public:
    
    /**
     * Create a new transition drawable with the specified list of layers. At least
     * 2 layers are required for this drawable to work properly.
     */
    TransitionDrawable(vector<shared_ptr<Drawable>> layers);
    
    virtual void invalidateDrawable(shared_ptr<Drawable> who) { LayerDrawable::invalidateDrawable(who); }
    virtual void scheduleDrawable(Drawable *who, const mindroid::sp<Runnable> &what, uint32_t when) { LayerDrawable::scheduleDrawable(who, what, when); }
    virtual void unscheduleDrawable(Drawable *who, const mindroid::sp<Runnable> &what) { LayerDrawable::unscheduleDrawable(who, what); }
    
    /**
     * Create a new transition drawable with no layer. To work correctly, at least 2
     * layers must be added to this drawable.
     *
     * @see #TransitionDrawable(Drawable[])
     */
    TransitionDrawable();
    
    TransitionDrawable(TransitionState state, Resources *res) : LayerDrawable(state, res) {
    }
    
    TransitionDrawable(TransitionState state, vector<shared_ptr<Drawable>> layers) : LayerDrawable(layers, state) {
    }
    
protected:
    
    virtual LayerDrawable::LayerState createConstantState(LayerDrawable::LayerState state, Resources *res);
    
public:
    
    /**
     * Begin the second layer on top of the first layer.
     *
     * @param durationMillis The length of the transition in milliseconds
     */
    void startTransition(int durationMillis);
    
    /**
     * Show only the first layer.
     */
    void resetTransition();
    
    /**
     * Reverses the transition, picking up where the transition currently is.
     * If the transition is not currently running, this will start the transition
     * with the specified duration. If the transition is already running, the last
     * known duration will be used.
     *
     * @param duration The duration to use if no transition is running.
     */
    void reverseTransition(int duration);
    
    virtual void draw(shared_ptr<Canvas> canvas);
    
    /**
     * Enables or disables the cross fade of the drawables. When cross fade
     * is disabled, the first drawable is always drawn opaque. With cross
     * fade enabled, the first drawable is drawn with the opposite alpha of
     * the second drawable. Cross fade is disabled by default.
     *
     * @param enabled True to enable cross fading, false otherwise.
     */
    void setCrossFadeEnabled(bool enabled) {
        mCrossFade = enabled;
    }
    
    /**
     * Indicates whether the cross fade is enabled for this transition.
     *
     * @return True if cross fading is enabled, false otherwise.
     */
    bool isCrossFadeEnabled() {
        return mCrossFade;
    }
};

ANDROID_END

#endif /* defined(__Androidpp__TransitionDrawable__) */
