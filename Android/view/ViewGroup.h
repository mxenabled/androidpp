/*
 * File:   ViewGroup.h
 * Author: saulhoward
 *
 * Created on October 7, 2013, 5:15 PM
 */

#ifndef VIEWGROUP_H
#define	VIEWGROUP_H

#include "AndroidMacros.h"

#include "Android/view/TouchTarget.h"
#include "Android/view/View.h"
#include "Android/utils/Exceptions.h"

#include <memory>

using namespace std;

ANDROID_BEGIN

class ViewGroup : public View, public ViewParent {
    
    friend class View;
    
private:
    
    static const int FLAG_MASK_FOCUSABILITY = 0x60000;
    
public:
    
    /**
     * Used to indicate that the scrolling drawing cache should be kept in memory.
     */
    static const int PERSISTENT_SCROLLING_CACHE = 0x2;
    
    /**
     * Used to indicate that all drawing caches should be kept in memory.
     */
    static const int PERSISTENT_ALL_CACHES = 0x3;
    
    /**
     * This view will get focus before any of its descendants.
     */
    static const int FOCUS_BEFORE_DESCENDANTS = 0x20000;
    
    /**
     * This view will get focus only if none of its descendants want it.
     */
    static const int FOCUS_AFTER_DESCENDANTS = 0x40000;
    
    /**
     * This view will block any of its descendants from getting focus, even
     * if they are focusable.
     */
    static const int FOCUS_BLOCK_DESCENDANTS = 0x60000;
    
    typedef vector<shared_ptr<View>>::iterator iter;
    
    ViewGroup() {}
    ViewGroup(Context *context) : View::View(context) {}
    ViewGroup(Context *context, AttributeSet *attrs);
    virtual ~ViewGroup();
    
    virtual void addTouchables(vector<shared_ptr<View>> &views);
    void addView(shared_ptr<View> child);
    void addView(shared_ptr<View> child, int index);
    void addView(shared_ptr<View> child, int width, int height);
    void addView(shared_ptr<View> child, LayoutParams *params);
    void addView(shared_ptr<View> child, int index, LayoutParams *params);
    virtual void bringChildToFront(shared_ptr<View> child);
    virtual bool canResolveLayoutDirection() { return View::canResolveLayoutDirection(); }
    virtual void childDrawableStateChanged(shared_ptr<View> child);
    void clearChildFocus(shared_ptr<View> child);
    virtual bool dispatchTouchEvent(MotionEvent *ev);
    virtual shared_ptr<View> findFocus();
    virtual LayoutParams *generateLayoutParams(AttributeSet *attrs);
    shared_ptr<View> getChildAt(int index);
    int getChildCount();
    vector<shared_ptr<View>> getChildrenViews() { return mChildren; }
    
    /**
     * Gets the descendant focusability of this view group.  The descendant
     * focusability defines the relationship between this view group and its
     * descendants when looking for a view to take focus in
     * {@link #requestFocus(int, android.graphics.Rect)}.
     *
     * @return one of {@link #FOCUS_BEFORE_DESCENDANTS}, {@link #FOCUS_AFTER_DESCENDANTS},
     *   {@link #FOCUS_BLOCK_DESCENDANTS}.
     */
    int getDescendantFocusability() { return mGroupFlags & FLAG_MASK_FOCUSABILITY; }
    virtual int getLayoutDirection() { return View::getLayoutDirection(); }
    virtual shared_ptr<ViewParent> getParent() { return View::getParent(); }
    
    /**
     * Returns true if this view has or contains focus
     *
     * @return true if this view has or contains focus
     */
    virtual bool hasFocus() {
        return (mPrivateFlags & PFLAG_FOCUSED) != 0 || mFocused != NULL;
    }
    iter indexOfChild(shared_ptr<View> child);
    virtual void invalidate() { View::invalidate(); }
    virtual void invalidate(bool invalidateCache) { View::invalidate(invalidateCache); }
    virtual void invalidateChild(shared_ptr<View> child, Rect &dirty);
    
