/*
 * File:   ViewGroup.cpp
 * Author: saulhoward
 *
 * Created on October 7, 2013, 5:15 PM
 */

#include "ViewGroup.h"

#include "Android/view/MarginLayoutParams.h"
#include "Android/view/MotionEvent.h"
#include "Android/utils/System.h"

#include <utils/Timers.h>
#include <vector>
#include <algorithm>
#include <iterator>

ANDROID_BEGIN

ViewGroup::ViewGroup(Context *context, AttributeSet *attrs) : View::View(context, attrs) {
    initViewGroup();
}

ViewGroup::~ViewGroup() {
}

void ViewGroup::addTouchables(vector<shared_ptr<View>> &views) {
    View::addTouchables(views);
    
    const vector<shared_ptr<View>> &children = mChildren;
    typedef vector<shared_ptr<View>>::const_iterator iter;
    
    for (iter it = children.begin(); it != children.end(); ++it) {
        shared_ptr<View> child = *it;
        if ((child->mViewFlags & VISIBILITY_MASK) == VISIBLE) {
            child->addTouchables(views);
        }
    }
}

void ViewGroup::attachViewToParent(shared_ptr<View> child, int index, LayoutParams *params) {
    child->mLayoutParams = params;
    
    if (index < 0) {
        index = mChildrenCount;
    }
    
    addInArray(child, index);
    
    child->mParent = static_pointer_cast<ViewGroup>(shared_from_this());
    child->mPrivateFlags = (child->mPrivateFlags & ~PFLAG_DIRTY_MASK
                           & ~PFLAG_DRAWING_CACHE_VALID)
    | PFLAG_DRAWN | PFLAG_INVALIDATED;
    mPrivateFlags |= PFLAG_INVALIDATED;
    
    if (child->hasFocus()) {
        requestChildFocus(child, child->findFocus());
    }
}

void ViewGroup::initViewGroup() {

    setFlags(WILL_NOT_DRAW, DRAW_MASK);
    mGroupFlags |= FLAG_CLIP_CHILDREN;
    mGroupFlags |= FLAG_CLIP_TO_PADDING;
    mGroupFlags |= FLAG_ANIMATION_DONE;
    mGroupFlags |= FLAG_ANIMATION_CACHE;
    mGroupFlags |= FLAG_ALWAYS_DRAWN_WITH_CACHE;
}

void ViewGroup::addView(shared_ptr<View> child) {
    addView(child, -1);
}

void ViewGroup::addView(shared_ptr<View> child, int index) {

    LayoutParams *params = child->getLayoutParams();

    if (!params) {
        params = generateDefaultLayoutParams();
    }

    addView(child, index, params);
}

void ViewGroup::addView(shared_ptr<View> child, int width, int height) {

    LayoutParams *params = child->getLayoutParams();
    params->width = width;
    params->height = height;

    addView(child, -1, params);
}

void ViewGroup::addView(shared_ptr<View> child, LayoutParams *params) {
    addView(child, -1, params);
}

void ViewGroup::addView(shared_ptr<View> child, int index, LayoutParams *params) {

    requestLayout();
    invalidate(true);
    addViewInner(child, index, params, false);
}

/**
 * Adds a view during layout. This is useful if in your onLayout() method,
 * you need to add more views (as does the list view for example).
 *
 * If index is negative, it means put it at the end of the list.
 *
 * @param child the view to add to the group
 * @param index the index at which the child must be added
 * @param params the layout parameters to associate with the child
 * @param preventRequestLayout if true, calling this method will not trigger a
 *        layout request on child
 * @return true if the child was added, false otherwise
 */
bool ViewGroup::addViewInLayout(shared_ptr<View> child, int index, LayoutParams *params,
                                  bool preventRequestLayout) {
    child->mParent = NULL;
    addViewInner(child, index, params, preventRequestLayout);
    child->mPrivateFlags = (child->mPrivateFlags & ~PFLAG_DIRTY_MASK) | PFLAG_DRAWN;
    return true;
}

void ViewGroup::addInArray(shared_ptr<View> child, int index) {

    if (index == mChildrenCount) {
        mChildren.push_back(child);
    } else if (index < mChildrenCount) {
        if (index < 0) {
            mChildren.insert(mChildren.begin(), child);
        } else {
            mChildren.insert(mChildren.begin() + index, child);
        }
    }

    mChildrenCount++;
}

/**
 * Adds a touch target for specified child to the beginning of the list.
 * Assumes the target child is not already present.
 */
TouchTarget *ViewGroup::addTouchTarget(shared_ptr<View> child, int pointerIdBits) {
    TouchTarget *target = TouchTarget::obtain(child, pointerIdBits);
    target->next = m_firstTouchTarget;
    m_firstTouchTarget = target;
    return target;
}

void ViewGroup::addViewInner(shared_ptr<View> child, int index, LayoutParams *params, bool preventRequestLayout) {

    if (child->getParent()) {

        CCLOG("Child already has parent");
        
        return; // Child already has parent
    }

    if (!checkLayoutParams(params)) {
        params = generateLayoutParams(params);
    }

    if (preventRequestLayout) {
        child->mLayoutParams = params;
    } else {
        child->setLayoutParams(params);
    }

    if (index < 0) {
        index = mChildrenCount;
    }

    addInArray(child, index);

    child->assignParent(dynamic_pointer_cast<ViewParent>(shared_from_this()));

    AttachInfo *ai = mAttachInfo;

    if (ai) {
        child->dispatchAttachedToWindow(ai, 0);
    }
}

void ViewGroup::bringChildToFront(shared_ptr<View> child) {
    iter index = indexOfChild(child);
    if (index != mChildren.end()) {
        removeFromArray(index);
        addInArray(child, mChildrenCount);
        child->mParent = dynamic_pointer_cast<ViewParent>(shared_from_this());
    }
}

/**
 * Cancels and clears all touch targets.
 */
void ViewGroup::cancelAndClearTouchTargets(MotionEvent *event) {
    if (m_firstTouchTarget) {
        //        bool syntheticEvent = false;
        //        if (!event) {
        //            nsecs_t now = systemTime(SYSTEM_TIME_REALTIME);
        //            event = MotionEvent::obtain(now, now, MotionEvent::ACTION_CANCEL, 0.0f, 0.0f, 0);
        //            event->setSource(InputDevice::SOURCE_TOUCHSCREEN);
        //            syntheticEvent = true;
        //        }
        
        for (TouchTarget *target = m_firstTouchTarget; target; target = target->next) {
            resetCancelNextUpFlag(target->child);
            dispatchTransformedTouchEvent(event, true, target->child, target->pointerIdBits);
        }
        clearTouchTargets();
        
        //        if (syntheticEvent) {
        //            event->recycle();
        //        }
    }
}

