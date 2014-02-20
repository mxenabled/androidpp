//
//  DialView.cpp
//  MoneyMobileX
//
//  Created by Saul Howard on 1/6/14.
//
//

#include "DialView.h"

#include <math.h>
#include <cmath>

const float DialView::VELOCITY_TOLERANCE = 40.0f;
const float DialView::HIGH = 1000.0f;
const float DialView::LOW = 200.0f;

void DialView::setTouchState(int touchState) {
    mTouchState = touchState;
}

/**
 * Set the dynamics object used for fling and snap behavior.
 *
 * @param dynamics The dynamics object
 */
void DialView::setDynamics(Dynamics *dynamics) {
    
    if (mDynamics != NULL) {
        dynamics->setState(getRotationDegree(), mDynamics->getVelocity(), Clock::monotonicTime());
    }
    
    mDynamics = dynamics;
}

/**
 * Set the rotation degree of the piechart.
 *
 * @param rotationDegree the degree to rotate the piechart to
 */
void DialView::setRotationDegree(float rotationDegree) {
    
    // Keep rotation degree positive
    if (rotationDegree < 0) rotationDegree += 360;
    
    if (mOnDialChangeListener != NULL) {
        notifyDialChanged(rotationDegree);
    }
    
    // Keep rotation degree between 0 - 360
    mRotationDegree = (int) rotationDegree % 360;
    
    invalidate(true);
}

float DialView::getRotationDegree() {
    return mRotationDegree;
}

void DialView::setOnDialChangeListener(OnDialChangeListener *onDialChangeListener) {
    mOnDialChangeListener = onDialChangeListener;
}

void DialView::initialize() {
    
    Context *context = getContext();
    
    mScrollThreshold = getTouchSlop();
    
    mPixelDensity = context->getResources()->getDisplayMetrics().density;
    
    mCenter = vector<float>(2);
}

void DialView::notifyDialChanged(const float rotationDegree) {
    
    const uint64_t currentTime = Clock::monotonicTime();
    const float timeChange = (float) (currentTime - mLastChange) / 1000.0f;
    mLastChange = currentTime;
    
    float change = rotationDegree - mRotationDegree;
    if (abs(change) > 300) change += (change < 0 ? 360 : -360);
    
    const float rate = abs(change) / timeChange;
    
    if (rate < LOW) {
        change /= 12.0f;
    } else if (rate > HIGH) {
        change *= 2.0f;
    }
    
    mOnDialChangeListener->onDialChanged(change);
}

