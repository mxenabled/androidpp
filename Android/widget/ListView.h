//
//  ListView.h
//  Androidpp
//
//  Created by Travis Elnicky on 2/5/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef __Androidpp__ListView__
#define __Androidpp__ListView__

#include "AndroidMacros.h"
#include "Android/widget/AbsListView.h"
#include "Android/utils/Object.h"

#include <memory>

using namespace std;

ANDROID_BEGIN

/*
 * Implementation Notes:
 *
 * Some terminology:
 *
 *     index    - index of the items that are currently visible
 *     position - index of the items in the cursor
 */

/**
 * A view that shows items in a vertically scrolling list. The items
 * come from the {@link ListAdapter} associated with this view.
 *
 * <p>See the <a href="{@docRoot}guide/topics/ui/layout/listview.html">List View</a>
 * guide.</p>
 *
 * @attr ref android.R.styleable#ListView_entries
 * @attr ref android.R.styleable#ListView_divider
 * @attr ref android.R.styleable#ListView_dividerHeight
 * @attr ref android.R.styleable#ListView_headerDividersEnabled
 * @attr ref android.R.styleable#ListView_footerDividersEnabled
 */

class ListView : public AbsListView {
public:
    ListView(Context *context);
    ListView(Context *context, AttributeSet *attrs);
    

    /**
     * A class that represents a fixed view in a list, for example a header at the top
     * or a footer at the bottom.
     */
    class FixedViewInfo {
    public:
        /** The view to add to the list */
        shared_ptr<View> view;
        /** The data backing the view. This is returned from {@link ListAdapter#getItem(int)}. */
        shared_ptr<Object> data;
        /** <code>true</code> if the fixed view should be selectable in the list */
        bool isSelectable = false;
    };

    int getMaxScrollAmount();
    virtual void addHeaderView(shared_ptr<View> v, shared_ptr<Object> data, bool isSelectable);
    virtual void addHeaderView(shared_ptr<View> v);
    virtual int getHeaderViewsCount();
    bool removeHeaderView(shared_ptr<View> v);
    void addFooterView(shared_ptr<View> v, shared_ptr<Object> data, bool isSelectable);
    void addFooterView(shared_ptr<View> v);
    int getFooterViewsCount();
    bool removeFooterView(shared_ptr<View> v);
    shared_ptr<ListAdapter> getAdapter();
    virtual void setAdapter(shared_ptr<ListAdapter> adapter);
//    void setRemoteViewsAdapter(Intent intent);
    virtual bool requestChildRectangleOnScreen(shared_ptr<View> child, Rect rect, bool immediate);

    /**
     * Smoothly scroll to the specified adapter position. The view will
     * scroll such that the indicated position is displayed.
     * @param position Scroll to this adapter position.
     */
    //@android.view.RemotableViewMethod
    void smoothScrollToPosition(int position);
    
    /**
     * Smoothly scroll to the specified adapter position offset. The view will
     * scroll such that the indicated position is displayed.
     * @param offset The amount to offset from the adapter position to scroll to.
     */
    //@android.view.RemotableViewMethod
    void smoothScrollByOffset(int offset);


    void setSelection(int position);
    void setSelectionFromTop(int position, int y);
    
    /**
     * setSelectionAfterHeaderView set the selection to be the first list item
     * after the header views.
     */
    void setSelectionAfterHeaderView();
    
//    //@Override
//    bool dispatchKeyEvent(KeyEvent event);
//    //@Override
//    bool onKeyDown(int keyCode, KeyEvent event);
//    //@Override
//    bool onKeyMultiple(int keyCode, int repeatCount, KeyEvent event);
//    //@Override
//    bool onKeyUp(int keyCode, KeyEvent event);
    
    /**
     * Indicates that the views created by the ListAdapter can contain focusable
     * items.
     *
     * @param itemsCanFocus true if items can get focus, false otherwise
     */
    void setItemsCanFocus(bool itemsCanFocus);
    
    /**
     * @return Whether the views created by the ListAdapter can contain focusable
     * items.
     */
    bool getItemsCanFocus();
    
