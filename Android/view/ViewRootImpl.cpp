/*
 * File:   ViewRootImpl.cpp
 * Author: saulhoward
 *
 * Created on October 14, 2013, 3:30 PM
 */

#include "Android/graphics/HardwareRenderer.h"
#include "Android/view/ViewRootImpl.h"
#include "Android/view/AttachInfo.h"
#include "Android/view/View.h"
#include "Android/utils/Exceptions.h"

#include <mindroid/os/Clock.h>

ANDROID_BEGIN

ViewRootImpl::ViewRootImpl(Context *context) {
    
    m_thread = mindroid::Thread::currentThread();
    mContext = context;
    mAttachInfo = new AttachInfo(this, mHandler);
    m_width = context->getResources()->getDisplayMetrics().widthPixels;
    m_height = context->getResources()->getDisplayMetrics().heightPixels;
}

ViewRootImpl::~ViewRootImpl() {
}

void ViewRootImpl::bringChildToFront(shared_ptr<View> child) {
}

void ViewRootImpl::checkThread() {
    if (m_thread != mindroid::Thread::currentThread()) {
//        throw CalledFromWrongThreadException("Only the original thread that created a view hierarchy can touch its views.");
    }
}

void ViewRootImpl::clearChildFocus(shared_ptr<View> child) {
    checkThread();
    scheduleTraversals();
}

void ViewRootImpl::doTraversal() {
    if (m_traversalScheduled) {
        m_traversalScheduled = false;
        performTraversals();
    }
}

bool ViewRootImpl::getHostVisibility() {
    return m_appVisible ? mView->getVisibility() : View::GONE;
}

int ViewRootImpl::getLayoutDirection() {
    return View::LAYOUT_DIRECTION_RESOLVED_DEFAULT;
}

shared_ptr<ViewParent> ViewRootImpl::getParent() {
    return NULL;
}

int ViewRootImpl::getRootMeasureSpec(int windowSize, int rootDimension) {

    int measureSpec;
    switch (rootDimension) {

        case LayoutParams::MATCH_PARENT:
            measureSpec = View::MeasureSpec::makeMeasureSpec(windowSize, View::MeasureSpec::EXACTLY);
            break;
        case LayoutParams::WRAP_CONTENT:
            measureSpec = View::MeasureSpec::makeMeasureSpec(windowSize, View::MeasureSpec::AT_MOST);
            break;
        default:
            measureSpec = View::MeasureSpec::makeMeasureSpec(rootDimension, View::MeasureSpec::EXACTLY);
            break;
    }

    return measureSpec;
}

int ViewRootImpl::getTextDirection() {
    return View::TEXT_DIRECTION_RESOLVED_DEFAULT;
}

int ViewRootImpl::getTextAlignment() {
    return View::TEXT_ALIGNMENT_RESOLVED_DEFAULT;
}

vector<shared_ptr<View>> ViewRootImpl::getValidLayoutRequesters(vector<shared_ptr<View>> layoutRequesters, bool secondLayoutRequests) {

    int numViewsRequestingLayout = layoutRequesters.size();
    vector<shared_ptr<View>> validLayoutRequesters;

    for (int i = 0; i < numViewsRequestingLayout; i++) {
        shared_ptr<View> view = layoutRequesters[i];
        if (view && view->mAttachInfo && view->mParent &&
                    (secondLayoutRequests || (view->mPrivateFlags & View::PFLAG_FORCE_LAYOUT) ==
                            View::PFLAG_FORCE_LAYOUT)) {
            bool gone = false;
            shared_ptr<View> parent = view;
            while (parent) {
                if ((parent->mViewFlags & View::VISIBILITY_MASK) == View::GONE) {
                    gone = true;
                    break;
                }
                if (parent->getParent()->isView()) {
                    parent = dynamic_pointer_cast<View>(parent->getParent());
                } else {
                    parent = NULL;
                }
            }
            if (!gone) {
                validLayoutRequesters.push_back(view);
            }
        }
    }

    if (!secondLayoutRequests) {
        // If we're checking the layout flags, then we need to clean them up also
        for (int i = 0; i < numViewsRequestingLayout; ++i) {
            shared_ptr<View> view = layoutRequesters[i];
            while (view &&
                    (view->mPrivateFlags & View::PFLAG_FORCE_LAYOUT) != 0) {
                view->mPrivateFlags &= ~View::PFLAG_FORCE_LAYOUT;
                if (view->getParent()->isView()) {
                    view = dynamic_pointer_cast<View>(view->getParent());
                } else {
                    view = NULL;
                }
            }
        }
    }

    layoutRequesters.clear();
    return validLayoutRequesters;
}

void ViewRootImpl::invalidate() {
    m_dirty.set(0, 0, m_width, m_height);
    scheduleTraversals();
}

void ViewRootImpl::invalidate(bool invalidateCache) {

}

void ViewRootImpl::invalidateChild(shared_ptr<View> child, Rect &dirty) {
    vector<int> empty = vector<int>(2);
    invalidateChildInParent(empty, dirty);
}

