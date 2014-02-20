//
//  VelocityTracker.h
//  cocos2dx
//
//  Created by Saul Howard on 12/11/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef __cocos2dx__VelocityTracker__
#define __cocos2dx__VelocityTracker__

#include "AndroidMacros.h"

#include "Android/utils/Pools.h"
#include "Android/view/MotionEvent.h"

#include "cocos2d.h"

#include <utils/Timers.h>
#include <utils/BitSet.h>

#include <string>

using namespace std;
using namespace android;

ANDROID_BEGIN
    
class VelocityTrackerStrategy;
class VelocityTrackerState;

/*
 * Calculates the velocity of pointer movements over time.
 */
class VelocityTracker {
public:
    
    struct Position {
        float x, y;
    };
    
    struct Estimator {
        static const size_t MAX_DEGREE = 4;
        
        // Estimator time base.
        nsecs_t time;
        
        // Polynomial coefficients describing motion in X and Y.
        float xCoeff[MAX_DEGREE + 1], yCoeff[MAX_DEGREE + 1];
        
        // Polynomial degree (number of coefficients), or zero if no information is
        // available.
        uint32_t degree;
        
        // Confidence (coefficient of determination), between 0 (no fit) and 1 (perfect fit).
        float confidence;
        
        inline void clear() {
            time = 0;
            degree = 0;
            confidence = 0;
            for (size_t i = 0; i <= MAX_DEGREE; i++) {
                xCoeff[i] = 0;
                yCoeff[i] = 0;
            }
        }
    };
    
    // Creates a velocity tracker using the specified strategy.
    // If strategy is NULL, uses the default strategy for the platform.
    VelocityTracker(const char* strategy = NULL);
    
    ~VelocityTracker();
    
    // Resets the velocity tracker state.
    void clear();
    
    // Resets the velocity tracker state for specific pointers.
    // Call this method when some pointers have changed and may be reusing
    // an id that was assigned to a different pointer earlier.
    void clearPointers(BitSet32 idBits);
    
    // Adds movement information for a set of pointers.
    // The idBits bitfield specifies the pointer ids of the pointers whose positions
    // are included in the movement.
    // The positions array contains position information for each pointer in order by
    // increasing id.  Its size should be equal to the number of one bits in idBits.
    void addMovement(nsecs_t eventTime, BitSet32 idBits, const Position* positions);
    
    // Adds movement information for all pointers in a MotionEvent, including historical samples.
    void addMovement(const MotionEvent* event);
    
    void computeCurrentVelocity(int32_t units);
    void computeCurrentVelocity(int32_t units, float maxVelocity);
    
    // Gets the velocity of the specified pointer id in position units per second.
    // Returns false and sets the velocity components to zero if there is
    // insufficient movement information for the pointer.
    void getVelocity(int32_t id, float* outVx, float* outVy) const;
    void getVelocity(float* outVx, float* outVy) const;
    bool getVelocityXY(int32_t id, float* outVx, float* outVy) const;
    float getXVelocity() const;
    float getYVelocity() const;
    
    // Gets an estimator for the recent movements of the specified pointer id.
    // Returns false and clears the estimator if there is no information available
    // about the pointer.
    bool getEstimator(int32_t id, Estimator* outEstimator) const;
    
    // Gets the active pointer id, or -1 if none.
    inline int32_t getActivePointerId() const { return mActivePointerId; }
    
    // Gets a bitset containing all pointer ids from the most recent movement.
    inline BitSet32 getCurrentPointerIdBits() const { return mCurrentPointerIdBits; }
    
    void recycle();
    
    static VelocityTracker *obtain();
    static VelocityTracker *obtain(const char* strategy);
    
private:
    
    struct Velocity {
        float vx, vy;
    };
    
    static Pools::SynchronizedPool<VelocityTracker> sPool;
    
    const char* DEFAULT_STRATEGY = "lsq2";
    const char* mStrategyStr = NULL;
    
    nsecs_t mLastEventTime;
    BitSet32 mCurrentPointerIdBits;
    int32_t mActivePointerId;
    VelocityTrackerStrategy* mStrategy;
    
    BitSet32 mCalculatedIdBits;
    Velocity mCalculatedVelocity[MAX_POINTERS];
    