    //@Override
    bool isOpaque();
    
    //@Override
    void setCacheColorHint(unsigned int color);
    
    /**
     * Returns the drawable that will be drawn between each item in the list.
     *
     * @return the current drawable drawn between list elements
     */
    shared_ptr<Drawable> getDivider();
    
    /**
     * Sets the drawable that will be drawn between each item in the list. If the drawable does
     * not have an intrinsic height, you should also call {@link #setDividerHeight(int)}
     *
     * @param divider The drawable to use.
     */
    void setDivider(shared_ptr<Drawable> divider);
    
    /**
     * @return Returns the height of the divider that will be drawn between each item in the list.
     */
    int getDividerHeight();
    
    /**
     * Sets the height of the divider that will be drawn between each item in the list. Calling
     * this will override the intrinsic height as set by {@link #setDivider(Drawable)}
     *
     * @param height The new height of the divider in pixels.
     */
    void setDividerHeight(int height);
    
    /**
     * Enables or disables the drawing of the divider for header views.
     *
     * @param headerDividersEnabled True to draw the headers, false otherwise.
     *
     * @see #setFooterDividersEnabled(boolean)
     * @see #addHeaderView(android.view.View)
     */
    void setHeaderDividersEnabled(bool headerDividersEnabled);
    
    /**
     * Enables or disables the drawing of the divider for footer views.
     *
     * @param footerDividersEnabled True to draw the footers, false otherwise.
     *
     * @see #setHeaderDividersEnabled(boolean)
     * @see #addFooterView(android.view.View)
     */
    void setFooterDividersEnabled(bool footerDividersEnabled);
    
    /**
     * Sets the drawable that will be drawn above all other list content.
     * This area can become visible when the user overscrolls the list.
     *
     * @param header The drawable to use
     */
    void setOverscrollHeader(shared_ptr<Drawable> header);
    
    /**
     * @return The drawable that will be drawn above all other list content
     */
    shared_ptr<Drawable> getOverscrollHeader();
    
    /**
     * Sets the drawable that will be drawn below all other list content.
     * This area can become visible when the user overscrolls the list,
     * or when the list's content does not fully fill the container area.
     *
     * @param footer The drawable to use
     */
    void setOverscrollFooter(shared_ptr<Drawable> footer);
    
    /**
     * @return The drawable that will be drawn below all other list content
     */
    shared_ptr<Drawable> getOverscrollFooter();

private:
    /**
     * Used to indicate a no preference for a position type.
     */
    static const int NO_POSITION = -1;

    shared_ptr<Drawable> mDivider;
    int mDividerHeight = 0;
    bool mClipDivider = false;

    shared_ptr<Drawable> mOverScrollHeader;
    shared_ptr<Drawable> mOverScrollFooter;
    
    /**
     * Holds results of focus aware arrow scrolling.
     */
    class ArrowScrollFocusResult {
    private:
        int mSelectedPosition;
        int mAmountToScroll;
    public:
        /**
         * How {@link android.widget.ListView#arrowScrollFocused} returns its values.
         */
        void populate(int selectedPosition, int amountToScroll) {
            mSelectedPosition = selectedPosition;
            mAmountToScroll = amountToScroll;
        }

        int getSelectedPosition() {
            return mSelectedPosition;
        }

        int getAmountToScroll() {
            return mAmountToScroll;
        }
    };
    
    class FocusSelector : public Runnable {
    private:
        int mPosition;
        int mPositionTop;
        ListView *mListView;
    public:
        FocusSelector(ListView *listView) { mListView = listView; }
        
        FocusSelector* setup(int position, int top) {
            mPosition = position;
            mPositionTop = top;
            return this;
        }

        void run() {
            mListView->setSelectionFromTop(mPosition, mPositionTop);
        }
    };
    
    /**
     * When arrow scrolling, ListView will never scroll more than this factor
     * times the height of the list.
     */
    static const float MAX_SCROLL_FACTOR;

