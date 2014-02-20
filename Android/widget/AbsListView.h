//
//  AbsListView.h
//  Androidpp
//
//  Created by Saul Howard on 2/7/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef __Androidpp__AbsListView__
#define __Androidpp__AbsListView__

#include "AndroidMacros.h"

#include "Android/graphics/Rect.h"
#include "Android/graphics/drawable/Drawable.h"
#include "Android/text/TextWatcher.h"
#include "Android/view/LayoutParams.h"
#include "Android/view/ViewTreeObserver.h"
#include "Android/widget/AdapterView.h"
#include "Android/widget/Filter.h"
#include "Android/widget/ListAdapter.h"

#include <mindroid/os/Runnable.h>
#include <mindroid/os/Ref.h>

#include <memory>
#include <vector>

using namespace mindroid;
using namespace std;

ANDROID_BEGIN

class ContextMenuInfo;
class EditText;
class EditorInfo;
class FastScroller;
class InputConnection;
class InputConnectionWrapper;
class PopupWindow;
class Scroller;
class VelocityTracker;

/**
 * Base class that can be used to implement virtualized lists of items. A list does
 * not have a spatial definition here. For instance, subclases of this class can
 * display the content of the list in a grid, in a carousel, as stack, etc.
 *
 * @attr ref android.R.styleable#AbsListView_listSelector
 * @attr ref android.R.styleable#AbsListView_drawSelectorOnTop
 * @attr ref android.R.styleable#AbsListView_stackFromBottom
 * @attr ref android.R.styleable#AbsListView_scrollingCache
 * @attr ref android.R.styleable#AbsListView_textFilterEnabled
 * @attr ref android.R.styleable#AbsListView_transcriptMode
 * @attr ref android.R.styleable#AbsListView_cacheColorHint
 * @attr ref android.R.styleable#AbsListView_fastScrollEnabled
 * @attr ref android.R.styleable#AbsListView_smoothScrollbar
 */
