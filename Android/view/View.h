/*
 * File:   View.h
 * Author: saulhoward
 *
 * Created on October 4, 2013, 12:49 PM
 */

#ifndef VIEW_H
#define	VIEW_H

#include "cocos2d.h"

#include "AndroidMacros.h"

#include "Android/content/Context.h"
#include "Android/graphics/Color.h"
#include "Android/graphics/Rect.h"
#include "Android/graphics/drawable/Drawable.h"
#include "Android/graphics/Camera.h"
#include "Android/graphics/Matrix.h"
#include "Android/utils/AttributeSet.h"
#include "Android/view/KeyEvent.h"
#include "Android/view/ViewParent.h"
#include "Android/view/ViewRootImpl.h"
#include "Android/view/LayoutParams.h"
#include "Android/view/AttachInfo.h"
#include "Android/view/GLES20DisplayList.h"
#include "Android/view/animation/Animation.h"

#include "Android/internal/R.h"

#include <string.h>
#include <memory>

#include <mindroid/os/Ref.h>
#include <mindroid/os/Runnable.h>

ANDROID_BEGIN

using namespace mindroid;
using namespace std;

enum ViewType {
    kViewTypeNone,
    kViewTypeView,
    kViewTypeViewGroup,
    kViewTypeLinearLayout,
    kViewTypeRelativeLayout,
    kViewTypeImageView,
    kViewTypeTextView
};

class KeyEvent;
class ViewGroup;
class MotionEvent;
class Object;
class ViewTreeObserver;

class View : public Drawable::Callback, public enable_shared_from_this<View> {
    
    friend class TextView;
    friend class ViewGroup;
    friend class ViewRootImpl;
    friend class HardwareRenderer;
    friend class ListView;
    friend class LayoutInflater;
    
private:
    
    /**
     * This view does not want keystrokes. Use with TAKES_FOCUS_MASK when
     * calling setFlags.
     */
    static const int NOT_FOCUSABLE = 0x00000000;
    
    /**
     * This view wants keystrokes. Use with TAKES_FOCUS_MASK when calling
     * setFlags.
     */
    static const int FOCUSABLE = 0x00000001;
    
    /**
     * Mask for use with setFlags indicating bits used for focus.
     */
    static const int FOCUSABLE_MASK = 0x00000001;
    
public:
    /**
     * Used to mark a View that has no ID.
     */
    static const string NO_ID;
    
    /**
     * Used to mark a View that has no ID.
     */
    static const int NO_ID_INT = -1;
    
    static const int TOUCH_SLOP = 8;
    
    /**
     * View flag indicating whether {@link #addFocusables(ArrayList, int, int)}
     * should add all focusable Views regardless if they are focusable in touch mode.
     */
    static const int FOCUSABLES_ALL = 0x00000000;
    
    /**
     * View flag indicating whether {@link #addFocusables(ArrayList, int, int)}
     * should add only Views focusable in touch mode.
     */
    static const int FOCUSABLES_TOUCH_MODE = 0x00000001;
    
    /**
     * Use with {@link #focusSearch(int)}. Move focus to the previous selectable
     * item.
     */
    static const int FOCUS_BACKWARD = 0x00000001;
    
    /**
     * Use with {@link #focusSearch(int)}. Move focus to the next selectable
     * item.
     */
    static const int FOCUS_FORWARD = 0x00000002;
    
    /**
     * Use with {@link #focusSearch(int)}. Move focus to the left.
     */
    static const int FOCUS_LEFT = 0x00000011;
    
    /**
     * Use with {@link #focusSearch(int)}. Move focus up.
     */
    static const int FOCUS_UP = 0x00000021;
    
    /**
     * Use with {@link #focusSearch(int)}. Move focus to the right.
     */
    static const int FOCUS_RIGHT = 0x00000042;
    
    /**
     * Use with {@link #focusSearch(int)}. Move focus down.
     */
    static const int FOCUS_DOWN = 0x00000082;
    
    /**
     * Minimum velocity to initiate a fling, as measured in dips per second
     */
    static const int MINIMUM_FLING_VELOCITY = 50;
    
    /**
     * Maximum velocity to initiate a fling, as measured in dips per second
     */
    static const int MAXIMUM_FLING_VELOCITY = 8000;
    
    /**
     * This view is visible.
     * Use with {@link #setVisibility} and <a href="#attr_android:visibility">{@code
     * android:visibility}.
     */
    static const int VISIBLE = 0x00000000;
    
    /**
     * This view is invisible, but it still takes up space for layout purposes.
     * Use with {@link #setVisibility} and <a href="#attr_android:visibility">{@code
     * android:visibility}.
     */
    static const int INVISIBLE = 0x00000004;
    
    /**
     * This view is invisible, and it doesn't take any space for layout
     * purposes. Use with {@link #setVisibility} and <a href="#attr_android:visibility">{@code
     * android:visibility}.
     */
    static const int GONE = 0x00000008;
    
    /**
     * Mask for use with setFlags indicating bits used for visibility.
     * {@hide}
     */
    static const int VISIBILITY_MASK = 0x0000000C;
    
    /**
     * Indicates that this view has reported that it can accept the current drag's content.
     * Cleared when the drag operation concludes.
     * @hide
     */
    static const int PFLAG2_DRAG_CAN_ACCEPT            = 0x00000001;
    
    /**
     * Indicates that this view is currently directly under the drag location in a
     * drag-and-drop operation involving content that it can accept.  Cleared when
     * the drag exits the view, or when the drag operation concludes.
     * @hide
     */
    static const int PFLAG2_DRAG_HOVERED               = 0x00000002;
    
    /**
     * Indicates that the view does not have a layer.
     *
     * @see #getLayerType()
     * @see #setLayerType(int, android.graphics.Paint)
     * @see #LAYER_TYPE_SOFTWARE
     * @see #LAYER_TYPE_HARDWARE
     */
    static const int LAYER_TYPE_NONE = 0;
    
    /**
     * Horizontal layout direction of this view is from Left to Right.
     * Use with {@link #setLayoutDirection}.
     */
    static const int LAYOUT_DIRECTION_LTR = 0;
    
    /**
     * Horizontal layout direction of this view is from Right to Left.
     * Use with {@link #setLayoutDirection}.
     */
    static const int LAYOUT_DIRECTION_RTL = 1;
    
    /**
     * Default horizontal layout direction.
     * @hide
     */
    static const int LAYOUT_DIRECTION_RESOLVED_DEFAULT = LAYOUT_DIRECTION_LTR;
    
    /**
     * Horizontal layout direction of this view is inherited from its parent.
     * Use with {@link #setLayoutDirection}.
     */
    static const int LAYOUT_DIRECTION_INHERIT = 2;
    
    /**
     * Text direction is inherited thru {@link ViewGroup}
     */
    static const int TEXT_DIRECTION_INHERIT = 0;
    
    /**
     * Text direction is using "first strong algorithm". The first strong directional character
     * determines the paragraph direction. If there is no strong directional character, the
     * paragraph direction is the view's resolved layout direction.
     */
    static const int TEXT_DIRECTION_FIRST_STRONG = 1;
    
    /**
     * Text direction is using "any-RTL" algorithm. The paragraph direction is RTL if it contains
     * any strong RTL character, otherwise it is LTR if it contains any strong LTR characters.
     * If there are neither, the paragraph direction is the view's resolved layout direction.
     */
    static const int TEXT_DIRECTION_ANY_RTL = 2;
    
    /**
     * Text direction is forced to LTR.
     */
    static const int TEXT_DIRECTION_LTR = 3;
    
    /**
     * Text direction is forced to RTL.
     */
    static const int TEXT_DIRECTION_RTL = 4;
    
    /**
     * Text direction is coming from the system Locale.
     */
    static const int TEXT_DIRECTION_LOCALE = 5;
    
    /**
     * Align to the end of the view, which is ALIGN_RIGHT if the view’s resolved
     * layoutDirection is LTR, and ALIGN_LEFT otherwise.
     *
     * Use with {@link #setTextAlignment(int)}
     */
    static const int TEXT_ALIGNMENT_VIEW_END = 6;
    
    /*
     * Default text alignment. The text alignment of this View is inherited from its parent.
     * Use with {@link #setTextAlignment(int)}
     */
    static const int TEXT_ALIGNMENT_INHERIT = 0;
    
    /**
     * Default for the root view. The gravity determines the text alignment, ALIGN_NORMAL,
     * ALIGN_CENTER, or ALIGN_OPPOSITE, which are relative to each paragraph’s text direction.
     *
     * Use with {@link #setTextAlignment(int)}
     */
    static const int TEXT_ALIGNMENT_GRAVITY = 1;
    
    /**
     * Align to the start of the paragraph, e.g. ALIGN_NORMAL.
     *
     * Use with {@link #setTextAlignment(int)}
     */
    static const int TEXT_ALIGNMENT_TEXT_START = 2;
    
    /**
     * Align to the end of the paragraph, e.g. ALIGN_OPPOSITE.
     *
     * Use with {@link #setTextAlignment(int)}
     */
    static const int TEXT_ALIGNMENT_TEXT_END = 3;
    
    /**
     * Center the paragraph, e.g. ALIGN_CENTER.
     *
     * Use with {@link #setTextAlignment(int)}
     */
    static const int TEXT_ALIGNMENT_CENTER = 4;
    
    /**
     * Align to the start of the view, which is ALIGN_LEFT if the view’s resolved
     * layoutDirection is LTR, and ALIGN_RIGHT otherwise.
     *
     * Use with {@link #setTextAlignment(int)}
     */
    static const int TEXT_ALIGNMENT_VIEW_START = 5;
    
    /**
     * Bits of {@link #getMeasuredWidthAndState()} and
     * {@link #getMeasuredWidthAndState()} that provide the actual measured size.
     */
    static const int MEASURED_SIZE_MASK = 0x00ffffff;
    
    /**
     * Bits of {@link #getMeasuredWidthAndState()} and
     * {@link #getMeasuredWidthAndState()} that provide the additional state bits.
     */
    static const int MEASURED_STATE_MASK = 0xff000000;
    
    /**
     * Bit shift of {@link #MEASURED_STATE_MASK} to get to the height bits
     * for functions that combine both width and height into a single int,
     * such as {@link #getMeasuredState()} and the childState argument of
     * {@link #resolveSizeAndState(int, int, int)}.
     */
    static const int MEASURED_HEIGHT_STATE_SHIFT = 16;
    
    static const int MEASURED_STATE_TOO_SMALL = 0x01000000;
    
    /**
     * The scrollbar style to display the scrollbars at the edge of the view,
     * without increasing the padding. The scrollbars will be overlaid with
     * translucency.
     */
    static const int SCROLLBARS_OUTSIDE_OVERLAY = 0x02000000;
    
    /**
     * The scrollbar style to display the scrollbars inside the content area,
     * without increasing the padding. The scrollbars will be overlaid with
     * translucency on the view's content.
     */
    static const int SCROLLBARS_INSIDE_OVERLAY = 0;
    
    /**
     * Defines the default duration in milliseconds before a press turns into
     * a long press
     */
    static const uint32_t DEFAULT_LONG_PRESS_TIMEOUT = 500;
    
    /**
     * Defines the duration in milliseconds of the pressed state in child
     * components.
     */
    static const uint32_t PRESSED_STATE_DURATION = 64;
    
    /**
     * Defines the duration in milliseconds we will wait to see if a touch event
     * is a tap or a scroll. If the user does not move within this interval, it is
     * considered to be a tap.
     */
    static const uint32_t TAP_TIMEOUT = 180;
    
    static int getTextAlignment(string alignment, int defaultAlignment);
    
    class MeasureSpec {
        
    private:
        static const int MODE_SHIFT = 30;
        static const int MODE_MASK  = 0x3 << MODE_SHIFT;
        
    public:
        /**
         * Measure specification mode: The parent has not imposed any constraint
         * on the child. It can be whatever size it wants.
         */
        static const int UNSPECIFIED = 0 << MODE_SHIFT;
        
        /**
         * Measure specification mode: The parent has determined an exact size
         * for the child. The child is going to be given those bounds regardless
         * of how big it wants to be.
         */
        static const int EXACTLY     = 1 << MODE_SHIFT;
        
        /**
         * Measure specification mode: The child can be as large as it wants up
         * to the specified size.
         */
        static const int AT_MOST     = 2 << MODE_SHIFT;
        
        static int makeMeasureSpec(int size, int mode) {
            return (size & ~MODE_MASK) | (mode & MODE_MASK);
        }
        
        static int getMode(int measureSpec) {
            return (measureSpec & MODE_MASK);
        }
        
        static int getSize(int measureSpec) {
            return (measureSpec & ~MODE_MASK);
        }
        