    /**
     * When arrow scrolling, need a certain amount of pixels to preview next
     * items.  This is usually the fading edge, but if that is small enough,
     * we want to make sure we preview at least this many pixels.
     */
    static const int MIN_SCROLL_PREVIEW_PIXELS = 2;

    vector<FixedViewInfo> mHeaderViewInfos = vector<FixedViewInfo>();
    vector<FixedViewInfo> mFooterViewInfos = vector<FixedViewInfo>();

    bool mIsCacheColorOpaque;
    bool mDividerIsOpaque;

    bool mHeaderDividersEnabled;
    bool mFooterDividersEnabled;

    bool mAreAllItemsSelectable = true;

    bool mItemsCanFocus = false;

    // used for temporary calculations.
    Rect mTempRect = Rect();
    Paint *mDividerPaint = NULL;

    // the single allocated result per list view; kinda cheesey but avoids
    // allocating these thingies too often.
    ArrowScrollFocusResult* mArrowScrollFocusResult = new ArrowScrollFocusResult();
    
    // Keeps focused children visible through resizes
    FocusSelector* mFocusSelector = NULL;
    
    void adjustViewsUpOrDown();
    void removeFixedViewInfo(shared_ptr<View> v, vector<FixedViewInfo> where);
    void resetList();
    void clearRecycledState(vector<FixedViewInfo> infos);

    /**
     * @return Whether the list needs to show the top fading edge
     */
    bool showingTopFadingEdge();

    /**
     * @return Whether the list needs to show the bottom fading edge
     */
    bool showingBottomFadingEdge();

    /**
     * {@inheritDoc}
     */
    //@Override
    void fillGap(bool down);
    
    /**
     * Fills the list from pos down to the end of the list view.
     *
     * @param pos The first position to put in the list
     *
     * @param nextTop The location where the top of the item associated with pos
     *        should be drawn
     *
     * @return The view that is currently selected, if it happens to be in the
     *         range that we draw.
     */
    shared_ptr<View> fillDown(int pos, int nextTop);
    
    /**
     * Fills the list from pos up to the top of the list view.
     *
     * @param pos The first position to put in the list
     *
     * @param nextBottom The location where the bottom of the item associated
     *        with pos should be drawn
     *
     * @return The view that is currently selected
     */
    shared_ptr<View> fillUp(int pos, int nextBottom);
    
    /**
     * Fills the list from top to bottom, starting with mFirstPosition
     *
     * @param nextTop The location where the top of the first item should be
     *        drawn
     *
     * @return The view that is currently selected
     */
    shared_ptr<View> fillFromTop(int nextTop);
    
    /**
     * Put mSelectedPosition in the middle of the screen and then build up and
     * down from there. This method forces mSelectedPosition to the center.
     *
     * @param childrenTop Top of the area in which children can be drawn, as
     *        measured in pixels
     * @param childrenBottom Bottom of the area in which children can be drawn,
     *        as measured in pixels
     * @return Currently selected view
     */
    shared_ptr<View> fillFromMiddle(int childrenTop, int childrenBottom);
    
    /**
     * Once the selected view as been placed, fill up the visible area above and
     * below it.
     *
     * @param sel The selected view
     * @param position The position corresponding to sel
     */
    void fillAboveAndBelow(shared_ptr<View> sel, int position);
    
    /**
     * Fills the grid based on positioning the new selection at a specific
     * location. The selection may be moved so that it does not intersect the
     * faded edges. The grid is then filled upwards and downwards from there.
     *
     * @param selectedTop Where the selected item should be
     * @param childrenTop Where to start drawing children
     * @param childrenBottom Last pixel where children can be drawn
     * @return The view that currently has selection
     */
    shared_ptr<View> fillFromSelection(int selectedTop, int childrenTop, int childrenBottom);
    
    /**
     * Calculate the bottom-most pixel we can draw the selection into
     *
     * @param childrenBottom Bottom pixel were children can be drawn
     * @param fadingEdgeLength Length of the fading edge in pixels, if present
     * @param selectedPosition The position that will be selected
     * @return The bottom-most pixel we can draw the selection into
     */
    int getBottomSelectionPixel(int childrenBottom, int fadingEdgeLength, int selectedPosition);
    