    /**
     * Quick invalidation method called by View.invalidateViewProperty. This doesn't set the
     * DRAWN flags and doesn't handle the Animation logic that the default invalidation methods
     * do; all we want to do here is schedule a traversal with the appropriate dirty rect.
     *
     * @hide
     */
    void invalidateChildFast(shared_ptr<View> child, Rect &dirty);
    virtual shared_ptr<ViewParent> invalidateChildInParent(vector<int> &location, Rect &dirty);
    
    /**
     * Indicates whether this ViewGroup will always try to draw its children using their
     * drawing cache. By default this property is enabled.
     *
     * @return true if the animation cache is enabled, false otherwise
     *
     * @see #setAlwaysDrawnWithCacheEnabled(boolean)
     * @see #setChildrenDrawnWithCacheEnabled(boolean)
     * @see View#setDrawingCacheEnabled(boolean)
     */
    bool isAlwaysDrawnWithCacheEnabled() {
        return (mGroupFlags & FLAG_ALWAYS_DRAWN_WITH_CACHE) == FLAG_ALWAYS_DRAWN_WITH_CACHE;
    }
    virtual bool isLayoutDirectionResolved() { return View::isLayoutDirectionResolved(); }
    virtual bool isLayoutRequested() { return View::isLayoutRequested(); }
    virtual bool isView() { return true; }
    
    /**
     * Offset the vertical location of all children of this view by the specified number of pixels.
     *
     * @param offset the number of pixels to offset
     *
     * @hide
     */
    void offsetChildrenTopAndBottom(int offset);
    
    /**
     * Offset a rectangle that is in a descendant's coordinate
     * space into our coordinate space.
     * @param descendant A descendant of this view
     * @param rect A rectangle defined in descendant's coordinate space.
     */
    void offsetDescendantRectToMyCoords(View *descendant, Rect rect) {
        offsetRectBetweenParentAndChild(descendant, rect, true, false);
    }
    
    /**
     * Offset a rectangle that is in our coordinate space into an ancestor's
     * coordinate space.
     * @param descendant A descendant of this view
     * @param rect A rectangle defined in descendant's coordinate space.
     */
    void offsetRectIntoDescendantCoords(View *descendant, Rect rect) {
        offsetRectBetweenParentAndChild(descendant, rect, false, false);
    }
    
    /**
     * Helper method that offsets a rect either from parent to descendant or
     * descendant to parent.
     */
    void offsetRectBetweenParentAndChild(View *descendant, Rect rect, bool offsetFromChildToParent, bool clipToBounds);
    virtual bool onInterceptTouchEvent(MotionEvent *ev) { return false; }
    void removeAllViews();
    void removeAllViewsInLayout();
    
    /**
     * {@inheritDoc}
     */
    virtual bool requestChildRectangleOnScreen(shared_ptr<View> child, Rect rectangle, bool immediate) {
        return false;
    }
    
    void requestChildFocus(shared_ptr<View> child, shared_ptr<View> focused);
    
    /**
     * {@inheritDoc}
     */
    void requestDisallowInterceptTouchEvent(bool disallowIntercept);
    virtual void requestLayout() { View::requestLayout(); };
    virtual void resetResolvedTextAlignment();
    virtual bool resolveTextAlignment();
    virtual bool shouldDelayChildPressedState();
    
    /**
     * Indicates whether this ViewGroup will always try to draw its children using their
     * drawing cache. This property can be set to true when the cache rendering is
     * slightly different from the children's normal rendering. Renderings can be different,
     * for instance, when the cache's quality is set to low.
     *
     * When this property is disabled, the ViewGroup will use the drawing cache of its
     * children only when asked to. It's usually the task of subclasses to tell ViewGroup
     * when to start using the drawing cache and when to stop using it.
     *
     * @param always true to always draw with the drawing cache, false otherwise
     *
     * @see #isAlwaysDrawnWithCacheEnabled()
     * @see #setChildrenDrawnWithCacheEnabled(boolean)
     * @see View#setDrawingCacheEnabled(boolean)
     * @see View#setDrawingCacheQuality(int)
     */
    void setAlwaysDrawnWithCacheEnabled(bool always) {
        setBooleanFlag(FLAG_ALWAYS_DRAWN_WITH_CACHE, always);
    }
    