        static int adjust(int measureSpec, int delta) {
            return makeMeasureSpec(getSize(measureSpec + delta), getMode(measureSpec));
        }
    };
    
    /**
     * Set to true when drawing cache is enabled and cannot be created.
     *
     * @hide
     */
    bool mCachingFailed = false;
    
    View();
    View(Context *context);
    View(Context *context, AttributeSet *attrs);
    virtual ~View() {
        if (mLayoutParams) delete mLayoutParams;
    }
    
    /**
     * Add any touchable views that are descendants of this view (possibly
     * including this view if it is touchable itself) to views.
     *
     * @param views Touchable views found so far
     */
    virtual void addTouchables(vector<shared_ptr<View>> &views);
    
    /**
     * Cancels a pending long press.  Your subclass can use this if you
     * want the context menu to come up if the user presses and holds
     * at the same place, but you don't want it to come up if they press
     * and then move around enough to cause scrolling.
     */
    virtual void cancelLongPress();
    
    /**
     * Check if layout direction resolution can be done.
     *
     * @return true if layout direction resolution can be done otherwise return false.
     *
     * @hide
     */
    virtual bool canResolveLayoutDirection();
    
    /**
     * Check if text alignment resolution can be done.
     *
     * @return true if text alignment resolution can be done otherwise return false.
     *
     * @hide
     */
    virtual bool canResolveTextAlignment();
    
    /**
     * Check if text direction resolution can be done.
     *
     * @return true if text direction resolution can be done otherwise return false.
     *
     * @hide
     */
    bool canResolveTextDirection();
    
    bool canHaveDisplayList();
    
    /**
     * Called by a parent to request that a child update its values for mScrollX
     * and mScrollY if necessary. This will typically be done if the child is
     * animating a scroll using a {@link android.widget.Scroller Scroller}
     * object.
     */
    virtual void computeScroll() {}
    
    /**
     * Called when this view wants to give up focus. If focus is cleared
     * {@link #onFocusChanged(boolean, int, android.graphics.Rect)} is called.
     * <p>
     * <strong>Note:</strong> When a View clears focus the framework is trying
     * to give focus to the first focusable View from the top. Hence, if this
     * View is the first from the top that can take focus, then all callbacks
     * related to clearing focus will be invoked after wich the framework will
     * give focus to this view.
     * </p>
     */
    void clearFocus();
    
    /**
     * @hide
     */
    virtual void dispatchStartTemporaryDetach();
    
    /**
     * Find the view in the hierarchy rooted at this view that currently has
     * focus.
     *
     * @return The view that currently has focus, or null if no focused view can
     *         be found.
     */
    virtual shared_ptr<View> findFocus() {
        return (mPrivateFlags & PFLAG_FOCUSED) != 0 ? shared_from_this() : NULL;
    }
    
    /**
     * Returns the layout direction for this view.
     *
     * @return One of {@link #LAYOUT_DIRECTION_LTR},
     *   {@link #LAYOUT_DIRECTION_RTL},
     *   {@link #LAYOUT_DIRECTION_INHERIT} or
     *   {@link #LAYOUT_DIRECTION_LOCALE}.
     *
     * @attr ref android.R.styleable#View_layoutDirection
     *
     * @hide
     */
    int getRawLayoutDirection();
    
    /**
     * This is called when a container is going to temporarily detach a child, with
     * {@link ViewGroup#detachViewFromParent(View) ViewGroup.detachViewFromParent}.
     * It will either be followed by {@link #onFinishTemporaryDetach()} or
     * {@link #onDetachedFromWindow()} when the container is done.
     */
    virtual void onStartTemporaryDetach();
    
    /**
     * @hide
     */
    virtual void dispatchFinishTemporaryDetach() { onFinishTemporaryDetach(); }
    
    /**
     * Called after {@link #onStartTemporaryDetach} when the container is done
     * changing the view.
     */
    virtual void onFinishTemporaryDetach() {}
    virtual bool dispatchTouchEvent(MotionEvent *ev);
    void dispatchWindowVisibilityChanged(int visibility);
    virtual void draw(shared_ptr<Canvas> canvas);
    View *findViewById(string id);
    
    /**
     * Look for a child view with the given tag.  If this view has the given
     * tag, return this view.
     *
     * @param tag The tag to search for, using "tag.equals(getTag())".
     * @return The View that has the given tag in the hierarchy or null
     */
    View *findViewWithTag(shared_ptr<Object> tag);
    static string generateViewId();
    
    Animation *getAnimation() { return m_currentAnimation; }
    /**
     * Gets the background drawable
     *
     * @return The drawable used as the background for this view, if any.
     *
     * @see #setBackground(Drawable)
     *
     * @attr ref android.R.styleable#View_background
     */
    shared_ptr<Drawable> getBackground() { return m_background; }
    GLES20DisplayList *getDisplayList();
    GLES20DisplayList *getDisplayList(GLES20DisplayList *displayList, bool isLayer);
    virtual int getBaseline();
    int getBottom() { return mBottom; }
    Context *getContext();
    
    /**
     * Return the visible drawing bounds of your view. Fills in the output
     * rectangle with the values from getScrollX(), getScrollY(),
     * getWidth(), and getHeight(). These bounds do not account for any
     * transformation properties currently set on the view, such as
     * {@link #setScaleX(float)} or {@link #setRotation(float)}.
     *
     * @param outRect The (scrolled) drawing bounds of the view.
     */
    void getDrawingRect(Rect outRect);
    vector<int> getDrawableState();
    
    /**
     * <p>Return the time at which the drawing of the view hierarchy started.</p>
     *
     * @return the drawing start time in milliseconds
     */
    uint64_t getDrawingTime() {
        return mAttachInfo != NULL ? mAttachInfo->mDrawingTime : 0;
    }
    
    /**
     * When a view has focus and the user navigates away from it, the next view is searched for
     * starting from the rectangle filled in by this method.
     *
     * By default, the rectangle is the {@link #getDrawingRect(android.graphics.Rect)})
     * of the view.  However, if your view maintains some idea of internal selection,
     * such as a cursor, or a selected row or column, you should override this method and
     * fill in a more specific rectangle.
     *
     * @param r The rectangle to fill in, in this view's coordinates.
     */
    virtual void getFocusedRect(Rect r) { getDrawingRect(r); }
    
    /**
     * @return A handler associated with the thread running the View. This
     * handler can be used to pump events in the UI events queue.
     */
    sp<Handler> getHandler();
    int getHeight() const { return mBottom - mTop; }
    
    /**
     * Hit rectangle in parent's coordinates
     *
     * @param outRect The hit rectangle of the view.
     */
    void getHitRect(Rect &outRect);
    string getId() { return m_id; }
    int getLayoutDirection() { return LAYOUT_DIRECTION_LTR; }
    LayoutParams *getLayoutParams();
    int getLeft() { return mLeft; }
    
    /**
     * <p>Computes the coordinates of this view on the screen. The argument
     * must be an array of two integers. After the method returns, the array
     * contains the x and y location in that order.</p>
     *
     * @param location an array of two integers in which to hold the coordinates
     */
    void getLocationOnScreen(vector<int> &location);
    
    /**
     * <p>Computes the coordinates of this view in its window. The argument
     * must be an array of two integers. After the method returns, the array
     * contains the x and y location in that order.</p>
     *
     * @param location an array of two integers in which to hold the coordinates
     */
    void getLocationInWindow(vector<int> &location);
    
    Matrix getMatrix();
    int getMeasuredHeight();
    int getMeasuredState();
    int getMeasuredWidth();
    int getScaledMinimumFlingVelocity() { return mMinimumFlingVelocity; }
    int getPaddingBottom() { return mPaddingBottom; }
    int getPaddingLeft() { return mPaddingLeft; }
    int getPaddingRight() { return mPaddingRight; }
    int getPaddingTop() { return mPaddingTop; }
    int getPaddingStart();
    int getPaddingEnd();
    
    /**
     * Return the value specifying the text alignment or policy that was set with
     * {@link #setTextAlignment(int)}.
     *
     * @return the defined text alignment. It can be one of:
     *
     * {@link #TEXT_ALIGNMENT_INHERIT},
     * {@link #TEXT_ALIGNMENT_GRAVITY},
     * {@link #TEXT_ALIGNMENT_CENTER},
     * {@link #TEXT_ALIGNMENT_TEXT_START},
     * {@link #TEXT_ALIGNMENT_TEXT_END},
     * {@link #TEXT_ALIGNMENT_VIEW_START},
     * {@link #TEXT_ALIGNMENT_VIEW_END}
     *
     * @attr ref android.R.styleable#View_textAlignment
     *
     * @hide
     */
    int getRawTextAlignment();
    
    /**
     * Return the value specifying the text direction or policy that was set with
     * {@link #setTextDirection(int)}.
     *
     * @return the defined text direction. It can be one of:
     *
     * {@link #TEXT_DIRECTION_INHERIT},
     * {@link #TEXT_DIRECTION_FIRST_STRONG}
     * {@link #TEXT_DIRECTION_ANY_RTL},
     * {@link #TEXT_DIRECTION_LTR},
     * {@link #TEXT_DIRECTION_RTL},
     * {@link #TEXT_DIRECTION_LOCALE}
     *
     * @attr ref android.R.styleable#View_textDirection
     *
     * @hide
     */
    int getRawTextDirection();
    
    /**
     * Return the resolved text direction.
     *
     * @return the resolved text direction. Returns one of:
     *
     * {@link #TEXT_DIRECTION_FIRST_STRONG}
     * {@link #TEXT_DIRECTION_ANY_RTL},
     * {@link #TEXT_DIRECTION_LTR},
     * {@link #TEXT_DIRECTION_RTL},
     * {@link #TEXT_DIRECTION_LOCALE}
     *
     * @attr ref android.R.styleable#View_textDirection
     */
    int getTextDirection();
    
    Resources *getResources() { return getContext()->getResources(); }
    int getRight() { return mRight; }
    
    /**
     * <p>Finds the topmost view in the current view hierarchy.</p>
     *
     * @return the topmost view containing this view
     */
    shared_ptr<View> getRootView();
    
    /**
     * Return the scrolled left position of this view. This is the left edge of
     * the displayed part of your view. You do not need to draw any pixels
     * farther left, since those are outside of the frame of your view on
     * screen.
     *
     * @return The left edge of the displayed part of your view, in pixels.
     */
    int getScrollX() { return mScrollX; }
    
    /**
     * Return the scrolled top position of this view. This is the top edge of
     * the displayed part of your view. You do not need to draw any pixels above
     * it, since those are outside of the frame of your view on screen.
     *
     * @return The top edge of the displayed part of your view, in pixels.
     */
    int getScrollY() { return mScrollY; }
    
    int getSolidColor() { return 0; }
    
    /**
     * Returns this view's tag.
     *
     * @return the Object stored in this view as a tag
     *
     * @see #setTag(Object)
     * @see #getTag(int)
     */
    shared_ptr<Object> getTag() { return mTag; }
    
    /**
     * Sets the tag associated with this view. A tag can be used to mark
     * a view in its hierarchy and does not have to be unique within the
     * hierarchy. Tags can also be used to store data within a view without
     * resorting to another data structure.
     *
     * @param tag an Object to tag the view with
     *
     * @see #getTag()
     * @see #setTag(int, Object)
     */
    void setTag(shared_ptr<Object> tag) {
        mTag = tag;
    }
    
    /**
     * Return the resolved text alignment.
     *
     * @return the resolved text alignment. Returns one of:
     *
     * {@link #TEXT_ALIGNMENT_GRAVITY},
     * {@link #TEXT_ALIGNMENT_CENTER},
     * {@link #TEXT_ALIGNMENT_TEXT_START},
     * {@link #TEXT_ALIGNMENT_TEXT_END},
     * {@link #TEXT_ALIGNMENT_VIEW_START},
     * {@link #TEXT_ALIGNMENT_VIEW_END}
     *
     * @attr ref android.R.styleable#View_textAlignment
     */
    int getTextAlignment();
    
    int getTop() { return mTop; }
    int getTouchSlop() { return mTouchSlop; }
    shared_ptr<ViewParent> getParent();
    
    
    /**
     * Returns the size of the vertical faded edges used to indicate that more
     * content in this view is visible.
     *
     * @return The size in pixels of the vertical faded edge or 0 if vertical
     *         faded edges are not enabled for this view.
     * @attr ref android.R.styleable#View_fadingEdgeLength
     */
    int getVerticalFadingEdgeLength();
    
    /**
     * Returns the width of the vertical scrollbar.
     *
     * @return The width in pixels of the vertical scrollbar or 0 if there
     *         is no vertical scrollbar.
     */
    int getVerticalScrollbarWidth();
    ViewRootImpl *getViewRootImpl();
    