void ViewGroup::cancelTouchTarget(shared_ptr<View> view) {
//    TouchTarget *predecessor = NULL;
//    TouchTarget *target = mFirstTouchTarget;
//    while (target != NULL) {
//        final TouchTarget next = target.next;
//        if (target.child == view) {
//            if (predecessor == null) {
//                mFirstTouchTarget = next;
//            } else {
//                predecessor.next = next;
//            }
//            target.recycle();
//            
//            final long now = SystemClock.uptimeMillis();
//            MotionEvent event = MotionEvent.obtain(now, now,
//                                                   MotionEvent.ACTION_CANCEL, 0.0f, 0.0f, 0);
//            event.setSource(InputDevice.SOURCE_TOUCHSCREEN);
//            view.dispatchTouchEvent(event);
//            event.recycle();
//            return;
//        }
//        predecessor = target;
//        target = next;
//    }
}

bool ViewGroup::canViewReceivePointerEvents(const shared_ptr<View> child) {
    return (child->mViewFlags & VISIBILITY_MASK) == VISIBLE;// || child->getAnimation();
}

bool ViewGroup::checkLayoutParams(LayoutParams *params) {
    return params != NULL;
}

void ViewGroup::clearChildFocus(shared_ptr<View> child) {
    
    mFocused = NULL;
    if (mParent != NULL) {
        mParent->clearChildFocus(shared_from_this());
    }
}

void ViewGroup::detachAllViewsFromParent() {
    const int count = mChildrenCount;
    if (count <= 0) {
        return;
    }
    
    vector<shared_ptr<View>> &children = mChildren;
    mChildrenCount = 0;
    
    for (int i = count - 1; i >= 0; i--) {
        children[i]->mParent = NULL;
        children[i] = NULL;
    }
}

void ViewGroup::childDrawableStateChanged(androidcpp::shared_ptr<View> child) {
    if ((mGroupFlags & FLAG_ADD_STATES_FROM_CHILDREN) != 0) {
        refreshDrawableState();
    }
}

/**
 * Clears all touch targets.
 */
void ViewGroup::clearTouchTargets() {
    TouchTarget *target = m_firstTouchTarget;
    if (target) {
        do {
            TouchTarget *next = target->next;
            target->recycle();
            target = next;
        } while (target);
        m_firstTouchTarget = NULL;
    }
}

void ViewGroup::dispatchAttachedToWindow(AttachInfo *info, int visibility) {
    mGroupFlags |= FLAG_PREVENT_DISPATCH_ATTACHED_TO_WINDOW;
    View::dispatchAttachedToWindow(info, visibility);
    mGroupFlags &= ~FLAG_PREVENT_DISPATCH_ATTACHED_TO_WINDOW;
    
    const vector<shared_ptr<View>> &children = mChildren;
    typedef vector<shared_ptr<View>>::const_iterator iter;
    for (iter it = children.begin(); it != children.end(); ++it) {
        shared_ptr<View> child = *it;
        child->dispatchAttachedToWindow(info, visibility | (child->mViewFlags & VISIBILITY_MASK));
    }
}

void ViewGroup::dispatchDraw(shared_ptr<Canvas> canvas) {
    
    const vector<shared_ptr<View>> &children = mChildren;
    
    bool more = false;
    const uint64_t drawingTime = 0;
    
    typedef vector<shared_ptr<View>>::const_iterator iter;
    for (iter it = children.begin(); it != children.end(); ++it) {
        shared_ptr<View> child = *it;
        if ((child->mViewFlags & VISIBILITY_MASK) == VISIBLE) {
            more |= drawChild(canvas, child, drawingTime);
        }
    }
}

void ViewGroup::dispatchGetDisplayList() {
    
    typedef vector<shared_ptr<View>>::const_iterator iter;
    for (iter it = mChildren.begin(); it != mChildren.end(); ++it) {
        shared_ptr<View> child = *it;
        if (((child->mViewFlags & VISIBILITY_MASK) == VISIBLE || child->getAnimation() != NULL) &&
            child->hasStaticLayer()) {
            child->m_recreateDisplayList = (child->mPrivateFlags & PFLAG_INVALIDATED)
                == PFLAG_INVALIDATED;
            child->mPrivateFlags &= ~PFLAG_INVALIDATED;
            child->getDisplayList();
            child->m_recreateDisplayList = false;
        }
    }
//    if (mOverlay != null) {
//        View overlayView = mOverlay.getOverlayView();
//        overlayView.mRecreateDisplayList = (overlayView.mPrivateFlags & PFLAG_INVALIDATED)
//        == PFLAG_INVALIDATED;
//        overlayView.mPrivateFlags &= ~PFLAG_INVALIDATED;
//        overlayView.getDisplayList();
//        overlayView.mRecreateDisplayList = false;
//    }
}

