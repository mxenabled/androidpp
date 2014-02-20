//
//  DialView.h
//  MoneyMobileX
//
//  Created by Saul Howard on 1/6/14.
//
//

#ifndef __MoneyMobileX__DialView__
#define __MoneyMobileX__DialView__

#include "Android/view/View.h"
#include "Android/view/VelocityTracker.h"

#include <mindroid/os/Runnable.h>
#include <mindroid/os/Handler.h>
#include <mindroid/os/Clock.h>

#include "cocos2d.h"

#include "Dynamics.h"

using namespace androidcpp;

class DialView : public View {
    
public:
    
    /**
     * User is not touching the piechart
     */
    static const int TOUCH_STATE_RESTING = 0;
    
    /**
     * User is rotating the piechart
     */
    static const int TOUCH_STATE_ROTATE = 2;
    
    class OnDialChangeListener {
        
    public:
        
        virtual void onDialChanged(float amount) = 0;
    };
    
    void setDynamics(Dynamics *dynamics);
    
    float getRotationDegree();
    
    void setOnDialChangeListener(OnDialChangeListener *onDialChangeListener);
    
    DialView(Context *context) : DialView(context, NULL) {
    }
    
    DialView(Context *context, AttributeSet *attrs) : View(context, attrs) {
        initialize();
    }
    
    bool onTouchEvent(MotionEvent *event);
    
    bool inCircle(const int x, const int y);
    
    void setDialImage(shared_ptr<Drawable> dialImage);
    
protected:
    
    void onLayout(bool changed, int left, int top, int right, int bottom);
    
    void onDraw(shared_ptr<Canvas> canvas);
    
private:
    
    /**
     * Unit used for the velocity tracker
     */
    static const int PIXELS_PER_SECOND = 1000;
    
    /**
     * Tolerance for the velocity
     */
    static const float VELOCITY_TOLERANCE;
    
    /**
     * User is touching the list and right now it's still a "click"
     */
    static const int TOUCH_STATE_CLICK = 1;
    
    static const float HIGH;
    static const float LOW;
    
    shared_ptr<Drawable> mDialImage;
    
    /**
     * Current touch state
     */
    int mTouchState = TOUCH_STATE_RESTING;
    
    /**
     * Distance to drag before we intercept touch events
     */
    int mScrollThreshold = 0;
    
    /**
     * Velocity tracker used to get fling velocities
     */
    VelocityTracker *mVelocityTracker = NULL;
    
    /**
     * Dynamics object used to handle fling and snap
     */
    Dynamics *mDynamics = NULL;
    
    /**
     * X-coordinate of the down event
     */
    int mTouchStartX = 0;
    
    /**
     * Y-coordinate of the down event
     */
    int mTouchStartY = 0;
    
    /**
     * Our starting rotation degree
     */
    float mRotationStart = 0;
    
    /**
     * The last rotation degree after touch
     */
    float mLastRotation = 0;
    
    /**
     * The rotating direction of the piechart
     */
    bool mRotatingClockwise = false;
    
    /**
     * The pixel density of the current device
     */
    float mPixelDensity = 0;
    
    /**
     * The center point of the piechart
     */
    vector<float> mCenter;
    
    uint64_t mLastChange = 0LL;
    
    /**
     * The current degrees of rotation of the piechart
     */
    float mRotationDegree = 0;
    
    OnDialChangeListener *mOnDialChangeListener = NULL;
    
    void setTouchState(int touchState);
    
    void setRotationDegree(float rotationDegree);
    
    void initialize();
    
    void notifyDialChanged(const float rotationDegree);
    
    float calculateVelocity();
    
    void startTouch(MotionEvent *event);
    
    void endTouch(const float velocity);
    
    bool startRotationIfNeeded(MotionEvent *event);
    
    void rotateChart(const float x, const float y);
    
    void rotateChart(float degree);
    
    void setRotatingClockwise(float previous);
    
    class DialRunnable : public Runnable {
    public:
        
        DialRunnable(DialView *dialView) {
            mDialView = dialView;
        }
        
        void run() {
            
            // if we don't have any dynamics set we do nothing
            if (mDialView->mDynamics == NULL) {
                return;
            }
            
            // we pretend that each frame of the fling/snap animation is
            // one touch gesture and therefore set the start position
            // every time
            mDialView->mDynamics->update(Clock::monotonicTime());
            
            // Keep the rotation amount between 0 - 360
            mDialView->rotateChart((int) mDialView->mDynamics->getPosition() % 360);
            
            if (!mDialView->mDynamics->isAtRest(VELOCITY_TOLERANCE)) {
                
                // the list is not at rest, so schedule a new frame
                mDialView->postDelayed(this, 8);
            }
        }
        
    private:
        
        DialView *mDialView = NULL;
    };
    
    /**
     * Runnable used to animate fling and snap
     */
    sp<DialRunnable> mDynamicsRunnable = NULL;
};

#define CL_TYPE DialView // the type we want to register
#define CL_BASE_TYPE View
#define CL_TYPE_NAME "DialView" // the name of the type (need not be the same as the type)
#include "Android/utils/cl_reg.h" // include the supermacro

#endif /* defined(__MoneyMobileX__DialView__) */