    /**
     * Returns the ViewTreeObserver for this view's hierarchy. The view tree
     * observer can be used to get notifications when global events, like
     * layout, happen.
     *
     * The returned ViewTreeObserver observer is not guaranteed to remain
     * valid for the lifetime of this View. If the caller of this method keeps
     * a long-lived reference to ViewTreeObserver, it should always check for
     * the return value of {@link ViewTreeObserver#isAlive()}.
     *
     * @return The ViewTreeObserver for this view's hierarchy.
     */
    ViewTreeObserver *getViewTreeObserver();
    
    int getVisibility();
    const int getWidth() { return mRight - mLeft; }
    
    /**
     * Returns the current visibility of the window this view is attached to
     * (either {@link #GONE}, {@link #INVISIBLE}, or {@link #VISIBLE}).
     *
     * @return Returns the current visibility of the view's window.
     */
    int getWindowVisibility();
    
    /**
     * Returns true if this view has focus iteself, or is the ancestor of the
     * view that has focus.
     *
     * @return True if this view has or contains focus, false otherwise.
     */
    virtual bool hasFocus() {
        return (mPrivateFlags & PFLAG_FOCUSED) != 0;
    }
    
    /**
     * Returns true if this view is focusable or if it contains a reachable View
     * for which {@link #hasFocusable()} returns true. A "reachable hasFocusable()"
     * is a View whose parents do not block descendants focus.
     *
     * Only {@link #VISIBLE} views are considered focusable.
     *
     * @return True if the view is focusable or if the view contains a focusable
     *         View, false otherwise.
     *
     * @see ViewGroup#FOCUS_BLOCK_DESCENDANTS
     */
    bool hasFocusable() {
        return (mViewFlags & VISIBILITY_MASK) == VISIBLE && isFocusable();
    }
    
    virtual bool hasOverlappingRendering() { return true; }
    
    /**
     * Returns true if this view is in a window that currently has window focus.
     * Note that this is not the same as the view itself having focus.
     *
     * @return True if this view is in a window that currently has window focus.
     */
    bool hasWindowFocus();
    
    virtual void invalidate();
    virtual void invalidate(bool invalidateCache);
    virtual void invalidate(int l, int t, int r, int b);
    virtual void invalidateDrawable(shared_ptr<Drawable> who);
    
    bool isClickable() { return (mViewFlags & CLICKABLE) == CLICKABLE; }
    
    /**
     * <p>Indicates whether the drawing cache is enabled for this view.</p>
     *
     * @return true if the drawing cache is enabled
     *
     * @see #setDrawingCacheEnabled(boolean)
     * @see #getDrawingCache()
     */
    bool isDrawingCacheEnabled() {
        return (mViewFlags & DRAWING_CACHE_ENABLED) == DRAWING_CACHE_ENABLED;
    }
    
    bool isEnabled() { return true; }
    bool isFocused() { return (mPrivateFlags & PFLAG_FOCUSED) != 0; }
    
    
    /**
     * Returns whether this View is able to take focus.
     *
     * @return True if this view can take focus, or false otherwise.
     * @attr ref android.R.styleable#View_focusable
     */
    bool isFocusable() {
        return FOCUSABLE == (mViewFlags & FOCUSABLE_MASK);
    }
    
    /**
     * <p>Indicate whether the horizontal edges are faded when the view is
     * scrolled horizontally.</p>
     *
     * @return true if the horizontal edges should are faded on scroll, false
     *         otherwise
     *
     * @see #setHorizontalFadingEdgeEnabled(boolean)
     *
     * @attr ref android.R.styleable#View_requiresFadingEdge
     */
    bool isHorizontalFadingEdgeEnabled();
    
    /**
     * <p>Indicate whether the vertical edges are faded when the view is
     * scrolled horizontally.</p>
     *
     * @return true if the vertical edges should are faded on scroll, false
     *         otherwise
     *
     * @see #setVerticalFadingEdgeEnabled(boolean)
     *
     * @attr ref android.R.styleable#View_requiresFadingEdge
     */
    bool isVerticalFadingEdgeEnabled() {
        return (mViewFlags & FADING_EDGE_VERTICAL) == FADING_EDGE_VERTICAL;
    }
    
    /**
     * Returns whether the device is currently in touch mode.  Touch mode is entered
     * once the user begins interacting with the device by touch, and affects various
     * things like whether focus is always visible to the user.
     *
     * @return Whether the device is in touch mode.
     */
    
    bool isInTouchMode() {
        return true;
//        if (mAttachInfo != null) {
//            return mAttachInfo.mInTouchMode;
//        } else {
//            return ViewRootImpl.isInTouchMode();
//        }
    }
    bool isLaidOut();
    bool isLongClickable() { return (mViewFlags & LONG_CLICKABLE) == LONG_CLICKABLE; }
    bool isInScrollingContainer();
    bool isLayoutRtl() { return (getLayoutDirection() == LAYOUT_DIRECTION_RTL); }
    bool isLayoutRequested();
    bool isOpaque();
    bool isPressed();
    
    /**
     * {@hide}
     *
     * @return true if the view belongs to the root namespace, false otherwise
     */
    bool isRootNamespace() {
        return (mPrivateFlags & PFLAG_IS_ROOT_NAMESPACE) != 0;
    }
    
    /**
     * Indicates the selection state of this view.
     *
     * @return true if the view is selected, false otherwise
     */
    
    bool isSelected() {
        return (mPrivateFlags & PFLAG_SELECTED) != 0;
    }
    
    /**
     * @return true if text alignment is inherited.
     *
     * @hide
     */
    bool isTextAlignmentInherited();
    
    /**
     * @return true if text alignment is resolved.
     *
     * @hide
     */
    bool isTextAlignmentResolved();
    
    /**
     * @return true if text direction is inherited.
     *
     * @hide
     */
    bool isTextDirectionInherited();
    
    /**
     * @return true if text direction is resolved.
     *
     * @hide
     */
    bool isTextDirectionResolved();
    
    /**
     * Call {@link Drawable#jumpToCurrentState() Drawable.jumpToCurrentState()}
     * on all Drawable objects associated with this view.
     */
    virtual void jumpDrawablesToCurrentState();
    virtual void layout(int left, int top, int right, int bottom);
    void measure(int widthMeasureSpec, int heightMeasureSpec);
    
    /**
     * Check whether the called view is a text editor, in which case it
     * would make sense to automatically display a soft input window for
     * it.  Subclasses should override this if they implement
     * {@link #onCreateInputConnection(EditorInfo)} to return true if
     * a call on that method would return a non-null InputConnection, and
     * they are really a first-class editor that the user would normally
     * start typing on when the go into a window containing your view.
     *
     * <p>The default implementation always returns false.  This does
     * <em>not</em> mean that its {@link #onCreateInputConnection(EditorInfo)}
     * will not be called or the user can not otherwise perform edits on your
     * view; it is just a hint to the system that this is not the primary
     * purpose of this view.
     *
     * @return Returns true if this view is a text editor, else false.
     */
    virtual bool onCheckIsTextEditor() { return false; }
    
    virtual bool onGenericMotionEvent(MotionEvent *event) { return false; }
    
    /**
     * Offset this view's vertical location by the specified number of pixels.
     *
     * @param offset the number of pixels to offset the view by
     */
    void offsetTopAndBottom(int offset);
    
    /**
     * Offset this view's horizontal location by the specified amount of pixels.
     *
     * @param offset the number of pixels to offset the view by
     */
    void offsetLeftAndRight(int offset);
    
    /**
     * Default implementation of {@link KeyEvent.Callback#onKeyDown(int, KeyEvent)
     * KeyEvent.Callback.onKeyDown()}: perform press of the view
     * when {@link KeyEvent#KEYCODE_DPAD_CENTER} or {@link KeyEvent#KEYCODE_ENTER}
     * is released, if the view is enabled and clickable.
     *
     * <p>Key presses in software keyboards will generally NOT trigger this listener,
     * although some may elect to do so in some situations. Do not rely on this to
     * catch software key presses.
     *
     * @param keyCode A key code that represents the button pressed, from
     *                {@link android.view.KeyEvent}.
     * @param event   The KeyEvent object that defines the button action.
     */
    virtual bool onKeyDown(int keyCode, KeyEvent *event);
    
    /**
     * Default implementation of {@link KeyEvent.Callback#onKeyUp(int, KeyEvent)
     * KeyEvent.Callback.onKeyUp()}: perform clicking of the view
     * when {@link KeyEvent#KEYCODE_DPAD_CENTER} or
     * {@link KeyEvent#KEYCODE_ENTER} is released.
     * <p>Key presses in software keyboards will generally NOT trigger this listener,
     * although some may elect to do so in some situations. Do not rely on this to
     * catch software key presses.
     *
     * @param keyCode A key code that represents the button pressed, from
     *                {@link android.view.KeyEvent}.
     * @param event   The KeyEvent object that defines the button action.
     */
    virtual bool onKeyUp(int keyCode, KeyEvent *event);
    
    /**
     * Default implementation of {@link KeyEvent.Callback#onKeyMultiple(int, int, KeyEvent)
     * KeyEvent.Callback.onKeyMultiple()}: always returns false (doesn't handle
     * the event).
     * <p>Key presses in software keyboards will generally NOT trigger this listener,
     * although some may elect to do so in some situations. Do not rely on this to
     * catch software key presses.
     *
     * @param keyCode     A key code that represents the button pressed, from
     *                    {@link android.view.KeyEvent}.
     * @param repeatCount The number of times the action was made.
     * @param event       The KeyEvent object that defines the button action.
     */
    virtual bool onKeyMultiple(int keyCode, int repeatCount, KeyEvent *event) { return false; }
    
    /**
     * Handle a key event before it is processed by any input method
     * associated with the view hierarchy.  This can be used to intercept
     * key events in special situations before the IME consumes them; a
     * typical example would be handling the BACK key to update the application's
     * UI instead of allowing the IME to see it and close itself.
     *
     * @param keyCode The value in event.getKeyCode().
     * @param event Description of the key event.
     * @return If you handled the event, return true. If you want to allow the
     *         event to be handled by the next receiver, return false.
     */
    virtual bool onKeyPreIme(int keyCode, KeyEvent *event) { return false; }
    
    /**
     * Called when layout direction has been resolved.
     *
     * The default implementation does nothing.
     *
     * @param layoutDirection The resolved layout direction.
     *
     * @see #LAYOUT_DIRECTION_LTR
     * @see #LAYOUT_DIRECTION_RTL
     *
     * @hide
     */
    void onResolveDrawables(int layoutDirection);
    
    /**
     * This method is called whenever the state of the screen this view is
     * attached to changes. A state change will usually occurs when the screen
     * turns on or off (whether it happens automatically or the user does it
     * manually.)
     *
     * @param screenState The new state of the screen. Can be either
     *                    {@link #SCREEN_STATE_ON} or {@link #SCREEN_STATE_OFF}
     */
    virtual void onScreenStateChanged(int screenState) {}
    virtual bool onTouchEvent(MotionEvent *ev);
    
    /**
     * Called when the window containing this view gains or loses focus.  Note
     * that this is separate from view focus: to receive key events, both
     * your view and its window must have focus.  If a window is displayed
     * on top of yours that takes input focus, then your own window will lose
     * focus but the view focus will remain unchanged.
     *
     * @param hasWindowFocus True if the window containing this view now has
     *        focus, false otherwise.
     */
    virtual void onWindowFocusChanged(bool hasWindowFocus);
    bool performClick();
    virtual bool performLongClick();
    bool pointInView(float localX, float localY) const;
    bool pointInView(float localX, float localY, float slop) const;
    bool post(const sp<Runnable>& runnable);
    bool post(function<void ()> function);
    bool postDelayed(const sp<Runnable>& runnable, uint32_t delay);
    bool postDelayed(function<void ()> function, uint32_t delay);
    void refreshDrawableState();
    bool removeCallbacks(const sp<Runnable>& runnable);
    void resetResolvedPadding();
    
    /**
     * Reset resolved text alignment. Text alignment will be resolved during a call to
     * {@link #onMeasure(int, int)}.
     *
     * @hide
     */
    virtual void resetResolvedTextAlignment();
    
    /**
     * Reset resolved text direction. Text direction will be resolved during a call to
     * {@link #onMeasure(int, int)}.
     *
     * @hide
     */
    void resetResolvedTextDirection();
    
    /**
     * Resolve the text direction.
     *
     * @return true if resolution has been done, false otherwise.
     *
     * @hide
     */
    bool resolveTextDirection();
    void resolvePadding();
    