    /**
     * Calculate the top-most pixel we can draw the selection into
     *
     * @param childrenTop Top pixel were children can be drawn
     * @param fadingEdgeLength Length of the fading edge in pixels, if present
     * @param selectedPosition The position that will be selected
     * @return The top-most pixel we can draw the selection into
     */
    int getTopSelectionPixel(int childrenTop, int fadingEdgeLength, int selectedPosition);
    
    /**
     * Fills the list based on positioning the new selection relative to the old
     * selection. The new selection will be placed at, above, or below the
     * location of the new selection depending on how the selection is moving.
     * The selection will then be pinned to the visible part of the screen,
     * excluding the edges that are faded. The list is then filled upwards and
     * downwards from there.
     *
     * @param oldSel The old selected view. Useful for trying to put the new
     *        selection in the same place
     * @param newSel The view that is to become selected. Useful for trying to
     *        put the new selection in the same place
     * @param delta Which way we are moving
     * @param childrenTop Where to start drawing children
     * @param childrenBottom Last pixel where children can be drawn
     * @return The view that currently has selection
     */
    shared_ptr<View> moveSelection(shared_ptr<View> oldSel, shared_ptr<View> newSel, int delta, int childrenTop, int childrenBottom);
    
    void measureScrapChild(shared_ptr<View> child, int position, int widthMeasureSpec);
    
    /**
     * Measures the height of the given range of children (inclusive) and
     * returns the height with this ListView's padding and divider heights
     * included. If maxHeight is provided, the measuring will stop when the
     * current height reaches maxHeight.
     *
     * @param widthMeasureSpec The width measure spec to be given to a child's
     *            {@link View#measure(int, int)}.
     * @param startPosition The position of the first child to be shown.
     * @param endPosition The (inclusive) position of the last child to be
     *            shown. Specify {@link #NO_POSITION} if the last child should be
     *            the last available child from the adapter.
     * @param maxHeight The maximum height that will be returned (if all the
     *            children don't fit in this value, this value will be
     *            returned).
     * @param disallowPartialChildPosition In general, whether the returned
     *            height should only contain entire children. This is more
     *            powerful--it is the first inclusive position at which partial
     *            children will not be allowed. Example: it looks nice to have
     *            at least 3 completely visible children, and in portrait this
     *            will most likely fit; but in landscape there could be times
     *            when even 2 children can not be completely shown, so a value
     *            of 2 (remember, inclusive) would be good (assuming
     *            startPosition is 0).
     * @return The height of this ListView with the given children.
     */
    const int measureHeightOfChildren(int widthMeasureSpec, int startPosition, int endPosition, const int maxHeight, int disallowPartialChildPosition);
    
    int findMotionRow(int y);
    
    /**
     * Put a specific item at a specific location on the screen and then build
     * up and down from there.
     *
     * @param position The reference view to use as the starting point
     * @param top Pixel offset from the top of this view to the top of the
     *        reference view.
     *
     * @return The selected view, or null if the selected view is outside the
     *         visible area.
     */
    shared_ptr<View> fillSpecific(int position, int top);
    
    /**
     * Check if we have dragged the bottom of the list too high (we have pushed the
     * top element off the top of the screen when we did not need to). Correct by sliding
     * everything back down.
     *
     * @param childCount Number of children
     */
    void correctTooHigh(int childCount);
    
    /**
     * Check if we have dragged the bottom of the list too low (we have pushed the
     * bottom element off the bottom of the screen when we did not need to). Correct by sliding
     * everything back up.
     *
     * @param childCount Number of children
     */
    void correctTooLow(int childCount);
    
    /**
     * @param focusedView the view that has accessibility focus.
     * @return the direct child that contains accessibility focus.
     */
//    View findAccessibilityFocusedChild(View focusedView)
    