shared_ptr<ViewParent> ViewRootImpl::invalidateChildInParent(vector<int> &location, Rect &dirty) {
    checkThread();
    
    if (dirty.isEmpty()) {
        invalidate();
        return NULL;
    } else if (dirty.isEmpty() && !m_isAnimating) {
        return NULL;
    }
    
//    if (mCurScrollY != 0 || mTranslator != NULL) {
//        mTempRect.set(dirty);
//        dirty = mTempRect;
//        if (mCurScrollY != 0) {
//            dirty.offset(0, -mCurScrollY);
//        }
//        if (mTranslator != null) {
//            mTranslator.translateRectInAppWindowToScreen(dirty);
//        }
//        if (mAttachInfo->mScalingRequired) {
//            dirty.inset(-1, -1);
//        }
//    }
    
    Rect &localDirty = m_dirty;
    if (!localDirty.isEmpty() && !localDirty.contains(dirty)) {
        mAttachInfo->m_setIgnoreDirtyState = true;
        mAttachInfo->m_ignoreDirtyState = true;
    }
    
    // Add the new dirty rect to the current one
    localDirty.unionWith(dirty);
    // Intersect with the bounds of the window to skip
    // updates that lie outside of the visible region
    const float appScale = mAttachInfo->m_applicationScale;
    const bool intersected = localDirty.intersect(0, 0,
                                                     (int) (m_width * appScale + 0.5f), (int) (m_height * appScale + 0.5f));
    if (!intersected) {
        localDirty.setEmpty();
    }
    if (!m_willDrawSoon && (intersected || m_isAnimating)) {
        scheduleTraversals();
    }
    
    return NULL;
}

bool ViewRootImpl::isInLayout() {
    return m_inLayout;
}

bool ViewRootImpl::isLayoutRequested() {
    return m_layoutRequested;
}

bool ViewRootImpl::measureHierarchy(const shared_ptr<View> &host, LayoutParams *lp, int desiredWindowWidth, int desiredWindowHeight) {

    int childWidthMeasureSpec;
    int childHeightMeasureSpec;
    bool windowSizeMayChange = false;

    childWidthMeasureSpec = getRootMeasureSpec(desiredWindowWidth, lp->width);
    childHeightMeasureSpec = getRootMeasureSpec(desiredWindowHeight, lp->height);
    performMeasure(childWidthMeasureSpec, childHeightMeasureSpec);
    if (m_width != host->getMeasuredWidth() || m_height != host->getMeasuredHeight()) {
        windowSizeMayChange = true;
    }

    return windowSizeMayChange;
}

void ViewRootImpl::performLayout(LayoutParams* lp, int desiredWindowWidth, int desiredWindowHeight) {

    m_layoutRequested = false;
    m_inLayout = true;

    const shared_ptr<View> &host = mView;

    host->layout(0, 0, host->getMeasuredWidth(), host->getMeasuredHeight());
    m_inLayout = false;
    int numViewsRequestingLayout = m_layoutRequesters.size();
    if (numViewsRequestingLayout > 0) {

        vector<shared_ptr<View>> validLayoutRequesters = getValidLayoutRequesters(m_layoutRequesters, false);
        if (validLayoutRequesters.size() > 0) {
            m_handlingLayoutInLayoutRequest = true;
            // Process fresh layout requests, then measure and layout
            int numValidRequests = validLayoutRequesters.size();
            for (int i = 0; i < numValidRequests; ++i) {
                shared_ptr<View> view = validLayoutRequesters[i];
                view->requestLayout();
            }
            measureHierarchy(host, lp, desiredWindowWidth, desiredWindowHeight);
            m_inLayout = true;
            host->layout(0, 0, host->getMeasuredWidth(), host->getMeasuredHeight());

            m_handlingLayoutInLayoutRequest = false;

            // Check the valid requests again, this time without checking/clearing the
            // layout flags, since requests happening during the second pass get noop'd
            validLayoutRequesters = getValidLayoutRequesters(m_layoutRequesters, true);
            if (validLayoutRequesters.size() > 0) {
                // Post second-pass requests to the next frame
                int numValidRequests = validLayoutRequesters.size();
                for (int i = 0; i < numValidRequests; ++i) {
                    shared_ptr<View> view = validLayoutRequesters[i];
                    view->requestLayout();
                }
            }
        }
    }

    m_inLayout = false;
}

void ViewRootImpl::performMeasure(int childWidthMeasureSpec, int childHeightMeasureSpec) {
    mView->measure(childWidthMeasureSpec, childHeightMeasureSpec);
}