bool ViewGroup::dispatchTouchEvent(MotionEvent *ev) {
    
    bool handled = false;
    
    const int action = ev->getAction();
    const int actionMasked = action & MotionEvent::ACTION_MASK;
    
    // Handle an initial down.
    if (actionMasked == MotionEvent::ACTION_DOWN) {
        // Throw away all previous state when starting a new touch gesture.
        // The framework may have dropped the up or cancel event for the previous gesture
        // due to an app switch, ANR, or some other state change.
        cancelAndClearTouchTargets(ev);
        resetTouchState();
    }
    
    // Check for interception.
    bool intercepted = false;
    if (actionMasked == MotionEvent::ACTION_DOWN || m_firstTouchTarget) {
        const bool disallowIntercept = (mGroupFlags & FLAG_DISALLOW_INTERCEPT) != 0;
        if (!disallowIntercept) {
            intercepted = onInterceptTouchEvent(ev);
            ev->setAction(action); // restore action in case it was changed
        } else {
            intercepted = false;
        }
    } else {
        // There are no touch targets and this action is not an initial down
        // so this view group continues to intercept touches.
        intercepted = true;
    }
    
    // Check for cancelation.
    const bool canceled = resetCancelNextUpFlag(View::shared_from_this()) || actionMasked == MotionEvent::ACTION_CANCEL;
    
    // Update list of touch targets for pointer down, if needed.
    const bool split = (mGroupFlags & FLAG_SPLIT_MOTION_EVENTS) != 0;
    TouchTarget *newTouchTarget = NULL;
    bool alreadyDispatchedToNewTouchTarget = false;
    
    if (!canceled && !intercepted) {
        
        if (actionMasked == MotionEvent::ACTION_DOWN
            || (split && actionMasked == MotionEvent::ACTION_POINTER_DOWN)
            || actionMasked == MotionEvent::ACTION_HOVER_MOVE) {
            
            const int actionIndex = ev->getActionIndex(); // always 0 for down
            const int idBitsToAssign = split ? 1 << ev->getPointerId(actionIndex) : TouchTarget::ALL_POINTER_IDS;
            
            // Clean up earlier touch targets for this pointer id in case they
            // have become out of sync.
            removePointersFromTouchTargets(idBitsToAssign);
            
            const int childrenCount = mChildrenCount;
            if (!newTouchTarget && childrenCount != 0) {
                const float x = ev->getX(actionIndex);
                const float y = ev->getY(actionIndex);
                // Find a child that can receive the event->
                // Scan children from front to back.
                const vector<shared_ptr<View>> &children = mChildren;
                
                const bool customOrder = isChildrenDrawingOrderEnabled();
                for (int i = childrenCount - 1; i >= 0; i--) {
                    const int childIndex = customOrder ? getChildDrawingOrder(childrenCount, i) : i;
                    shared_ptr<View> child = children[childIndex];
                    if (!canViewReceivePointerEvents(child) || !isTransformedTouchPointInView(x, y, child)) {
                        continue;
                    }
                    
                    newTouchTarget = getTouchTarget(child);
                    if (newTouchTarget) {
                        // Child is already receiving touch within its bounds.
                        // Give it the new pointer in addition to the ones it is handling.
                        newTouchTarget->pointerIdBits |= idBitsToAssign;
                        break;
                    }
                    
                    resetCancelNextUpFlag(child);
                    if (dispatchTransformedTouchEvent(ev, false, child, idBitsToAssign)) {
                        // Child wants to receive touch within its bounds.
//                        m_lastTouchDownTime = ev->getDownTime();
                        m_lastTouchDownIndex = childIndex;
                        m_lastTouchDownX = ev->getX();
                        m_lastTouchDownY = ev->getY();
                        newTouchTarget = addTouchTarget(child, idBitsToAssign);
                        alreadyDispatchedToNewTouchTarget = true;
                        break;
                    }
                }
            }
            
            if (!newTouchTarget && m_firstTouchTarget) {
                // Did not find a child to receive the event->
                // Assign the pointer to the least recently added target.
                newTouchTarget = m_firstTouchTarget;
                while (newTouchTarget->next) {
                    newTouchTarget = newTouchTarget->next;
                }
                newTouchTarget->pointerIdBits |= idBitsToAssign;
            }
        }
    }
    
    // Dispatch to touch targets.
    if (!m_firstTouchTarget) {
        // No touch targets so treat this as an ordinary view->
        handled = dispatchTransformedTouchEvent(ev, canceled, NULL, TouchTarget::ALL_POINTER_IDS);
    } else {
        // Dispatch to touch targets, excluding the new touch target if we already
        // dispatched to it.  Cancel touch targets if necessary.
        TouchTarget *predecessor = NULL;
        TouchTarget *target = m_firstTouchTarget;
        while (target) {
            TouchTarget *next = target->next;
            if (alreadyDispatchedToNewTouchTarget && target == newTouchTarget) {
                handled = true;
            } else {
                const bool cancelChild = resetCancelNextUpFlag(target->child) || intercepted;
                if (dispatchTransformedTouchEvent(ev, cancelChild,
                                                  target->child, target->pointerIdBits)) {
                    handled = true;
                }
                if (cancelChild) {
                    if (!predecessor) {
                        m_firstTouchTarget = next;
                    } else {
                        predecessor->next = next;
                    }
                    target->recycle();
                    target = next;
                    continue;
                }
            }
            predecessor = target;
            target = next;
        }
    }
    
    // Update list of touch targets for pointer up or cancel, if needed.
    if (canceled
        || actionMasked == MotionEvent::ACTION_UP
        || actionMasked == MotionEvent::ACTION_HOVER_MOVE) {
        resetTouchState();
    } else if (split && actionMasked == MotionEvent::ACTION_POINTER_UP) {
        const int actionIndex = ev->getActionIndex();
        const int idBitsToRemove = 1 << ev->getPointerId(actionIndex);
        removePointersFromTouchTargets(idBitsToRemove);
    }
    
    return handled;
}

void ViewGroup::dispatchSetPressed(bool pressed) {
    const vector<shared_ptr<View>> &children = mChildren;
    typedef vector<shared_ptr<View>>::const_iterator iter;
    
    for (iter it = children.begin(); it != children.end(); ++it) {
        shared_ptr<View> child = *it;
        // Children that are clickable on their own should not
        // show a pressed state when their parent view does.
        // Clearing a pressed state always propagates.
        if (!pressed || (!child->isClickable() && !child->isLongClickable())) {
            child->setPressed(pressed);
        }
    }
}

/**
 * Transforms a motion event into the coordinate space of a particular child view,
 * filters out irrelevant pointer ids, and overrides its action if necessary.
 * If child is NULL, assumes the MotionEvent will be sent to this ViewGroup instead.
 */