    /**
     * Resolve the text alignment.
     *
     * @return true if resolution has been done, false otherwise.
     *
     * @hide
     */
    virtual bool resolveTextAlignment();
    
    /**
     * Call this to try to give focus to a specific view or to one of its
     * descendants.
     *
     * A view will not actually take focus if it is not focusable ({@link #isFocusable} returns
     * false), or if it is focusable and it is not focusable in touch mode
     * ({@link #isFocusableInTouchMode}) while the device is in touch mode.
     *
     * See also {@link #focusSearch(int)}, which is what you call to say that you
     * have focus, and you want your parent to look for the next one.
     *
     * This is equivalent to calling {@link #requestFocus(int, Rect)} with arguments
     * {@link #FOCUS_DOWN} and <code>null</code>.
     *
     * @return Whether this view or one of its descendants actually took focus.
     */
    bool requestFocus() {
        return requestFocus(View::FOCUS_DOWN);
    }
    
    /**
     * Call this to try to give focus to a specific view or to one of its
     * descendants and give it a hint about what direction focus is heading.
     *
     * A view will not actually take focus if it is not focusable ({@link #isFocusable} returns
     * false), or if it is focusable and it is not focusable in touch mode
     * ({@link #isFocusableInTouchMode}) while the device is in touch mode.
     *
     * See also {@link #focusSearch(int)}, which is what you call to say that you
     * have focus, and you want your parent to look for the next one.
     *
     * This is equivalent to calling {@link #requestFocus(int, Rect)} with
     * <code>null</code> set for the previously focused rectangle.
     *
     * @param direction One of FOCUS_UP, FOCUS_DOWN, FOCUS_LEFT, and FOCUS_RIGHT
     * @return Whether this view or one of its descendants actually took focus.
     */
    bool requestFocus(int direction) {
        return requestFocus(direction, NULL);
    }
    
    /**
     * Call this to try to give focus to a specific view or to one of its descendants
     * and give it hints about the direction and a specific rectangle that the focus
     * is coming from.  The rectangle can help give larger views a finer grained hint
     * about where focus is coming from, and therefore, where to show selection, or
     * forward focus change internally.
     *
     * A view will not actually take focus if it is not focusable ({@link #isFocusable} returns
     * false), or if it is focusable and it is not focusable in touch mode
     * ({@link #isFocusableInTouchMode}) while the device is in touch mode.
     *
     * A View will not take focus if it is not visible.
     *
     * A View will not take focus if one of its parents has
     * {@link android.view.ViewGroup#getDescendantFocusability()} equal to
     * {@link ViewGroup#FOCUS_BLOCK_DESCENDANTS}.
     *
     * See also {@link #focusSearch(int)}, which is what you call to say that you
     * have focus, and you want your parent to look for the next one.
     *
     * You may wish to override this method if your custom {@link View} has an internal
     * {@link View} that it wishes to forward the request to.
     *
     * @param direction One of FOCUS_UP, FOCUS_DOWN, FOCUS_LEFT, and FOCUS_RIGHT
     * @param previouslyFocusedRect The rectangle (in this View's coordinate system)
     *        to give a finer grained hint about where focus is coming from.  May be null
     *        if there is no hint.
     * @return Whether this view or one of its descendants actually took focus.
     */
    bool requestFocus(int direction, Rect *previouslyFocusedRect) {
        return requestFocusNoSearch(direction, previouslyFocusedRect);
    }
    
private:
    
    bool requestFocusNoSearch(int direction, Rect *previouslyFocusedRect);
    bool hasAncestorThatBlocksDescendantFocus();
    
public:
    
    virtual void requestLayout();
    virtual void scheduleDrawable(Drawable *who, const sp<Runnable> &what, uint32_t when);
    
    /**
     * Set the scrolled position of your view. This will cause a call to
     * {@link #onScrollChanged(int, int, int, int)} and the view will be
     * invalidated.
     * @param x the x position to scroll to
     * @param y the y position to scroll to
     */
    void scrollTo(int x, int y);
    
    /**
     * Set the size of the faded edge used to indicate that more content in this
     * view is available.  Will not change whether the fading edge is enabled; use
     * {@link #setVerticalFadingEdgeEnabled(boolean)} or
     * {@link #setHorizontalFadingEdgeEnabled(boolean)} to enable the fading edge
     * for the vertical or horizontal fading edges.
     *
     * @param length The size in pixels of the faded edge used to indicate that more
     *        content in this view is visible.
     */
    void setFadingEdgeLength(int length) {
        initScrollCache();
//        mScrollCache.fadingEdgeLength = length;
    }
    
    /**
     * Resolve the layout parameters depending on the resolved layout direction
     *
     * @hide
     */
    void resolveLayoutParams();
    
    void setBackground(shared_ptr<Drawable> background);
    
    /**
     * Set the background to a given resource. The resource should refer to
     * a Drawable object or 0 to remove the background.
     * @param resid The identifier of the resource.
     *
     * @attr ref android.R.styleable#View_background
     */
    void setBackgroundResource(string resid);
    
    void setClickable(bool clickable) { setFlags(clickable ? CLICKABLE : 0, CLICKABLE); }
    
    /**
     * <p>Enables or disables the drawing cache. When the drawing cache is enabled, the next call
     * to {@link #getDrawingCache()} or {@link #buildDrawingCache()} will draw the view in a
     * bitmap. Calling {@link #draw(android.graphics.Canvas)} will not draw from the cache when
     * the cache is enabled. To benefit from the cache, you must request the drawing cache by
     * calling {@link #getDrawingCache()} and draw it on screen if the returned bitmap is not
     * null.</p>
     *
     * <p>Enabling the drawing cache is similar to
     * {@link #setLayerType(int, android.graphics.Paint) setting a layer} when hardware
     * acceleration is turned off. When hardware acceleration is turned on, enabling the
     * drawing cache has no effect on rendering because the system uses a different mechanism
     * for acceleration which ignores the flag. If you want to use a Bitmap for the view, even
     * when hardware acceleration is enabled, see {@link #setLayerType(int, android.graphics.Paint)}
     * for information on how to enable software and hardware layers.</p>
     *
     * <p>This API can be used to manually generate
     * a bitmap copy of this view, by setting the flag to <code>true</code> and calling
     * {@link #getDrawingCache()}.</p>
     *
     * @param enabled true to enable the drawing cache, false otherwise
     *
     * @see #isDrawingCacheEnabled()
     * @see #getDrawingCache()
     * @see #buildDrawingCache()
     * @see #setLayerType(int, android.graphics.Paint)
     */
    void setDrawingCacheEnabled(bool enabled) {
        mCachingFailed = false;
        setFlags(enabled ? DRAWING_CACHE_ENABLED : 0, DRAWING_CACHE_ENABLED);
    }
    
    virtual void setEnabled(bool enabled);
    
    /**
     * Set the layout direction for this view. This will propagate a reset of layout direction
     * resolution to the view's children and resolve layout direction for this view.
     *
     * @param layoutDirection the layout direction to set. Should be one of:
     *
     * {@link #LAYOUT_DIRECTION_LTR},
     * {@link #LAYOUT_DIRECTION_RTL},
     * {@link #LAYOUT_DIRECTION_INHERIT},
     * {@link #LAYOUT_DIRECTION_LOCALE}.
     *
     * Resolution will be done if the value is set to LAYOUT_DIRECTION_INHERIT. The resolution
     * proceeds up the parent chain of the view to get the value. If there is no parent, then it
     * will return the default {@link #LAYOUT_DIRECTION_LTR}.
     *
     * @attr ref android.R.styleable#View_layoutDirection
     */
    void setLayoutDirection(int layoutDirection);
    void setLongClickable(bool clickable) { setFlags(clickable ? LONG_CLICKABLE : 0, LONG_CLICKABLE); }
    
    /**
     * <p>Define whether the horizontal edges should be faded when this view
     * is scrolled horizontally.</p>
     *
     * @param horizontalFadingEdgeEnabled true if the horizontal edges should
     *                                    be faded when the view is scrolled
     *                                    horizontally
     *
     * @see #isHorizontalFadingEdgeEnabled()
     *
     * @attr ref android.R.styleable#View_requiresFadingEdge
     */
    void setHorizontalFadingEdgeEnabled(bool horizontalFadingEdgeEnabled);
    
    /**
     * <p>Define whether the vertical edges should be faded when this view
     * is scrolled vertically.</p>
     *
     * @param verticalFadingEdgeEnabled true if the vertical edges should
     *                                  be faded when the view is scrolled
     *                                  vertically
     *
     * @see #isVerticalFadingEdgeEnabled()
     *
     * @attr ref android.R.styleable#View_requiresFadingEdge
     */
    void setVerticalFadingEdgeEnabled(bool verticalFadingEdgeEnabled);
    
    void setId(string id);
    void setIsRootNamespace(bool isRoot);
    void setLayoutParams(LayoutParams *params);
    virtual void setOnClickListener(function<void (View&)> onClickListener);
    void setOnLongClickListener(function<void (View&)> onLongClickListener);
    virtual void setPadding(int left, int top, int right, int bottom);
    /**
     * Sets the relative padding. The view may add on the space required to display
     * the scrollbars, depending on the style and visibility of the scrollbars.
     * So the values returned from {@link #getPaddingStart}, {@link #getPaddingTop},
     * {@link #getPaddingEnd} and {@link #getPaddingBottom} may be different
     * from the values set in this call.
     *
     * @attr ref android.R.styleable#View_padding
     * @attr ref android.R.styleable#View_paddingBottom
     * @attr ref android.R.styleable#View_paddingStart
     * @attr ref android.R.styleable#View_paddingEnd
     * @attr ref android.R.styleable#View_paddingTop
     * @param start the start padding in pixels
     * @param top the top padding in pixels
     * @param end the end padding in pixels
     * @param bottom the bottom padding in pixels
     */
    virtual void setPaddingRelative(int start, int top, int end, int bottom);
    void setPressed(bool pressed);
    
    /**
     * Changes the selection state of this view. A view can be selected or not.
     * Note that selection is not the same as focus. Views are typically
     * selected in the context of an AdapterView like ListView or GridView;
     * the selected view is the view that is highlighted.
     *
     * @param selected true if the view must be selected, false otherwise
     */
    void setSelected(bool selected);
    
    /**
     * <p>Indicate whether the horizontal scrollbar should be drawn or not. The
     * scrollbar is not drawn by default.</p>
     *
     * @return true if the horizontal scrollbar should be painted, false
     *         otherwise
     *
     * @see #setHorizontalScrollBarEnabled(boolean)
     */
    bool isHorizontalScrollBarEnabled() {
        return (mViewFlags & SCROLLBARS_HORIZONTAL) == SCROLLBARS_HORIZONTAL;
    }
    
    /**
     * <p>Define whether the horizontal scrollbar should be drawn or not. The
     * scrollbar is not drawn by default.</p>
     *
     * @param horizontalScrollBarEnabled true if the horizontal scrollbar should
     *                                   be painted
     *
     * @see #isHorizontalScrollBarEnabled()
     */
    void setHorizontalScrollBarEnabled(bool horizontalScrollBarEnabled);
    
    /**
     * <p>Indicate whether the vertical scrollbar should be drawn or not. The
     * scrollbar is not drawn by default.</p>
     *
     * @return true if the vertical scrollbar should be painted, false
     *         otherwise
     *
     * @see #setVerticalScrollBarEnabled(boolean)
     */
    bool isVerticalScrollBarEnabled() {
        return (mViewFlags & SCROLLBARS_VERTICAL) == SCROLLBARS_VERTICAL;
    }
    
    /**
     * <p>Define whether the vertical scrollbar should be drawn or not. The
     * scrollbar is not drawn by default.</p>
     *
     * @param verticalScrollBarEnabled true if the vertical scrollbar should
     *                                 be painted
     *
     * @see #isVerticalScrollBarEnabled()
     */
    void setVerticalScrollBarEnabled(bool verticalScrollBarEnabled);
    
    /**
     * Reset the resolved layout direction. Layout direction will be resolved during a call to
     * {@link #onMeasure(int, int)}.
     *
     * @hide
     */
    void resetResolvedLayoutDirection() {
        // Reset the current resolved bits
        mPrivateFlags2 &= ~PFLAG2_LAYOUT_DIRECTION_RESOLVED_MASK;
    }
    
    /**
     * Reset resolution of all RTL related properties.
     *
     * @hide
     */
    void resetRtlProperties();
    
    /**
     * Resolve all RTL related properties.
     *
     * @return true if resolution of RTL properties has been done
     *
     * @hide
     */
    bool resolveRtlPropertiesIfNeeded();
    
