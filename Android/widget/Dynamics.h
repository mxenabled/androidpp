//
//  Dynamics.h
//  cocos2dx
//
//  Created by Saul Howard on 12/11/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef cocos2dx_Dynamics_h
#define cocos2dx_Dynamics_h

#include "AndroidMacros.h"

#include <float.h>

ANDROID_BEGIN

class Dynamics {
    
private:
    
    /**
     * The maximum delta time, in milliseconds, between two updates
     */
    static const int MAX_TIMESTEP = 50;
    
public:
    
    void setMaxPosition(float mMaxPosition) {
        this->mMaxPosition = mMaxPosition;
    }
    
    void setMinPosition(float mMinPosition) {
        this->mMinPosition = mMinPosition;
    }
    
    /**
     * Sets the state of the dynamics object. Should be called before starting
     * to call update.
     *
     * @param position The current position.
     * @param velocity The current velocity in pixels per second.
     * @param now      The current time
     */
    void setState(const float position, const float velocity, const long now) {
        mVelocity = velocity;
        mPosition = position;
        mLastTime = now;
    }
    
    /**
     * Returns the current position. Normally used after a call to update() in
     * order to get the updated position.
     *
     * @return The current position
     */
    float getPosition() {
        return mPosition;
    }
    
    /**
     * Gets the velocity. Unit is in pixels per second.
     *
     * @return The velocity in pixels per second
     */
    float getVelocity() {
        return mVelocity;
    }
    
    /**
     * Used to find out if the list is at rest, that is, has no velocity.
     * Normally used to know if more calls to update are
     * needed.
     *
     * @param velocityTolerance Velocity is regarded as 0 if less than
     *                          velocityTolerance
     * @return true if list is at rest, false otherwise
     */
    bool isAtRest(const float velocityTolerance) {
        
        const bool standingStill = std::abs(mVelocity) < velocityTolerance;
        
        return standingStill;
    }
    
    /**
     * Used to find out if the list is at rest, that is, has no velocity and is
     * inside the the limits. Normally used to know if more calls to update are
     * needed.
     *
     * @param velocityTolerance Velocity is regarded as 0 if less than
     *            velocityTolerance
     * @param positionTolerance Position is regarded as inside the limits even
     *            if positionTolerance above or below
     *
     * @return true if list is at rest, false otherwise
     */
    bool isAtRest(const float velocityTolerance, const float positionTolerance) {
        const bool standingStill = std::abs(mVelocity) < velocityTolerance;
        const bool withinLimits = mPosition - positionTolerance < mMaxPosition
        && mPosition + positionTolerance > mMinPosition;
        return standingStill && withinLimits;
    }
    
    /**
     * Updates the position and velocity.
     *
     * @param now The current time
     */
    void update(const long now) {
        
        int dt = (int) (now - mLastTime);
        
        if (dt > MAX_TIMESTEP) {
            dt = MAX_TIMESTEP;
        }
        
        onUpdate(dt);
        
        mLastTime = now;
    }
    
protected:
    
    /**
     * The current position
     */
    float mPosition = 0.0f;
    
    /**
     * The current velocity
     */
    float mVelocity = 0.0f;
    
    /**
     * The time of the last update
     */
    long mLastTime = 0;
    
    /** The current maximum position */
    float mMaxPosition = FLT_MAX;
    
    /** The current minimum position */
    float mMinPosition = -FLT_MAX;
    
    /**
     * Gets the distance to the closest limit (max and min position).
     *
     * @return If position is more than max position: distance to max position. If
     *         position is less than min position: distance to min position. If
     *         within limits: 0
     */
    float getDistanceToLimit() {
        float distanceToLimit = 0;
        
        if (mPosition > mMaxPosition) {
            distanceToLimit = mMaxPosition - mPosition;
        } else if (mPosition < mMinPosition) {
            distanceToLimit = mMinPosition - mPosition;
        }
        
        return distanceToLimit;
    }
    
    /**
     * Updates the position and velocity.
     *
     * @param dt The delta time since last time
     */
    virtual void onUpdate(int dt) = 0;
};

ANDROID_END

#endif