bool ViewGroup::dispatchTransformedTouchEvent(MotionEvent *event, bool cancel,
                                              shared_ptr<View> child, int desiredPointerIdBits) {
    bool handled = false;
    
    // Canceling motions is a special case.  We don't need to perform any transformations
    // or filtering.  The important part is the action, not the contents.
    const int oldAction = event->getAction();
    if (cancel || oldAction == MotionEvent::ACTION_CANCEL) {
        event->setAction(MotionEvent::ACTION_CANCEL);
        if (child == NULL) {
            handled = View::dispatchTouchEvent(event);
        } else {
            handled = child->dispatchTouchEvent(event);
        }
        event->setAction(oldAction);
        return handled;
    }
    
    // Calculate the number of pointers to deliver.
    const int oldPointerIdBits = event->getPointerIdBits();
    const int newPointerIdBits = oldPointerIdBits & desiredPointerIdBits;
    
    // If for some reason we ended up in an inconsistent state where it looks like we
    // might produce a motion event with no pointers in it, then drop the event.
    if (newPointerIdBits == 0) {
        return false;
    }
    
    // If the number of pointers is the same and we don't need to perform any fancy
    // irreversible transformations, then we can reuse the motion event for this
    // dispatch as long as we are careful to revert any changes we make.
    // Otherwise we need to make a copy.
//    MotionEvent *transformedEvent = NULL;
//    if (newPointerIdBits == oldPointerIdBits) {
//        if (!child || child->hasIdentityMatrix()) {
            if (child == NULL) {
                handled = View::dispatchTouchEvent(event);
            } else {
                const float offsetX = mScrollX - child->mLeft;
                const float offsetY = mScrollY - child->mTop;
                event->offsetLocation(offsetX, offsetY);
                
                handled = child->dispatchTouchEvent(event);
                
                event->offsetLocation(-offsetX, -offsetY);
            }
            return handled;
//        }
//        transformedEvent = MotionEvent::obtain(event);
//    } else {
//        transformedEvent = event->split(newPointerIdBits);
//    }
//    
//    // Perform any necessary transformations and dispatch.
//    if (!child) {
//        handled = View::dispatchTouchEvent(transformedEvent);
//    } else {
//        const float offsetX = mScrollX - child->mLeft;
//        const float offsetY = mScrollY - child->mTop;
//        transformedEvent->offsetLocation(offsetX, offsetY);
//        if (!child->hasIdentityMatrix()) {
//            transformedEvent->transform(child->getInverseMatrix());
//        }
        
//        handled = child->dispatchTouchEvent(transformedEvent);
//    }
//    
//    return handled;
}

bool ViewGroup::drawChild(shared_ptr<Canvas> canvas, shared_ptr<View> child, uint64_t drawingTime) {
    return child->draw(canvas, this, drawingTime);
}

void ViewGroup::drawableStateChanged() {
    View::drawableStateChanged();
    
    if ((mGroupFlags & FLAG_NOTIFY_CHILDREN_ON_DRAWABLE_STATE_CHANGE) != 0) {
        if ((mGroupFlags & FLAG_ADD_STATES_FROM_CHILDREN) != 0) {
            throw IllegalStateException("addStateFromChildren cannot be enabled if a child has duplicateParentState set to true");
        }
        
        const vector<shared_ptr<View>> &children = mChildren;
        typedef vector<shared_ptr<View>>::const_iterator iter;
        
        for (iter it = children.begin(); it != children.end(); ++it) {
            shared_ptr<View> child = *it;
            if ((child->mViewFlags & DUPLICATE_PARENT_STATE) != 0) {
                child->refreshDrawableState();
            }
        }
    }
}

shared_ptr<View> ViewGroup::findFocus() {
    if (isFocused()) {
        return shared_from_this();
    }
    
    if (mFocused != NULL) {
        return mFocused->findFocus();
    }
    return NULL;
}

shared_ptr<View> ViewGroup::findViewTraversal(string id) {
    if (id == m_id) {
        return shared_from_this();
    }
    
    const vector<shared_ptr<View>> &children = mChildren;
    typedef vector<shared_ptr<View>>::const_iterator iter;
    for (iter it = children.begin(); it != children.end(); ++it) {
        shared_ptr<View> child = *it;
        if ((child->mPrivateFlags & PFLAG_IS_ROOT_NAMESPACE) == 0) {
            child = child->findSharedViewById(id);
            
            if (child != NULL) {
                return child;
            }
        }
    }
    
    return NULL;
}

shared_ptr<View> ViewGroup::findViewWithTagTraversal(shared_ptr<Object> tag) {
    if (tag != NULL && tag->equals(mTag.get())) {
        return shared_from_this();
    }
    
    const vector<shared_ptr<View>> &children = mChildren;
    typedef vector<shared_ptr<View>>::const_iterator iter;
    for (iter it = children.begin(); it != children.end(); ++it) {
        shared_ptr<View> v = *it;
        
        if ((v->mPrivateFlags & PFLAG_IS_ROOT_NAMESPACE) == 0) {
            v = v->findViewWithTagTraversal(tag);
            
            if (v != NULL) {
                return v;
            }
        }
    }
    
    return NULL;
}

LayoutParams *ViewGroup::generateDefaultLayoutParams() {
    return new LayoutParams(LayoutParams::WRAP_CONTENT, LayoutParams::WRAP_CONTENT);
}

LayoutParams *ViewGroup::generateLayoutParams(LayoutParams *layoutParams) {
    return layoutParams;
}

LayoutParams *ViewGroup::generateLayoutParams(AttributeSet *attrs) {
    return new LayoutParams(getContext(), attrs);
}

shared_ptr<View> ViewGroup::getChildAt(int index) {
	if (index < 0 || index >= mChildrenCount) {
		return NULL;
	}
    
	return mChildren[index];
}

int ViewGroup::getChildCount() {
	return mChildrenCount;
}

int ViewGroup::getChildMeasureSpec(int spec, int padding, int childDimension) {
    int specMode = MeasureSpec::getMode(spec);
    int specSize = MeasureSpec::getSize(spec);
    
    int size = max(0, specSize - padding);
    
    int resultSize = 0;
    int resultMode = 0;
    
    switch (specMode) {
            // Parent has imposed an exact size on us
        case MeasureSpec::EXACTLY:
            if (childDimension >= 0) {
                resultSize = childDimension;
                resultMode = MeasureSpec::EXACTLY;
            } else if (childDimension == LayoutParams::MATCH_PARENT) {
                // Child wants to be our size. So be it.
                resultSize = size;
                resultMode = MeasureSpec::EXACTLY;
            } else if (childDimension == LayoutParams::WRAP_CONTENT) {
                // Child wants to determine its own size. It can't be
                // bigger than us.
                resultSize = size;
                resultMode = MeasureSpec::AT_MOST;
            }
            break;
            
            // Parent has imposed a maximum size on us
        case MeasureSpec::AT_MOST:
            if (childDimension >= 0) {
                // Child wants a specific size... so be it
                resultSize = childDimension;
                resultMode = MeasureSpec::EXACTLY;
            } else if (childDimension == LayoutParams::MATCH_PARENT) {
                // Child wants to be our size, but our size is not fixed.
                // Constrain child to not be bigger than us.
                resultSize = size;
                resultMode = MeasureSpec::AT_MOST;
            } else if (childDimension == LayoutParams::WRAP_CONTENT) {
                // Child wants to determine its own size. It can't be
                // bigger than us.
                resultSize = size;
                resultMode = MeasureSpec::AT_MOST;
            }
            break;
            
            // Parent asked to see how big we want to be
        case MeasureSpec::UNSPECIFIED:
            if (childDimension >= 0) {
                // Child wants a specific size... let him have it
                resultSize = childDimension;
                resultMode = MeasureSpec::EXACTLY;
            } else if (childDimension == LayoutParams::MATCH_PARENT) {
                // Child wants to be our size... find out how big it should
                // be
                resultSize = 0;
                resultMode = MeasureSpec::UNSPECIFIED;
            } else if (childDimension == LayoutParams::WRAP_CONTENT) {
                // Child wants to determine its own size.... find out how
                // big it should be
                resultSize = 0;
                resultMode = MeasureSpec::UNSPECIFIED;
            }
            break;
    }
    return MeasureSpec::makeMeasureSpec(resultSize, resultMode);
}