    /**
     * Setting a solid background color for the drawing cache's bitmaps will improve
     * performance and memory usage. Note, though that this should only be used if this
     * view will always be drawn on top of a solid color.
     *
     * @param color The background color to use for the drawing cache's bitmap
     *
     * @see #setDrawingCacheEnabled(boolean)
     * @see #buildDrawingCache()
     * @see #getDrawingCache()
     */
    void setDrawingCacheBackgroundColor(int color) {
        if (color != mDrawingCacheBackgroundColor) {
            mDrawingCacheBackgroundColor = color;
            mPrivateFlags &= ~PFLAG_DRAWING_CACHE_VALID;
        }
    }

    /**
     * Set the text alignment.
     *
     * @param textAlignment The text alignment to set. Should be one of
     *
     * {@link #TEXT_ALIGNMENT_INHERIT},
     * {@link #TEXT_ALIGNMENT_GRAVITY},
     * {@link #TEXT_ALIGNMENT_CENTER},
     * {@link #TEXT_ALIGNMENT_TEXT_START},
     * {@link #TEXT_ALIGNMENT_TEXT_END},
     * {@link #TEXT_ALIGNMENT_VIEW_START},
     * {@link #TEXT_ALIGNMENT_VIEW_END}
     *
     * Resolution will be done if the value is set to TEXT_ALIGNMENT_INHERIT. The resolution
     * proceeds up the parent chain of the view to get the value. If there is no parent, then it
     * will return the default {@link #TEXT_ALIGNMENT_GRAVITY}.
     *
     * @attr ref android.R.styleable#View_textAlignment
     */
    void setTextAlignment(int textAlignment);
    
    /**
     * Set the text direction.
     *
     * @param textDirection the direction to set. Should be one of:
     *
     * {@link #TEXT_DIRECTION_INHERIT},
     * {@link #TEXT_DIRECTION_FIRST_STRONG}
     * {@link #TEXT_DIRECTION_ANY_RTL},
     * {@link #TEXT_DIRECTION_LTR},
     * {@link #TEXT_DIRECTION_RTL},
     * {@link #TEXT_DIRECTION_LOCALE}
     *
     * Resolution will be done if the value is set to TEXT_DIRECTION_INHERIT. The resolution
     * proceeds up the parent chain of the view to get the value. If there is no parent, then it will
     * return the default {@link #TEXT_DIRECTION_FIRST_STRONG}.
     *
     * @attr ref android.R.styleable#View_textDirection
     */
    void setTextDirection(int textDirection);
    void setVisibility(int visibility);
    void setWillNotCacheDrawing(bool willNotCacheDrawing);
    void setWillNotDraw(bool willNotDraw);
    
    virtual void unscheduleDrawable(Drawable *who, const sp<Runnable> &what);
    void unscheduleDrawable(Drawable *who);
    bool willNotDraw();
    
    static int combineMeasuredStates(int curState, int newState) { return curState | newState; }
    static shared_ptr<View> createInstance(const char *name, Context *context, AttributeSet *attrs);
    static int getDefaultSize(int size, int measureSpec);
    static int resolveSize(int size, int measureSpec);
    static int resolveSizeAndState(int size, int measureSpec, int childMeasuredState);
    
private:
    
    /**
     * Flag indicating whether a view failed the quickReject() check in draw(). This condition
     * is used to check whether later changes to the view's transform should invalidate the
     * view to force the quickReject test to run again.
     */
    static const int PFLAG2_VIEW_QUICK_REJECTED        = 0x10000000;
    static const int PFLAG_DRAWING_CACHE_VALID         = 0x00008000;
    static const int DRAWING_CACHE_ENABLED             = 0x00008000;
    static const int DRAWING_CACHE_QUALITY_MASK        = 0x00180000;
    
    static const int PFLAG_DRAW_ANIMATION              = 0x00000040;
    static const int PFLAG_ONLY_DRAWS_BACKGROUND       = 0x00000100;
    static const int PFLAG_DRAWABLE_STATE_DIRTY        = 0x00000400;
    static const int PFLAG_SKIP_DRAW                   = 0x00000080;
    static const int PFLAG_IS_ROOT_NAMESPACE           = 0x00000008;
    static const int PFLAG_DRAWN                       = 0x00000020;
    static const int PFLAG_DIRTY_MASK                  = 0x00600000;
    static const int PFLAG_FORCE_LAYOUT                = 0x00001000;
    static const int PFLAG_INVALIDATED                 = 0x80000000;
    static const int PFLAG_LAYOUT_REQUIRED             = 0x00002000;
    
    /**
     * Indicates whether the view is opaque.
     *
     * @hide
     */
    static const int PFLAG_OPAQUE_MASK                 = 0x01800000;
    
    /**
     * Indicates whether the background is opaque.
     *
     * @hide
     */
    static const int PFLAG_OPAQUE_BACKGROUND           = 0x00800000;
    
    /**
     * View flag indicating whether this view was invalidated (fully or partially.)
     *
     * @hide
     */
    static const int PFLAG_DIRTY                       = 0x00200000;
    
    /**
     * View flag indicating whether this view was invalidated by an opaque
     * invalidate request.
     *
     * @hide
     */
    static const int PFLAG_DIRTY_OPAQUE                = 0x00400000;
    
    /**
     * Indicates whether the scrollbars are opaque.
     *
     * @hide
     */
    static const int PFLAG_OPAQUE_SCROLLBARS           = 0x01000000;
    
    /**
     * Mask to check if the scrollbar style is inside or outside.
     * {@hide}
     */
    static const int SCROLLBARS_OUTSIDE_MASK = 0x02000000;
    
    /**
     * Mask for scrollbar style.
     * {@hide}
     */
    static const int SCROLLBARS_STYLE_MASK = 0x03000000;
    
    /**
     * <p>This view shows horizontal scrollbars.</p>
     * {@hide}
     */
    static const int SCROLLBARS_HORIZONTAL = 0x00000100;
    
    /**
     * <p>This view shows vertical scrollbars.</p>
     * {@hide}
     */
    static const int SCROLLBARS_VERTICAL = 0x00000200;
    
    static const int VIEW_STATE_WINDOW_FOCUSED = 1;
    static const int VIEW_STATE_SELECTED = 1 << 1;
    static const int VIEW_STATE_FOCUSED = 1 << 2;
    static const int VIEW_STATE_ENABLED = 1 << 3;
    static const int VIEW_STATE_PRESSED = 1 << 4;
    static const int VIEW_STATE_ACTIVATED = 1 << 5;
    static const int VIEW_STATE_ACCELERATED = 1 << 6;
    static const int VIEW_STATE_HOVERED = 1 << 7;
    static const int VIEW_STATE_DRAG_CAN_ACCEPT = 1 << 8;
    static const int VIEW_STATE_DRAG_HOVERED = 1 << 9;
    
    /**
     * Indicates a prepressed state;
     * the short time between ACTION_DOWN and recognizing
     * a 'real' press. Prepressed is used to recognize quick taps
     * even when they are shorter than ViewConfiguration.getTapTimeout().
     *
     * @hide
     */
    static const int PFLAG_PREPRESSED                  = 0x02000000;
    
    /**
     * Indicates whether the view is temporarily detached.
     *
     * @hide
     */
    static const int PFLAG_CANCEL_NEXT_UP_EVENT        = 0x04000000;
    static const int PFLAG_PRESSED                     = 0x00004000;
    static const int PFLAG_MEASURED_DIMENSION_SET      = 0x00000800;
    static const int PFLAG2_PADDING_RESOLVED           = 0x20000000;
    
    /**
     * Flag indicating that the start/end drawables has been resolved into left/right ones.
     */
    static const int PFLAG2_DRAWABLE_RESOLVED = 0x40000000;
    
    /**
     * Bit shift to get the horizontal layout direction. (bits after DRAG_HOVERED)
     * @hide
     */
    static const int PFLAG2_LAYOUT_DIRECTION_MASK_SHIFT = 2;
    
    /**
     * Mask for use with private flags indicating bits used for horizontal layout direction.
     * @hide
     */
    static const int PFLAG2_LAYOUT_DIRECTION_MASK = 0x00000003 << PFLAG2_LAYOUT_DIRECTION_MASK_SHIFT;
    
    /**
     * Indicates whether the view horizontal layout direction has been resolved and drawn to the
     * right-to-left direction.
     * @hide
     */
    static const int PFLAG2_LAYOUT_DIRECTION_RESOLVED_RTL = 4 << PFLAG2_LAYOUT_DIRECTION_MASK_SHIFT;
    
    /**
     * Indicates whether the view horizontal layout direction has been resolved.
     * @hide
     */
    static const int PFLAG2_LAYOUT_DIRECTION_RESOLVED = 8 << PFLAG2_LAYOUT_DIRECTION_MASK_SHIFT;
    
    /**
     * Mask for use with private flags indicating bits used for resolved horizontal layout direction.
     * @hide
     */
    static const int PFLAG2_LAYOUT_DIRECTION_RESOLVED_MASK = 0x0000000C
    << PFLAG2_LAYOUT_DIRECTION_MASK_SHIFT;
    
    /**
     * Default horizontal layout direction.
     */
    static const int LAYOUT_DIRECTION_DEFAULT = LAYOUT_DIRECTION_INHERIT;
    
    static const int PFLAG_WANTS_FOCUS                 = 0x00000001;
    static const int PFLAG_FOCUSED                     = 0x00000002;
    static const int PFLAG_SELECTED                    = 0x00000004;
    static const int UNDEFINED_PADDING                 = INT_MIN;
    static const int VISIBILITY_FLAGS[3];
    
    /**
     * Indicates that the view has received HOVER_ENTER.  Cleared on HOVER_EXIT.
     * @hide
     */
    static const int PFLAG_HOVERED             = 0x10000000;
    static const int PFLAG_ACTIVATED                   = 0x40000000;
    
    /**
     * Indicates that pivotX or pivotY were explicitly set and we should not assume the center
     * for transform operations
     *
     * @hide
     */
    static const int PFLAG_PIVOT_EXPLICITLY_SET = 0x20000000;
    static const int PFLAG_HAS_BOUNDS                  = 0x00000010;
    
    /**
     * <p>This view doesn't show fading edges.</p>
     * {@hide}
     */
    static const int FADING_EDGE_NONE = 0x00000000;
    
    /**
     * <p>This view shows horizontal fading edges.</p>
     * {@hide}
     */
    static const int FADING_EDGE_HORIZONTAL = 0x00001000;
    
    /**
     * <p>This view shows vertical fading edges.</p>
     * {@hide}
     */
    static const int FADING_EDGE_VERTICAL = 0x00002000;
    
    /**
     * <p>Mask for use with setFlags indicating bits used for indicating which
     * fading edges are enabled.</p>
     * {@hide}
     */
    static const int FADING_EDGE_MASK = 0x00003000;
    
    /**
     * <p>Indicates this view can be clicked. When clickable, a View reacts
     * to clicks by notifying the OnClickListener.<p>
     * {@hide}
     */
    static const int CLICKABLE = 0x00004000;
    
    /**
     * <p>
     * Indicates this view can be long clicked. When long clickable, a View
     * reacts to long clicks by notifying the OnLongClickListener or showing a
     * context menu.
     * </p>
     * {@hide}
     */
    static const int LONG_CLICKABLE = 0x00200000;
    
    /**
     * <p>Indicates that this view gets its drawable states from its direct parent
     * and ignores its original internal states.</p>
     *
     * @hide
     */
    static const int DUPLICATE_PARENT_STATE = 0x00400000;
    
    /**
     * This view is enabled. Interpretation varies by subclass.
     * Use with ENABLED_MASK when calling setFlags.
     * {@hide}
     */
    static const int ENABLED = 0x00000000;
    
    /**
     * This view is disabled. Interpretation varies by subclass.
     * Use with ENABLED_MASK when calling setFlags.
     * {@hide}
     */
    static const int DISABLED = 0x00000020;
    
    /**
     * Mask for use with setFlags indicating bits used for indicating whether
     * this view is enabled
     * {@hide}
     */
    static const int ENABLED_MASK = 0x00000020;
    
    /**
     * This view won't draw. {@link #onDraw(android.graphics.Canvas)} won't be
     * called and further optimizations will be performed. It is okay to have
     * this flag set and a background. Use with DRAW_MASK when calling setFlags.
     * {@hide}
     */
    static const int WILL_NOT_DRAW = 0x00000080;
    
    /**
     * Mask for use with setFlags indicating bits used for indicating whether
     * this view is will draw
     * {@hide}
     */
    static const int DRAW_MASK = 0x00000080;
    
    /* Masks for mPrivateFlags3 */
    