    /**
     * Set the descendant focusability of this view group. This defines the relationship
     * between this view group and its descendants when looking for a view to
     * take focus in {@link #requestFocus(int, android.graphics.Rect)}.
     *
     * @param focusability one of {@link #FOCUS_BEFORE_DESCENDANTS}, {@link #FOCUS_AFTER_DESCENDANTS},
     *   {@link #FOCUS_BLOCK_DESCENDANTS}.
     */
    void setDescendantFocusability(int focusability);
    
    static int getChildMeasureSpec(int spec, int padding, int childDimension);
    
    virtual bool canResolveTextAlignment() { return View::canResolveTextAlignment(); }
    virtual bool canResolveTextDirection() { return View::canResolveLayoutDirection(); }
    virtual bool isTextDirectionResolved() { return View::isTextDirectionResolved(); }
    virtual int getTextDirection() { return View::getTextAlignment(); }
    virtual bool isTextAlignmentResolved() { return View::isTextAlignmentResolved(); }
    virtual int getTextAlignment() { return View::getTextAlignment(); }
    
    /**
     * Returns the focused child of this view, if any. The child may have focus
     * or contain focus.
     *
     * @return the focused child or null.
     */
    shared_ptr<View> getFocusedChild();
    
protected:
    
    /**
     * When set, the drawing method will call {@link #getChildDrawingOrder(int, int)}
     * to get the index of the child to draw for that iteration.
     *
     * @hide
     */
    static const int FLAG_USE_CHILD_DRAWING_ORDER = 0x400;
    
    /**
     * When set, and if FLAG_ALWAYS_DRAWN_WITH_CACHE is not set, this ViewGroup will try to
     * draw its children with their drawing cache.
     */
    static const int FLAG_CHILDREN_DRAWN_WITH_CACHE = 0x8000;
    
    /**
     * When set, this ViewGroup supports static transformations on children; this causes
     * {@link #getChildStaticTransformation(View, android.view.animation.Transformation)} to be
     * invoked when a child is drawn.
     *
     * Any subclass overriding
     * {@link #getChildStaticTransformation(View, android.view.animation.Transformation)} should
     * set this flags in {@link #mGroupFlags}.
     *
     * {@hide}
     */
    static const int FLAG_SUPPORT_STATIC_TRANSFORMATIONS = 0x800;
    
    /**
     * When set, this ViewGroup should not intercept touch events.
     * {@hide}
     */
    static const int FLAG_DISALLOW_INTERCEPT = 0x80000;

    
    /**
     * Indicates which types of drawing caches are to be kept in memory.
     * This field should be made private, so it is hidden from the SDK.
     * {@hide}
     */
    int mPersistentDrawingCache = 0;
    int mGroupFlags = 0;
    
    virtual bool addViewInLayout(shared_ptr<View> child, int index, LayoutParams *params,
                                 bool preventRequestLayout);
    
    /**
     * Attaches a view to this view group. Attaching a view assigns this group as the parent,
     * sets the layout parameters and puts the view in the list of children so that
     * it can be retrieved by calling {@link #getChildAt(int)}.
     * <p>
     * This method is intended to be lightweight and makes no assumptions about whether the
     * parent or child should be redrawn. Proper use of this method will include also making
     * any appropriate {@link #requestLayout()} or {@link #invalidate()} calls.
     * For example, callers can {@link #post(Runnable) post} a {@link Runnable}
     * which performs a {@link #requestLayout()} on the next frame, after all detach/attach
     * calls are finished, causing layout to be run prior to redrawing the view hierarchy.
     * <p>
     * This method should be called only for views which were detached from their parent.
     *
     * @param child the child to attach
     * @param index the index at which the child should be attached
     * @param params the layout parameters of the child
     *
     * @see #removeDetachedView(View, boolean)
     * @see #detachAllViewsFromParent()
     * @see #detachViewFromParent(View)
     * @see #detachViewFromParent(int)
     */
    void attachViewToParent(shared_ptr<View> child, int index, LayoutParams *params);
    virtual bool checkLayoutParams(LayoutParams *params);
    