    VelocityTracker(char *strategy);
    
    bool configureStrategy(const char* strategy);
    
    static VelocityTrackerStrategy* createStrategy(const char* strategy);
};


/*
 * Implements a particular velocity tracker algorithm.
 */
class VelocityTrackerStrategy {
protected:
    VelocityTrackerStrategy() { }
    
public:
    virtual ~VelocityTrackerStrategy() { }
    
    virtual void clear() = 0;
    virtual void clearPointers(BitSet32 idBits) = 0;
    virtual void addMovement(nsecs_t eventTime, BitSet32 idBits,
                             const VelocityTracker::Position* positions) = 0;
    virtual bool getEstimator(int32_t id, VelocityTracker::Estimator* outEstimator) const = 0;
};


/*
 * Velocity tracker algorithm based on least-squares linear regression.
 */
class LeastSquaresVelocityTrackerStrategy : public VelocityTrackerStrategy {
public:
    enum Weighting {
        // No weights applied.  All data points are equally reliable.
        WEIGHTING_NONE,
        
        // Weight by time delta.  Data points clustered together are weighted less.
        WEIGHTING_DELTA,
        
        // Weight such that points within a certain horizon are weighed more than those
        // outside of that horizon.
        WEIGHTING_CENTRAL,
        
        // Weight such that points older than a certain amount are weighed less.
        WEIGHTING_RECENT,
    };
    
    // Degree must be no greater than Estimator::MAX_DEGREE.
    LeastSquaresVelocityTrackerStrategy(uint32_t degree, Weighting weighting = WEIGHTING_NONE);
    virtual ~LeastSquaresVelocityTrackerStrategy();
    
    virtual void clear();
    virtual void clearPointers(BitSet32 idBits);
    virtual void addMovement(nsecs_t eventTime, BitSet32 idBits,
                             const VelocityTracker::Position* positions);
    virtual bool getEstimator(int32_t id, VelocityTracker::Estimator* outEstimator) const;
    
private:
    // Sample horizon.
    // We don't use too much history by default since we want to react to quick
    // changes in direction.
    static const nsecs_t HORIZON = 100 * 1000000; // 100 ms
    
    // Number of samples to keep.
    static const uint32_t HISTORY_SIZE = 20;
    
    struct Movement {
        nsecs_t eventTime;
        BitSet32 idBits;
        VelocityTracker::Position positions[MAX_POINTERS];
        
        inline const VelocityTracker::Position& getPosition(uint32_t id) const {
            return positions[idBits.getIndexOfBit(id)];
        }
    };
    
    float chooseWeight(uint32_t index) const;
    
    const uint32_t mDegree;
    const Weighting mWeighting;
    uint32_t mIndex;
    Movement mMovements[HISTORY_SIZE];
};


/*
 * Velocity tracker algorithm that uses an IIR filter.
 */
class IntegratingVelocityTrackerStrategy : public VelocityTrackerStrategy {
public:
    // Degree must be 1 or 2.
    IntegratingVelocityTrackerStrategy(uint32_t degree);
    ~IntegratingVelocityTrackerStrategy();
    
    virtual void clear();
    virtual void clearPointers(BitSet32 idBits);
    virtual void addMovement(nsecs_t eventTime, BitSet32 idBits,
                             const VelocityTracker::Position* positions);
    virtual bool getEstimator(int32_t id, VelocityTracker::Estimator* outEstimator) const;
    
private:
    // Current state estimate for a particular pointer.
    struct State {
        nsecs_t updateTime;
        uint32_t degree;
        
        float xpos, xvel, xaccel;
        float ypos, yvel, yaccel;
    };
    
    const uint32_t mDegree;
    BitSet32 mPointerIdBits;
    State mPointerState[MAX_POINTER_ID + 1];
    
    void initState(State& state, nsecs_t eventTime, float xpos, float ypos) const;
    void updateState(State& state, nsecs_t eventTime, float xpos, float ypos) const;
    void populateEstimator(const State& state, VelocityTracker::Estimator* outEstimator) const;
};

ANDROID_END

#endif /* defined(__cocos2dx__VelocityTracker__) */