    /**
     * @param child a direct child of this list.
     * @return Whether child is a header or footer view.
     */
    bool isDirectChildHeaderOrFooter(shared_ptr<View> child);
    
    /**
     * Obtain the view and add it to our list of children. The view can be made
     * fresh, converted from an unused view, or used as is if it was in the
     * recycle bin.
     *
     * @param position Logical position in the list
     * @param y Top or bottom edge of the view to add
     * @param flow If flow is true, align top edge to y. If false, align bottom
     *        edge to y.
     * @param childrenLeft Left edge where children should be positioned
     * @param selected Is this position selected?
     * @return View that was added
     */
    shared_ptr<View> makeAndAddView(int position, int y, bool flow, int childrenLeft, bool selected);
    
    /**
     * Add a view as a child and make sure it is measured (if necessary) and
     * positioned properly.
     *
     * @param child The view to add
     * @param position The position of this child
     * @param y The y position relative to which this view will be positioned
     * @param flowDown If true, align top edge to y. If false, align bottom
     *        edge to y.
     * @param childrenLeft Left edge where children should be positioned
     * @param selected Is this position selected?
     * @param recycled Has this view been pulled from the recycle bin? If so it
     *        does not need to be remeasured.
     */
    void setupChild(shared_ptr<View> child, int position, int y, bool flowDown, int childrenLeft,
                    bool selected, bool recycled);
    
    /**
     * Makes the item at the supplied position selected.
     *
     * @param position the position of the item to select
     */
    void setSelectionInt(int position);
    
    /**
     * Find a position that can be selected (i.e., is not a separator).
     *
     * @param position The starting position to look at.
     * @param lookDown Whether to look down for other positions.
     * @return The next selectable position starting at position and then searching either up or
     *         down. Returns {@link #INVALID_POSITION} if nothing can be found.
     */
    virtual int lookForSelectablePosition(int position, bool lookDown);
    
//    bool commonKey(int keyCode, int count, KeyEvent event);
    
    /**
     * Scrolls up or down by the number of items currently present on screen.
     *
     * @param direction either {@link View#FOCUS_UP} or {@link View#FOCUS_DOWN}
     * @return whether selection was moved
     */
    bool pageScroll(int direction);
    
    /**
     * Go to the last or first item if possible (not worrying about panning across or navigating
     * within the internal focus of the currently selected item.)
     *
     * @param direction either {@link View#FOCUS_UP} or {@link View#FOCUS_DOWN}
     *
     * @return whether selection was moved
     */
    bool fullScroll(int direction);
    
    /**
     * To avoid horizontal focus searches changing the selected item, we
     * manually focus search within the selected item (as applicable), and
     * prevent focus from jumping to something within another item.
     * @param direction one of {View.FOCUS_LEFT, View.FOCUS_RIGHT}
     * @return Whether this consumes the key event.
     */
    bool handleHorizontalFocusWithinListItem(int direction);
    
    /**
     * Scrolls to the next or previous item if possible.
     *
     * @param direction either {@link View#FOCUS_UP} or {@link View#FOCUS_DOWN}
     *
     * @return whether selection was moved
     */
    bool arrowScroll(int direction);
    
    /**
     * Used by {@link #arrowScrollImpl(int)} to help determine the next selected position
     * to move to. This can return a position currently not represented by a view on screen
     * but only in the direction given.
     *
     * @param selectedPos Current selected position to move from
     * @param direction Direction to move in
     * @return Desired selected position after moving in the given direction
     */
    const int nextSelectedPositionForDirection(int selectedPos, int direction);
    
    /**
     * Handle an arrow scroll going up or down.  Take into account whether items are selectable,
     * whether there are focusable items etc.
     *
     * @param direction Either {@link android.view.View#FOCUS_UP} or {@link android.view.View#FOCUS_DOWN}.
     * @return Whether any scrolling, selection or focus change occured.
     */
    bool arrowScrollImpl(int direction);
    