    /**
     * Prevents the specified child to be laid out during the next layout pass.
     *
     * @param child the child on which to perform the cleanup
     */
    void cleanupLayoutState(shared_ptr<View> child) {
        child->mPrivateFlags &= ~View::PFLAG_FORCE_LAYOUT;
    }
    
    /**
     * Detaches all views from the parent. Detaching a view should be followed
     * either by a call to
     * {@link #attachViewToParent(View, int, android.view.ViewGroup.LayoutParams)}
     * or a call to {@link #removeDetachedView(View, boolean)}. Detachment should only be
     * temporary; reattachment or removal should happen within the same drawing cycle as
     * detachment. When a view is detached, its parent is null and cannot be retrieved by a
     * call to {@link #getChildAt(int)}.
     *
     * @see #detachViewFromParent(View)
     * @see #detachViewFromParent(int)
     * @see #detachViewsFromParent(int, int)
     * @see #attachViewToParent(View, int, android.view.ViewGroup.LayoutParams)
     * @see #removeDetachedView(View, boolean)
     */
    void detachAllViewsFromParent();
    
    /**
     * Detaches a view from its parent. Detaching a view should be followed
     * either by a call to
     * {@link #attachViewToParent(View, int, android.view.ViewGroup.LayoutParams)}
     * or a call to {@link #removeDetachedView(View, boolean)}. Detachment should only be
     * temporary; reattachment or removal should happen within the same drawing cycle as
     * detachment. When a view is detached, its parent is null and cannot be retrieved by a
     * call to {@link #getChildAt(int)}.
     *
     * @param child the child to detach
     *
     * @see #detachViewFromParent(int)
     * @see #detachViewsFromParent(int, int)
     * @see #detachAllViewsFromParent()
     * @see #attachViewToParent(View, int, android.view.ViewGroup.LayoutParams)
     * @see #removeDetachedView(View, boolean)
     */
    void detachViewFromParent(shared_ptr<View> child) {
        removeFromArray(indexOfChild(child));
    }
    
    /**
     * Detaches a range of views from their parents. Detaching a view should be followed
     * either by a call to
     * {@link #attachViewToParent(View, int, android.view.ViewGroup.LayoutParams)}
     * or a call to {@link #removeDetachedView(View, boolean)}. Detachment should only be
     * temporary; reattachment or removal should happen within the same drawing cycle as
     * detachment. When a view is detached, its parent is null and cannot be retrieved by a
     * call to {@link #getChildAt(int)}.
     *
     * @param start the first index of the childrend range to detach
     * @param count the number of children to detach
     *
     * @see #detachViewFromParent(View)
     * @see #detachViewFromParent(int)
     * @see #detachAllViewsFromParent()
     * @see #attachViewToParent(View, int, android.view.ViewGroup.LayoutParams)
     * @see #removeDetachedView(View, boolean)
     */
    virtual void detachViewsFromParent(int start, int count) { removeFromArray(start, count); }
    virtual void dispatchDraw(shared_ptr<Canvas> canvas);
    virtual void dispatchGetDisplayList();
    
    virtual void dispatchSetPressed(bool pressed);
    virtual void drawableStateChanged();
    virtual bool drawChild(shared_ptr<Canvas> canvas, shared_ptr<View> child, uint64_t drawingTime);
    virtual shared_ptr<View> findViewTraversal(string id);
    virtual shared_ptr<View> findViewWithTagTraversal(shared_ptr<Object> tag);
    virtual LayoutParams *generateDefaultLayoutParams();
    virtual LayoutParams *generateLayoutParams(LayoutParams *layoutParams);
    virtual int getChildDrawingOrder(int childCount, int i) { return i; }
    bool getChildStaticTransformation(shared_ptr<View> child, Transformation t) { return false; }
    bool isChildrenDrawingOrderEnabled();
    virtual bool isTransformedTouchPointInView(float x, float y, const shared_ptr<View> child);
    
    /**
     * Ask all of the children of this view to measure themselves, taking into
     * account both the MeasureSpec requirements for this view and its padding.
     * We skip children that are in the GONE state The heavy lifting is done in
     * getChildMeasureSpec.
     *
     * @param widthMeasureSpec The width requirements for this view
     * @param heightMeasureSpec The height requirements for this view
     */
    virtual void measureChildren(int widthMeasureSpec, int heightMeasureSpec);
    