shared_ptr<View> ViewGroup::getFocusedChild() {
    return mFocused;
}

void ViewGroup::offsetRectBetweenParentAndChild(View *descendant, Rect rect,
                                     bool offsetFromChildToParent, bool clipToBounds) {
    
    // already in the same coord system :)
    if (descendant == this) {
        return;
    }
    
    shared_ptr<ViewParent> theParent = descendant->mParent;
    shared_ptr<View> p = dynamic_pointer_cast<View>(theParent);
    
    // search and offset up to the parent
    while ((theParent != NULL)
           && p != NULL
           && (theParent.get() != this)) {
        
        if (offsetFromChildToParent) {
            rect.offset(descendant->mLeft - descendant->mScrollX,
                        descendant->mTop - descendant->mScrollY);
            if (clipToBounds) {
                rect.intersect(0, 0, p->mRight - p->mLeft, p->mBottom - p->mTop);
            }
        } else {
            if (clipToBounds) {
                rect.intersect(0, 0, p->mRight - p->mLeft, p->mBottom - p->mTop);
            }
            rect.offset(descendant->mScrollX - descendant->mLeft,
                        descendant->mScrollY - descendant->mTop);
        }
        
        descendant = p.get();
        theParent = descendant->mParent;
    }
    
    // now that we are up to this view, need to offset one more time
    // to get into our coordinate space
    if (theParent.get() == this) {
        if (offsetFromChildToParent) {
            rect.offset(descendant->mLeft - descendant->mScrollX,
                        descendant->mTop - descendant->mScrollY);
        } else {
            rect.offset(descendant->mScrollX - descendant->mLeft,
                        descendant->mScrollY - descendant->mTop);
        }
    } else {
        throw IllegalArgumentException("parameter must be a descendant of this view");
    }
}

/**
 * Gets the touch target for specified child view->
 * Returns NULL if not found.
 */
TouchTarget *ViewGroup::getTouchTarget(const shared_ptr<View> child) {
    for (TouchTarget *target = m_firstTouchTarget; target; target = target->next) {
        if (target->child == child) {
            return target;
        }
    }
    return NULL;
}

/**
 * Returns the position in the group of the specified child view.
 *
 * @param child the view for which to get the position
 * @return a positive integer representing the position of the view in the
 *         group, or -1 if the view does not exist in the group
 */
ViewGroup::iter ViewGroup::indexOfChild(shared_ptr<View> child) {
    
    iter it;
    
    for (it = mChildren.begin(); it != mChildren.end(); ++it) {
        
        shared_ptr<View> view = *it;
        if (view == child) {
            return it;
        }
    }
    
    return it;
}

void ViewGroup::invalidateChild(shared_ptr<View> child, Rect &dirty) {
    shared_ptr<ViewParent> parent = dynamic_pointer_cast<ViewParent>(shared_from_this());
    
    AttachInfo *attachInfo = mAttachInfo;
    if (attachInfo) {
        // If the child is drawing an animation, we want to copy this flag onto
        // ourselves and the parent to make sure the invalidate request goes
        // through
        const bool drawAnimation = (child->mPrivateFlags & PFLAG_DRAW_ANIMATION)
        == PFLAG_DRAW_ANIMATION;
        
        // Check whether the child that requests the invalidate is fully opaque
        // Views being animated or transformed are not considered opaque because we may
        // be invalidating their old position and need the parent to paint behind them.
        Matrix childMatrix = child->getMatrix();
        bool isOpaque = child->isOpaque() && !drawAnimation &&
        child->getAnimation() == NULL && childMatrix.isIdentity();
        // Mark the child as dirty, using the appropriate flag
        // Make sure we do not set both flags at the same time
        int opaqueFlag = isOpaque ? PFLAG_DIRTY_OPAQUE : PFLAG_DIRTY;
        
//        if (child->m_layerType != LAYER_TYPE_NONE) {
            mPrivateFlags |= PFLAG_INVALIDATED;
            mPrivateFlags &= ~PFLAG_DRAWING_CACHE_VALID;
            child->mLocalDirtyRect.unionWith(dirty);
//        }
        
        vector<int> &location = attachInfo->mInvalidateChildLocation;
        location[CHILD_LEFT_INDEX] = child->mLeft;
        location[CHILD_TOP_INDEX] = child->mTop;
        if (!childMatrix.isIdentity() ||
            (mGroupFlags & ViewGroup::FLAG_SUPPORT_STATIC_TRANSFORMATIONS) != 0) {
            RectF &boundingRect = attachInfo->mTmpTransformRect;
            boundingRect.set(dirty);
            Matrix *transformMatrix = NULL;
            if ((mGroupFlags & ViewGroup::FLAG_SUPPORT_STATIC_TRANSFORMATIONS) != 0) {
                Transformation &t = attachInfo->mTmpTransformation;
                bool transformed = getChildStaticTransformation(child, t);
                if (transformed) {
                    transformMatrix = &attachInfo->m_tmpMatrix;
                    *transformMatrix = t.getMatrix();
                    if (!childMatrix.isIdentity()) {
                        transformMatrix->preConcat(childMatrix);
                    }
                } else {
                    transformMatrix = &childMatrix;
                }
            } else {
                transformMatrix = &childMatrix;
            }
            transformMatrix->mapRect(&boundingRect);
            dirty.set((int) (boundingRect.left() - 0.5f),
                      (int) (boundingRect.top() - 0.5f),
                      (int) (boundingRect.right() + 0.5f),
                      (int) (boundingRect.bottom() + 0.5f));
        }
        
        do {
            shared_ptr<View> view;
            if (parent->isView()) {
                view = dynamic_pointer_cast<View>(parent);
            }
            
            if (drawAnimation) {
                if (view != NULL) {
                    view->mPrivateFlags |= PFLAG_DRAW_ANIMATION;
                } //else if (parent instanceof ViewRootImpl) {
//                    ((ViewRootImpl) parent).mIsAnimating = true;
//                }
            }
            
            // If the parent is dirty opaque or not dirty, mark it dirty with the opaque
            // flag coming from the child that initiated the invalidate
            if (view) {
                if ((view->mViewFlags & FADING_EDGE_MASK) != 0 && view->getSolidColor() == 0) {
                    opaqueFlag = PFLAG_DIRTY;
                }
                if ((view->mPrivateFlags & PFLAG_DIRTY_MASK) != PFLAG_DIRTY) {
                    view->mPrivateFlags = (view->mPrivateFlags & ~PFLAG_DIRTY_MASK) | opaqueFlag;
                }
            }
            
            parent = parent->invalidateChildInParent(location, dirty);
            if (view != NULL) {
                // Account for transform on current parent
                Matrix m = view->getMatrix();
                if (!m.isIdentity()) {
                    RectF &boundingRect = attachInfo->mTmpTransformRect;
                    boundingRect.set(dirty);
                    m.mapRect(&boundingRect);
                    dirty.set((int) (boundingRect.left() - 0.5f),
                              (int) (boundingRect.top() - 0.5f),
                              (int) (boundingRect.right() + 0.5f),
                              (int) (boundingRect.bottom() + 0.5f));
                }
            }
        } while (parent != NULL);
    }
}