    /**
     * When selection changes, it is possible that the previously selected or the
     * next selected item will change its size.  If so, we need to offset some folks,
     * and re-layout the items as appropriate.
     *
     * @param selectedView The currently selected view (before changing selection).
     *   should be <code>NULL</code> if there was no previous selection.
     * @param direction Either {@link android.view.View#FOCUS_UP} or
     *        {@link android.view.View#FOCUS_DOWN}.
     * @param newSelectedPosition The position of the next selection.
     * @param newFocusAssigned whether new focus was assigned.  This matters because
     *        when something has focus, we don't want to show selection (ugh).
     */
    void handleNewSelectionChange(View *selectedView, int direction, int newSelectedPosition,
                                  bool newFocusAssigned);

    /**
     * Re-measure a child, and if its height changes, lay it out preserving its
     * top, and adjust the children below it appropriately.
     * @param child The child
     * @param childIndex The view group index of the child.
     * @param numChildren The number of children in the view group.
     */
    void measureAndAdjustDown(shared_ptr<View> child, int childIndex, int numChildren);
    
    /**
     * Measure a particular list child.
     * TODO: unify with setUpChild.
     * @param child The child.
     */
    void measureItem(shared_ptr<View> child);
    
    /**
     * Layout a child that has been measured, preserving its top position.
     * TODO: unify with setUpChild.
     * @param child The child.
     */
    void relayoutMeasuredItem(shared_ptr<View> child);
    
    /**
     * @return The amount to preview next items when arrow srolling.
     */
    int getArrowScrollPreviewLength();
    
    /**
     * Determine how much we need to scroll in order to get the next selected view
     * visible, with a fading edge showing below as applicable.  The amount is
     * capped at {@link #getMaxScrollAmount()} .
     *
     * @param direction either {@link android.view.View#FOCUS_UP} or
     *        {@link android.view.View#FOCUS_DOWN}.
     * @param nextSelectedPosition The position of the next selection, or
     *        {@link #INVALID_POSITION} if there is no next selectable position
     * @return The amount to scroll. Note: this is always positive!  Direction
     *         needs to be taken into account when actually scrolling.
     */
    int amountToScroll(int direction, int nextSelectedPosition);
    
    /**
     * @param direction either {@link android.view.View#FOCUS_UP} or
     *        {@link android.view.View#FOCUS_DOWN}.
     * @return The position of the next selectable position of the views that
     *         are currently visible, taking into account the fact that there might
     *         be no selection.  Returns {@link #INVALID_POSITION} if there is no
     *         selectable view on screen in the given direction.
     */
    int lookForSelectablePositionOnScreen(int direction);
    
    /**
     * Do an arrow scroll based on focus searching.  If a new view is
     * given focus, return the selection delta and amount to scroll via
     * an {@link ArrowScrollFocusResult}, otherwise, return NULL.
     *
     * @param direction either {@link android.view.View#FOCUS_UP} or
     *        {@link android.view.View#FOCUS_DOWN}.
     * @return The result if focus has changed, or <code>NULL</code>.
     */
    ArrowScrollFocusResult* arrowScrollFocused(const int direction);
    
    /**
     * @param newFocus The view that would have focus.
     * @return the position that contains newFocus
     */
    int positionOfNewFocus(shared_ptr<View> newFocus);
    
    /**
     * Return true if child is an ancestor of parent, (or equal to the parent).
     */
    bool isViewAncestorOf(shared_ptr<View> child, shared_ptr<View> parent);
    
    /**
     * Determine how much we need to scroll in order to get newFocus in view.
     * @param direction either {@link android.view.View#FOCUS_UP} or
     *        {@link android.view.View#FOCUS_DOWN}.
     * @param newFocus The view that would take focus.
     * @param positionOfNewFocus The position of the list item containing newFocus
     * @return The amount to scroll.  Note: this is always positive!  Direction
     *   needs to be taken into account when actually scrolling.
     */
    int amountToScrollToNewFocus(int direction, shared_ptr<View> newFocus, int positionOfNewFocus);
    