    /**
     * Flag indicating that view has a transform animation set on it. This is used to track whether
     * an animation is cleared between successive frames, in order to tell the associated
     * DisplayList to clear its animation matrix.
     */
    static const int PFLAG3_VIEW_IS_ANIMATING_TRANSFORM = 0x1;
    
    /**
     * Flag indicating that view has an alpha animation set on it. This is used to track whether an
     * animation is cleared between successive frames, in order to tell the associated
     * DisplayList to restore its alpha value.
     */
    static const int PFLAG3_VIEW_IS_ANIMATING_ALPHA = 0x2;
    
    /**
     * Flag indicating that the view has been through at least one layout since it
     * was last attached to a window.
     */
    static const int PFLAG3_IS_LAID_OUT = 0x4;
    
    /**
     * Flag indicating that a call to measure() was skipped and should be done
     * instead when layout() is invoked.
     */
    static const int PFLAG3_MEASURE_NEEDED_BEFORE_LAYOUT = 0x8;
    
    /**
     * Flag indicating that an overridden method correctly  called down to
     * the superclass implementation as required by the API spec.
     */
    static const int PFLAG3_CALLED_SUPER = 0x10;
    
    /**
     * Convenience value to check for float values that are close enough to zero to be considered
     * zero.
     */
    static const float NONZERO_EPSILON;
    
    /**
     * Default text direction is inherited
     */
    static const int TEXT_DIRECTION_DEFAULT = TEXT_DIRECTION_INHERIT;
    
    /**
     * Default resolved text direction
     * @hide
     */
    static const int TEXT_DIRECTION_RESOLVED_DEFAULT = TEXT_DIRECTION_FIRST_STRONG;
    
    /**
     * Bit shift to get the horizontal layout direction. (bits after LAYOUT_DIRECTION_RESOLVED)
     * @hide
     */
    static const int PFLAG2_TEXT_DIRECTION_MASK_SHIFT = 6;
    
    /**
     * Mask for use with private flags indicating bits used for text direction.
     * @hide
     */
    static const int PFLAG2_TEXT_DIRECTION_MASK = 0x00000007
    << PFLAG2_TEXT_DIRECTION_MASK_SHIFT;
    
    /**
     * Array of text direction flags for mapping attribute "textDirection" to correct
     * flag value.
     * @hide
     */
    static const vector<int> PFLAG2_TEXT_DIRECTION_FLAGS;
    
    /**
     * Indicates whether the view text direction has been resolved.
     * @hide
     */
    static const int PFLAG2_TEXT_DIRECTION_RESOLVED = 0x00000008
    << PFLAG2_TEXT_DIRECTION_MASK_SHIFT;
    
    /**
     * Bit shift to get the horizontal layout direction. (bits after DRAG_HOVERED)
     * @hide
     */
    static const int PFLAG2_TEXT_DIRECTION_RESOLVED_MASK_SHIFT = 10;
    
    /**
     * Mask for use with private flags indicating bits used for resolved text direction.
     * @hide
     */
    static const int PFLAG2_TEXT_DIRECTION_RESOLVED_MASK = 0x00000007
    << PFLAG2_TEXT_DIRECTION_RESOLVED_MASK_SHIFT;
    
    /**
     * Indicates whether the view text direction has been resolved to the "first strong" heuristic.
     * @hide
     */
    static const int PFLAG2_TEXT_DIRECTION_RESOLVED_DEFAULT =
    TEXT_DIRECTION_RESOLVED_DEFAULT << PFLAG2_TEXT_DIRECTION_RESOLVED_MASK_SHIFT;
    
    /**
     * Default text alignment is inherited
     */
    static const int TEXT_ALIGNMENT_DEFAULT = TEXT_ALIGNMENT_GRAVITY;
    
    /**
     * Default resolved text alignment
     * @hide
     */
    static const int TEXT_ALIGNMENT_RESOLVED_DEFAULT = TEXT_ALIGNMENT_GRAVITY;
    
    /**
     * Bit shift to get the horizontal layout direction. (bits after DRAG_HOVERED)
     * @hide
     */
    static const int PFLAG2_TEXT_ALIGNMENT_MASK_SHIFT = 13;
    
    /**
     * Mask for use with private flags indicating bits used for text alignment.
     * @hide
     */
    static const int PFLAG2_TEXT_ALIGNMENT_MASK = 0x00000007 << PFLAG2_TEXT_ALIGNMENT_MASK_SHIFT;
    
    /**
     * Array of text direction flags for mapping attribute "textAlignment" to correct
     * flag value.
     * @hide
     */
    static const vector<int> PFLAG2_TEXT_ALIGNMENT_FLAGS;
    
    /**
     * Indicates whether the view text alignment has been resolved.
     * @hide
     */
    static const int PFLAG2_TEXT_ALIGNMENT_RESOLVED = 0x00000008 << PFLAG2_TEXT_ALIGNMENT_MASK_SHIFT;
    
    /**
     * Bit shift to get the resolved text alignment.
     * @hide
     */
    static const int PFLAG2_TEXT_ALIGNMENT_RESOLVED_MASK_SHIFT = 17;
    
    /**
     * Mask for use with private flags indicating bits used for text alignment.
     * @hide
     */
    static const int PFLAG2_TEXT_ALIGNMENT_RESOLVED_MASK = 0x00000007
    << PFLAG2_TEXT_ALIGNMENT_RESOLVED_MASK_SHIFT;
    
    /**
     * Indicates whether if the view text alignment has been resolved to gravity
     */
    static const int PFLAG2_TEXT_ALIGNMENT_RESOLVED_DEFAULT =
    TEXT_ALIGNMENT_RESOLVED_DEFAULT << PFLAG2_TEXT_ALIGNMENT_RESOLVED_MASK_SHIFT;
    
    /**
     * Group of bits indicating that RTL properties resolution is done.
     */
    static const int ALL_RTL_PROPERTIES_RESOLVED = PFLAG2_LAYOUT_DIRECTION_RESOLVED |
    PFLAG2_TEXT_DIRECTION_RESOLVED |
    PFLAG2_TEXT_ALIGNMENT_RESOLVED |
    PFLAG2_PADDING_RESOLVED |
    PFLAG2_DRAWABLE_RESOLVED;
    
    /**
     * <p>Indicates that no drawing cache should ever be created for this view.<p>
     * {@hide}
     */
    static const int WILL_NOT_CACHE_DRAWING = 0x000020000;
    
    
    /* End of masks for mPrivateFlags3 */
    
    struct StaticBlock {
        StaticBlock() {
            
            R::styleable::ViewDrawableStates = vector<int>({
                R::attr::state_window_focused,
                R::attr::state_selected,
                R::attr::state_focused,
                R::attr::state_enabled,
                R::attr::state_pressed,
                R::attr::state_activated,
                R::attr::state_accelerated,
                R::attr::state_hovered,
                R::attr::state_drag_can_accept,
                R::attr::state_drag_hovered,
                R::attr::state_checked
            });
            
            vector<int> orderedIds(VIEW_STATE_IDS.size());
            int size = R::styleable::ViewDrawableStates.size();
            for (int i = 0; i < size; i++) {
                int viewState = R::styleable::ViewDrawableStates[i];
                for (int j = 0; j < VIEW_STATE_IDS.size(); j += 2) {
                    if (VIEW_STATE_IDS[j] == viewState) {
                        orderedIds[i * 2] = viewState;
                        orderedIds[i * 2 + 1] = VIEW_STATE_IDS[j + 1];
                    }
                }
            }
            const int NUM_BITS = VIEW_STATE_IDS.size() / 2;
            VIEW_STATE_SETS = vector<vector<int>>(1 << NUM_BITS);
            for (int i = 0; i < VIEW_STATE_SETS.size(); i++) {
                int numBits = bitCount(i);
                vector<int> set(numBits);
                int pos = 0;
                for (int j = 0; j < orderedIds.size(); j += 2) {
                    if ((i & orderedIds[j+1]) != 0) {
                        set[pos++] = orderedIds[j];
                    }
                }
                VIEW_STATE_SETS[i] = set;
            }
            
            ENABLED_STATE_SET = VIEW_STATE_SETS[VIEW_STATE_ENABLED];
        }
        
        int bitCount(int i) {
            // HD, Figure 5-2
            i = i - ((i >> 1) & 0x55555555);
            i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
            i = (i + (i >> 4)) & 0x0f0f0f0f;
            i = i + (i >> 8);
            i = i + (i >> 16);
            return i & 0x3f;
        }
    };
    
    static StaticBlock staticBlock;
    
    static vector<vector<int>> VIEW_STATE_SETS;
    
protected:
    
    static vector<int> ENABLED_STATE_SET;
    
private:
    
    class TransformationInfo {
        
    public:
        
        TransformationInfo() {}
        
        /**
         * The transform matrix for the View. This transform is calculated internally
         * based on the rotation, scaleX, and scaleY properties. The identity matrix
         * is used by default. Do *not* use this variable directly; instead call
         * getMatrix(), which will automatically recalculate the matrix if necessary
         * to get the correct matrix based on the latest rotation and scale properties.
         */
        Matrix mMatrix = Matrix();
        
        /**
         * The transform matrix for the View. This transform is calculated internally
         * based on the rotation, scaleX, and scaleY properties. The identity matrix
         * is used by default. Do *not* use this variable directly; instead call
         * getInverseMatrix(), which will automatically recalculate the matrix if necessary
         * to get the correct matrix based on the latest rotation and scale properties.
         */
        Matrix mInverseMatrix;
        
        /**
         * An internal variable that tracks whether we need to recalculate the
         * transform matrix, based on whether the rotation or scaleX/Y properties
         * have changed since the matrix was last calculated.
         */
        bool mMatrixDirty = false;
        
        /**
         * An internal variable that tracks whether we need to recalculate the
         * transform matrix, based on whether the rotation or scaleX/Y properties
         * have changed since the matrix was last calculated.
         */
        bool mInverseMatrixDirty = true;
        
        /**
         * A variable that tracks whether we need to recalculate the
         * transform matrix, based on whether the rotation or scaleX/Y properties
         * have changed since the matrix was last calculated. This variable
         * is only valid after a call to updateMatrix() or to a function that
         * calls it such as getMatrix(), hasIdentityMatrix() and getInverseMatrix().
         */
        bool mMatrixIsIdentity = true;
        
        /**
         * The Camera object is used to compute a 3D matrix when rotationX or rotationY are set.
         */
        Camera *mCamera = NULL;
        
        /**
         * This matrix is used when computing the matrix for 3D rotations.
         */
        Matrix *matrix3D = NULL;
        
        /**
         * These prev values are used to recalculate a centered pivot point when necessary. The
         * pivot point is only used in matrix operations (when rotation, scale, or translation are
         * set), so thes values are only used then as well.
         */
        int mPrevWidth = -1;
        int mPrevHeight = -1;
        
        /**
         * The degrees rotation around the vertical axis through the pivot point.
         */
        
        float mRotationY = 0.0f;
        
        /**
         * The degrees rotation around the horizontal axis through the pivot point.
         */
        
        float mRotationX = 0.0f;
        
        /**
         * The degrees rotation around the pivot point.
         */
        
        float mRotation = 0.0f;
        
        /**
         * The amount of translation of the object away from its left property (post-layout).
         */
        
        float mTranslationX = 0.0f;
        
        /**
         * The amount of translation of the object away from its top property (post-layout).
         */
        
        float mTranslationY = 0.0f;
        
        /**
         * The amount of scale in the x direction around the pivot point. A
         * value of 1 means no scaling is applied.
         */
        
        float mScaleX = 1.0f;
        
        /**
         * The amount of scale in the y direction around the pivot point. A
         * value of 1 means no scaling is applied.
         */
        
        float mScaleY = 1.0f;
        
        /**
         * The x location of the point around which the view is rotated and scaled.
         */
        
        float mPivotX = 0.0f;
        
        /**
         * The y location of the point around which the view is rotated and scaled.
         */
        
        float mPivotY = 0.0f;
        
        /**
         * The opacity of the View. This is a value from 0 to 1, where 0 means
         * completely transparent and 1 means completely opaque.
         */
        
        float mAlpha = 1.0f;
        
        inline Matrix get3DMatrix() {
            if (matrix3D) {
                return *matrix3D;
            }
            
            return Matrix();
        }
    };
    
    TransformationInfo mTransformationInfo;
    
    class PerformClick : public Runnable {
    public:
        
        PerformClick(shared_ptr<View> view) { mView = view; }
        
        virtual void run() {
            mView->performClick();
        }
    private:
        shared_ptr<View> mView;
    };
    
    class CheckForTap : public Runnable {
    public:
        
