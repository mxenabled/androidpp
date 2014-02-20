//
//  BaseScrollView.h
//  cocos2dx
//
//  Created by Saul Howard on 12/11/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef cocos2dx_BaseScrollView_h
#define cocos2dx_BaseScrollView_h

#include "AndroidMacros.h"

#include "Android/widget/BaseAdapter.h"
#include "Android/widget/AdapterView.h"
#include "Android/widget/Dynamics.h"
#include "Android/view/VelocityTracker.h"

#include <mindroid/os/Clock.h>
#include <mindroid/os/Ref.h>

ANDROID_BEGIN

template<class T>
class BaseScrollView : public AdapterView<T> {
    
public:
    
    BaseScrollView(Context *context) : BaseScrollView(context, NULL) {
    }
    
    BaseScrollView(Context *context, AttributeSet *attrs) : AdapterView<T>(context, attrs) {
    }
    
    /**
     * Set the dynamics object used for fling and snap behavior.
     *
     * @param dynamics The dynamics object
     */
    void setDynamics(Dynamics *dynamics) {
        if (mDynamics != NULL) {
            dynamics->setState(mDynamics->getPosition(), mDynamics->getVelocity(), Clock::monotonicTime());
        }
        mDynamics = dynamics;
        mDynamics->setMaxPosition(500);
        mDynamics->setMinPosition(0);
    }
    
    virtual bool onInterceptTouchEvent(MotionEvent *event) {
        switch (event->getAction()) {
            case MotionEvent::ACTION_DOWN:
                startTouch(event);
                return false;
                
            case MotionEvent::ACTION_MOVE:
                return startScrollIfNeeded(event);
                
            default:
                endTouch(0);
                return false;
        }
    }
    
    virtual bool onTouchEvent(MotionEvent *event) {
        
        switch (event->getAction()) {
            case MotionEvent::ACTION_DOWN:
            {
                startTouch(event);
                break;
            }
                
            case MotionEvent::ACTION_MOVE:
            {
                if (mTouchState == TOUCH_STATE_CLICK) {
                    startScrollIfNeeded(event);
                }
                if (mTouchState == TOUCH_STATE_SCROLL) {
                    mVelocityTracker->addMovement(event);
                    int scrollChange = (int) event->getY() - mLastTouchY;
                    scrollList(scrollChange);
                }
                break;
            }
                
            case MotionEvent::ACTION_UP:
            {
                float velocity = 0;
                if (mTouchState == TOUCH_STATE_CLICK) {
                    clickChildAt((int) event->getX(), (int) event->getY());
                } else if (mTouchState == TOUCH_STATE_SCROLL) {
                    mVelocityTracker->addMovement(event);
                    mVelocityTracker->getState()->computeCurrentVelocity(PIXELS_PER_SECOND);
                    velocity = mVelocityTracker->getVelocity(NULL, &velocity);
                }
                endTouch(velocity);
                break;
            }
                
            default:
            {
                endTouch(0);
                break;
            }
        }
        
        mLastTouchY = (int) event->getY();
        
        return true;
    }
    
protected:
    
    void cancelScroll() {
        removeCallbacks(mDynamicsRunnable);
    }
    
    /**
     * Scrolls the list. Takes care of updating rotation (if enabled) and
     * snapping
     *
     * @param scrolledDistance The distance to scroll
     */
    virtual bool scrollList(const int scrolledDistance) = 0;
    
    /**
     * Calls the item click listener for the child with at the specified
     * coordinates
     *
     * @param x The x-coordinate
     * @param y The y-coordinate
     */
    virtual void clickChildAt(const int x, const int y) = 0;
    
    /**
     * Calls the item long click listener for the child with the specified index
     *
     * @param index Child index
     */
    void longClickChild(const int index) {}
    
private:
    
    /** Unit used for the velocity tracker */
    static const int32_t PIXELS_PER_SECOND = 1000;
    
    /** Tolerance for the position used for snapping */
    static const float POSITION_TOLERANCE;
    
    /** Tolerance for the velocity */
    static const float VELOCITY_TOLERANCE;
    
    /** Represents an invalid child index */
    static const int INVALID_INDEX = -1;
    
    /** Distance to drag before we intercept touch events */
    static const int TOUCH_SCROLL_THRESHOLD = 10;
    
    /** User is not touching the list */
    static const int TOUCH_STATE_RESTING = 0;
    
    /** User is touching the list and right now it's still a "click" */
    static const int TOUCH_STATE_CLICK = 1;
    
    /** User is scrolling the list */
    static const int TOUCH_STATE_SCROLL = 2;
    
    /** Current touch state */
    int mTouchState = TOUCH_STATE_RESTING;
    
    /** X-coordinate of the down event */
    int mTouchStartX = 0;
    
    /** Y-coordinate of the down event */
    int mTouchStartY = 0;
    
    int mLastTouchY = 0;
    
    /** Velocity tracker used to get fling velocities */
    VelocityTracker *mVelocityTracker = NULL;
    
    /** Dynamics object used to handle fling and snap */
    Dynamics *mDynamics = NULL;
    
    /** Reusable rect */
    Rect mRect;
    
    class DynamicsRunnable : public Runnable {
    public:
        
        DynamicsRunnable(BaseScrollView *view) { mView = view; }
        
