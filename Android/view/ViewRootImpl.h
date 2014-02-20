/*
 * File:   ViewRootImpl.h
 * Author: saulhoward
 *
 * Created on October 14, 2013, 3:30 PM
 */

#ifndef VIEWROOTIMPL_H
#define	VIEWROOTIMPL_H

#include "AndroidMacros.h"

#include "Android/content/Context.h"
#include "Android/content/res/Configuration.h"
#include "Android/view/LayoutParams.h"
#include "Android/view/ViewParent.h"
#include "Android/graphics/Rect.h"

#include <vector>
#include <algorithm>
#include <memory>

#include <mindroid/os/Handler.h>
#include <mindroid/os/Thread.h>
#include <mindroid/os/Looper.h>
#include <mindroid/os/LooperThread.h>
#include <mindroid/os/Semaphore.h>
#include <mindroid/os/Ref.h>

#include "cocos2d.h"

ANDROID_BEGIN

class View;
class AttachInfo;

using namespace mindroid;
using namespace std;

class ViewRootImpl : public ViewParent, public enable_shared_from_this<ViewRootImpl> {

    friend class Activity;

public:
    ViewRootImpl(Context *context);
    virtual ~ViewRootImpl();

    virtual void bringChildToFront(shared_ptr<View> child);
    virtual bool canResolveLayoutDirection() { return true; }
    virtual void childDrawableStateChanged(shared_ptr<View> child) {}
    void clearChildFocus(shared_ptr<View> child);
    virtual shared_ptr<ViewParent> getParent();
    void invalidate();
    virtual void invalidate(bool invalidateCache);
    virtual void invalidateChild(shared_ptr<View> child, Rect &dirty);
    virtual shared_ptr<ViewParent> invalidateChildInParent(vector<int> &location, Rect &dirty);
    bool isInLayout();
    virtual bool isLayoutRequested();
    virtual bool isView() { return false; }
    void requestChildFocus(shared_ptr<View> child, shared_ptr<View> focused);
    virtual void requestLayout();
    bool requestLayoutDuringLayout(shared_ptr<View> view);
    void setView(shared_ptr<View> view, LayoutParams *attrs, shared_ptr<View> panelParentView);
    
    virtual bool canResolveTextDirection() {
        return true;
    }
    
    virtual int getLayoutDirection();
    
    virtual bool isLayoutDirectionResolved() {
        return true;
    }
    
    virtual bool isTextDirectionResolved() {
        return true;
    }
    
    virtual int getTextDirection();
    
    virtual bool canResolveTextAlignment() {
        return true;
    }
    
    virtual bool isTextAlignmentResolved() {
        return true;
    }
    
    virtual int getTextAlignment();
    
    virtual void requestDisallowInterceptTouchEvent(bool disallowIntercept) {
        // ViewAncestor never intercepts touch event, so this can be a no-op
    }
    
private:
    
    class TraversalRunnable : public Runnable {
    public:
        
        TraversalRunnable(ViewRootImpl *viewRootImpl) { mViewRootImpl = viewRootImpl; }
        
        virtual void run() {
            mViewRootImpl->doTraversal();
        }
    private:
        ViewRootImpl *mViewRootImpl;
    };
    
    
    class ViewRootHandler : public Handler
    {
    public:
        static const uint32_t MSG_START_ASYNC_TASKS = 1;
        static const uint32_t MSG_PRINT_INFO = 2;
        
        ViewRootHandler(ViewRootImpl *viewRootImpl) { mViewRootImpl = viewRootImpl; }
        
        virtual void handleMessage(const mindroid::sp<Message>& message) {
            switch (message->what) {
                case MSG_START_ASYNC_TASKS: {
                    CCLOG("Start async");
                    break;
                }
                case MSG_PRINT_INFO:
                    CCLOG("Handler1::handleMessage %p with ID %d by Looper %p",
                          message.getPointer(), message->what, Looper::myLooper());
                    break;
                default:
                    CCLOG("Handler1::handleMessage: Oops, this message ID is not valid");
                    break;
            }
        }
    private:
        ViewRootImpl *mViewRootImpl;
    };
    
    bool m_added = false;
    bool m_appVisible = true;
    AttachInfo *mAttachInfo = NULL;
    bool m_first = true;
    bool m_fullRedrawNeeded = true;
    sp<Handler> mHandler = new ViewRootHandler(this);
    bool m_handlingLayoutInLayoutRequest = false;
    int m_height = 0;
    bool m_isDrawing = false;
    bool m_inLayout;
    bool m_isInTraversal;
    bool m_isAnimating = false;
    bool m_layoutRequested;
    vector<shared_ptr<View>> m_layoutRequesters;
    Rect m_dirty;
    int mViewLayoutDirectionInitial = 0;
    bool m_stopped = false;
    sp<mindroid::Thread> m_thread = NULL;
    sp<TraversalRunnable> m_traversalRunnable = new TraversalRunnable(this);
    bool m_traversalScheduled = false;
    shared_ptr<View> mView;
    bool mViewVisibility;
    int m_width = 0;
    bool m_willDrawSoon = false;

    void checkThread();
    void doTraversal();
    void draw(bool fullRedrawNeeded);
    bool getHostVisibility();
    int getRootMeasureSpec(int windowSize, int rootDimension);
    vector<shared_ptr<View>> getValidLayoutRequesters(vector<shared_ptr<View>> layoutRequesters, bool secondLayoutRequests);
    bool measureHierarchy(const shared_ptr<View> &host, LayoutParams *lp, int desiredWindowWidth, int desiredWindowHeight);
    void performDraw();
    void performLayout(LayoutParams *lp, int desiredWindowWidth, int desiredWindowHeight);
    void performMeasure(int childWidthMeasureSpec, int childHeightMeasureSpec);
    void performTraversals();
    void scheduleTraversals();
    void unscheduleTraversals();
    
protected:
    Context *mContext;
    
    bool m_isDirty = false;
    Configuration mLastConfiguration = Configuration();
};

ANDROID_END

#endif	/* VIEWROOTIMPL_H */