        CheckForTap(shared_ptr<View> view) { mView = view; }
        virtual void run() {
            mView->mPrivateFlags &= ~View::PFLAG_PREPRESSED;
            mView->setPressed(true);
            mView->checkForLongClick(View::TAP_TIMEOUT);
        }
        
    private:
        
        shared_ptr<View> mView;
    };
    
    class CheckForLongPress : public Runnable {
        
    public:
        
        CheckForLongPress(shared_ptr<View> view) { mView = view; }
        
        virtual void run() {
            if (mView->isPressed() && (mView->mParent)
                && m_originalWindowAttachCount == mView->mWindowAttachCount) {
                if (mView->performLongClick()) {
                    mView->mHasPerformedLongPress = true;
                }
            }
        }
        
        void rememberWindowAttachCount() {
            m_originalWindowAttachCount = mView->mWindowAttachCount;
        }
        
    private:
        
        shared_ptr<View> mView;
        int m_originalWindowAttachCount = 0;
    };
    
    class UnsetPressedState : public Runnable {
    public:
        
        UnsetPressedState(shared_ptr<View> view) { mView = view; }
        virtual void run() {
            mView->setPressed(false);
        }
        
    private:
        
        shared_ptr<View> mView;
    };
    
    shared_ptr<Drawable> m_background;
    string mBackgroundResource = "";
    bool m_backgroundSizeChanged = false;
    Context *mContext = NULL;
    Resources *mResources = NULL;
    GLES20DisplayList *mDisplayList = NULL;
    vector<int> m_drawableState;
    
    /**
     * Solid color to use as a background when creating the drawing cache. Enables
     * the cache to use 16 bit bitmaps instead of 32 bit.
     */
    int mDrawingCacheBackgroundColor = 0;
    bool mHasPerformedLongPress = false;
    bool m_lastIsOpaque = false;
    LayoutParams *mLayoutParams = NULL;
    Rect mLocalDirtyRect;
    map<uint64_t, uint64_t> m_measureCache;
    int m_minHeight = 0;
    int mMinimumFlingVelocity = 0;
    int mMaximumFlingVelocity = 0;
    sp<CheckForLongPress> m_pendingCheckForLongPress = NULL;
    int m_minWidth = 0;
    function<void (View&)> m_onClickListener;
    function<void (View&)> m_onLongClickListener;
    Rect mPadding;
    sp<CheckForTap> m_pendingCheckForTap = NULL;
    sp<PerformClick> m_performClick = NULL;
    bool m_recreateDisplayList = false;
    int mTouchSlop = 0;
    sp<UnsetPressedState> m_unsetPressedState = NULL;
    int m_userPaddingStart = UNDEFINED_PADDING;
    int m_userPaddingEnd = UNDEFINED_PADDING;
    int m_userPaddingLeftInitial = 0;
    int m_userPaddingRightInitial = 0;
    shared_ptr<ViewParent> mParent;
    
    const static vector<int> VIEW_STATE_IDS;
    
    /**
     * Count of how many windows this view has been attached to.
     */
    int mWindowAttachCount = 0;
    
    void assignParent(shared_ptr<ViewParent> parent);
    void checkForLongClick(int delayOffset);
    void clearDisplayList();
    virtual void dispatchAttachedToWindow(AttachInfo *info, int visibility);
    void dispatchDetachedFromWindow();
    bool draw(shared_ptr<Canvas> canvas, ViewGroup *parent, uint64_t drawingTime);
    bool hasIdentityMatrix();
    
    /**
     * <p>
     * Initalizes the scrollability cache if necessary.
     * </p>
     */
    void initScrollCache();
    
    /**
     * Called when any RTL property (layout direction or text direction or text alignment) has
     * been changed.
     *
     * Subclasses need to override this method to take care of cached information that depends on the
     * resolved layout direction, or to inform child views that inherit their layout direction.
     *
     * The default implementation does nothing.
     *
     * @param layoutDirection the direction of the layout
     *
     * @see #LAYOUT_DIRECTION_LTR
     * @see #LAYOUT_DIRECTION_RTL
     */
    void onRtlPropertiesChanged(int layoutDirection) {
    }
    
    /**
     * Resolve and cache the layout direction. LTR is set initially. This is implicitly supposing
     * that the parent directionality can and will be resolved before its children.
     *
     * @return true if resolution has been done, false otherwise.
     *
     * @hide
     */
    bool resolveLayoutDirection();
    
    /**
     * @return true if layout direction has been resolved.
     * @hide
     */
    bool isLayoutDirectionResolved() {
        return (mPrivateFlags2 & PFLAG2_LAYOUT_DIRECTION_RESOLVED) == PFLAG2_LAYOUT_DIRECTION_RESOLVED;
    }
    
    /**
     * Return if padding has been resolved
     *
     * @hide
     */
    bool isPaddingResolved() {
        return (mPrivateFlags2 & PFLAG2_PADDING_RESOLVED) == PFLAG2_PADDING_RESOLVED;
    }
    
    /**
     * @return true if RTL properties need resolution.
     *
     */
    bool needRtlPropertiesResolution() {
        return (mPrivateFlags2 & ALL_RTL_PROPERTIES_RESOLVED) != ALL_RTL_PROPERTIES_RESOLVED;
    }
    
    void removeLongPressCallback();
    void removeTapCallback();

    /**
     * Remove the prepress detection timer.
     */
    void removeUnsetPressCallback();
    void resetPressedState();
    void sizeChange(int newWidth, int newHeight, int oldWidth, int oldHeight);
    void setDisplayListProperties(GLES20DisplayList *displayList);
    bool skipInvalidate();
    void updateMatrix();
    
    static ViewType getViewType(const char *name);
    static bool nonzero(float value);
    
protected:
    
    /**
     * <p>Indicates this view can take / keep focus when int touch mode.</p>
     * {@hide}
     */
    static const int FOCUSABLE_IN_TOUCH_MODE = 0x00040000;
    
    AttachInfo *mAttachInfo = NULL;
    int mBottom = 0;
    Animation *m_currentAnimation = NULL;
    
    /**
     * Special tree observer used when mAttachInfo is null.
     */
    ViewTreeObserver *mFloatingTreeObserver = NULL;
    string m_id = NO_ID;
    int mLeft = 0;
    int m_measuredHeight = 0;
    int m_measuredWidth = 0;
    int m_oldHeightMeasureSpec = INT_MIN;
    int m_oldWidthMeasureSpec = INT_MIN;
    
    int m_layerType = LAYER_TYPE_NONE;
    
    /**
     * The left padding in pixels, that is the distance in pixels between the
     * left edge of this view and the left edge of its content.
     * {@hide}
     */
    int mPaddingLeft = 0;
    /**
     * The right padding in pixels, that is the distance in pixels between the
     * right edge of this view and the right edge of its content.
     * {@hide}
     */
    int mPaddingRight = 0;
    /**
     * The top padding in pixels, that is the distance in pixels between the
     * top edge of this view and the top edge of its content.
     * {@hide}
     */
    int mPaddingTop = 0;
    /**
     * The bottom padding in pixels, that is the distance in pixels between the
     * bottom edge of this view and the bottom edge of its content.
     * {@hide}
     */
    int mPaddingBottom = 0;
    int mPrivateFlags = 0;
    int mPrivateFlags2 = 0;
    int mPrivateFlags3 = 0;
    int mRight = 0;
    int mTop = 0;
    
    /**
     * The view's tag.
     * {@hide}
     *
     * @see #setTag(Object)
     * @see #getTag()
     */
    shared_ptr<Object> mTag;
    
    /**
     * The offset, in pixels, by which the content of this view is scrolled
     * horizontally.
     * {@hide}
     */
    int mScrollX = 0;
    /**
     * The offset, in pixels, by which the content of this view is scrolled
     * vertically.
     * {@hide}
     */
    int mScrollY = 0;
    int m_userPaddingRight = 0;
    int m_userPaddingBottom = 0;
    int m_userPaddingLeft = 0;
    int mViewFlags = 0;
    
    shared_ptr<View> findSharedViewById(string id);
    virtual shared_ptr<View> findViewTraversal(string id);
    virtual shared_ptr<View> findViewWithTagTraversal(shared_ptr<Object> tag);
    void computeOpaqueFlags();
    
    /**
     * <p>Compute the horizontal range that the horizontal scrollbar
     * represents.</p>
     *
     * <p>The range is expressed in arbitrary units that must be the same as the
     * units used by {@link #computeHorizontalScrollExtent()} and
     * {@link #computeHorizontalScrollOffset()}.</p>
     *
     * <p>The default range is the drawing width of this view.</p>
     *
     * @return the total horizontal range represented by the horizontal
     *         scrollbar
     *
     * @see #computeHorizontalScrollExtent()
     * @see #computeHorizontalScrollOffset()
     * @see android.widget.ScrollBarDrawable
     */
    virtual int computeHorizontalScrollRange() { return getWidth(); }
    
    /**
     * <p>Compute the horizontal offset of the horizontal scrollbar's thumb
     * within the horizontal range. This value is used to compute the position
     * of the thumb within the scrollbar's track.</p>
     *
     * <p>The range is expressed in arbitrary units that must be the same as the
     * units used by {@link #computeHorizontalScrollRange()} and
     * {@link #computeHorizontalScrollExtent()}.</p>
     *
     * <p>The default offset is the scroll offset of this view.</p>
     *
     * @return the horizontal offset of the scrollbar's thumb
     *
     * @see #computeHorizontalScrollRange()
     * @see #computeHorizontalScrollExtent()
     * @see android.widget.ScrollBarDrawable
     */
    virtual int computeHorizontalScrollOffset() { return mScrollX; }
    
    /**
     * <p>Compute the horizontal extent of the horizontal scrollbar's thumb
     * within the horizontal range. This value is used to compute the length
     * of the thumb within the scrollbar's track.</p>
     *
     * <p>The range is expressed in arbitrary units that must be the same as the
     * units used by {@link #computeHorizontalScrollRange()} and
     * {@link #computeHorizontalScrollOffset()}.</p>
     *
     * <p>The default extent is the drawing width of this view.</p>
     *
     * @return the horizontal extent of the scrollbar's thumb
     *
     * @see #computeHorizontalScrollRange()
     * @see #computeHorizontalScrollOffset()
     * @see android.widget.ScrollBarDrawable
     */
    virtual int computeHorizontalScrollExtent() { return getWidth(); }
    
    /**
     * <p>Compute the vertical range that the vertical scrollbar represents.</p>
     *
     * <p>The range is expressed in arbitrary units that must be the same as the
     * units used by {@link #computeVerticalScrollExtent()} and
     * {@link #computeVerticalScrollOffset()}.</p>
     *
     * @return the total vertical range represented by the vertical scrollbar
     *
     * <p>The default range is the drawing height of this view.</p>
     *
     * @see #computeVerticalScrollExtent()
     * @see #computeVerticalScrollOffset()
     * @see android.widget.ScrollBarDrawable
     */
    virtual int computeVerticalScrollRange() { return getHeight(); }
    
    /**
     * <p>Compute the vertical offset of the vertical scrollbar's thumb
     * within the horizontal range. This value is used to compute the position
     * of the thumb within the scrollbar's track.</p>
     *
     * <p>The range is expressed in arbitrary units that must be the same as the
     * units used by {@link #computeVerticalScrollRange()} and
     * {@link #computeVerticalScrollExtent()}.</p>
     *
     * <p>The default offset is the scroll offset of this view.</p>
     *
     * @return the vertical offset of the scrollbar's thumb
     *
     * @see #computeVerticalScrollRange()
     * @see #computeVerticalScrollExtent()
     * @see android.widget.ScrollBarDrawable
     */
    virtual int computeVerticalScrollOffset() { return mScrollY; }
    
    /**
     * <p>Compute the vertical extent of the horizontal scrollbar's thumb
     * within the vertical range. This value is used to compute the length
     * of the thumb within the scrollbar's track.</p>
     *
     * <p>The range is expressed in arbitrary units that must be the same as the
     * units used by {@link #computeVerticalScrollRange()} and
     * {@link #computeVerticalScrollOffset()}.</p>
     *
     * <p>The default extent is the drawing height of this view.</p>
     *
     * @return the vertical extent of the scrollbar's thumb
     *
     * @see #computeVerticalScrollRange()
     * @see #computeVerticalScrollOffset()
     * @see android.widget.ScrollBarDrawable
     */
    virtual int computeVerticalScrollExtent() { return getHeight(); }
    virtual void dispatchGetDisplayList() {}
    virtual void dispatchDraw(shared_ptr<Canvas> canvas) {}
    virtual void dispatchSetPressed(bool pressed) {}
    
