//
//  MotionEvent.h
//  cocos2dx
//
//  Created by Saul Howard on 11/4/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef __cocos2dx__MotionEvent__
#define __cocos2dx__MotionEvent__

#include "AndroidMacros.h"
#include "cocos2d.h"

#include <mindroid/os/Clock.h>

/*
 * Maximum number of pointers supported per motion event.
 * Smallest number of pointers is 1.
 * (We want at least 10 but some touch controllers obstensibly configured for 10 pointers
 * will occasionally emit 11.  There is not much harm making this constant bigger.)
 */
#define MAX_POINTERS 16

/*
 * Maximum pointer id value supported in a motion event.
 * Smallest pointer id is 0.
 * (This is limited by our use of BitSet32 to track pointer assignments.)
 */
#define MAX_POINTER_ID 31

using namespace cocos2d;

ANDROID_BEGIN

class MotionEvent {
public:
    MotionEvent() {}
    virtual ~MotionEvent();
    
    /**
     * An invalid pointer id.
     *
     * This value (-1) can be used as a placeholder to indicate that a pointer id
     * has not been assigned or is not available.  It cannot appear as
     * a pointer id inside a {@link MotionEvent}.
     */
    static const int INVALID_POINTER_ID = -1;
    
    /**
     * Bit mask of the parts of the action code that are the action itself.
     */
    static const int ACTION_MASK             = 0xff;
    
    /**
     * Constant for {@link #getActionMasked}: A pressed gesture has started, the
     * motion contains the initial starting location.
     * <p>
     * This is also a good time to check the button state to distinguish
     * secondary and tertiary button clicks and handle them appropriately.
     * Use {@link #getButtonState} to retrieve the button state.
     * </p>
     */
    static const int ACTION_DOWN             = 0;
    
    /**
     * Constant for {@link #getActionMasked}: A pressed gesture has finished, the
     * motion contains the final release location as well as any intermediate
     * points since the last down or move event.
     */
    static const int ACTION_UP               = 1;
    
    /**
     * Constant for {@link #getActionMasked}: A change has happened during a
     * press gesture (between {@link #ACTION_DOWN} and {@link #ACTION_UP}).
     * The motion contains the most recent point, as well as any intermediate
     * points since the last down or move event.
     */
    static const int ACTION_MOVE             = 2;
    
    /**
     * Constant for {@link #getActionMasked}: The current gesture has been aborted.
     * You will not receive any more points in it.  You should treat this as
     * an up event, but not perform any action that you normally would.
     */
    static const int ACTION_CANCEL           = 3;
    
    /**
     * Constant for {@link #getActionMasked}: A movement has happened outside of the
     * normal bounds of the UI element.  This does not provide a full gesture,
     * but only the initial location of the movement/touch.
     */
    static const int ACTION_OUTSIDE          = 4;
    
    /**
     * Constant for {@link #getActionMasked}: A non-primary pointer has gone down.
     * <p>
     * Use {@link #getActionIndex} to retrieve the index of the pointer that changed.
     * </p><p>
     * The index is encoded in the {@link #ACTION_POINTER_INDEX_MASK} bits of the
     * unmasked action returned by {@link #getAction}.
     * </p>
     */
    static const int ACTION_POINTER_DOWN     = 5;
    
    /**
     * Constant for {@link #getActionMasked}: A non-primary pointer has gone up.
     * <p>
     * Use {@link #getActionIndex} to retrieve the index of the pointer that changed.
     * </p><p>
     * The index is encoded in the {@link #ACTION_POINTER_INDEX_MASK} bits of the
     * unmasked action returned by {@link #getAction}.
     * </p>
     */
    static const int ACTION_POINTER_UP       = 6;
    
    /**
     * Constant for {@link #getActionMasked}: A change happened but the pointer
     * is not down (unlike {@link #ACTION_MOVE}).  The motion contains the most
     * recent point, as well as any intermediate points since the last
     * hover move event.
     * <p>
     * This action is always delivered to the window or view under the pointer.
     * </p><p>
     * This action is not a touch event so it is delivered to
     * {@link View#onGenericMotionEvent(MotionEvent)} rather than
     * {@link View#onTouchEvent(MotionEvent)}.
     * </p>
     */
    static const int ACTION_HOVER_MOVE       = 7;
    