    /**
     * Ask one of the children of this view to measure itself, taking into
     * account both the MeasureSpec requirements for this view and its padding.
     * The heavy lifting is done in getChildMeasureSpec.
     *
     * @param child The child to measure
     * @param parentWidthMeasureSpec The width requirements for this view
     * @param parentHeightMeasureSpec The height requirements for this view
     */
    virtual void measureChild(shared_ptr<View> child, int parentWidthMeasureSpec,
                      int parentHeightMeasureSpec);
    virtual void measureChildWithMargins(shared_ptr<View> child, int parentWidthMeasureSpec, int widthUsed, int parentHeightMeasureSpec, int heightUsed);
    virtual vector<int> onCreateDrawableState(int extraSpace);
    void onViewRemoved(shared_ptr<View> child);
    
    /**
     * Finishes the removal of a detached view. This method will dispatch the detached from
     * window event and notify the hierarchy change listener.
     * <p>
     * This method is intended to be lightweight and makes no assumptions about whether the
     * parent or child should be redrawn. Proper use of this method will include also making
     * any appropriate {@link #requestLayout()} or {@link #invalidate()} calls.
     * For example, callers can {@link #post(Runnable) post} a {@link Runnable}
     * which performs a {@link #requestLayout()} on the next frame, after all detach/remove
     * calls are finished, causing layout to be run prior to redrawing the view hierarchy.
     *
     * @param child the child to be definitely removed from the view hierarchy
     * @param animate if true and the view has an animation, the view is placed in the
     *                disappearing views list, otherwise, it is detached from the window
     *
     * @see #attachViewToParent(View, int, android.view.ViewGroup.LayoutParams)
     * @see #detachAllViewsFromParent()
     * @see #detachViewFromParent(View)
     * @see #detachViewFromParent(int)
     */
    virtual void removeDetachedView(shared_ptr<View> child, bool animate);
    
    /**
     * Enables or disables the drawing cache for each child of this view group.
     *
     * @param enabled true to enable the cache, false to dispose of it
     */
    virtual void setChildrenDrawingCacheEnabled(bool enabled);
    
    /**
     * Tells the ViewGroup to draw its children using their drawing cache. This property
     * is ignored when {@link #isAlwaysDrawnWithCacheEnabled()} is true. A child's drawing cache
     * will be used only if it has been enabled.
     *
     * Subclasses should call this method to start and stop using the drawing cache when
     * they perform performance sensitive operations, like scrolling or animating.
     *
     * @param enabled true if children should be drawn with their cache, false otherwise
     *
     * @see #setAlwaysDrawnWithCacheEnabled(boolean)
     * @see #isChildrenDrawnWithCacheEnabled()
     */
    virtual void setChildrenDrawnWithCacheEnabled(bool enabled) {
        setBooleanFlag(FLAG_CHILDREN_DRAWN_WITH_CACHE, enabled);
    }
    
private:
    
    // When set, ViewGroup invalidates only the child's rectangle
    // Set by default
    static const int FLAG_CLIP_CHILDREN = 0x1;
    
    // When set, ViewGroup excludes the padding area from the invalidate rectangle
    // Set by default
    static const int FLAG_CLIP_TO_PADDING = 0x2;
    
    // When set, there is either no layout animation on the ViewGroup or the layout
    // animation is over
    // Set by default
    static const int FLAG_ANIMATION_DONE = 0x10;
    
    // If set, this ViewGroup has padding; if unset there is no padding and we don't need
    // to clip it, even if FLAG_CLIP_TO_PADDING is set
    static const int FLAG_PADDING_NOT_NULL = 0x20;
    
    // When set, this ViewGroup caches its children in a Bitmap before starting a layout animation
    // Set by default
    static const int FLAG_ANIMATION_CACHE = 0x40;
    
    /**
     * When set, this ViewGroup tries to always draw its children using their drawing cache.
     */
    static const int FLAG_ALWAYS_DRAWN_WITH_CACHE = 0x4000;
    
