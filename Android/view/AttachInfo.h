/*
 * File:   AttachInfo.h
 * Author: saulhoward
 *
 * Created on October 15, 2013, 3:16 PM
 */

#ifndef ATTACHINFO_H
#define	ATTACHINFO_H

#include "AndroidMacros.h"

#include "Android/graphics/HardwareRenderer.h"
#include "Android/graphics/Matrix.h"
#include "Android/graphics/Rect.h"
#include "Android/view/animation/Transformation.h"

#include <mindroid/os/Handler.h>

#include <memory>
#include <vector>

ANDROID_BEGIN

using namespace mindroid;
using namespace std;

class View;
class ViewRootImpl;
class Canvas;
class RectF;
class ViewTreeObserver;

class AttachInfo {
public:
    float m_applicationScale = 1.0f;
    
    /**
     * Indicates the time at which drawing started to occur.
     */
    uint64_t mDrawingTime = 0;
    
    shared_ptr<View> mRootView;
    shared_ptr<View> mViewRequestingLayout;
    ViewRootImpl *mViewRootImpl = NULL;
    sp<Handler> mHandler = NULL;
    shared_ptr<Canvas> m_hardwareCanvas;
    HardwareRenderer *m_hardwareRenderer;
    bool m_ignoreDirtyState = false;
    vector<int> mInvalidateChildLocation;
    bool m_setIgnoreDirtyState = false;
    
    /**
     * Set to true if a view has been scrolled.
     */
    bool mViewScrollChanged = false;
    
    bool mHasWindowFocus = true;
    
    /**
     * The view tree observer used to dispatch global events like
     * layout, pre-draw, touch mode change, etc.
     */
    ViewTreeObserver *mTreeObserver;
    
    /**
     * Temporary for use in transforming invalidation rect
     */
    Transformation mTmpTransformation;
    
    /**
     * Global to the view hierarchy used as a temporary for dealing with
     * x/y location when view is transformed.
     */
    vector<float> mTmpTransformLocation = vector<float>(2);
    Matrix m_tmpMatrix;
    RectF mTmpTransformRect;
    Rect mTmpInvalRect;
    bool mViewVisibilityChanged;
    int mWindowVisibility;
    
    /**
     * Left position of this view's window
     */
    int mWindowLeft = 0;
    
    /**
     * Top position of this view's window
     */
    int mWindowTop = 0;

    AttachInfo(ViewRootImpl *viewRootImpl, sp<Handler> handler);
};

ANDROID_END

#endif	/* ATTACHINFO_H */

