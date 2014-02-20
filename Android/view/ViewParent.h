/*
 * File:   ViewParent.h
 * Author: saulhoward
 *
 * Created on October 10, 2013, 4:16 PM
 */

#ifndef VIEWPARENT_H
#define	VIEWPARENT_H

#include "AndroidMacros.h"

#include <vector>

ANDROID_BEGIN

class View;
class Rect;

class ViewParent {
public:

    virtual ~ViewParent() {}

    virtual void bringChildToFront(shared_ptr<View> child) = 0;
    /**
     * Tells if this view parent can resolve the layout direction.
     * See {@link View#setLayoutDirection(int)}
     *
     * @return True if this view parent can resolve the layout direction.
     *
     * @hide
     */
    virtual bool canResolveLayoutDirection() = 0;
    
    /**
     * Tells if this view parent layout direction is resolved.
     * See {@link View#setLayoutDirection(int)}
     *
     * @return True if this view parent layout direction is resolved.
     *
     * @hide
     */
    virtual bool isLayoutDirectionResolved() = 0;
    
    /**
     * Return this view parent layout direction. See {@link View#getLayoutDirection()}
     *
     * @return {@link View#LAYOUT_DIRECTION_RTL} if the layout direction is RTL or returns
     * {@link View#LAYOUT_DIRECTION_LTR} if the layout direction is not RTL.
     *
     * @hide
     */
    virtual int getLayoutDirection() = 0;
    
    /**
     * Tells if this view parent can resolve the text direction.
     * See {@link View#setTextDirection(int)}
     *
     * @return True if this view parent can resolve the text direction.
     *
     * @hide
     */
    virtual bool canResolveTextDirection() = 0;
    
    /**
     * Tells if this view parent text direction is resolved.
     * See {@link View#setTextDirection(int)}
     *
     * @return True if this view parent text direction is resolved.
     *
     * @hide
     */
    virtual bool isTextDirectionResolved() = 0;
    
    /**
     * Return this view parent text direction. See {@link View#getTextDirection()}
     *
     * @return the resolved text direction. Returns one of:
     *
     * {@link View#TEXT_DIRECTION_FIRST_STRONG}
     * {@link View#TEXT_DIRECTION_ANY_RTL},
     * {@link View#TEXT_DIRECTION_LTR},
     * {@link View#TEXT_DIRECTION_RTL},
     * {@link View#TEXT_DIRECTION_LOCALE}
     *
     * @hide
     */
    virtual int getTextDirection() = 0;
    
    /**
     * Tells if this view parent can resolve the text alignment.
     * See {@link View#setTextAlignment(int)}
     *
     * @return True if this view parent can resolve the text alignment.
     *
     * @hide
     */
    virtual bool canResolveTextAlignment() = 0;
    
    /**
     * Tells if this view parent text alignment is resolved.
     * See {@link View#setTextAlignment(int)}
     *
     * @return True if this view parent text alignment is resolved.
     *
     * @hide
     */
    virtual bool isTextAlignmentResolved() = 0;
    
    /**
     * Return this view parent text alignment. See {@link android.view.View#getTextAlignment()}
     *
     * @return the resolved text alignment. Returns one of:
     *
     * {@link View#TEXT_ALIGNMENT_GRAVITY},
     * {@link View#TEXT_ALIGNMENT_CENTER},
     * {@link View#TEXT_ALIGNMENT_TEXT_START},
     * {@link View#TEXT_ALIGNMENT_TEXT_END},
     * {@link View#TEXT_ALIGNMENT_VIEW_START},
     * {@link View#TEXT_ALIGNMENT_VIEW_END}
     *
     * @hide
     */
    virtual int getTextAlignment() = 0;
    
    virtual void childDrawableStateChanged(shared_ptr<View> child) = 0;
    virtual void requestLayout() = 0;
    
    /**
     * Called when a child does not want this parent and its ancestors to
     * intercept touch events with
     * {@link ViewGroup#onInterceptTouchEvent(MotionEvent)}.
     *
     * <p>This parent should pass this call onto its parents. This parent must obey
     * this request for the duration of the touch (that is, only clear the flag
     * after this parent has received an up or a cancel.</p>
     *
     * @param disallowIntercept True if the child does not want the parent to
     *            intercept touch events.
     */
    virtual void requestDisallowInterceptTouchEvent(bool disallowIntercept) = 0;
    
    virtual shared_ptr<ViewParent> getParent() = 0;
    
    /**
     * Called when a child of this parent wants focus
     *
     * @param child The child of this ViewParent that wants focus. This view
     *        will contain the focused view. It is not necessarily the view that
     *        actually has focus.
     * @param focused The view that is a descendant of child that actually has
     *        focus
     */
    virtual void requestChildFocus(shared_ptr<View> child, shared_ptr<View> focused) = 0;
    
    /**
     * Called when a child of this parent is giving up focus
     *
     * @param child The view that is giving up focus
     */
    virtual void clearChildFocus(shared_ptr<View> child) = 0;
    
    virtual void invalidate(bool invalidateCache) = 0;
    
    /**
     * All or part of a child is dirty and needs to be redrawn.
     *
     * @param child The child which is dirty
     * @param r The area within the child that is invalid
     */
    virtual void invalidateChild(shared_ptr<View> child, Rect &dirty) = 0;
    virtual shared_ptr<ViewParent> invalidateChildInParent(std::vector<int> &location, Rect &dirty) = 0;
    virtual bool isLayoutRequested() = 0;
    virtual bool isView() = 0;
};

ANDROID_END

#endif	/* VIEWPARENT_H */