void ViewRootImpl::performTraversals() {
    
    const shared_ptr<View> &host = mView;

    if (host == NULL || !m_added) return;

    m_isInTraversal = true;
    m_willDrawSoon = true;
    bool windowSizeMayChange = false;
    LayoutParams lp(LayoutParams::MATCH_PARENT, LayoutParams::MATCH_PARENT);

    int desiredWindowWidth = mContext->getResources()->getDisplayMetrics().widthPixels;
    int desiredWindowHeight = mContext->getResources()->getDisplayMetrics().heightPixels;
    
    AttachInfo *attachInfo = mAttachInfo;
    int viewVisibility = getHostVisibility();
    bool viewVisibilityChanged = mViewVisibility != viewVisibility;
    
    if (m_first) {
        // TODO: m_fullRedrawNeeded = true;
        m_layoutRequested = true;
        attachInfo->mWindowVisibility = viewVisibility;
        viewVisibilityChanged = false;
        host->dispatchAttachedToWindow(attachInfo, 0);
        
        mLastConfiguration.setTo(host->getResources()->getConfiguration());
        // Set the layout direction if it has not been set before (inherit is the default)
        if (mViewLayoutDirectionInitial == View::LAYOUT_DIRECTION_INHERIT) {
            host->setLayoutDirection(mLastConfiguration.getLayoutDirection());
        }

    } else {

    }

    if (viewVisibilityChanged) {
        attachInfo->mWindowVisibility = viewVisibility;
        host->dispatchWindowVisibilityChanged(viewVisibility);
    }

    bool layoutRequested = m_layoutRequested && !m_stopped;
    if (layoutRequested) {
        windowSizeMayChange |= measureHierarchy(host, &lp, desiredWindowWidth, desiredWindowHeight);
    }

    if (layoutRequested) {
        // Clear this now, so that if anything requests a layout in the
        // rest of this function we will catch it and re-run a full
        // layout pass.
        m_layoutRequested = false;
    }
    
    attachInfo->mWindowLeft = 0;
    attachInfo->mWindowTop = 0;

    bool didLayout = layoutRequested && !m_stopped;

    if (didLayout) {
        performLayout(&lp, desiredWindowWidth, desiredWindowHeight);
    }

    if (m_first) {

        bool hwInitialized = false;

        if (mAttachInfo->m_hardwareRenderer) {
            hwInitialized = mAttachInfo->m_hardwareRenderer->initialize();
        }

        if (mAttachInfo->m_hardwareRenderer) {
            if (hwInitialized ||
                    m_width != mAttachInfo->m_hardwareRenderer->getWidth() ||
                    m_height != mAttachInfo->m_hardwareRenderer->getHeight()) {
                mAttachInfo->m_hardwareRenderer->setup(m_width, m_height);
            }
        }
    }

    m_first = false;
    m_willDrawSoon = false;
    mViewVisibility = viewVisibility;

    performDraw();

    m_isInTraversal = false;
}

void ViewRootImpl::performDraw() {

    if (m_first) return;

    const bool fullRedrawNeeded = m_fullRedrawNeeded;
    m_fullRedrawNeeded = false;
    m_isDrawing = true;
    
    draw(fullRedrawNeeded);
    
    m_isDirty = true;
    m_isDrawing = false;
}

void ViewRootImpl::draw(bool fullRedrawNeeded) {

    AttachInfo *attachInfo = mAttachInfo;
    
    if (attachInfo->mViewScrollChanged) {
        attachInfo->mViewScrollChanged = false;
//        attachInfo->mTreeObserver.dispatchOnScrollChanged();
    }
    
    attachInfo->mDrawingTime = Clock::monotonicTime();
    attachInfo->m_hardwareRenderer->draw(mView, attachInfo, NULL);
}

void ViewRootImpl::requestChildFocus(shared_ptr<View> child, shared_ptr<View> focused) {
    checkThread();
    scheduleTraversals();
}

void ViewRootImpl::requestLayout() {

    if (!m_handlingLayoutInLayoutRequest) {
        m_layoutRequested = true;
        scheduleTraversals();
    }
}

bool ViewRootImpl::requestLayoutDuringLayout(shared_ptr<View> view) {

    if (!view->mParent || view->mAttachInfo) return true;

    if (find(m_layoutRequesters.begin(), m_layoutRequesters.end(), view) != m_layoutRequesters.end()) {
        m_layoutRequesters.push_back(view);
    }

    if (m_handlingLayoutInLayoutRequest) {
        // Let the request proceed normally; it will be processed in a second layout pass
        // if necessary
        return true;
    } else {
        // Don't let the request proceed during the second layout pass.
        // It will post to the next frame instead.
        return false;
    }
}

void ViewRootImpl::scheduleTraversals() {
    if (!m_traversalScheduled) {
        m_traversalScheduled = true;
        mHandler->post(m_traversalRunnable);
    }
}

void ViewRootImpl::unscheduleTraversals() {
    if (m_traversalScheduled) {
        m_traversalScheduled = false;
        mHandler->removeCallbacks(m_traversalRunnable);
    }
}

void ViewRootImpl::setView(shared_ptr<View> view, LayoutParams *attrs, shared_ptr<View> panelParentView) {

    if (mView == NULL) {
        mView = view;
        mViewLayoutDirectionInitial = mView->getRawLayoutDirection();
        mAttachInfo->mRootView = view;

        mAttachInfo->m_hardwareRenderer = HardwareRenderer::create(true);

        m_added = true;
        
        view->assignParent(shared_from_this());

        requestLayout();
    }
}

ANDROID_END