        virtual void run() {
            
            // if we don't have any dynamics set we do nothing
            if (mView->mDynamics == NULL) {
                return;
            }
            // we pretend that each frame of the fling/snap animation is
            // one touch gesture and therefore set the start position
            // every time
            mView->mDynamics->update(Clock::monotonicTime());
            
            int scrollChange = (int) mView->mDynamics->getPosition();
            bool atMax = mView->scrollList(scrollChange);
            
            if (!atMax && !mView->mDynamics->isAtRest(VELOCITY_TOLERANCE, POSITION_TOLERANCE)) {
                // the list is not at rest, so schedule a new frame
                mView->postDelayed(this, 16);
            }
        }
    private:
        BaseScrollView *mView;
    };
    
    /** Runnable used to animate fling and snap */
    mindroid::sp<DynamicsRunnable> mDynamicsRunnable = NULL;
    
    class LongPressRunnable : public Runnable {
    public:
        
        LongPressRunnable(BaseScrollView *view) { mView = view; }
        
        virtual void run() {
            
            if (mView->mTouchState == TOUCH_STATE_CLICK) {
                const int index = mView->getContainingChildIndex(mView->mTouchStartX, mView->mTouchStartY);
                if (index != INVALID_INDEX) {
                    mView->longClickChild(index);
                }
            }
        }
    private:
        BaseScrollView *mView;
    };
    
    /** Used to check for long press actions */
    mindroid::sp<LongPressRunnable> *mLongPressRunnable = NULL;
    
    /**
     * Sets and initializes all things that need to when we start a touch
     * gesture.
     *
     * @param event The down event
     */
    void startTouch(MotionEvent *event) {
        // user is touching the list -> no more fling
        removeCallbacks(mDynamicsRunnable);
        
        // save the start place
        mTouchStartX = (int) event->getX();
        mTouchStartY = (int) event->getY();
        
        // start checking for a long press
        startLongPressCheck();
        
        // obtain a velocity tracker and feed it its first event
        mVelocityTracker = VelocityTracker::obtain();
        mVelocityTracker->addMovement(event);
        
        // we don't know if it's a click or a scroll yet, but until we know
        // assume it's a click
        mTouchState = TOUCH_STATE_CLICK;
    }
    
    /**
     * Resets and recycles all things that need to when we end a touch gesture
     *
     * @param velocity The velocity of the gesture
     */
    void endTouch(const float velocity) {
        
        if (mVelocityTracker != NULL) {
            // recycle the velocity tracker
            mVelocityTracker->recycle();
            mVelocityTracker = NULL;
        }
        
        // remove any existing check for longpress
        removeCallbacks(mLongPressRunnable);
        
        if (mDynamicsRunnable == NULL) {
            mDynamicsRunnable = new DynamicsRunnable(this);
        }
        
        if (mDynamics != NULL) {
            // update the dynamics with the correct position and start the
            // runnable
            mDynamics->setState(0, velocity, Clock::monotonicTime());
            post(mDynamicsRunnable);
        }
        
        // reset touch state
        mTouchState = TOUCH_STATE_RESTING;
    }
    
    /**
     * Checks if the user has moved far enough for this to be a scroll and if
     * so, sets the list in scroll mode
     *
     * @param event The (move) event
     * @return true if scroll was started, false otherwise
     */
    bool startScrollIfNeeded(const MotionEvent *event) {
        const int xPos = (int) event->getX();
        const int yPos = (int) event->getY();
        if (xPos < mTouchStartX - TOUCH_SCROLL_THRESHOLD
            || xPos > mTouchStartX + TOUCH_SCROLL_THRESHOLD
            || yPos < mTouchStartY - TOUCH_SCROLL_THRESHOLD
            || yPos > mTouchStartY + TOUCH_SCROLL_THRESHOLD) {
            // we've moved far enough for this to be a scroll
            removeCallbacks(mLongPressRunnable);
            mTouchState = TOUCH_STATE_SCROLL;
            return true;
        }
        return false;
    }
    
    /**
     * Posts (and creates if necessary) a runnable that will when executed call
     * the long click listener
     */
    void startLongPressCheck() {
        // create the runnable if we haven't already
        if (mLongPressRunnable == NULL) {
            mLongPressRunnable = new LongPressRunnable(this);
        }
        
        // then post it with a delay
        postDelayed(mLongPressRunnable, View::DEFAULT_LONG_PRESS_TIMEOUT);
    }
    
    /**
     * Returns the index of the child that contains the coordinates given.
     *
     * @param x X-coordinate
     * @param y Y-coordinate
     * @return The index of the child that contains the coordinates. If no child
     *         is found then it returns INVALID_INDEX
     */
    int getContainingChildIndex(const int x, const int y) {
        if (mRect == NULL) {
            mRect = new Rect();
        }
        for (int index = 0; index < ViewGroup::getChildCount(); index++) {
            ViewGroup::getChildAt(index)->getHitRect(mRect);
            if (mRect.contains(x, y)) {
                return index;
            }
        }
        return INVALID_INDEX;
    }
};

template<class T>
const float BaseScrollView<T>::POSITION_TOLERANCE = 0.4f;

template<class T>
const float BaseScrollView<T>::VELOCITY_TOLERANCE = 0.5f;

ANDROID_END

#endif