void ViewGroup::invalidateChildFast(shared_ptr<View> child, Rect &dirty) {
    shared_ptr<ViewParent> parent = dynamic_pointer_cast<ViewParent>(shared_from_this());
    
    const AttachInfo *attachInfo = mAttachInfo;
    if (attachInfo != NULL) {
        child->mLocalDirtyRect.unionWith(dirty);
        
        int left = child->mLeft;
        int top = child->mTop;
        if (!child->getMatrix().isIdentity()) {
            child->transformRect(dirty);
        }
        
        do {
            shared_ptr<ViewGroup> parentVG = static_pointer_cast<ViewGroup>(parent);
            if (parentVG != NULL) {
               
                // Layered parents should be recreated, not just re-issued
                parentVG->invalidate();
                parent = NULL;
            } else {
                // Reached the top; this calls into the usual invalidate method in
                // ViewRootImpl, which schedules a traversal
                vector<int> location = attachInfo->mInvalidateChildLocation;
                location[0] = left;
                location[1] = top;
                parent = parent->invalidateChildInParent(location, dirty);
            }
        } while (parent != NULL);
    }
}

/**
 * Don't call or override this method. It is used for the implementation of
 * the view hierarchy.
 *
 * This implementation returns null if this ViewGroup does not have a parent,
 * if this ViewGroup is already fully invalidated or if the dirty rectangle
 * does not intersect with this ViewGroup's bounds.
 */
shared_ptr<ViewParent> ViewGroup::invalidateChildInParent(vector<int> &location, Rect &dirty) {
    if ((mPrivateFlags & PFLAG_DRAWN) == PFLAG_DRAWN ||
        (mPrivateFlags & PFLAG_DRAWING_CACHE_VALID) == PFLAG_DRAWING_CACHE_VALID) {
        if ((mGroupFlags & (FLAG_OPTIMIZE_INVALIDATE | FLAG_ANIMATION_DONE)) !=
            FLAG_OPTIMIZE_INVALIDATE) {
            dirty.offset(location[CHILD_LEFT_INDEX] - mScrollX,
                         location[CHILD_TOP_INDEX] - mScrollY);
            if ((mGroupFlags & FLAG_CLIP_CHILDREN) == 0) {
                dirty.unionWith(0, 0, mRight - mLeft, mBottom - mTop);
            }
            
            const int left = mLeft;
            const int top = mTop;
            
            if ((mGroupFlags & FLAG_CLIP_CHILDREN) == FLAG_CLIP_CHILDREN) {
                if (!dirty.intersect(0, 0, mRight - left, mBottom - top)) {
                    dirty.setEmpty();
                }
            }
            mPrivateFlags &= ~PFLAG_DRAWING_CACHE_VALID;
            
            location[CHILD_LEFT_INDEX] = left;
            location[CHILD_TOP_INDEX] = top;
            
//            if (mLayerType != LAYER_TYPE_NONE) {
                mPrivateFlags |= PFLAG_INVALIDATED;
                mLocalDirtyRect.unionWith(dirty);
//            }
            
            return mParent;
            
        } else {
            mPrivateFlags &= ~PFLAG_DRAWN & ~PFLAG_DRAWING_CACHE_VALID;
            
            location[CHILD_LEFT_INDEX] = mLeft;
            location[CHILD_TOP_INDEX] = mTop;
            if ((mGroupFlags & FLAG_CLIP_CHILDREN) == FLAG_CLIP_CHILDREN) {
                dirty.set(0, 0, mRight - mLeft, mBottom - mTop);
            } else {
                // in case the dirty rect extends outside the bounds of this container
                dirty.unionWith(0, 0, mRight - mLeft, mBottom - mTop);
            }
            
//            if (mLayerType != LAYER_TYPE_NONE) {
                mPrivateFlags |= PFLAG_INVALIDATED;
                mLocalDirtyRect.unionWith(dirty);
//            }
            
            return mParent;
        }
    }
    
    return NULL;
}

/**
 * Quick invalidation method that simply transforms the dirty rect into the parent's
 * coordinate system, pruning the invalidation if the parent has already been invalidated.
 */