    /**
     * Bits in the action code that represent a pointer index, used with
     * {@link #ACTION_POINTER_DOWN} and {@link #ACTION_POINTER_UP}.  Shifting
     * down by {@link #ACTION_POINTER_INDEX_SHIFT} provides the actual pointer
     * index where the data for the pointer going up or down can be found; you can
     * get its identifier with {@link #getPointerId(int)} and the actual
     * data with {@link #getX(int)} etc.
     *
     * @see #getActionIndex
     */
    static const int32_t ACTION_POINTER_INDEX_MASK  = 0xff00;
    
    /**
     * Bit shift for the action bits holding the pointer index as
     * defined by {@link #ACTION_POINTER_INDEX_MASK}.
     *
     * @see #getActionIndex
     */
    static const int ACTION_POINTER_INDEX_SHIFT = 8;
    
    void init(CCSet *touches, int32_t action) {
        m_touches = touches;
        m_action = action;
        m_eventTime = mindroid::Clock::monotonicTime();
    }
    
    int32_t getAction() const {
        return m_action;
    }
    
    int getActionMasked() const {
        return m_action & ACTION_MASK;
    }
    
    int getActionIndex() const {
        return (m_action & ACTION_POINTER_INDEX_MASK) >> ACTION_POINTER_INDEX_SHIFT;
    }
    
    int getX() const;
    int getY() const;
    int getPointerCount() const {
        return m_touches->count();
    }
    
    int getPointerId() const;
    int getPointerId(int pointerIndex) const;
    int getPointerIdBits() const;
    int findPointerIndex(int pointerId) const;
    int getX(int pointerIndex) const;
    int getY(int pointerIndex) const;
    void offsetLocation(float xOffset, float yOffset);
    
    inline nsecs_t getEventTime() const { return m_eventTime; }
    
    void setAction(int action) {
        m_action = action;
    }
    
private:
    
    int32_t m_action = 0;
    CCSet *m_touches = NULL;
    
    CCTouch *getDefault() const;
    CCTouch *getTouchAt(int index) const;
    nsecs_t m_eventTime = 0;
};

/*
 * Motion event actions.
 */

/* Bit shift for the action bits holding the pointer index as
 * defined by AMOTION_EVENT_ACTION_POINTER_INDEX_MASK.
 */
#define AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT 8

enum {
    /* Bit mask of the parts of the action code that are the action itself.
     */
    AMOTION_EVENT_ACTION_MASK = 0xff,
    
    /* Bits in the action code that represent a pointer index, used with
     * AMOTION_EVENT_ACTION_POINTER_DOWN and AMOTION_EVENT_ACTION_POINTER_UP.  Shifting
     * down by AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT provides the actual pointer
     * index where the data for the pointer going up or down can be found.
     */
    AMOTION_EVENT_ACTION_POINTER_INDEX_MASK  = 0xff00,
    
    /* A pressed gesture has started, the motion contains the initial starting location.
     */
    AMOTION_EVENT_ACTION_DOWN = 0,
    
    /* A pressed gesture has finished, the motion contains the final release location
     * as well as any intermediate points since the last down or move event.
     */
    AMOTION_EVENT_ACTION_UP = 1,
    
    /* A change has happened during a press gesture (between AMOTION_EVENT_ACTION_DOWN and
     * AMOTION_EVENT_ACTION_UP).  The motion contains the most recent point, as well as
     * any intermediate points since the last down or move event.
     */
    AMOTION_EVENT_ACTION_MOVE = 2,
    
    /* The current gesture has been aborted.
     * You will not receive any more points in it.  You should treat this as
     * an up event, but not perform any action that you normally would.
     */
    AMOTION_EVENT_ACTION_CANCEL = 3,
    
    /* A movement has happened outside of the normal bounds of the UI element.
     * This does not provide a full gesture, but only the initial location of the movement/touch.
     */
    AMOTION_EVENT_ACTION_OUTSIDE = 4,
    
    /* A non-primary pointer has gone down.
     * The bits in AMOTION_EVENT_ACTION_POINTER_INDEX_MASK indicate which pointer changed.
     */
    AMOTION_EVENT_ACTION_POINTER_DOWN = 5,
    
    /* A non-primary pointer has gone up.
     * The bits in AMOTION_EVENT_ACTION_POINTER_INDEX_MASK indicate which pointer changed.
     */
    AMOTION_EVENT_ACTION_POINTER_UP = 6,
    