class AbsListView : public AdapterView<ListAdapter>, public TextWatcher,
public ViewTreeObserver::OnGlobalLayoutListener, Filter::FilterListener,
public ViewTreeObserver::OnTouchModeChangeListener {
    
public:
    
    class OnScrollListener;
    class ListViewLayoutParams;
    class RecyclerListener;
    class RecycleBin;
    
private:
    
    class WindowRunnnable;
    class PerformClick;
    class CheckForLongPress;
    class CheckForKeyLongPress;
    class CheckForTap;
    class FlingRunnable;
    
public:
    
    /**
     * Disables the transcript mode.
     *
     * @see #setTranscriptMode(int)
     */
    static const int TRANSCRIPT_MODE_DISABLED = 0;
    /**
     * The list will automatically scroll to the bottom when a data set change
     * notification is received and only if the last item is already visible
     * on screen.
     *
     * @see #setTranscriptMode(int)
     */
    static const int TRANSCRIPT_MODE_NORMAL = 1;
    /**
     * The list will automatically scroll to the bottom, no matter what items
     * are currently visible.
     *
     * @see #setTranscriptMode(int)
     */
    static const int TRANSCRIPT_MODE_ALWAYS_SCROLL = 2;
    
protected:
    
    /**
     * Indicates that we are not in the middle of a touch gesture
     */
    static const int TOUCH_MODE_REST = -1;
    
    /**
     * Indicates we just received the touch event and we are waiting to see if the it is a tap or a
     * scroll gesture.
     */
    static const int TOUCH_MODE_DOWN = 0;
    
    /**
     * Indicates the touch has been recognized as a tap and we are now waiting to see if the touch
     * is a longpress
     */
    static const int TOUCH_MODE_TAP = 1;
    
    /**
     * Indicates we have waited for everything we can wait for, but the user's finger is still down
     */
    static const int TOUCH_MODE_DONE_WAITING = 2;
    
    /**
     * Indicates the touch gesture is a scroll
     */
    static const int TOUCH_MODE_SCROLL = 3;
    
    /**
     * Indicates the view is in the process of being flung
     */
    static const int TOUCH_MODE_FLING = 4;
    
    /**
     * Indicates that the user is currently dragging the fast scroll thumb
     */
    static const int TOUCH_MODE_FAST_SCROLL = 5;
    
    /**
     * Regular layout - usually an unsolicited layout from the view system
     */
    static const int LAYOUT_NORMAL = 0;
    
    /**
     * Show the first item
     */
    static const int LAYOUT_FORCE_TOP = 1;
    
    /**
     * Force the selected item to be on somewhere on the screen
     */
    static const int LAYOUT_SET_SELECTION = 2;
    
    /**
     * Show the last item
     */
    static const int LAYOUT_FORCE_BOTTOM = 3;
    
    /**
     * Make a mSelectedItem appear in a specific location and build the rest of
     * the views from there. The top is specified by mSpecificTop.
     */
    static const int LAYOUT_SPECIFIC = 4;
    
    /**
     * Layout to sync as a result of a data change. Restore mSyncPosition to have its top
     * at mSpecificTop
     */
    static const int LAYOUT_SYNC = 5;
    
    /**
     * Layout as a result of using the navigation keys
     */
    static const int LAYOUT_MOVE_SELECTION = 6;
    
    /**
     * Controls how the next layout will happen
     */
    int mLayoutMode = LAYOUT_NORMAL;
    
    /**
     * Should be used by subclasses to listen to changes in the dataset
     */
    AdapterView::AdapterDataSetObserver *mDataSetObserver = NULL;

    /**
     * The adapter containing the data to be displayed by this view
     */
    shared_ptr<ListAdapter> mAdapter;

private:
    /**
     * Indicates whether the list selector should be drawn on top of the children or behind
     */
    bool mDrawSelectorOnTop = false;
    
    /**
     * The drawable used to draw the selector
     */
    shared_ptr<Drawable> mSelector;

protected:
    /**
     * Defines the selector's location and dimension at drawing time
     */
    Rect mSelectorRect = Rect();

private:
    /**
     * The selection's left padding
     */
    int mSelectionLeftPadding = 0;
    
    /**
     * The selection's top padding
     */
    int mSelectionTopPadding = 0;
    
    /**
     * The selection's right padding
     */
    int mSelectionRightPadding = 0;
    
    /**
     * The selection's bottom padding
     */
    int mSelectionBottomPadding = 0;

protected:
    /**
     * This view's padding
     */
    Rect mListPadding = Rect();

    /**
     * Subclasses must retain their measure spec from onMeasure() into this member
     */
    int mWidthMeasureSpec = 0;
    
    /**
     * The top scroll indicator
     */
    View *mScrollUp = NULL;
    
    /**
     * The down scroll indicator
     */
    View *mScrollDown = NULL;
    
    /**
     * When the view is scrolling, this flag is set to true to indicate subclasses that
     * the drawing cache was enabled on the children
     */
    bool mCachingStarted = false;
    
    /**
     * The position of the view that received the down motion event
     */
    int mMotionPosition = 0;
    
    /**
     * The offset to the top of the mMotionPosition view when the down motion event was received
     */
    int mMotionViewOriginalTop = 0;
    
    /**
     * The desired offset to the top of the mMotionPosition view after a scroll
     */
    int mMotionViewNewTop = 0;
    
    /**
     * The X value associated with the the down motion event
     */
    int mMotionX = 0;
    
    /**
     * The Y value associated with the the down motion event
     */
    int mMotionY = 0;

    /**
     * One of TOUCH_MODE_REST, TOUCH_MODE_DOWN, TOUCH_MODE_TAP, TOUCH_MODE_SCROLL, or
     * TOUCH_MODE_DONE_WAITING
     */
    int mTouchMode = TOUCH_MODE_REST;


    /**
     * Y value from on the previous motion event (if any)
     */
    int mLastY = 0;
    
    /**
     * How far the finger moved before we started scrolling
     */
    int mMotionCorrection = 0;

private:
    /**
     * Determines speed during touch scrolling
     */
    VelocityTracker *mVelocityTracker = NULL;
    
    /**
     * Handles one frame of a fling
     */
    FlingRunnable *mFlingRunnable = NULL;

protected:
    /**
     * The offset in pixels form the top of the AdapterView to the top
     * of the currently selected view. Used to save and restore state.
     */
    int mSelectedTop = 0;

    /**
     * Indicates whether the list is stacked from the bottom edge or
     * the top edge.
     */
    bool mStackFromBottom = false;

private:
    /**
     * When set to true, the list automatically discards the children's
     * bitmap cache after scrolling.
     */
    bool mScrollingCacheEnabled = false;
    
    /**
     * Whether or not to enable the fast scroll feature on this list
     */
    bool mFastScrollEnabled = false;
    
    /**
     * Optional callback to notify client when scroll position has changed
     */
    OnScrollListener *mOnScrollListener = NULL;
    
    /**
     * Keeps track of our accessory window
     */
    PopupWindow *mPopup = NULL;
    
    /**
     * Used with type filter window
     */
    EditText *mTextFilter = NULL;
    
    /**
     * Indicates whether to use pixels-based or position-based scrollbar
     * properties.
     */
    bool mSmoothScrollbarEnabled = true;
    
    /**
     * Indicates that this view supports filtering
     */
    bool mTextFilterEnabled = false;
    
    /**
     * Indicates that this view is currently displaying a filtered view of the data
     */
    bool mFiltered = false;
    
    /**
     * Rectangle used for hit testing children
     */
    Rect mTouchFrame;

protected:
    
    /**
     * The position to resurrect the selected position to.
     */
    int mResurrectToPosition = INVALID_POSITION;

private:
    
    ContextMenuInfo *mContextMenuInfo = NULL;
    
    /**
     * Used to request a layout when we changed touch mode
     */
    static const int TOUCH_MODE_UNKNOWN = -1;
    static const int TOUCH_MODE_ON = 0;
    static const int TOUCH_MODE_OFF = 1;
    
    int mLastTouchMode = TOUCH_MODE_UNKNOWN;
    
    static const bool PROFILE_SCROLLING = false;
    bool mScrollProfilingStarted = false;
    
    static const bool PROFILE_FLINGING = false;
    bool mFlingProfilingStarted = false;
    
    /**
     * The last CheckForLongPress runnable we posted, if any
     */
    sp<CheckForLongPress> mPendingCheckForLongPress = NULL;
    
    /**
     * The last CheckForTap runnable we posted, if any
     */
    sp<Runnable> mPendingCheckForTap;
    
    /**
     * The last CheckForKeyLongPress runnable we posted, if any
     */
    sp<CheckForKeyLongPress> mPendingCheckForKeyLongPress;
    
    /**
     * Acts upon click
     */
    sp<AbsListView::PerformClick> mPerformClick;
    
    /**
     * This view is in transcript mode -- it shows the bottom of the list when the data
     * changes
     */
    int mTranscriptMode = 0;
    
    /**
     * Indicates that this list is always drawn on top of a solid, single-color, opaque
     * background
     */
    int mCacheColorHint = 0;
    
    /**
     * The select child's view (from the adapter's getView) is enabled.
     */
    bool mIsChildViewEnabled = false;
    
    /**
     * The last scroll state reported to clients through {@link OnScrollListener}.
     */
    int mLastScrollState = OnScrollListener::SCROLL_STATE_IDLE;
    
    /**
     * Helper object that renders and controls the fast scroll thumb.
     */
    FastScroller *mFastScroller = NULL;
    
    bool mGlobalLayoutListenerAddedFilter = false;
    
    int mTouchSlop = 0;
    float mDensityScale = 0;
    
    InputConnection *mDefInputConnection = NULL;
    InputConnectionWrapper *mPublicInputConnection = NULL;
    
    sp<Runnable> mClearScrollingCache = NULL;
    int mMinimumVelocity = 0;
    int mMaximumVelocity = 0;
    
public:
    
    /**
     * Interface definition for a callback to be invoked when the list or grid
     * has been scrolled.
     */
    class OnScrollListener {
        
    public:
        
        /**
         * The view is not scrolling. Note navigating the list using the trackball counts as
         * being in the idle state since these transitions are not animated.
         */
        static const int SCROLL_STATE_IDLE = 0;
        
        /**
         * The user is scrolling using touch, and their finger is still on the screen
         */
        static const int SCROLL_STATE_TOUCH_SCROLL = 1;
        
        /**
         * The user had previously been scrolling using touch and had performed a fling. The
         * animation is now coasting to a stop
         */
        static const int SCROLL_STATE_FLING = 2;
        
        /**
         * Callback method to be invoked while the list view or grid view is being scrolled. If the
         * view is being scrolled, this method will be called before the next frame of the scroll is
         * rendered. In particular, it will be called before any calls to
         * {@link Adapter#getView(int, View, ViewGroup)}.
         *
         * @param view The view whose scroll state is being reported
         *
         * @param scrollState The current scroll state. One of {@link #SCROLL_STATE_IDLE},
         * {@link #SCROLL_STATE_TOUCH_SCROLL} or {@link #SCROLL_STATE_IDLE}.
         */
        virtual void onScrollStateChanged(AbsListView *view, int scrollState) = 0;
        
        /**
         * Callback method to be invoked when the list or grid has been scrolled. This will be
         * called after the scroll has completed
         * @param view The view whose scroll state is being reported
         * @param firstVisibleItem the index of the first visible cell (ignore if
         *        visibleItemCount == 0)
         * @param visibleItemCount the number of visible cells
         * @param totalItemCount the number of items in the list adaptor
         */
        virtual void onScroll(AbsListView *view, int firstVisibleItem, int visibleItemCount,
                             int totalItemCount) = 0;
    };
    
    AbsListView(Context *context);
    
    AbsListView(Context *context, AttributeSet *attrs);
    
private:
    
    void initAbsListView();
    
public:
    
    /**
     * Enables fast scrolling by letting the user quickly scroll through lists by
     * dragging the fast scroll thumb. The adapter attached to the list may want
     * to implement {@link SectionIndexer} if it wishes to display alphabet preview and
     * jump between sections of the list.
     * @see SectionIndexer
     * @see #isFastScrollEnabled()
     * @param enabled whether or not to enable fast scrolling
     */
    void setFastScrollEnabled(bool enabled);
    
    /**
     * Returns the current state of the fast scroll feature.
     * @see #setFastScrollEnabled(bool)
     * @return true if fast scroll is enabled, false otherwise
     */
    bool isFastScrollEnabled();
    
protected:
    
    /**
     * If fast scroll is visible, then don't draw the vertical scrollbar.
     * @hide
     */
    virtual bool isVerticalScrollBarHidden();
    
public:
    
    /**
     * When smooth scrollbar is enabled, the position and size of the scrollbar thumb
     * is computed based on the number of visible pixels in the visible items. This
     * however assumes that all list items have the same height. If you use a list in
     * which items have different heights, the scrollbar will change appearance as the
     * user scrolls through the list. To avoid this issue, you need to disable this
     * property.
     *
     * When smooth scrollbar is disabled, the position and size of the scrollbar thumb
     * is based solely on the number of items in the adapter and the position of the
     * visible items inside the adapter. This provides a stable scrollbar as the user
     * navigates through a list of items with varying heights.
     *
     * @param enabled Whether or not to enable smooth scrollbar.
     *
     * @see #setSmoothScrollbarEnabled(bool)
     * @attr ref android.R.styleable#AbsListView_smoothScrollbar
     */
    void setSmoothScrollbarEnabled(bool enabled);
    
    /**
     * Returns the current state of the fast scroll feature.
     *
     * @return True if smooth scrollbar is enabled is enabled, false otherwise.
     *
     * @see #setSmoothScrollbarEnabled(bool)
     */
    bool isSmoothScrollbarEnabled();
    
    /**
     * Set the listener that will receive notifications every time the list scrolls.
     *
     * @param l the scroll listener
     */
    void setOnScrollListener(OnScrollListener *l);
    
protected:
    
    /**
     * Notify our scroll listener (if there is one) of a change in scroll state
     */
    virtual void invokeOnItemScrollListener();
    
public:
    
    /**
     * Indicates whether the children's drawing cache is used during a scroll.
     * By default, the drawing cache is enabled but this will consume more memory.
     *
     * @return true if the scrolling cache is enabled, false otherwise
     *
     * @see #setScrollingCacheEnabled(bool)
     * @see View#setDrawingCacheEnabled(bool)
     */
    bool isScrollingCacheEnabled();
    
    /**
     * Enables or disables the children's drawing cache during a scroll.
     * By default, the drawing cache is enabled but this will use more memory.
     *
     * When the scrolling cache is enabled, the caches are kept after the
     * first scrolling. You can manually clear the cache by calling
     * {@link android.view.ViewGroup#setChildrenDrawingCacheEnabled(bool)}.
     *
     * @param enabled true to enable the scroll cache, false otherwise
     *
     * @see #isScrollingCacheEnabled()
     * @see View#setDrawingCacheEnabled(bool)
     */
    void setScrollingCacheEnabled(bool enabled);
    
    /**
     * Enables or disables the type filter window. If enabled, typing when
     * this view has focus will filter the children to match the users input.
     * Note that the {@link Adapter} used by this view must implement the
     * {@link Filterable} interface.
     *
     * @param textFilterEnabled true to enable type filtering, false otherwise
     *
     * @see Filterable
     */
    void setTextFilterEnabled(bool textFilterEnabled);
    
    /**
     * Indicates whether type filtering is enabled for this view
     *
     * @return true if type filtering is enabled, false otherwise
     *
     * @see #setTextFilterEnabled(bool)
     * @see Filterable
     */
    bool isTextFilterEnabled();
    
    virtual void getFocusedRect(Rect r);
    
private:
    
    void useDefaultSelector();
    
public:
    
    /**
     * Indicates whether the content of this view is pinned to, or stacked from,
     * the bottom edge.
     *
     * @return true if the content is stacked from the bottom edge, false otherwise
     */
    bool isStackFromBottom();
    
    /**
     * When stack from bottom is set to true, the list fills its content starting from
     * the bottom of the view.
     *
     * @param stackFromBottom true to pin the view's content to the bottom edge,
     *        false to pin the view's content to the top edge
     */
    void setStackFromBottom(bool stackFromBottom);
    
protected:
    
    virtual void requestLayoutIfNecessary();
    
//    class SavedState : public BaseSavedState {
//        long selectedId;
//        long firstId;
//        int viewTop;
//        int position;
//        int height;
//        String filter;
//        
//        /**
//         * Constructor called from {@link AbsListView#onSaveInstanceState()}
//         */
//        SavedState(Parcelable superState) {
//            super(superState);
//        }
//        
//        /**
//         * Constructor called from {@link #CREATOR}
//         */
//        SavedState(Parcel in) {
//            super(in);
//            selectedId = in.readLong();
//            firstId = in.readLong();
//            viewTop = in.readInt();
//            position = in.readInt();
//            height = in.readInt();
//            filter = in.readString();
//        }
//        
//        @Override
//        public void writeToParcel(Parcel out, int flags) {
//            super.writeToParcel(out, flags);
//            out.writeLong(selectedId);
//            out.writeLong(firstId);
//            out.writeInt(viewTop);
//            out.writeInt(position);
//            out.writeInt(height);
//            out.writeString(filter);
//        }
//        
//        @Override
//        public String toString() {
//            return "AbsListView.SavedState{"
//            + Integer.toHexString(System.identityHashCode(this))
//            + " selectedId=" + selectedId
//            + " firstId=" + firstId
//            + " viewTop=" + viewTop
//            + " position=" + position
//            + " height=" + height
//            + " filter=" + filter + "}";
//        }
//        
//        public static const Parcelable.Creator<SavedState> CREATOR
//        = new Parcelable.Creator<SavedState>() {
//            public SavedState createFromParcel(Parcel in) {
//                return new SavedState(in);
//            }
//            
//            public SavedState[] newArray(int size) {
//                return new SavedState[size];
//            }
//        };
//    };
//    
//    @Override
//    public Parcelable onSaveInstanceState() {
//        /*
//         * This doesn't really make sense as the place to dismiss the
//         * popups, but there don't seem to be any other useful hooks
//         * that happen early enough to keep from getting complaints
//         * about having leaked the window.
//         */
//        dismissPopup();
//        
//        Parcelable superState = super.onSaveInstanceState();
//        
//        SavedState ss = new SavedState(superState);
//        
//        bool haveChildren = getChildCount() > 0;
//        long selectedId = getSelectedItemId();
//        ss.selectedId = selectedId;
//        ss.height = getHeight();
//        
//        if (selectedId >= 0) {
//            // Remember the selection
//            ss.viewTop = mSelectedTop;
//            ss.position = getSelectedItemPosition();
//            ss.firstId = INVALID_POSITION;
//        } else {
//            if (haveChildren) {
//                // Remember the position of the first child
//                View v = getChildAt(0);
//                ss.viewTop = v.getTop();
//                ss.position = mFirstPosition;
//                ss.firstId = mAdapter.getItemId(mFirstPosition);
//            } else {
//                ss.viewTop = 0;
//                ss.firstId = INVALID_POSITION;
//                ss.position = 0;
//            }
//        }
//        
//        ss.filter = null;
//        if (mFiltered) {
//            const EditText textFilter = mTextFilter;
//            if (textFilter != null) {
//                Editable filterText = textFilter.getText();
//                if (filterText != null) {
//                    ss.filter = filterText.toString();
//                }
//            }
//        }
//        
//        return ss;
//    }
//    
//    @Override
//    public void onRestoreInstanceState(Parcelable state) {
//        SavedState ss = (SavedState) state;
//        
//        super.onRestoreInstanceState(ss.getSuperState());
//        mDataChanged = true;
//        
//        mSyncHeight = ss.height;
//        
//        if (ss.selectedId >= 0) {
//            mNeedSync = true;
//            mSyncRowId = ss.selectedId;
//            mSyncPosition = ss.position;
//            mSpecificTop = ss.viewTop;
//            mSyncMode = SYNC_SELECTED_POSITION;
//        } else if (ss.firstId >= 0) {
//            setSelectedPositionInt(INVALID_POSITION);
//            // Do this before setting mNeedSync since setNextSelectedPosition looks at mNeedSync
//            setNextSelectedPositionInt(INVALID_POSITION);
//            mNeedSync = true;
//            mSyncRowId = ss.firstId;
//            mSyncPosition = ss.position;
//            mSpecificTop = ss.viewTop;
//            mSyncMode = SYNC_FIRST_POSITION;
//        }
//        
//        setFilterText(ss.filter);
//        
//        requestLayout();
//    }
    
private:
    
    bool acceptFilter();
    
public:
    
    /**
     * Sets the initial value for the text filter.
     * @param filterText The text to use for the filter.
     *
     * @see #setTextFilterEnabled
     */
    void setFilterText(shared_ptr<String> filterText);
    
    /**
     * Returns the list's text filter, if available.
     * @return the list's text filter or null if filtering isn't enabled
     */
    CharSequence *getTextFilter();
    
protected:
    
    virtual void onFocusChanged(bool gainFocus, int direction, Rect *previouslyFocusedRect);
    
public:
    
    virtual void requestLayout();
    
protected:
    
    /**
     * The list is empty. Clear everything out.
     */
    virtual void resetList();
    
    virtual int computeVerticalScrollExtent();
    
    virtual int computeVerticalScrollOffset();
    
    virtual int computeVerticalScrollRange();
    
    virtual float getTopFadingEdgeStrength();
    
    virtual float getBottomFadingEdgeStrength();
    
    virtual void onMeasure(int widthMeasureSpec, int heightMeasureSpec);
    
    /**
     * Subclasses should NOT override this method but
     *  {@link #layoutChildren()} instead.
     */
    virtual void onLayout(bool changed, int l, int t, int r, int b);
    
    /**
     * @hide
     */
    virtual bool setFrame(int left, int top, int right, int bottom);
    
    /**
     * Subclasses must override this method to layout their children.
     */
    virtual void layoutChildren();
    
    virtual void updateScrollIndicators();
    
public:
    
    virtual View *getSelectedView();
    
    /**
     * List padding is the maximum of the normal view's padding and the padding of the selector.
     *
     * @see android.view.View#getPaddingTop()
     * @see #getSelector()
     *
     * @return The top list padding.
     */
    int getListPaddingTop();
    
    /**
     * List padding is the maximum of the normal view's padding and the padding of the selector.
     *
     * @see android.view.View#getPaddingBottom()
     * @see #getSelector()
     *
     * @return The bottom list padding.
     */
    int getListPaddingBottom();
    
    /**
     * List padding is the maximum of the normal view's padding and the padding of the selector.
     *
     * @see android.view.View#getPaddingLeft()
     * @see #getSelector()
     *
     * @return The left list padding.
     */
    int getListPaddingLeft();
    
    /**
     * List padding is the maximum of the normal view's padding and the padding of the selector.
     *
     * @see android.view.View#getPaddingRight()
     * @see #getSelector()
     *
     * @return The right list padding.
     */
    int getListPaddingRight();
    
protected:
    
    /**
     * Get a view and have it show the data associated with the specified
     * position. This is called when we have already discovered that the view is
     * not available for reuse in the recycle bin. The only choices left are
     * converting an old view or making a new one.
     *
     * @param position The position to display
     * @return A view displaying the data associated with the specified position
     */
    virtual shared_ptr<View> obtainView(int position);
    
    virtual void positionSelector(shared_ptr<View> sel);
    
private:
    
    void positionSelector(int l, int t, int r, int b);
    
protected:
    
    virtual void dispatchDraw(shared_ptr<Canvas> canvas);
    
    virtual void onSizeChanged(int w, int h, int oldw, int oldh);
    
    /**
     * @return True if the current touch mode requires that we draw the selector in the pressed
     *         state.
     */
    virtual bool touchModeDrawsInPressedState();
    
    /**
     * Indicates whether this view is in a state where the selector should be drawn. This will
     * happen if we have focus but are not in touch mode, or we are in the middle of displaying
     * the pressed state for an item.
     *
     * @return True if the selector should be shown
     */
    virtual bool shouldShowSelector();
    
private:
    
    void drawSelector(shared_ptr<Canvas> canvas);
    
public:
    
    /**
     * Controls whether the selection highlight drawable should be drawn on top of the item or
     * behind it.
     *
     * @param onTop If true, the selector will be drawn on the item it is highlighting. The default
     *        is false.
     *
     * @attr ref android.R.styleable#AbsListView_drawSelectorOnTop
     */
    void setDrawSelectorOnTop(bool onTop);
    
    /**
     * Set a Drawable that should be used to highlight the currently selected item.
     *
     * @param resID A Drawable resource to use as the selection highlight.
     *
     * @attr ref android.R.styleable#AbsListView_listSelector
     */
    void setSelector(string resID);
    
    void setSelector(shared_ptr<Drawable> sel);
    
    /**
     * Returns the selector {@link android.graphics.drawable.Drawable} that is used to draw the
     * selection in the list.
     *
     * @return the drawable used to display the selector
     */
    shared_ptr<Drawable> getSelector();
    
protected:
    
    /**
     * Sets the selector state to "pressed" and posts a CheckForKeyLongPress to see if
     * this is a long press.
     */
    virtual void keyPressed();
    
public:
    
    void setScrollIndicators(View *up, View *down);
    
protected:
    
    virtual void drawableStateChanged();
    
    virtual vector<int> onCreateDrawableState(int extraSpace);
    
public:
    
    virtual bool verifyDrawable(shared_ptr<Drawable> dr);
    
protected:
    
    virtual void onAttachedToWindow();
    
    virtual void onDetachedFromWindow();
    
public:
    
    virtual void onWindowFocusChanged(bool hasWindowFocus);
    
protected:
    
    /**
     * Creates the ContextMenuInfo returned from {@link #getContextMenuInfo()}. This
     * methods knows the view, position and ID of the item that received the
     * long press.
     *
     * @param view The view that received the long press.
     * @param position The position of the item that received the long press.
     * @param id The ID of the item that received the long press.
     * @return The extra information that should be returned by
     *         {@link #getContextMenuInfo()}.
     */
//    ContextMenuInfo *createContextMenuInfo(View *view, int position, long id);
    
private:
    
    class ScrollingCacheRunnable : public Runnable {
        
    private:
        
        AbsListView *mAbsListView = NULL;
        
    public:
        
        ScrollingCacheRunnable(AbsListView *absListView) {
            mAbsListView = absListView;
        }
        
        void run() {
            if (mAbsListView->mCachingStarted) {
                mAbsListView->mCachingStarted = false;
                mAbsListView->setChildrenDrawnWithCacheEnabled(false);
                if ((mAbsListView->mPersistentDrawingCache & PERSISTENT_SCROLLING_CACHE) == 0) {
                    mAbsListView->setChildrenDrawingCacheEnabled(false);
                }
                if (!mAbsListView->isAlwaysDrawnWithCacheEnabled()) {
                    mAbsListView->invalidate();
                }
            }
        }
    };
    
    class PressedRunnable : public Runnable {
        
    private:
        
        AbsListView *mAbsListView = NULL;
        shared_ptr<View> mChild;
        sp<AbsListView::PerformClick> mPerformClick = NULL;
        
    public:
        
        PressedRunnable(AbsListView *absListView, shared_ptr<View> child, sp<AbsListView::PerformClick> performClick) {
            mAbsListView = absListView;
            mChild = child;
            mPerformClick = performClick;
        }
        
        void run() {
            mChild->setPressed(false);
            mAbsListView->setPressed(false);
            if (!mAbsListView->mDataChanged) {
                mAbsListView->post(mPerformClick);
            }
            mAbsListView->mTouchMode = TOUCH_MODE_REST;
        }
    };
    
    /**
     * A base class for Runnables that will check that their view is still attached to
     * the original window as when the Runnable was created.
     *
     */
    class WindowRunnnable {
        
    protected:
        
        AbsListView *mAbsListView = NULL;
        
    public:
        
        int mOriginalAttachCount = 0;
        
        WindowRunnnable(AbsListView *absListView) {
            mAbsListView = absListView;
        }
        
        void rememberWindowAttachCount();
        
        bool sameWindow();
    };
    
    class PerformClick : public WindowRunnnable, public Runnable {
        
    public:
        
        shared_ptr<View> mChild;
        int mClickMotionPosition = 0;
        
        PerformClick(AbsListView *absListView) : WindowRunnnable(absListView) {
        }
        
        void run();
    };
    
    class CheckForLongPress : public WindowRunnnable, public Runnable {
        
    public:
        
        CheckForLongPress(AbsListView *absListView) : WindowRunnnable(absListView) {
        }
        
        void run();
    };
    
    class CheckForKeyLongPress : public WindowRunnnable, public Runnable {
    
    public:
        
        CheckForKeyLongPress(AbsListView *absListView) : WindowRunnnable(absListView) {
        }
        
        void run();
    };
    
    bool performLongPress(shared_ptr<View> child, const int longPressPosition, const long longPressId);
    
protected:
    
//    virtual ContextMenuInfo *getContextMenuInfo();
    
public:
    
//    virtual bool showContextMenuForChild(View *originalView);
    
    virtual bool onKeyUp(int keyCode, KeyEvent *event);
    
protected:
    
    virtual void dispatchSetPressed(bool pressed);
    
public:
    
    /**
     * Maps a point to a position in the list.
     *
     * @param x X in local coordinate
     * @param y Y in local coordinate
     * @return The position of the item which contains the specified point, or
     *         {@link #INVALID_POSITION} if the point does not intersect an item.
     */
    int pointToPosition(int x, int y);
    
    
    /**
     * Maps a point to a the rowId of the item which intersects that point.
     *
     * @param x X in local coordinate
     * @param y Y in local coordinate
     * @return The rowId of the item which contains the specified point, or {@link #INVALID_ROW_ID}
     *         if the point does not intersect an item.
     */
    long pointToRowId(int x, int y);
    
private:
    
    class CheckForTap : public Runnable {
    
    private:
        
        AbsListView *mAbsListView = NULL;
        
    public:
        
        CheckForTap(AbsListView *absListView) {
            mAbsListView = absListView;
        }
        
        void run();
    };
    
    bool startScrollIfNeeded(int deltaY);
    
public:
    
    void onTouchModeChanged(bool isInTouchMode);
    
    virtual bool onTouchEvent(MotionEvent *ev);
    
    virtual void draw(shared_ptr<Canvas> canvas);
    
    virtual bool onInterceptTouchEvent(MotionEvent *ev);
    
    /**
     * {@inheritDoc}
     */
    virtual void addTouchables(vector<shared_ptr<View>> &views);
    
protected:
    
    /**
     * Fires an "on scroll state changed" event to the registered
     * {@link android.widget.AbsListView.OnScrollListener}, if any. The state change
     * is fired only if the specified state is different from the previously known state.
     *
     * @param newState The new scroll state.
     */
    void reportScrollStateChange(int newState);
    
private:
    
    /**
     * Responsible for fling behavior. Use {@link #start(int)} to
     * initiate a fling. Each frame of the fling is handled in {@link #run()}.
     * A FlingRunnable will keep re-posting itself until the fling is done.
     *
     */
    class FlingRunnable : public Runnable {
        
    private:
        
        AbsListView *mAbsListView = NULL;
        
        /**
         * Tracks the decay of a fling scroll
         */
        Scroller *mScroller = NULL;
        
        /**
         * Y value reported by mScroller on the previous fling
         */
        int mLastFlingY = 0;
        
    public:
        
        FlingRunnable(AbsListView *absListView);
        
        void start(int initialVelocity);
        
    private:
        
        void endFling();
        
    public:
        
        void run();
    };
    
    void createScrollingCache();
    
    void clearScrollingCache();
    
protected:
    
    /**
     * Track a motion scroll
     *
     * @param deltaY Amount to offset mMotionView. This is the accumulated delta since the motion
     *        began. Positive numbers mean the user's finger is moving down the screen.
     * @param incrementalDeltaY Change in deltaY from the previous event.
     */
    virtual void trackMotionScroll(int deltaY, int incrementalDeltaY);
    
    /**
     * Returns the number of header views in the list. Header views are special views
     * at the top of the list that should not be recycled during a layout.
     *
     * @return The number of header views, 0 in the default implementation.
     */
    virtual int getHeaderViewsCount();
    
    /**
     * Returns the number of footer views in the list. Footer views are special views
     * at the bottom of the list that should not be recycled during a layout.
     *
     * @return The number of footer views, 0 in the default implementation.
     */
    virtual int getFooterViewsCount();
    
    /**
     * Fills the gap left open by a touch-scroll. During a touch scroll, children that
     * remain on screen are shifted and the other ones are discarded. The role of this
     * method is to fill the gap thus created by performing a partial layout in the
     * empty space.
     *
     * @param down true if the scroll is going down, false if it is going up
     */
    virtual void fillGap(bool down) = 0;
    
    virtual void hideSelector();
    
    /**
     * @return A position to select. First we try mSelectedPosition. If that has been clobbered by
     * entering touch mode, we then try mResurrectToPosition. Values are pinned to the range
     * of items available in the adapter
     */
    virtual int reconcileSelectedPosition();
    
    /**
     * Find the row closest to y. This row will be used as the motion row when scrolling
     *
     * @param y Where the user touched
     * @return The position of the first (or only) item in the row closest to y
     */
    virtual int findMotionRow(int y) = 0;
    
public:
    
    /**
     * Causes all the views to be rebuilt and redrawn.
     */
    void invalidateViews();
    
protected:
    
    /**
     * Makes the item at the supplied position selected.
     *
     * @param position the position of the new selection
     */
    virtual void setSelectionInt(int position) = 0;
    
    /**
     * Attempt to bring the selection back if the user is switching from touch
     * to trackball mode
     * @return Whether selection was set to something.
     */
    virtual bool resurrectSelection();
    
    virtual void handleDataChanged();
    
private:
    
    /**
     * Removes the filter window
     */
    void dismissPopup();
    
    /**
     * Shows the filter window
     */
    void showPopup();
    
    void positionPopup();
    
protected:
    
    /**
     * What is the distance between the source and destination rectangles given the direction of
     * focus navigation between them? The direction basically helps figure out more quickly what is
     * self evident by the relationship between the rects...
     *
     * @param source the source rectangle
     * @param dest the destination rectangle
     * @param direction the direction
     * @return the distance between the rectangles
     */
    static int getDistance(Rect &source, Rect &dest, int direction);
    
    virtual bool isInFilterMode();
    
    /**
     * Sends a key to the text filter window
     *
     * @param keyCode The keycode for the event
     * @param event The actual key event
     *
     * @return True if the text filter handled the event, false otherwise.
     */
    virtual bool sendToTextFilter(int keyCode, int count, KeyEvent *event);
    
public:
    
    /**
     * Return an InputConnection for editing of the filter text.
     */
//    virtual InputConnection *onCreateInputConnection(EditorInfo *outAttrs);
    
    /**
     * For filtering we proxy an input connection to an internal text editor,
     * and this allows the proxying to happen.
     */
//    virtual bool checkInputConnectionProxy(View *view);
    
private:
    
    /**
     * Creates the window for the text filter and populates it with an EditText field;
     *
     * @param animateEntrance true if the window should appear with an animation
     */
//    void createTextFilter(bool animateEntrance);
    
public:
    
    /**
     * Clear the text filter.
     */
    void clearTextFilter();
    
    /**
     * Returns if the ListView currently has a text filter.
     */
    bool hasTextFilter();
    
    void onGlobalLayout();
    
    /**
     * For our text watcher that is associated with the text filter.  Does
     * nothing.
     */
    void beforeTextChanged(shared_ptr<CharSequence> s, int start, int count, int after);
    
    /**
     * For our text watcher that is associated with the text filter. Performs
     * the actual filtering as the text changes, and takes care of hiding and
     * showing the popup displaying the currently entered filter text.
     */
    void onTextChanged(shared_ptr<CharSequence> s, int start, int before, int count);
    
    /**
     * For our text watcher that is associated with the text filter.  Does
     * nothing.
     */
    void afterTextChanged(Editable *s);
    
    void onFilterComplete(int count);
    
protected:
    
    virtual LayoutParams *generateLayoutParams(LayoutParams *p);
    
public:
    
    virtual LayoutParams *generateLayoutParams(AttributeSet *attrs);
    
protected:
    
    virtual bool checkLayoutParams(LayoutParams *p);
    
public:
    
    /**
     * Puts the list or grid into transcript mode. In this mode the list or grid will always scroll
     * to the bottom to show new items.
     *
     * @param mode the transcript mode to set
     *
     * @see #TRANSCRIPT_MODE_DISABLED
     * @see #TRANSCRIPT_MODE_NORMAL
     * @see #TRANSCRIPT_MODE_ALWAYS_SCROLL
     */
    void setTranscriptMode(int mode);
    
    /**
     * Returns the current transcript mode.
     *
     * @return {@link #TRANSCRIPT_MODE_DISABLED}, {@link #TRANSCRIPT_MODE_NORMAL} or
     *         {@link #TRANSCRIPT_MODE_ALWAYS_SCROLL}
     */
    int getTranscriptMode();
    
    virtual int getSolidColor();
    
    /**
     * When set to a non-zero value, the cache color hint indicates that this list is always drawn
     * on top of a solid, single-color, opaque background
     *
     * @param color The background color
     */
    void setCacheColorHint(int color);
    
    /**
     * When set to a non-zero value, the cache color hint indicates that this list is always drawn
     * on top of a solid, single-color, opaque background
     *
     * @return The cache color hint
     */
    int getCacheColorHint();
    
    /**
     * Move all views (excluding headers and footers) held by this AbsListView into the supplied
     * List. This includes views displayed on the screen as well as views stored in AbsListView's
     * internal view recycler.
     *
     * @param views A list into which to put the reclaimed views
     */
    void reclaimViews(vector<shared_ptr<View>> &views);
    
protected:
    
    /**
     * @hide
     */
//    virtual bool onConsistencyCheck(int consistency);
    
private:
    
    bool checkScrap(vector<shared_ptr<View>> scrap);
    
public:
    
    /**
     * Sets the recycler listener to be notified whenever a View is set aside in
     * the recycler for later reuse. This listener can be used to free resources
     * associated to the View.
     *
     * @param listener The recycler listener to be notified of views set aside
     *        in the recycler.
     *
     * @see android.widget.AbsListView.RecycleBin
     * @see android.widget.AbsListView.RecyclerListener
     */
    void setRecyclerListener(RecyclerListener *listener);
    
    /**
     * AbsListView extends LayoutParams to provide a place to hold the view type.
     */
    class ListViewLayoutParams : public LayoutParams {
        
        friend class AbsListView;
        friend class ListView;
        
    private:
        
        /**
         * View type for this view, as returned by
         * {@link android.widget.Adapter#getItemViewType(int) }
         */
        int viewType = 0;
        
        /**
         * When this bool is set, the view has been added to the AbsListView
         * at least once. It is used to know whether headers/footers have already
         * been added to the list view and whether they should be treated as
         * recycled views or not.
         */
        bool recycledHeaderFooter = false;
        
    public:
        
        ListViewLayoutParams(Context *c, AttributeSet *attrs);
        
        ListViewLayoutParams(int w, int h);
        
        ListViewLayoutParams(int w, int h, int viewType);
        
        ListViewLayoutParams(LayoutParams *source);
    };
    
    /**
     * A RecyclerListener is used to receive a notification whenever a View is placed
     * inside the RecycleBin's scrap heap. This listener is used to free resources
     * associated to Views placed in the RecycleBin.
     *
     * @see android.widget.AbsListView.RecycleBin
     * @see android.widget.AbsListView#setRecyclerListener(android.widget.AbsListView.RecyclerListener)
     */
    class RecyclerListener {
        
    public:
        
        /**
         * Indicates that the specified View was moved into the recycler's scrap heap.
         * The view is not displayed on screen any more and any expensive resource
         * associated with the view should be discarded.
         *
         * @param view
         */
        virtual void onMovedToScrapHeap(shared_ptr<View> view) = 0;
    };
    
    /**
     * The RecycleBin facilitates reuse of views across layouts. The RecycleBin has two levels of
     * storage: ActiveViews and ScrapViews. ActiveViews are those views which were onscreen at the
     * start of a layout. By construction, they are displaying current information. At the end of
     * layout, all views in ActiveViews are demoted to ScrapViews. ScrapViews are old views that
     * could potentially be used by the adapter to avoid allocating views unnecessarily.
     *
     * @see android.widget.AbsListView#setRecyclerListener(android.widget.AbsListView.RecyclerListener)
     * @see android.widget.AbsListView.RecyclerListener
     */
    class RecycleBin {
        
        friend class AbsListView;
        friend class ListView;
    
    private:
        
        AbsListView *mAbsListView = NULL;
        
        RecyclerListener *mRecyclerListener = NULL;
        
        /**
         * The position of the first view stored in mActiveViews.
         */
        int mFirstActivePosition = 0;
        
        /**
         * Views that were on screen at the start of layout. This array is populated at the start of
         * layout, and at the end of layout all view in mActiveViews are moved to mScrapViews.
         * Views in mActiveViews represent a contiguous range of Views, with position of the first
         * view store in mFirstActivePosition.
         */
        vector<shared_ptr<View>> mActiveViews = vector<shared_ptr<View>>(0);
        
        /**
         * Unsorted views that can be used by the adapter as a convert view.
         */
        vector<vector<shared_ptr<View>>> mScrapViews;
        
        int mViewTypeCount = 0;
        
        vector<shared_ptr<View>> mCurrentScrap;
        
    public:
        
        RecycleBin(AbsListView *absListView) {
            mAbsListView = absListView;
        }
        
        void setViewTypeCount(int viewTypeCount);
        
        bool shouldRecycleViewType(int viewType);
        
        /**
         * Clears the scrap heap.
         */
        void clear();
        
    protected:
        
        /**
         * Fill ActiveViews with all of the children of the AbsListView.
         *
         * @param childCount The minimum number of views mActiveViews should hold
         * @param firstActivePosition The position of the first view that will be stored in
         *        mActiveViews
         */
        virtual void fillActiveViews(int childCount, int firstActivePosition);
        
        /**
         * Get the view corresponding to the specified position. The view will be removed from
         * mActiveViews if it is found.
         *
         * @param position The position to look up in mActiveViews
         * @return The view if it is found, null otherwise
         */
        virtual shared_ptr<View> getActiveView(int position);
        
        /**
         * @return A view from the ScrapViews collection. These are unordered.
         */
        virtual shared_ptr<View> getScrapView(int position);
        
        /**
         * Put a view into the ScapViews list. These views are unordered.
         *
         * @param scrap The view to add
         */
        virtual void addScrapView(shared_ptr<View> scrap);
        
        /**
         * Move all views remaining in mActiveViews to mScrapViews.
         */
        virtual void scrapActiveViews();
        
    private:
        
        /**
         * Makes sure that the size of mScrapViews does not exceed the size of mActiveViews.
         * (This can happen if an adapter does not recycle its views).
         */
        void pruneScrapViews();
        
        /**
         * Puts all views in the scrap heap into the supplied list.
         */
        void reclaimScrapViews(vector<shared_ptr<View>> &views);
    };
    
protected:
    
    /**
     * The data set used to store unused views that should be reused during the next layout
     * to avoid creating new ones
     */
    RecycleBin mRecycler = RecycleBin(this);
};

ANDROID_END

#endif /* defined(__Androidpp__AbsListView__) */