    /**
     * When set, this ViewGroup's drawable states also include those
     * of its children.
     */
    static const int FLAG_ADD_STATES_FROM_CHILDREN = 0x2000;
    
    /**
     * When set, this ViewGroup will split MotionEvents to multiple child Views when appropriate.
     */
    static const int FLAG_SPLIT_MOTION_EVENTS = 0x200000;
    
    /**
     * When set, this group will go through its list of children to notify them of
     * any drawable state change.
     */
    static const int FLAG_NOTIFY_CHILDREN_ON_DRAWABLE_STATE_CHANGE = 0x10000;
    
    /**
     * When set, this ViewGroup will not dispatch onAttachedToWindow calls
     * to children when adding new views. This is used to prevent multiple
     * onAttached calls when a ViewGroup adds children in its own onAttached method.
     */
    static const int FLAG_PREVENT_DISPATCH_ATTACHED_TO_WINDOW = 0x400000;
    
    /**
     * Indicates whether the view is temporarily detached.
     *
     * @hide
     */
    static const int PFLAG_CANCEL_NEXT_UP_EVENT        = 0x04000000;
    
    // When set, this ViewGroup converts calls to invalidate(Rect) to invalidate() during a
    // layout animation; this avoid clobbering the hierarchy
    // Automatically set when the layout animation starts, depending on the animation's
    // characteristics
    static const int FLAG_OPTIMIZE_INVALIDATE = 0x80;
    
    // Index of the child's left position in the mLocation array
    static const int CHILD_LEFT_INDEX = 0;
    // Index of the child's top position in the mLocation array
    static const int CHILD_TOP_INDEX = 1;
        int mChildrenCount = 0;
    TouchTarget *m_firstTouchTarget = NULL;
    
    // The view contained within this ViewGroup that has or contains focus.
    shared_ptr<View> mFocused;
    int m_lastTouchDownIndex = -1;
    long m_lastTouchDownTime = 0;
    float m_lastTouchDownX = 0.0f;
    float m_lastTouchDownY = 0.0f;
protected:
    int mScrollX = 0;
    int mScrollY = 0;
private:
    void addInArray(shared_ptr<View> child, int index);
    TouchTarget *addTouchTarget(shared_ptr<View> child, int pointerIdBits);
    void addViewInner(shared_ptr<View> child, int index, LayoutParams *params, bool preventRequestLayout);
    static bool canViewReceivePointerEvents(const shared_ptr<View> child);
    void cancelAndClearTouchTargets(MotionEvent *event);
    void cancelTouchTarget(shared_ptr<View> view);
    void clearTouchTargets();
    virtual void dispatchAttachedToWindow(AttachInfo *info, int visibility);
    bool dispatchTransformedTouchEvent(MotionEvent *event, bool cancel, shared_ptr<View> child, int desiredPointerIdBits);
    TouchTarget *getTouchTarget(const shared_ptr<View> child);
    void initViewGroup();
    
    /**
     * Quick invalidation method that simply transforms the dirty rect into the parent's
     * coordinate system, pruning the invalidation if the parent has already been invalidated.
     */
    shared_ptr<ViewParent> invalidateChildInParentFast(int left, int top, Rect dirty);
    void removeFromArray(iter index);
    
    // This method also sets the children's mParent to null
    void removeFromArray(int start, int count);    void removePointersFromTouchTargets(int pointerIdBits);
    static bool resetCancelNextUpFlag(shared_ptr<View> view);
    void resetTouchState();    
    void setBooleanFlag(int flag, bool value) {
        if (value) {
            mGroupFlags |= flag;
        } else {
            mGroupFlags &= ~flag;
        }
    }
    
protected:
    
    vector<shared_ptr<View>> mChildren;

    /**
     * We clip to padding when FLAG_CLIP_TO_PADDING and FLAG_PADDING_NOT_NULL
     * are set at the same time.
     */
    static const int CLIP_TO_PADDING_MASK = FLAG_CLIP_TO_PADDING | FLAG_PADDING_NOT_NULL;
};

ANDROID_END

#endif	/* VIEWGROUP_H */