shared_ptr<ViewParent> ViewGroup::invalidateChildInParentFast(int left, int top, Rect dirty) {
    if ((mPrivateFlags & PFLAG_DRAWN) == PFLAG_DRAWN ||
        (mPrivateFlags & PFLAG_DRAWING_CACHE_VALID) == PFLAG_DRAWING_CACHE_VALID) {
        dirty.offset(left - mScrollX, top - mScrollY);
        if ((mGroupFlags & FLAG_CLIP_CHILDREN) == 0) {
            dirty.unionWith(0, 0, mRight - mLeft, mBottom - mTop);
        }
        
        if ((mGroupFlags & FLAG_CLIP_CHILDREN) == 0 ||
            dirty.intersect(0, 0, mRight - mLeft, mBottom - mTop)) {
            
            if (m_layerType != LAYER_TYPE_NONE) {
                mLocalDirtyRect.unionWith(dirty);
            }
            if (!getMatrix().isIdentity()) {
                transformRect(dirty);
            }
            
            return mParent;
        }
    }
    
    return NULL;
}

bool ViewGroup::isChildrenDrawingOrderEnabled() {
    return (mGroupFlags & FLAG_USE_CHILD_DRAWING_ORDER) == FLAG_USE_CHILD_DRAWING_ORDER;
}

// TODO: Removed outLocalPoint variable as we don't have port for data type, see Android Source
bool ViewGroup::isTransformedTouchPointInView(float x, float y, const shared_ptr<View> child) {
    float localX = x + mScrollX - child->mLeft;
    float localY = y + mScrollY - child->mTop;
//    if (!child->hasIdentityMatrix() && mAttachInfo) {
//        const float[] localXY = mAttachInfo.mTmpTransformLocation;
//        localXY[0] = localX;
//        localXY[1] = localY;
//        child->getInverseMatrix().mapPoints(localXY);
//        localX = localXY[0];
//        localY = localXY[1];
//    }
    const bool isInView = child->pointInView(localX, localY);
//    if (isInView && outLocalPoint) {
//        outLocalPoint->set(localX, localY);
//    }
    return isInView;
}

void ViewGroup::measureChildren(int widthMeasureSpec, int heightMeasureSpec) {
    const vector<shared_ptr<View>> &children = mChildren;
    typedef vector<shared_ptr<View>>::const_iterator iter;
    for (iter it = children.begin(); it != children.end(); ++it) {
        shared_ptr<View> child = *it;
        if ((child->mViewFlags & VISIBILITY_MASK) != GONE) {
            measureChild(child, widthMeasureSpec, heightMeasureSpec);
        }
    }
}

void ViewGroup::measureChild(shared_ptr<View> child, int parentWidthMeasureSpec,
                            int parentHeightMeasureSpec) {
    LayoutParams *lp = child->getLayoutParams();
    
    const int childWidthMeasureSpec = getChildMeasureSpec(parentWidthMeasureSpec,
                                                          mPaddingLeft + mPaddingRight, lp->width);
    const int childHeightMeasureSpec = getChildMeasureSpec(parentHeightMeasureSpec,
                                                           mPaddingTop + mPaddingBottom, lp->height);
    
    child->measure(childWidthMeasureSpec, childHeightMeasureSpec);
}

void ViewGroup::measureChildWithMargins(shared_ptr<View> child,
                                        int parentWidthMeasureSpec, int widthUsed,
                                        int parentHeightMeasureSpec, int heightUsed) {
    MarginLayoutParams *lp = static_cast<MarginLayoutParams*>(child->getLayoutParams());
    
    const int childWidthMeasureSpec = getChildMeasureSpec(parentWidthMeasureSpec,
                                                          mPaddingLeft + mPaddingRight + lp->leftMargin + lp->rightMargin
                                                          + widthUsed, lp->width);
    const int childHeightMeasureSpec = getChildMeasureSpec(parentHeightMeasureSpec,
                                                           mPaddingTop + mPaddingBottom + lp->topMargin + lp->bottomMargin
                                                           + heightUsed, lp->height);
    
    child->measure(childWidthMeasureSpec, childHeightMeasureSpec);
}

void ViewGroup::offsetChildrenTopAndBottom(int offset) {
    const int count = mChildrenCount;
    const vector<shared_ptr<View>> &children = mChildren;
    bool invalidate = false;
    
    for (int i = 0; i < count; i++) {
        const shared_ptr<View> &v = children[i];
        v->mTop += offset;
        v->mBottom += offset;
        if (v->mDisplayList != NULL) {
            invalidate = true;
            v->mDisplayList->offsetTopBottom(offset);
        }
    }
    
    if (invalidate) {
        invalidateViewProperty(false, false);
    }
}

vector<int> ViewGroup::onCreateDrawableState(int extraSpace) {
    if ((mGroupFlags & FLAG_ADD_STATES_FROM_CHILDREN) == 0) {
        return View::onCreateDrawableState(extraSpace);
    }
    
    int need = 0;
    int n = getChildCount();
    for (int i = 0; i < n; i++) {
        vector<int> childState = getChildAt(i)->getDrawableState();
        need += childState.size();
    }
    
    vector<int> state = View::onCreateDrawableState(extraSpace + need);
    
    for (int i = 0; i < n; i++) {
        vector<int> childState = getChildAt(i)->getDrawableState();
        state = mergeDrawableStates(state, childState);
    }
    
    return state;
}

void ViewGroup::onViewRemoved(shared_ptr<View> child) {
}

void ViewGroup::removePointersFromTouchTargets(int pointerIdBits) {
    TouchTarget *predecessor = NULL;
    TouchTarget *target = m_firstTouchTarget;
    while (target) {
        TouchTarget *next = target->next;
        if ((target->pointerIdBits & pointerIdBits) != 0) {
            target->pointerIdBits &= ~pointerIdBits;
            if (target->pointerIdBits == 0) {
                if (!predecessor) {
                    m_firstTouchTarget = next;
                } else {
                    predecessor->next = next;
                }
                target->recycle();
                target = next;
                continue;
            }
        }
        predecessor = target;
        target = next;
    }
}

void ViewGroup::removeAllViews() {
    removeAllViewsInLayout();
    requestLayout();
    invalidate(true);
}

void ViewGroup::removeAllViewsInLayout() {

    const int count = mChildrenCount;
    if (count <= 0) {
        return;
    }

    mChildrenCount = 0;

    const bool detach = !mAttachInfo;
    
    typedef vector<shared_ptr<View>>::const_reverse_iterator iter;
    for (iter it = mChildren.rbegin(); it != mChildren.rend(); ++it) {
        shared_ptr<View> view = *it;

        if (detach) {
            view->dispatchDetachedFromWindow();
        }

        onViewRemoved(view);

        view->mParent = NULL;
    }
    
    mChildren.clear();
}