    /* A change happened but the pointer is not down (unlike AMOTION_EVENT_ACTION_MOVE).
     * The motion contains the most recent point, as well as any intermediate points since
     * the last hover move event.
     */
    AMOTION_EVENT_ACTION_HOVER_MOVE = 7,
    
    /* The motion event contains relative vertical and/or horizontal scroll offsets.
     * Use getAxisValue to retrieve the information from AMOTION_EVENT_AXIS_VSCROLL
     * and AMOTION_EVENT_AXIS_HSCROLL.
     * The pointer may or may not be down when this event is dispatched.
     * This action is always delivered to the winder under the pointer, which
     * may not be the window currently touched.
     */
    AMOTION_EVENT_ACTION_SCROLL = 8,
    
    /* The pointer is not down but has entered the boundaries of a window or view.
     */
    AMOTION_EVENT_ACTION_HOVER_ENTER = 9,
    
    /* The pointer is not down but has exited the boundaries of a window or view.
     */
    AMOTION_EVENT_ACTION_HOVER_EXIT = 10,
};

/*
 * Constants that identify each individual axis of a motion event.
 * Refer to the documentation on the MotionEvent class for descriptions of each axis.
 */
enum {
    AMOTION_EVENT_AXIS_X = 0,
    AMOTION_EVENT_AXIS_Y = 1,
    AMOTION_EVENT_AXIS_PRESSURE = 2,
    AMOTION_EVENT_AXIS_SIZE = 3,
    AMOTION_EVENT_AXIS_TOUCH_MAJOR = 4,
    AMOTION_EVENT_AXIS_TOUCH_MINOR = 5,
    AMOTION_EVENT_AXIS_TOOL_MAJOR = 6,
    AMOTION_EVENT_AXIS_TOOL_MINOR = 7,
    AMOTION_EVENT_AXIS_ORIENTATION = 8,
    AMOTION_EVENT_AXIS_VSCROLL = 9,
    AMOTION_EVENT_AXIS_HSCROLL = 10,
    AMOTION_EVENT_AXIS_Z = 11,
    AMOTION_EVENT_AXIS_RX = 12,
    AMOTION_EVENT_AXIS_RY = 13,
    AMOTION_EVENT_AXIS_RZ = 14,
    AMOTION_EVENT_AXIS_HAT_X = 15,
    AMOTION_EVENT_AXIS_HAT_Y = 16,
    AMOTION_EVENT_AXIS_LTRIGGER = 17,
    AMOTION_EVENT_AXIS_RTRIGGER = 18,
    AMOTION_EVENT_AXIS_THROTTLE = 19,
    AMOTION_EVENT_AXIS_RUDDER = 20,
    AMOTION_EVENT_AXIS_WHEEL = 21,
    AMOTION_EVENT_AXIS_GAS = 22,
    AMOTION_EVENT_AXIS_BRAKE = 23,
    AMOTION_EVENT_AXIS_DISTANCE = 24,
    AMOTION_EVENT_AXIS_TILT = 25,
    AMOTION_EVENT_AXIS_GENERIC_1 = 32,
    AMOTION_EVENT_AXIS_GENERIC_2 = 33,
    AMOTION_EVENT_AXIS_GENERIC_3 = 34,
    AMOTION_EVENT_AXIS_GENERIC_4 = 35,
    AMOTION_EVENT_AXIS_GENERIC_5 = 36,
    AMOTION_EVENT_AXIS_GENERIC_6 = 37,
    AMOTION_EVENT_AXIS_GENERIC_7 = 38,
    AMOTION_EVENT_AXIS_GENERIC_8 = 39,
    AMOTION_EVENT_AXIS_GENERIC_9 = 40,
    AMOTION_EVENT_AXIS_GENERIC_10 = 41,
    AMOTION_EVENT_AXIS_GENERIC_11 = 42,
    AMOTION_EVENT_AXIS_GENERIC_12 = 43,
    AMOTION_EVENT_AXIS_GENERIC_13 = 44,
    AMOTION_EVENT_AXIS_GENERIC_14 = 45,
    AMOTION_EVENT_AXIS_GENERIC_15 = 46,
    AMOTION_EVENT_AXIS_GENERIC_16 = 47,
    
    // NOTE: If you add a new axis here you must also add it to several other files.
    //       Refer to frameworks/base/core/java/android/view/MotionEvent.java for the full list.
};

ANDROID_END

#endif /* defined(__cocos2dx__MotionEvent__) */