bool DialView::onTouchEvent(MotionEvent *event) {
    
    if (!isEnabled() || (!inCircle((int) event->getX(), (int) event->getY()) && mTouchState == TOUCH_STATE_RESTING)) {
        return false;
    }
    
    switch (event->getAction()) {
            
        case MotionEvent::ACTION_DOWN:
        {
            startTouch(event);
            break;
        }
            
        case MotionEvent::ACTION_MOVE:
        {
            if (mTouchState == TOUCH_STATE_CLICK) {
                startRotationIfNeeded(event);
            }
            
            if (mTouchState == TOUCH_STATE_ROTATE) {
                mVelocityTracker->addMovement(event);
                rotateChart(event->getX(), event->getY());
            }
            
            break;
        }
            
        case MotionEvent::ACTION_UP:
        {
            
            float velocity = 0;
            
            if (mTouchState == TOUCH_STATE_ROTATE) {
                
                mVelocityTracker->addMovement(event);
                mVelocityTracker->computeCurrentVelocity(PIXELS_PER_SECOND);
                
                velocity = calculateVelocity();
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
    
    return true;
}

/**
 * Calculates the overall vector velocity given both the x and y
 * velocities and normalized to be pixel independent.
 *
 * @return the overall vector velocity
 */
float DialView::calculateVelocity() {
    
    int direction = mRotatingClockwise ? 1 : -1;
    
    float velocityX = 0;
    float velocityY = 0;
    
    mVelocityTracker->getVelocity(&velocityX, &velocityY);
    
    velocityX /= mPixelDensity;
    velocityY /= mPixelDensity;
    
    return (float) sqrt(velocityX * velocityX + velocityY * velocityY) * direction / 2;
}

/**
 * Sets and initializes all things that need to when we start a touch
 * gesture.
 *
 * @param event The down event
 */
void DialView::startTouch(MotionEvent *event) {
    
    // user is touching the list -> no more fling
    removeCallbacks(mDynamicsRunnable);
    
    mLastRotation = getRotationDegree();
    
    // save the start place
    mTouchStartX = (int) event->getX();
    mTouchStartY = (int) event->getY();
    
    // obtain a velocity tracker and feed it its first event
    mVelocityTracker = VelocityTracker::obtain();
    mVelocityTracker->addMovement(event);
    
    // we don't know if it's a click or a scroll yet, but until we know
    // assume it's a click
    setTouchState(TOUCH_STATE_CLICK);
}

/**
 * Resets and recycles all things that need to when we end a touch gesture
 */
void DialView::endTouch(const float velocity) {
    
    // recycle the velocity tracker
    if (mVelocityTracker != NULL) {
        mVelocityTracker->recycle();
        mVelocityTracker = NULL;
    }
    
    // create the dynamics runnable if we haven't before
    if (mDynamicsRunnable == NULL) {
        mDynamicsRunnable = new DialRunnable(this);
    }
    
    if (mDynamics != NULL && abs(velocity) > getScaledMinimumFlingVelocity()) {
        // update the dynamics with the correct position and start the runnable
        mDynamics->setState(getRotationDegree(), velocity, Clock::monotonicTime());
        post(mDynamicsRunnable);
    }
    
    // reset touch state
    setTouchState(TOUCH_STATE_RESTING);
}

/**
 * Checks if the user has moved far enough for this to be a scroll and if
 * so, sets the list in scroll mode
 *
 * @param event The (move) event
 * @return true if scroll was started, false otherwise
 */
bool DialView::startRotationIfNeeded(MotionEvent *event) {
    
    const int xPos = (int) event->getX();
    const int yPos = (int) event->getY();
    
    if (isEnabled()
        && (xPos < mTouchStartX - mScrollThreshold
            || xPos > mTouchStartX + mScrollThreshold
            || yPos < mTouchStartY - mScrollThreshold
            || yPos > mTouchStartY + mScrollThreshold)) {
            
            setTouchState(TOUCH_STATE_ROTATE);
            
            mRotationStart = (float) (atan2(mCenter[1] - yPos, mCenter[0] - xPos)) * 180.0f / M_PI;
            
            return true;
        }
    
    return false;
}

/**
 * Rotate the piechart based on a given (x,y) coordinate
 *
 * @param x The x-coordinate
 * @param y The y-coordinate
 */
void DialView::rotateChart(const float x, const float y) {
    
    float degree = (float) ((atan2(mCenter[1] - y, mCenter[0] - x) * 180.0f / M_PI) - mRotationStart);
    
    // Rotate from the last rotation position to prevent rotation jumps
    rotateChart(mLastRotation + degree);
}

/**
 * Rotates the piechart rotation degree. Takes care of rotation (if enabled) and
 * snapping
 *
 * @param degree The degree to rotate to
 */
void DialView::rotateChart(float degree) {
    
    const float previous = getRotationDegree();
    
    setRotationDegree(degree);
    
    setRotatingClockwise(previous);
}

/**
 * Checks which way the piechart is rotating.
 *
 * @param previous The previous degree the piechart was rotated to
 */
void DialView::setRotatingClockwise(float previous) {
    
    const float change = (mRotationDegree - previous);
    mRotatingClockwise = (change > 0 && abs(change) < 300) || (abs(change) > 300 && mRotatingClockwise);
}

/**
 * Does the touch lie within the bounds of the current Pie Chart.
 *
 * @param x The x-coordinate
 * @param y The y-coordinate
 * @return True if the touch was inside the Pie Chart's circular bounds
 */
bool DialView::inCircle(const int x, const int y) {
    
    double dx = (x - mCenter[0]) * (x - mCenter[0]);
    double dy = (y - mCenter[1]) * (y - mCenter[1]);
    
    return (dx + dy) < ((getWidth() / 2) * (getWidth() / 2));
}

void DialView::setDialImage(shared_ptr<Drawable> dialImage) {
    
    if (dialImage == mDialImage) return;
    
    if (dialImage != NULL) {
        mDialImage = dialImage;
    }
}

void DialView::onLayout(bool changed, int left, int top, int right, int bottom) {
    View::onLayout(changed, left, top, right, bottom);
    
    // Get the center coordinates of the view
    mCenter[0] = (float) abs(left - right) / 2;
    mCenter[1] = (float) abs(top - bottom) / 2;
    
    if (mDialImage != NULL) {
        mDialImage->setBounds(0, 0,  mRight - mLeft, mBottom - mTop);
    }
}

void DialView::onDraw(shared_ptr<Canvas> canvas) {
    View::onDraw(canvas);
    
    if (mDialImage != NULL)  {
        canvas->rotate(mRotationDegree, mCenter[0], mCenter[1]);
        mDialImage->draw(canvas);
        canvas->restore();
    }
}