void ViewGroup::removeDetachedView(shared_ptr<View> child, bool animate) {
//    if (mTransition != NULL) {
//        mTransition.removeChild(this, child);
//    }
//    
//    if (child == mFocused) {
//        child->clearFocus();
//    }
//    
//    child->clearAccessibilityFocus();
    
    cancelTouchTarget(child);
//    cancelHoverTarget(child);
    
//    if ((animate && child->getAnimation() != NULL) ||
//        (mTransitioningViews != NULL && mTransitioningViews.contains(child))) {
//        addDisappearingView(child);
//    } else
    if (child->mAttachInfo != NULL) {
        child->dispatchDetachedFromWindow();
    }
    
//    if (child->hasTransientState()) {
//        childHasTransientStateChanged(child, false);
//    }
    
    onViewRemoved(child);
}

// This method also sets the child's mParent to null
void ViewGroup::removeFromArray(iter index) {
    
    if (index == mChildren.end()) return;
    
    shared_ptr<View> view = *index;
    
//    if (!(m_transitioningViews != NULL && m_transitioningViews.contains(mChildren[index]))) {
        view->mParent = NULL;
//    }
    
    --mChildrenCount;
    mChildren.erase(index);
    
    int count = distance(mChildren.begin(), index);
    if (m_lastTouchDownIndex == count) {
        m_lastTouchDownTime = 0;
        m_lastTouchDownIndex = -1;
    } else if (m_lastTouchDownIndex > count) {
        m_lastTouchDownIndex--;
    }
}

void ViewGroup::removeFromArray(int start, int count) {
    vector<shared_ptr<View>> &children = mChildren;
    const int childrenCount = mChildrenCount;
    
    start = max(0, start);
    const int end = min(childrenCount, start + count);
    
    if (start == end) {
        return;
    }
    
    if (end == childrenCount) {
        for (int i = start; i < end; i++) {
            children[i]->mParent = NULL;
            children[i] = NULL;
        }
    } else {
        for (int i = start; i < end; i++) {
            children[i]->mParent = NULL;
        }
        
        // Since we're looping above, we might as well do the copy, but is arraycopy()
        // faster than the extra 2 bounds checks we would do in the loop?
        System::arraycopy(children, end, children, start, childrenCount - end);
        
        for (int i = childrenCount - (end - start); i < childrenCount; i++) {
            children[i] = NULL;
        }
    }
    
    mChildrenCount -= (end - start);
}

void ViewGroup::requestChildFocus(shared_ptr<View> child, shared_ptr<View> focused) {

    if (getDescendantFocusability() == FOCUS_BLOCK_DESCENDANTS) {
        return;
    }
    
    // Unfocus us, if necessary
    View::unFocus();
    
    // We had a previous notion of who had focus. Clear it.
    if (mFocused != child) {
        if (mFocused != NULL) {
            mFocused->unFocus();
        }
        
        mFocused = child;
    }
    if (mParent != NULL) {
        mParent->requestChildFocus(shared_from_this(), focused);
    }
}

void ViewGroup::requestDisallowInterceptTouchEvent(bool disallowIntercept) {
    
    if (disallowIntercept == ((mGroupFlags & FLAG_DISALLOW_INTERCEPT) != 0)) {
        // We're already in this state, assume our ancestors are too
        return;
    }
    
    if (disallowIntercept) {
        mGroupFlags |= FLAG_DISALLOW_INTERCEPT;
    } else {
        mGroupFlags &= ~FLAG_DISALLOW_INTERCEPT;
    }
    
    // Pass it up to our parent
    if (mParent != NULL) {
        mParent->requestDisallowInterceptTouchEvent(disallowIntercept);
    }
}

bool ViewGroup::resetCancelNextUpFlag(shared_ptr<View> view) {
    if ((view->mPrivateFlags & PFLAG_CANCEL_NEXT_UP_EVENT) != 0) {
        view->mPrivateFlags &= ~PFLAG_CANCEL_NEXT_UP_EVENT;
        return true;
    }
    return false;
}

/**
 * Resets all touch state in preparation for a new cycle.
 */
void ViewGroup::resetTouchState() {
    clearTouchTargets();
    resetCancelNextUpFlag(View::shared_from_this());
    mGroupFlags &= ~FLAG_DISALLOW_INTERCEPT;
}

void ViewGroup::resetResolvedTextAlignment() {
    View::resetResolvedTextAlignment();
    
    int count = getChildCount();
    for (int i = 0; i < count; i++) {
        shared_ptr<View> child = getChildAt(i);
        if (child->isTextAlignmentInherited()) {
            child->resetResolvedTextAlignment();
        }
    }
}

bool ViewGroup::resolveTextAlignment() {
    const bool result = View::resolveTextAlignment();
    if (result) {
        int count = getChildCount();
        for (int i = 0; i < count; i++) {
            shared_ptr<View> child = getChildAt(i);
            if (child->isTextAlignmentInherited()) {
                child->resolveTextAlignment();
            }
        }
    }
    return result;
}

void ViewGroup::setChildrenDrawingCacheEnabled(bool enabled) {
    if (enabled || (mPersistentDrawingCache & PERSISTENT_ALL_CACHES) != PERSISTENT_ALL_CACHES) {
        const vector<shared_ptr<View>> &children = mChildren;
        typedef vector<shared_ptr<View>>::const_iterator iter;
        for (iter it = children.begin(); it != children.end(); ++it) {
            shared_ptr<View> child = *it;
            child->setDrawingCacheEnabled(enabled);
        }
    }
}

bool ViewGroup::shouldDelayChildPressedState() {
    return true;
}

void ViewGroup::setDescendantFocusability(int focusability) {
    switch (focusability) {
        case FOCUS_BEFORE_DESCENDANTS:
        case FOCUS_AFTER_DESCENDANTS:
        case FOCUS_BLOCK_DESCENDANTS:
            break;
        default:
            throw IllegalArgumentException("must be one of FOCUS_BEFORE_DESCENDANTS, FOCUS_AFTER_DESCENDANTS, FOCUS_BLOCK_DESCENDANTS");
    }
    mGroupFlags &= ~FLAG_MASK_FOCUSABILITY;
    mGroupFlags |= (focusability & FLAG_MASK_FOCUSABILITY);
}

ANDROID_END