    /**
     * Dispatch setSelected to all of this View's children.
     *
     * @see #setSelected(boolean)
     *
     * @param selected The new selected state
     */
    virtual void dispatchSetSelected(bool selected) { }
    virtual void drawableStateChanged();
    int getSuggestedMinimumHeight();
    int getSuggestedMinimumWidth();
    
    /**
     * Returns the strength, or intensity, of the top faded edge. The strength is
     * a value between 0.0 (no fade) and 1.0 (full fade). The default implementation
     * returns 0.0 or 1.0 but no value in between.
     *
     * Subclasses should override this method to provide a smoother fade transition
     * when scrolling occurs.
     *
     * @return the intensity of the top fade as a float between 0.0f and 1.0f
     */
    virtual float getTopFadingEdgeStrength() { return computeVerticalScrollOffset() > 0 ? 1.0f : 0.0f; }
    
    /**
     * Returns the strength, or intensity, of the bottom faded edge. The strength is
     * a value between 0.0 (no fade) and 1.0 (full fade). The default implementation
     * returns 0.0 or 1.0 but no value in between.
     *
     * Subclasses should override this method to provide a smoother fade transition
     * when scrolling occurs.
     *
     * @return the intensity of the bottom fade as a float between 0.0f and 1.0f
     */
    virtual float getBottomFadingEdgeStrength() {
        return computeVerticalScrollOffset() + computeVerticalScrollExtent() <
        computeVerticalScrollRange() ? 1.0f : 0.0f;
    }
    
    /**
     * Returns the strength, or intensity, of the left faded edge. The strength is
     * a value between 0.0 (no fade) and 1.0 (full fade). The default implementation
     * returns 0.0 or 1.0 but no value in between.
     *
     * Subclasses should override this method to provide a smoother fade transition
     * when scrolling occurs.
     *
     * @return the intensity of the left fade as a float between 0.0f and 1.0f
     */
    virtual float getLeftFadingEdgeStrength() {
        return computeHorizontalScrollOffset() > 0 ? 1.0f : 0.0f;
    }
    
    /**
     * Returns the strength, or intensity, of the right faded edge. The strength is
     * a value between 0.0 (no fade) and 1.0 (full fade). The default implementation
     * returns 0.0 or 1.0 but no value in between.
     *
     * Subclasses should override this method to provide a smoother fade transition
     * when scrolling occurs.
     *
     * @return the intensity of the right fade as a float between 0.0f and 1.0f
     */
    virtual float getRightFadingEdgeStrength() {
        return computeHorizontalScrollOffset() + computeHorizontalScrollExtent() <
        computeHorizontalScrollRange() ? 1.0f : 0.0f;
    }
    
    /**
     * @return The number of times this view has been attached to a window
     */
    int getWindowAttachCount() {
        return mWindowAttachCount;
    }
    
    /**
     * Give this view focus. This will cause
     * {@link #onFocusChanged(boolean, int, android.graphics.Rect)} to be called.
     *
     * Note: this does not check whether this {@link View} should get focus, it just
     * gives it focus no matter what.  It should only be called internally by framework
     * code that knows what it is doing, namely {@link #requestFocus(int, Rect)}.
     *
     * @param direction values are {@link View#FOCUS_UP}, {@link View#FOCUS_DOWN},
     *        {@link View#FOCUS_LEFT} or {@link View#FOCUS_RIGHT}. This is the direction which
     *        focus moved when requestFocus() is called. It may not always
     *        apply, in which case use the default View.FOCUS_DOWN.
     * @param previouslyFocusedRect The rectangle of the view that had focus
     *        prior in this View's coordinate system.
     */
    void handleFocusGainInternal(int direction, Rect *previouslyFocusedRect);
    
    /**
     * Indicates whether this view has a static layer. A view with layer type
     * {@link #LAYER_TYPE_NONE} is a static layer. Other types of layers are
     * dynamic.
     */
    bool hasStaticLayer() { return true; }
    void internalSetPadding(int left, int top, int right, int bottom);
    void invalidateParentCaches();
    
    /**
     * Used to indicate that the parent of this view should be invalidated. This functionality
     * is used to force the parent to rebuild its display list (when hardware-accelerated),
     * which is necessary when various parent-managed properties of the view change, such as
     * alpha, translationX/Y, scrollX/Y, scaleX/Y, and rotation/X/Y. This method will propagate
     * an invalidation event to the parent.
     *
     * @hide
     */
    void invalidateParentIfNeeded() {
        shared_ptr<View> v = dynamic_pointer_cast<View>(mParent);
        if (v != NULL) {
            v->invalidate(true);
        }
    }
    
    /**
     * Quick invalidation for View property changes (alpha, translationXY, etc.). We don't want to
     * set any flags or handle all of the cases handled by the default invalidation methods.
     * Instead, we just want to schedule a traversal in ViewRootImpl with the appropriate
     * dirty rect. This method calls into fast invalidation methods in ViewGroup that
     * walk up the hierarchy, transforming the dirty rect as necessary.
     *
     * The method also handles normal invalidation logic if display list properties are not
     * being used in this view. The invalidateParent and forceRedraw flags are used by that
     * backup approach, to handle these cases used in the various property-setting methods.
     *
     * @param invalidateParent Force a call to invalidateParentCaches() if display list properties
     * are not being used in this view
     * @param forceRedraw Mark the view as DRAWN to force the invalidation to propagate, if display
     * list properties are not being used in this view
     */
    void invalidateViewProperty(bool invalidateParent, bool forceRedraw);
    
    /**
     * If the View draws content inside its padding and enables fading edges,
     * it needs to support padding offsets. Padding offsets are added to the
     * fading edges to extend the length of the fade so that it covers pixels
     * drawn inside the padding.
     *
     * Subclasses of this class should override this method if they need
     * to draw content inside the padding.
     *
     * @return True if padding offset must be applied, false otherwise.
     *
     * @see #getLeftPaddingOffset()
     * @see #getRightPaddingOffset()
     * @see #getTopPaddingOffset()
     * @see #getBottomPaddingOffset()
     *
     * @since CURRENT
     */
    virtual bool isPaddingOffsetRequired() { return false; }

    /**
     * Override this if the vertical scrollbar needs to be hidden in a subclass, like when
     * FastScroller is visible.
     * @return whether to temporarily hide the vertical scrollbar
     * @hide
     */
    virtual bool isVerticalScrollBarHidden() { return false; }
    
    /**
     * @hide
     * @param offsetRequired
     */
    virtual int getFadeTop(bool offsetRequired);
    
    /**
     * @hide
     * @param offsetRequired
     */
    virtual int getFadeHeight(bool offsetRequired);
    
    /**
     * Amount by which to extend the left fading region. Called only when
     * {@link #isPaddingOffsetRequired()} returns true.
     *
     * @return The left padding offset in pixels.
     *
     * @see #isPaddingOffsetRequired()
     *
     * @since CURRENT
     */
    virtual int getLeftPaddingOffset() { return 0; }
    
    /**
     * Amount by which to extend the right fading region. Called only when
     * {@link #isPaddingOffsetRequired()} returns true.
     *
     * @return The right padding offset in pixels.
     *
     * @see #isPaddingOffsetRequired()
     *
     * @since CURRENT
     */
    virtual int getRightPaddingOffset() { return 0; }
    
    /**
     * Amount by which to extend the top fading region. Called only when
     * {@link #isPaddingOffsetRequired()} returns true.
     *
     * @return The top padding offset in pixels.
     *
     * @see #isPaddingOffsetRequired()
     *
     * @since CURRENT
     */
    virtual int getTopPaddingOffset() { return 0; }
    
    /**
     * Amount by which to extend the bottom fading region. Called only when
     * {@link #isPaddingOffsetRequired()} returns true.
     *
     * @return The bottom padding offset in pixels.
     *
     * @see #isPaddingOffsetRequired()
     *
     * @since CURRENT
     */
    virtual int getBottomPaddingOffset() { return 0; }
    
    /**
     * Return true if the application tag in the AndroidManifest has set "supportRtl" to true
     */
    bool hasRtlSupport() {
        return false;
    }

    /**
     * <p>
     * Initializes the scrollbars from a given set of styled attributes. This
     * method should be called by subclasses that need scrollbars and when an
     * instance of these subclasses is created programmatically rather than
     * being inflated from XML. This method is automatically called when the XML
     * is inflated.
     * </p>
     *
     * @param a the styled attributes set to initialize the scrollbars from
     */
    void initializeScrollbars(AttributeSet *attrs);
    
    /**
     * Merge your own state values in <var>additionalState</var> into the base
     * state values <var>baseState</var> that were returned by
     * {@link #onCreateDrawableState(int)}.
     *
     * @param baseState The base state values returned by
     * {@link #onCreateDrawableState(int)}, which will be modified to also hold your
     * own additional state values.
     *
     * @param additionalState The additional state values you would like
     * added to <var>baseState</var>; this array is not modified.
     *
     * @return As a convenience, the <var>baseState</var> array you originally
     * passed into the function is returned.
     *
     * @see #onCreateDrawableState(int)
     */
    static vector<int> mergeDrawableStates(vector<int> baseState, vector<int> additionalState);
    virtual void onAttachedToWindow();
    vector<int> onCreateDrawableState(int extraSpace);
    virtual void onDetachedFromWindow();
    virtual void onDraw(shared_ptr<Canvas> canvas);
    
    /**
     * Finalize inflating a view from XML.  This is called as the last phase
     * of inflation, after all child views have been added.
     *
     * <p>Even if the subclass overrides onFinishInflate, they should always be
     * sure to call the super method, so that we get called.
     */
    virtual void onFinishInflate() {}
    
    /**
     * Called by the view system when the focus state of this view changes.
     * When the focus change event is caused by directional navigation, direction
     * and previouslyFocusedRect provide insight into where the focus is coming from.
     * When overriding, be sure to call up through to the super class so that
     * the standard focus handling will occur.
     *
     * @param gainFocus True if the View has focus; false otherwise.
     * @param direction The direction focus has moved when requestFocus()
     *                  is called to give this view focus. Values are
     *                  {@link #FOCUS_UP}, {@link #FOCUS_DOWN}, {@link #FOCUS_LEFT},
     *                  {@link #FOCUS_RIGHT}, {@link #FOCUS_FORWARD}, or {@link #FOCUS_BACKWARD}.
     *                  It may not always apply, in which case use the default.
     * @param previouslyFocusedRect The rectangle, in this view's coordinate
     *        system, of the previously focused view.  If applicable, this will be
     *        passed in as finer grained information about where the focus is coming
     *        from (in addition to direction).  Will be <code>null</code> otherwise.
     */
    virtual void onFocusChanged(bool gainFocus, int direction, Rect *previouslyFocusedRect);
    virtual void onLayout(bool changed, int left, int top, int right, int bottom);
    virtual void onMeasure(int widthMeasureSpec, int heightMeasureSpec);
    
    /**
     * This is called in response to an internal scroll in this view (i.e., the
     * view scrolled its own contents). This is typically as a result of
     * {@link #scrollBy(int, int)} or {@link #scrollTo(int, int)} having been
     * called.
     *
     * @param l Current horizontal scroll origin.
     * @param t Current vertical scroll origin.
     * @param oldl Previous horizontal scroll origin.
     * @param oldt Previous vertical scroll origin.
     */
    virtual void onScrollChanged(int l, int t, int oldl, int oldt);
    virtual void onSizeChanged(int w, int h, int oldw, int oldh) {}
    virtual void onWindowVisibilityChanged(int visibility);
    void resetResolvedDrawables() { mPrivateFlags2 &= ~PFLAG2_DRAWABLE_RESOLVED; }
    
    bool isDrawablesResolved() {
        return (mPrivateFlags2 & PFLAG2_DRAWABLE_RESOLVED) == PFLAG2_DRAWABLE_RESOLVED;
    }
    
    /**
     * Resolve the Drawables depending on the layout direction. This is implicitly supposing
     * that the View directionality can and will be resolved before its Drawables.
     *
     * Will call {@link View#onResolveDrawables} when resolution is done.
     *
     * @hide
     */
    virtual void resolveDrawables();
    bool rootViewRequestFocus();
    void setFlags(int flags, int mask);
    virtual bool setFrame(int left, int top, int right, int bottom);
    void setMeasuredDimension(int measuredWidth, int measuredHeight);
    void notifyGlobalFocusCleared(shared_ptr<View> oldFocus);
    /**
     * Utility method to transform a given Rect by the current matrix of this view.
     */
    void transformRect(Rect &rect);
    virtual bool verifyDrawable(shared_ptr<Drawable> who) { return who == m_background; }
    
    /**
     * Called internally by the view system when a new view is getting focus.
     * This is what clears the old focus.
     */
    void unFocus();
};

ANDROID_END

#endif	/* VIEW_H */