    /**
     * Determine the distance to the nearest edge of a view in a particular
     * direction.
     *
     * @param descendant A descendant of this list.
     * @return The distance, or 0 if the nearest edge is already on screen.
     */
    int distanceToView(shared_ptr<View> descendant);
    
    /**
     * Scroll the children by amount, adding a view at the end and removing
     * views that fall off as necessary.
     *
     * @param amount The amount (positive or negative) to scroll.
     */
    void scrollListItemsBy(int amount);
    
    shared_ptr<View> addViewAbove(shared_ptr<View> theView, int position);
    shared_ptr<View> addViewBelow(shared_ptr<View> theView, int position);
    
    void drawOverscrollHeader(shared_ptr<Canvas> canvas, shared_ptr<Drawable> drawable, Rect bounds);
    void drawOverscrollFooter(shared_ptr<Canvas> canvas, shared_ptr<Drawable> drawable, Rect bounds);
    
    /**
     * Draws a divider for the given child in the given bounds.
     *
     * @param canvas The canvas to draw to.
     * @param bounds The bounds of the divider.
     * @param childIndex The index of child (of the View) above the divider.
     *            This will be -1 if there is no child above the divider to be
     *            drawn.
     */
    void drawDivider(shared_ptr<Canvas> canvas, Rect bounds, int childIndex);
    
    /* (non-Javadoc)
     *
     * Look in the passed in list of headers or footers for the view.
     */
    shared_ptr<View> findViewInHeadersOrFooters(vector<FixedViewInfo> where, string id);
    
    /* (non-Javadoc)
     *
     * Look in the passed in list of headers or footers for the view with the tag.
     */
    shared_ptr<View> findViewWithTagInHeadersOrFooters(vector<FixedViewInfo> where, shared_ptr<Object> tag);
    
    /* (non-Javadoc)
     *
     * Look in the passed in list of headers or footers for the first view that matches
     * the predicate.
     */
//    shared_ptr<View> findViewByPredicateInHeadersOrFooters(vector<FixedViewInfo> where,
//                                                           Predicate<View> predicate, View childToSkip);
    
protected:
    //@Override
    virtual void onSizeChanged(int w, int h, int oldw, int oldh);
    
    //@Override
    virtual void onMeasure(int widthMeasureSpec, int heightMeasureSpec);
    
    /**
     * @return True to recycle the views used to measure this ListView in
     *         UNSPECIFIED/AT_MOST modes, false otherwise.
     * @hide
     */
    //@ViewDebug.ExportedProperty(category = "list")
    virtual bool recycleOnMeasure();
    
    //@Override
    virtual void layoutChildren();

    //@Override
    virtual bool canAnimate();
    
    //@Override
    virtual void dispatchDraw(shared_ptr<Canvas> canvas);
    
    //@Override
    virtual bool drawChild(shared_ptr<Canvas> canvas, shared_ptr<View> child, uint64_t drawingTime);
    
    //@Override
    virtual void onFocusChanged(bool gainFocus, int direction, Rect *previouslyFocusedRect);
    
    /*
     * (non-Javadoc)
     *
     * Children specified in XML are assumed to be header views. After we have
     * parsed them move them out of the children list and into mHeaderViews.
     */
    //@Override
    virtual void onFinishInflate();
    
    /* (non-Javadoc)
     * @see android.view.View#findViewById(int)
     * First look in our children, then in any header and footer views that may be scrolled off.
     */
    //@Override
    shared_ptr<View> findViewTraversal(string id);
    
    /* (non-Javadoc)
     * @see android.view.View#findViewWithTag(Object)
     * First look in our children, then in any header and footer views that may be scrolled off.
     */
    //@Override
    shared_ptr<View> findViewWithTagTraversal(shared_ptr<Object> tag);
    
    /**
     * @hide
     * @see android.view.View#findViewByPredicate(Predicate)
     * First look in our children, then in any header and footer views that may be scrolled off.
     */
    //@Override
//    shared_ptr<View> findViewByPredicateTraversal(Predicate<View> predicate, View childToSkip);


};

ANDROID_END

#endif /* defined(__Androidpp__ListView__) */
