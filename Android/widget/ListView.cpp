//
//  ListView.cpp
//  Androidpp
//
//  Created by Travis Elnicky on 2/5/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#include "ListView.h"

#include "Android/graphics/Paint.h"
#include "Android/graphics/drawable/ColorDrawable.h"
#include "Android/widget/HeaderViewListAdapter.h"
#include "Android/view/LayoutParams.h"

#include <string>

ANDROID_BEGIN

const float ListView::MAX_SCROLL_FACTOR = 0.33f;

ListView::ListView(Context *context) : AbsListView(context, NULL) {
}

ListView::ListView(Context *context, AttributeSet *attrs) : AbsListView(context, attrs) {
    
    for(int i = 0; i < attrs->getAttributeCount(); i++) {
        
        const char *name = attrs->getAttributeName(i);
        if (strcmp(R::styleable::entries, name) == 0) {
// TODO: implement entries for listView
//    CharSequence[] entries = a.getTextArray(
//                                            com.android.internal.R.styleable.ListView_entries);
//    if (entries != NULL) {
//        setAdapter(new ArrayAdapter<CharSequence>(context,
//                                                  com.android.internal.R.layout.simple_list_item_1, entries));
//    }
        } else if (strcmp(R::styleable::divider, name) == 0) {
            shared_ptr<Drawable> d = attrs->getAttributeDrawableValue(context->getResources(), name);
            if (d != NULL) {
                // If a divider is specified use its intrinsic height for divider height
                setDivider(d);
            }
        } else if (strcmp(R::styleable::overScrollHeader, name) == 0) {
            shared_ptr<Drawable> osHeader = attrs->getAttributeDrawableValue(context->getResources(), name);
            if (osHeader != NULL) {
                setOverscrollHeader(osHeader);
            }
        } else if (strcmp(R::styleable::overScrollFooter, name) == 0) {
            shared_ptr<Drawable> osFooter = attrs->getAttributeDrawableValue(context->getResources(), name);
            if (osFooter != NULL) {
                setOverscrollFooter(osFooter);
            }
        } else if (strcmp(R::styleable::dividerHeight, name) == 0) {
            int dividerHeight = attrs->getDimensionValue(context->getResources(), name, 0);
            if (dividerHeight != 0) {
                setDividerHeight(dividerHeight);
            }
        } else if (strcmp(R::styleable::headerDividersEnabled, name) == 0) {
            mHeaderDividersEnabled = attrs->getAttributeBooleanValue(name, true);
        } else if (strcmp(R::styleable::footerDividersEnabled, name) == 0) {
            mFooterDividersEnabled = attrs->getAttributeBooleanValue(name, true);
        }
    }
}

/**
 * @return The maximum amount a list view will scroll in response to
 *   an arrow event.
 */
int ListView::getMaxScrollAmount() {
    return (int) (MAX_SCROLL_FACTOR * (mBottom - mTop));
}

/**
 * Make sure views are touching the top or bottom edge, as appropriate for
 * our gravity
 */
void ListView::adjustViewsUpOrDown() {
    const int childCount = getChildCount();
    int delta;
    
    if (childCount > 0) {
        shared_ptr<View> child;
        
        if (!mStackFromBottom) {
            // Uh-oh -- we came up short. Slide all views up to make them
            // align with the top
            child = getChildAt(0);
            delta = child->getTop() - mListPadding.top();
            if (mFirstPosition != 0) {
                // It's OK to have some space above the first item if it is
                // part of the vertical spacing
                delta -= mDividerHeight;
            }
            if (delta < 0) {
                // We only are looking to see if we are too low, not too high
                delta = 0;
            }
        } else {
            // we are too high, slide all views down to align with bottom
            child = getChildAt(childCount - 1);
            delta = child->getBottom() - (getHeight() - mListPadding.bottom());
            
            if (mFirstPosition + childCount < mItemCount) {
                // It's OK to have some space below the last item if it is
                // part of the vertical spacing
                delta += mDividerHeight;
            }
            
            if (delta > 0) {
                delta = 0;
            }
        }
        
        if (delta != 0) {
            offsetChildrenTopAndBottom(-delta);
        }
    }
}

/**
 * Add a fixed view to appear at the top of the list. If addHeaderView is
 * called more than once, the views will appear in the order they were
 * added. Views added using this call can take focus if they want.
 * <p>
 * NOTE: Call this before calling setAdapter. This is so ListView can wrap
 * the supplied cursor with one that will also account for header and footer
 * views.
 *
 * @param v The view to add.
 * @param data Data to associate with this view
 * @param isSelectable whether the item is selectable
 */
void ListView::addHeaderView(shared_ptr<View> v, shared_ptr<Object> data, bool isSelectable) {

    if (mAdapter != NULL && (dynamic_pointer_cast<HeaderViewListAdapter>(mAdapter) == NULL)) {
        throw IllegalStateException("Cannot add header view to list -- setAdapter has already been called.");
    }
    
    FixedViewInfo info = FixedViewInfo();
    info.view = v;
    info.data = data;
    info.isSelectable = isSelectable;
    mHeaderViewInfos.push_back(info);
    
    // in the case of re-adding a header view, or adding one later on,
    // we need to notify the observer
    if (mAdapter != NULL && mDataSetObserver != NULL) {
        mDataSetObserver->onChanged();
    }
}

/**
 * Add a fixed view to appear at the top of the list. If addHeaderView is
 * called more than once, the views will appear in the order they were
 * added. Views added using this call can take focus if they want.
 * <p>
 * NOTE: Call this before calling setAdapter. This is so ListView can wrap
 * the supplied cursor with one that will also account for header and footer
 * views.
 *
 * @param v The view to add.
 */
void ListView::addHeaderView(shared_ptr<View> v) {
    addHeaderView(v, NULL, true);
}

int ListView::getHeaderViewsCount() {
    return mHeaderViewInfos.size();
}

/**
 * Removes a previously-added header view.
 *
 * @param v The view to remove
 * @return true if the view was removed, false if the view was not a header
 *         view
 */
bool ListView::removeHeaderView(shared_ptr<View> v) {
    if (mHeaderViewInfos.size() > 0) {
        bool result = false;
        if (mAdapter != NULL && dynamic_pointer_cast<HeaderViewListAdapter>(mAdapter)->removeHeader(v)) {
            if (mDataSetObserver != NULL) {
                mDataSetObserver->onChanged();
            }
            result = true;
        }
        removeFixedViewInfo(v, mHeaderViewInfos);
        return result;
    }
    return false;
}

void ListView::removeFixedViewInfo(shared_ptr<View> v, vector<FixedViewInfo> where) {
    vector<FixedViewInfo>::iterator it;
    for (it = where.begin(); it != where.end(); ++it) {
        FixedViewInfo info = *it;
        if (info.view == v) {
            where.erase(it);
            break;
        }
    }
}

/**
 * Add a fixed view to appear at the bottom of the list. If addFooterView is
 * called more than once, the views will appear in the order they were
 * added. Views added using this call can take focus if they want.
 * <p>
 * NOTE: Call this before calling setAdapter. This is so ListView can wrap
 * the supplied cursor with one that will also account for header and footer
 * views.
 *
 * @param v The view to add.
 * @param data Data to associate with this view
 * @param isSelectable true if the footer view can be selected
 */
void ListView::addFooterView(shared_ptr<View> v, shared_ptr<Object> data, bool isSelectable) {
    
    // NOTE: do not enforce the adapter being NULL here, since unlike in
    // addHeaderView, it was never enforced here, and so existing apps are
    // relying on being able to add a footer and then calling setAdapter to
    // force creation of the HeaderViewListAdapter wrapper
    
    FixedViewInfo info;
    info.view = v;
    info.data = data;
    info.isSelectable = isSelectable;
    mFooterViewInfos.push_back(info);
    
    // in the case of re-adding a footer view, or adding one later on,
    // we need to notify the observer
    if (mAdapter != NULL && mDataSetObserver != NULL) {
        mDataSetObserver->onChanged();
    }
}

/**
 * Add a fixed view to appear at the bottom of the list. If addFooterView is called more
 * than once, the views will appear in the order they were added. Views added using
 * this call can take focus if they want.
 * <p>NOTE: Call this before calling setAdapter. This is so ListView can wrap the supplied
 * cursor with one that will also account for header and footer views.
 *
 *
 * @param v The view to add.
 */
void ListView::addFooterView(shared_ptr<View> v) {
    addFooterView(v, NULL, true);
}

int ListView::getFooterViewsCount() {
    return mFooterViewInfos.size();
}

/**
 * Removes a previously-added footer view.
 *
 * @param v The view to remove
 * @return
 * true if the view was removed, false if the view was not a footer view
 */
bool ListView::removeFooterView(shared_ptr<View> v) {
    if (mFooterViewInfos.size() > 0) {
        bool result = false;
        if (mAdapter != NULL && dynamic_pointer_cast<HeaderViewListAdapter>(mAdapter)->removeFooter(v)) {
            if (mDataSetObserver != NULL) {
                mDataSetObserver->onChanged();
            }
            result = true;
        }
        removeFixedViewInfo(v, mFooterViewInfos);
        return result;
    }
    return false;
}

/**
 * Returns the adapter currently in use in this ListView. The returned adapter
 * might not be the same adapter passed to {@link #setAdapter(ListAdapter)} but
 * might be a {@link WrapperListAdapter}.
 *
 * @return The adapter currently used to display data in this ListView.
 *
 * @see #setAdapter(ListAdapter)
 */
shared_ptr<ListAdapter> ListView::getAdapter() {
    return mAdapter;
}

// TODO:
//    /**
//     * Sets up this AbsListView to use a remote views adapter which connects to a RemoteViewsService
//     * through the specified intent.
//     * @param intent the intent used to identify the RemoteViewsService for the adapter to connect to.
//     */
//    @android.view.RemotableViewMethod
//    public void setRemoteViewsAdapter(Intent intent) {
//        super.setRemoteViewsAdapter(intent);
//    }

/**
 * Sets the data behind this ListView.
 *
 * The adapter passed to this method may be wrapped by a {@link WrapperListAdapter},
 * depending on the ListView features currently in use. For instance, adding
 * headers and/or footers will cause the adapter to be wrapped.
 *
 * @param adapter The ListAdapter which is responsible for maintaining the
 *        data backing this list and for producing a view to represent an
 *        item in that data set.
 *
 * @see #getAdapter()
 */
void ListView::setAdapter(shared_ptr<ListAdapter> adapter) {
    if (mAdapter != NULL && mDataSetObserver != NULL) {
        mAdapter->unregisterDataSetObserver(mDataSetObserver);
    }
    
    resetList();
    AbsListView::mRecycler.clear();

    if (mHeaderViewInfos.size() > 0|| mFooterViewInfos.size() > 0) {
        mAdapter = make_shared<HeaderViewListAdapter>(mHeaderViewInfos, mFooterViewInfos, adapter);
    } else {
        mAdapter = adapter;
    }

    mOldSelectedPosition = INVALID_POSITION;
    mOldSelectedRowId = INVALID_ROW_ID;
    
    // AbsListView#setAdapter will update choice mode states.
    AbsListView::setAdapter(adapter);

    if (mAdapter != NULL) {
        mAreAllItemsSelectable = mAdapter->areAllItemsEnabled();
        mOldItemCount = mItemCount;
        mItemCount = mAdapter->getCount();
        checkFocus();
        
        mDataSetObserver = new AdapterDataSetObserver(this);
        mAdapter->registerDataSetObserver(mDataSetObserver);
        
        mRecycler.setViewTypeCount(mAdapter->getViewTypeCount());
        
        int position;
        if (mStackFromBottom) {
            position = lookForSelectablePosition(mItemCount - 1, false);
        } else {
            position = lookForSelectablePosition(0, true);
        }
        setSelectedPositionInt(position);
        setNextSelectedPositionInt(position);
        
        if (mItemCount == 0) {
            // Nothing selected
            checkSelectionChanged();
        }
    } else {
        mAreAllItemsSelectable = true;
        checkFocus();
        // Nothing selected
        checkSelectionChanged();
    }

    requestLayout();
}


/**
 * The list is empty. Clear everything out.
 */
// @Override
void ListView::resetList() {
    // The parent's resetList() will remove all views from the layout so we need to
    // cleanup the state of our footers and headers
    clearRecycledState(mHeaderViewInfos);
    clearRecycledState(mFooterViewInfos);
    
    AbsListView::resetList();
    
    mLayoutMode = LAYOUT_NORMAL;
}

void ListView::clearRecycledState(vector<FixedViewInfo> infos) {

    const int count = infos.size();
    vector<FixedViewInfo>::iterator it;
    for (it = infos.begin(); it != infos.end(); ++it) {
        const shared_ptr<View> child = (*it).view;

        ListViewLayoutParams *p = (ListViewLayoutParams*) child->getLayoutParams();
        if (p != NULL) {
            p->recycledHeaderFooter = false;
        }
    }
}

bool ListView::showingTopFadingEdge() {
    int listTop = mScrollY + mListPadding.top();
    return (mFirstPosition > 0) || (getChildAt(0)->getTop() > listTop);
}

bool ListView::showingBottomFadingEdge() {
    int childCount = getChildCount();
    int bottomOfBottomChild = getChildAt(childCount - 1)->getBottom();
    int lastVisiblePosition = mFirstPosition + childCount - 1;
    
    int listBottom = mScrollY + getHeight() - mListPadding.bottom();
    
    return (lastVisiblePosition < mItemCount - 1)
    || (bottomOfBottomChild < listBottom);
}

bool ListView::requestChildRectangleOnScreen(shared_ptr<View> child, Rect rect, bool immediate) {
    
    int rectTopWithinChild = rect.top();
    
    // offset so rect is in coordinates of the this view
    rect.offset(child->getLeft(), child->getTop());
    rect.offset(-child->getScrollX(), -child->getScrollY());
    
    int height = getHeight();
    int listUnfadedTop = getScrollY();
    int listUnfadedBottom = listUnfadedTop + height;
    int fadingEdge = getVerticalFadingEdgeLength();
    
    if (showingTopFadingEdge()) {
        // leave room for top fading edge as long as rect isn't at very top
        if ((mSelectedPosition > 0) || (rectTopWithinChild > fadingEdge)) {
            listUnfadedTop += fadingEdge;
        }
    }
    
    int childCount = getChildCount();
    int bottomOfBottomChild = getChildAt(childCount - 1)->getBottom();
    
    if (showingBottomFadingEdge()) {
        // leave room for bottom fading edge as long as rect isn't at very bottom
        if ((mSelectedPosition < mItemCount - 1)
            || (rect.bottom() < (bottomOfBottomChild - fadingEdge))) {
            listUnfadedBottom -= fadingEdge;
        }
    }
    
    int scrollYDelta = 0;
    
    if (rect.bottom() > listUnfadedBottom && rect.top() > listUnfadedTop) {
        // need to MOVE DOWN to get it in view: move down just enough so
        // that the entire rectangle is in view (or at least the first
        // screen size chunk).
        
        if (rect.height() > height) {
            // just enough to get screen size chunk on
            scrollYDelta += (rect.top() - listUnfadedTop);
        } else {
            // get entire rect at bottom of screen
            scrollYDelta += (rect.bottom() - listUnfadedBottom);
        }
        
        // make sure we aren't scrolling beyond the end of our children
        int distanceToBottom = bottomOfBottomChild - listUnfadedBottom;
        scrollYDelta = min(scrollYDelta, distanceToBottom);
    } else if (rect.top() < listUnfadedTop && rect.bottom() < listUnfadedBottom) {
        // need to MOVE UP to get it in view: move up just enough so that
        // entire rectangle is in view (or at least the first screen
        // size chunk of it).
        
        if (rect.height() > height) {
            // screen size chunk
            scrollYDelta -= (listUnfadedBottom - rect.bottom());
        } else {
            // entire rect at top
            scrollYDelta -= (listUnfadedTop - rect.top());
        }
        
        // make sure we aren't scrolling any further than the top our children
        int top = getChildAt(0)->getTop();
        int deltaToTop = top - listUnfadedTop;
        scrollYDelta = max(scrollYDelta, deltaToTop);
    }
    
    bool scroll = scrollYDelta != 0;
    if (scroll) {
        scrollListItemsBy(-scrollYDelta);
        positionSelector(child);
        mSelectedTop = child->getTop();
        View::invalidate();
    }
    return scroll;
}

void ListView::fillGap(bool down) {
    const int count = getChildCount();
    if (down) {
        int paddingTop = 0;
        if ((mGroupFlags & CLIP_TO_PADDING_MASK) == CLIP_TO_PADDING_MASK) {
            paddingTop = getListPaddingTop();
        }
        const int startOffset = count > 0 ? getChildAt(count - 1)->getBottom() + mDividerHeight :
        paddingTop;
        fillDown(mFirstPosition + count, startOffset);
        correctTooHigh(getChildCount());
    } else {
        int paddingBottom = 0;
        if ((mGroupFlags & CLIP_TO_PADDING_MASK) == CLIP_TO_PADDING_MASK) {
            paddingBottom = getListPaddingBottom();
        }
        const int startOffset = count > 0 ? getChildAt(0)->getTop() - mDividerHeight :
        getHeight() - paddingBottom;
        fillUp(mFirstPosition - 1, startOffset);
        correctTooLow(getChildCount());
    }
}

shared_ptr<View> ListView::fillDown(int pos, int nextTop) {
    shared_ptr<View> selectedView = NULL;
    
    int end = (mBottom - mTop) - mListPadding.bottom();
    
    if ((mGroupFlags & CLIP_TO_PADDING_MASK) == CLIP_TO_PADDING_MASK) {
        end -= mListPadding.bottom();
    }
    
    while (nextTop < end && pos < mItemCount) {
        // is this the selected item?
        bool selected = pos == mSelectedPosition;
        shared_ptr<View> child = makeAndAddView(pos, nextTop, true, mListPadding.left(), selected);
        
        nextTop = child->getBottom() + mDividerHeight;
        if (selected) {
            selectedView = child;
        }
        pos++;
    }
    
    return selectedView;
}

shared_ptr<View> ListView::fillUp(int pos, int nextBottom) {
    shared_ptr<View> selectedView = NULL;
    
    int end = mListPadding.top();
    if ((mGroupFlags & CLIP_TO_PADDING_MASK) == CLIP_TO_PADDING_MASK) {
        end = mListPadding.top();
    }
    
    while (nextBottom > end && pos >= 0) {
        // is this the selected item?
        bool selected = pos == mSelectedPosition;
        shared_ptr<View> child = makeAndAddView(pos, nextBottom, false, mListPadding.left(), selected);
        nextBottom = child->getTop() - mDividerHeight;
        if (selected) {
            selectedView = child;
        }
        pos--;
    }
    
    mFirstPosition = pos + 1;
    
    return selectedView;
}

/**
 * Fills the list from top to bottom, starting with mFirstPosition
 *
 * @param nextTop The location where the top of the first item should be
 *        drawn
 *
 * @return The view that is currently selected
 */
shared_ptr<View> ListView::fillFromTop(int nextTop) {
    mFirstPosition = min(mFirstPosition, mSelectedPosition);
    mFirstPosition = min(mFirstPosition, mItemCount - 1);
    if (mFirstPosition < 0) {
        mFirstPosition = 0;
    }
    return fillDown(mFirstPosition, nextTop);
}

shared_ptr<View> ListView::fillFromMiddle(int childrenTop, int childrenBottom) {
    int height = childrenBottom - childrenTop;
    
    int position = reconcileSelectedPosition();
    
    shared_ptr<View> sel = makeAndAddView(position, childrenTop, true,
                                          mListPadding.left(), true);
    mFirstPosition = position;
    
    int selHeight = sel->getMeasuredHeight();
    if (selHeight <= height) {
        sel->offsetTopAndBottom((height - selHeight) / 2);
    }
    
    fillAboveAndBelow(sel, position);
    
    if (!mStackFromBottom) {
        correctTooHigh(getChildCount());
    } else {
        correctTooLow(getChildCount());
    }
    
    return sel;
}

void ListView::fillAboveAndBelow(shared_ptr<View> sel, int position) {
    const int dividerHeight = mDividerHeight;
    if (!mStackFromBottom) {
        fillUp(position - 1, sel->getTop() - dividerHeight);
        adjustViewsUpOrDown();
        fillDown(position + 1, sel->getBottom() + dividerHeight);
    } else {
        fillDown(position + 1, sel->getBottom() + dividerHeight);
        adjustViewsUpOrDown();
        fillUp(position - 1, sel->getTop() - dividerHeight);
    }
}

shared_ptr<View> ListView::fillFromSelection(int selectedTop, int childrenTop, int childrenBottom) {
    int fadingEdgeLength = getVerticalFadingEdgeLength();
    const int selectedPosition = mSelectedPosition;
    
    shared_ptr<View> sel;

    const int topSelectionPixel = getTopSelectionPixel(childrenTop, fadingEdgeLength,
                                                       selectedPosition);
    const int bottomSelectionPixel = getBottomSelectionPixel(childrenBottom, fadingEdgeLength,
                                                             selectedPosition);
    
    sel = makeAndAddView(selectedPosition, selectedTop, true, mListPadding.left(), true);
    
    
    // Some of the newly selected item extends below the bottom of the list
    if (sel->getBottom() > bottomSelectionPixel) {
        // Find space available above the selection into which we can scroll
        // upwards
        const int spaceAbove = sel->getTop() - topSelectionPixel;
        
        // Find space required to bring the bottom of the selected item
        // fully into view
        const int spaceBelow = sel->getBottom() - bottomSelectionPixel;
        const int offset = min(spaceAbove, spaceBelow);
        
        // Now offset the selected item to get it into view
        sel->offsetTopAndBottom(-offset);
    } else if (sel->getTop() < topSelectionPixel) {
        // Find space required to bring the top of the selected item fully
        // into view
        const int spaceAbove = topSelectionPixel - sel->getTop();
        
        // Find space available below the selection into which we can scroll
        // downwards
        const int spaceBelow = bottomSelectionPixel - sel->getBottom();
        const int offset = min(spaceAbove, spaceBelow);
        
        // Offset the selected item to get it into view
        sel->offsetTopAndBottom(offset);
    }
    
    // Fill in views above and below
    fillAboveAndBelow(sel, selectedPosition);
    
    if (!mStackFromBottom) {
        correctTooHigh(getChildCount());
    } else {
        correctTooLow(getChildCount());
    }
    
    return sel;
}

int ListView::getBottomSelectionPixel(int childrenBottom, int fadingEdgeLength, int selectedPosition) {
    int bottomSelectionPixel = childrenBottom;
    if (selectedPosition != mItemCount - 1) {
        bottomSelectionPixel -= fadingEdgeLength;
    }
    return bottomSelectionPixel;
}

int ListView::getTopSelectionPixel(int childrenTop, int fadingEdgeLength, int selectedPosition) {
    // first pixel we can draw the selection into
    int topSelectionPixel = childrenTop;
    if (selectedPosition > 0) {
        topSelectionPixel += fadingEdgeLength;
    }
    return topSelectionPixel;
}

void ListView::smoothScrollToPosition(int position) {
// TODO:    AbsListView::smoothScrollToPosition(position);
}

void ListView::smoothScrollByOffset(int offset) {
// TODO:    smoothScrollByOffset(offset);
}

shared_ptr<View> ListView::moveSelection(shared_ptr<View> oldSel, shared_ptr<View> newSel, int delta, int childrenTop, int childrenBottom) {
    int fadingEdgeLength = getVerticalFadingEdgeLength();
    const int selectedPosition = mSelectedPosition;
    
    shared_ptr<View> sel;
    
    const int topSelectionPixel = getTopSelectionPixel(childrenTop, fadingEdgeLength,
                                                       selectedPosition);
    const int bottomSelectionPixel = getBottomSelectionPixel(childrenTop, fadingEdgeLength,
                                                             selectedPosition);
    
    if (delta > 0) {
        /*
         * Case 1: Scrolling down.
         */
        
        /*
         *     Before           After
         *    |       |        |       |
         *    +-------+        +-------+
         *    |   A   |        |   A   |
         *    |   1   |   =>   +-------+
         *    +-------+        |   B   |
         *    |   B   |        |   2   |
         *    +-------+        +-------+
         *    |       |        |       |
         *
         *    Try to keep the top of the previously selected item where it was.
         *    oldSel = A
         *    sel = B
         */
        
        // Put oldSel (A) where it belongs
        oldSel = makeAndAddView(selectedPosition - 1, oldSel->getTop(), true,
                                mListPadding.left(), false);
        
        const int dividerHeight = mDividerHeight;
        
        // Now put the new selection (B) below that
        sel = makeAndAddView(selectedPosition, oldSel->getBottom() + dividerHeight, true,
                             mListPadding.left(), true);
        
        // Some of the newly selected item extends below the bottom of the list
        if (sel->getBottom() > bottomSelectionPixel) {
            
            // Find space available above the selection into which we can scroll upwards
            int spaceAbove = sel->getTop() - topSelectionPixel;
            
            // Find space required to bring the bottom of the selected item fully into view
            int spaceBelow = sel->getBottom() - bottomSelectionPixel;
            
            // Don't scroll more than half the height of the list
            int halfVerticalSpace = (childrenBottom - childrenTop) / 2;
            int offset = min(spaceAbove, spaceBelow);
            offset = min(offset, halfVerticalSpace);
            
            // We placed oldSel, so offset that item
            oldSel->offsetTopAndBottom(-offset);
            // Now offset the selected item to get it into view
            sel->offsetTopAndBottom(-offset);
        }
        
        // Fill in views above and below
        if (!mStackFromBottom) {
            fillUp(mSelectedPosition - 2, sel->getTop() - dividerHeight);
            adjustViewsUpOrDown();
            fillDown(mSelectedPosition + 1, sel->getBottom() + dividerHeight);
        } else {
            fillDown(mSelectedPosition + 1, sel->getBottom() + dividerHeight);
            adjustViewsUpOrDown();
            fillUp(mSelectedPosition - 2, sel->getTop() - dividerHeight);
        }
    } else if (delta < 0) {
        /*
         * Case 2: Scrolling up.
         */
        
        /*
         *     Before           After
         *    |       |        |       |
         *    +-------+        +-------+
         *    |   A   |        |   A   |
         *    +-------+   =>   |   1   |
         *    |   B   |        +-------+
         *    |   2   |        |   B   |
         *    +-------+        +-------+
         *    |       |        |       |
         *
         *    Try to keep the top of the item about to become selected where it was.
         *    newSel = A
         *    olSel = B
         */
        
        if (newSel != NULL) {
            // Try to position the top of newSel (A) where it was before it was selected
            sel = makeAndAddView(selectedPosition, newSel->getTop(), true, mListPadding.left(),
                                 true);
        } else {
            // If (A) was not on screen and so did not have a view, position
            // it above the oldSel (B)
            sel = makeAndAddView(selectedPosition, oldSel->getTop(), false, mListPadding.left(),
                                 true);
        }
        
        // Some of the newly selected item extends above the top of the list
        if (sel->getTop() < topSelectionPixel) {
            // Find space required to bring the top of the selected item fully into view
            int spaceAbove = topSelectionPixel - sel->getTop();
            
            // Find space available below the selection into which we can scroll downwards
            int spaceBelow = bottomSelectionPixel - sel->getBottom();
            
            // Don't scroll more than half the height of the list
            int halfVerticalSpace = (childrenBottom - childrenTop) / 2;
            int offset = min(spaceAbove, spaceBelow);
            offset = min(offset, halfVerticalSpace);
            
            // Offset the selected item to get it into view
            sel->offsetTopAndBottom(offset);
        }
        
        // Fill in views above and below
        fillAboveAndBelow(sel, selectedPosition);
    } else {
        
        int oldTop = oldSel->getTop();
        
        /*
         * Case 3: Staying still
         */
        sel = makeAndAddView(selectedPosition, oldTop, true, mListPadding.left(), true);
        
        // We're staying still...
        if (oldTop < childrenTop) {
            // ... but the top of the old selection was off screen.
            // (This can happen if the data changes size out from under us)
            int newBottom = sel->getBottom();
            if (newBottom < childrenTop + 20) {
                // Not enough visible -- bring it onscreen
                sel->offsetTopAndBottom(childrenTop - sel->getTop());
            }
        }
        
        // Fill in views above and below
        fillAboveAndBelow(sel, selectedPosition);
    }
    
    return sel;
}

void ListView::onSizeChanged(int w, int h, int oldw, int oldh) {
    if (getChildCount() > 0) {
        shared_ptr<View> focusedChild = ViewGroup::getFocusedChild();
        if (focusedChild != NULL) {
            const int childPosition = mFirstPosition + (indexOfChild(focusedChild) - getChildrenViews().begin());
            const int childBottom = focusedChild->getBottom();
            const int offset = max(0, childBottom - (h - mPaddingTop));
            const int top = focusedChild->getTop() - offset;
            if (mFocusSelector == NULL) {
                mFocusSelector = new FocusSelector(this);
            }
            post(mFocusSelector->setup(childPosition, top));
        }
    }
    AbsListView::onSizeChanged(w, h, oldw, oldh);
}

void ListView::onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
    // Sets up mListPadding
    AbsListView::onMeasure(widthMeasureSpec, heightMeasureSpec);
    
    int widthMode = MeasureSpec::getMode(widthMeasureSpec);
    int heightMode = MeasureSpec::getMode(heightMeasureSpec);
    int widthSize = MeasureSpec::getSize(widthMeasureSpec);
    int heightSize = MeasureSpec::getSize(heightMeasureSpec);
    
    int childWidth = 0;
    int childHeight = 0;
    int childState = 0;
    
    mItemCount = mAdapter == NULL ? 0 : mAdapter->getCount();
    if (mItemCount > 0 && (widthMode == MeasureSpec::UNSPECIFIED ||
                           heightMode == MeasureSpec::UNSPECIFIED)) {
        const shared_ptr<View> child = obtainView(0);
        
        measureScrapChild(child, 0, widthMeasureSpec);
        
        childWidth = child->getMeasuredWidth();
        childHeight = child->getMeasuredHeight();
        childState = combineMeasuredStates(childState, child->getMeasuredState());
        
        if (recycleOnMeasure()) {
            mRecycler.addScrapView(child);
        }
    }
    
    if (widthMode == MeasureSpec::UNSPECIFIED) {
        widthSize = mListPadding.left() + mListPadding.right() + childWidth + getVerticalScrollbarWidth();
    } else {
        widthSize |= (childState & MEASURED_STATE_MASK);
    }
    
    if (heightMode == MeasureSpec::UNSPECIFIED) {
        heightSize = mListPadding.top() + mListPadding.bottom() + childHeight +
        getVerticalFadingEdgeLength() * 2;
    }
    
    if (heightMode == MeasureSpec::AT_MOST) {
        // TODO: after first layout we should maybe start at the first visible position, not 0
        heightSize = measureHeightOfChildren(widthMeasureSpec, 0, NO_POSITION, heightSize, -1);
    }
    
    setMeasuredDimension(widthSize , heightSize);
    mWidthMeasureSpec = widthMeasureSpec;
}

void ListView::measureScrapChild(shared_ptr<View> child, int position, int widthMeasureSpec) {
    ListViewLayoutParams *p = (ListViewLayoutParams*) child->getLayoutParams();

    if (p == NULL) {
        p = new ListViewLayoutParams(LayoutParams::MATCH_PARENT, LayoutParams::WRAP_CONTENT, 0);
        child->setLayoutParams(p);
    }
    p->viewType = mAdapter->getItemViewType(position);
    
    int childWidthSpec = ViewGroup::getChildMeasureSpec(widthMeasureSpec, mListPadding.left() + mListPadding.right(), p->width);
    int lpHeight = p->height;
    int childHeightSpec;
    if (lpHeight > 0) {
        childHeightSpec = MeasureSpec::makeMeasureSpec(lpHeight, MeasureSpec::EXACTLY);
    } else {
        childHeightSpec = MeasureSpec::makeMeasureSpec(0, MeasureSpec::UNSPECIFIED);
    }
    child->measure(childWidthSpec, childHeightSpec);
}

/**
 * @return True to recycle the views used to measure this ListView in
 *         UNSPECIFIED/AT_MOST modes, false otherwise.
 * @hide
 */
bool ListView::recycleOnMeasure() {
    return true;
}

const int ListView::measureHeightOfChildren(int widthMeasureSpec, int startPosition, int endPosition, const int maxHeight, int disallowPartialChildPosition) {
    
    shared_ptr<ListAdapter> &adapter = mAdapter;
    if (adapter == NULL) {
        return mListPadding.top() + mListPadding.bottom();
    }
    
    // Include the padding of the list
    int returnedHeight = mListPadding.top() + mListPadding.bottom();
    const int dividerHeight = ((mDividerHeight > 0) && mDivider != NULL) ? mDividerHeight : 0;
    // The previous height value that was less than maxHeight and contained
    // no partial children
    int prevHeightWithoutPartialChild = 0;
    int i;
    shared_ptr<View> child;
    
    // mItemCount - 1 since endPosition parameter is inclusive
    endPosition = (endPosition == NO_POSITION) ? adapter->getCount() - 1 : endPosition;
    AbsListView::RecycleBin recycleBin = mRecycler;
    const bool recyle = recycleOnMeasure();
    
    for (i = startPosition; i <= endPosition; ++i) {
        child = obtainView(i);
        
        measureScrapChild(child, i, widthMeasureSpec);
        
        if (i > 0) {
            // Count the divider for all but one child
            returnedHeight += dividerHeight;
        }
        
        // Recycle the view before we possibly return from the method
        if (recyle) {
            recycleBin.addScrapView(child);
        }
        
        returnedHeight += child->getMeasuredHeight();
        
        if (returnedHeight >= maxHeight) {
            // We went over, figure out which height to return.  If returnedHeight > maxHeight,
            // then the i'th position did not fit completely.
            return (disallowPartialChildPosition >= 0) // Disallowing is enabled (> -1)
            && (i > disallowPartialChildPosition) // We've past the min pos
            && (prevHeightWithoutPartialChild > 0) // We have a prev height
            && (returnedHeight != maxHeight) // i'th child did not fit completely
            ? prevHeightWithoutPartialChild
            : maxHeight;
        }
        
        if ((disallowPartialChildPosition >= 0) && (i >= disallowPartialChildPosition)) {
            prevHeightWithoutPartialChild = returnedHeight;
        }
    }
    
    // At this point, we went through the range of children, and they each
    // completely fit, so return the returnedHeight
    return returnedHeight;
}

int ListView::findMotionRow(int y) {
    int childCount = getChildCount();
    if (childCount > 0) {
        if (!mStackFromBottom) {
            for (int i = 0; i < childCount; i++) {
                shared_ptr<View> v = getChildAt(i);
                if (y <= v->getBottom()) {
                    return mFirstPosition + i;
                }
            }
        } else {
            for (int i = childCount - 1; i >= 0; i--) {
                shared_ptr<View> v = getChildAt(i);
                if (y >= v->getTop()) {
                    return mFirstPosition + i;
                }
            }
        }
    }
    return INVALID_POSITION;
}

shared_ptr<View> ListView::fillSpecific(int position, int top) {
    bool tempIsSelected = position == mSelectedPosition;
    shared_ptr<View> temp = makeAndAddView(position, top, true, mListPadding.left(), tempIsSelected);

    // Possibly changed again in fillUp if we add rows above this one.
    mFirstPosition = position;
    
    shared_ptr<View> above;
    shared_ptr<View> below;
    
    const int dividerHeight = mDividerHeight;
    if (!mStackFromBottom) {
        above = fillUp(position - 1, temp->getTop() - dividerHeight);
        // This will correct for the top of the first view not touching the top of the list
        adjustViewsUpOrDown();
        below = fillDown(position + 1, temp->getBottom() + dividerHeight);
        int childCount = getChildCount();
        if (childCount > 0) {
            correctTooHigh(childCount);
        }
    } else {
        below = fillDown(position + 1, temp->getBottom() + dividerHeight);
        // This will correct for the bottom of the last view not touching the bottom of the list
        adjustViewsUpOrDown();
        above = fillUp(position - 1, temp->getTop() - dividerHeight);
        int childCount = getChildCount();
        if (childCount > 0) {
            correctTooLow(childCount);
        }
    }
    
    if (tempIsSelected) {
        return temp;
    } else if (above != NULL) {
        return above;
    } else {
        return below;
    }
}

void ListView::correctTooHigh(int childCount) {
    // First see if the last item is visible. If it is not, it is OK for the
    // top of the list to be pushed up.
    int lastPosition = mFirstPosition + childCount - 1;
    if (lastPosition == mItemCount - 1 && childCount > 0) {
        
        // Get the last child ...
        const shared_ptr<View> lastChild = getChildAt(childCount - 1);
        
        // ... and its bottom edge
        const int lastBottom = lastChild->getBottom();
        
        // This is bottom of our drawable area
        const int end = (mBottom - mTop) - mListPadding.bottom();
        
        // This is how far the bottom edge of the last view is from the bottom of the
        // drawable area
        int bottomOffset = end - lastBottom;
        shared_ptr<View> firstChild = getChildAt(0);
        const int firstTop = firstChild->getTop();
        
        // Make sure we are 1) Too high, and 2) Either there are more rows above the
        // first row or the first row is scrolled off the top of the drawable area
        if (bottomOffset > 0 && (mFirstPosition > 0 || firstTop < mListPadding.top()))  {
            if (mFirstPosition == 0) {
                // Don't pull the top too far down
                bottomOffset = min(bottomOffset, mListPadding.top() - firstTop);
            }
            // Move everything down
            offsetChildrenTopAndBottom(bottomOffset);
            if (mFirstPosition > 0) {
                // Fill the gap that was opened above mFirstPosition with more rows, if
                // possible
                fillUp(mFirstPosition - 1, firstChild->getTop() - mDividerHeight);
                // Close up the remaining gap
                adjustViewsUpOrDown();
            }
            
        }
    }
}

void ListView::correctTooLow(int childCount) {
    // First see if the first item is visible. If it is not, it is OK for the
    // bottom of the list to be pushed down.
    if (mFirstPosition == 0 && childCount > 0) {
        
        // Get the first child ...
        const shared_ptr<View> firstChild = getChildAt(0);
        
        // ... and its top edge
        const int firstTop = firstChild->getTop();
        
        // This is top of our drawable area
        const int start = mListPadding.top();
        
        // This is bottom of our drawable area
        const int end = (mBottom - mTop) - mListPadding.bottom();
        
        // This is how far the top edge of the first view is from the top of the
        // drawable area
        int topOffset = firstTop - start;
        shared_ptr<View> lastChild = getChildAt(childCount - 1);
        const int lastBottom = lastChild->getBottom();
        int lastPosition = mFirstPosition + childCount - 1;
        
        // Make sure we are 1) Too low, and 2) Either there are more rows below the
        // last row or the last row is scrolled off the bottom of the drawable area
        if (topOffset > 0) {
            if (lastPosition < mItemCount - 1 || lastBottom > end)  {
                if (lastPosition == mItemCount - 1) {
                    // Don't pull the bottom too far up
                    topOffset = min(topOffset, lastBottom - end);
                }
                // Move everything up
                offsetChildrenTopAndBottom(-topOffset);
                if (lastPosition < mItemCount - 1) {
                    // Fill the gap that was opened below the last position with more rows, if
                    // possible
                    fillDown(lastPosition + 1, lastChild->getBottom() + mDividerHeight);
                    // Close up the remaining gap
                    adjustViewsUpOrDown();
                }
            } else if (lastPosition == mItemCount - 1) {
                adjustViewsUpOrDown();
            }
        }
    }
}

void ListView::layoutChildren() {
    const bool blockLayoutRequests = mBlockLayoutRequests;
    if (!blockLayoutRequests) {
        mBlockLayoutRequests = true;
    } else {
        return;
    }
    
    try {
        AbsListView::layoutChildren();
        
        View::invalidate();
        
        if (mAdapter == NULL) {
            resetList();
            invokeOnItemScrollListener();
            return;
        }
        
        int childrenTop = mListPadding.top();
        int childrenBottom = mBottom - mTop - mListPadding.bottom();
        
        int childCount = getChildCount();
        int index = 0;
        int delta = 0;
        
        shared_ptr<View> sel;
        shared_ptr<View> oldSel;
        shared_ptr<View> oldFirst;
        shared_ptr<View> newSel;
        
        shared_ptr<View> focusLayoutRestoreView;
// TODO:
//        AccessibilityNodeInfo accessibilityFocusLayoutRestoreNode = NULL;
//        View accessibilityFocusLayoutRestoreView = NULL;
//        int accessibilityFocusPosition = INVALID_POSITION;
        
        // Remember stuff we will need down below
        switch (mLayoutMode) {
            case LAYOUT_SET_SELECTION:
                index = mNextSelectedPosition - mFirstPosition;
                if (index >= 0 && index < childCount) {
                    newSel = getChildAt(index);
                }
                break;
            case LAYOUT_FORCE_TOP:
            case LAYOUT_FORCE_BOTTOM:
            case LAYOUT_SPECIFIC:
            case LAYOUT_SYNC:
                break;
            case LAYOUT_MOVE_SELECTION:
            default:
                // Remember the previously selected view
                index = mSelectedPosition - mFirstPosition;
                if (index >= 0 && index < childCount) {
                    oldSel = getChildAt(index);
                }
                
                // Remember the previous first child
                oldFirst = getChildAt(0);
                
                if (mNextSelectedPosition >= 0) {
                    delta = mNextSelectedPosition - mSelectedPosition;
                }
                
                // Caution: newSel might be NULL
                newSel = getChildAt(index + delta);
        }
        
        
        bool dataChanged = mDataChanged;
        if (dataChanged) {
            handleDataChanged();
        }
        
        // Handle the empty set by removing all views that are visible
        // and calling it a day
        if (mItemCount == 0) {
            resetList();
            invokeOnItemScrollListener();
            return;
        } else if (mItemCount != mAdapter->getCount()) {
            throw IllegalStateException("The content of the adapter has changed but ListView did not receive a notification. Make sure the content of your adapter is not modified from a background thread, but only from the UI thread. [in ListView(" + getId() + ", ) with Adapter()]");
        }
        
        setSelectedPositionInt(mNextSelectedPosition);
        
        // Remember which child, if any, had accessibility focus. This must
        // occur before recycling any views, since that will clear
        // accessibility focus.
        const ViewRootImpl *viewRootImpl = getViewRootImpl();
        if (viewRootImpl != NULL) {
// TODO:
//            const View accessFocusedView = viewRootImpl->getAccessibilityFocusedHost();
//            if (accessFocusedView != NULL) {
//                const View accessFocusedChild = findAccessibilityFocusedChild(
//                                                                              accessFocusedView);
//                if (accessFocusedChild != NULL) {
//                    if (!dataChanged || isDirectChildHeaderOrFooter(accessFocusedChild)) {
//                        // If the views won't be changing, try to maintain
//                        // focus on the current view host and (if
//                        // applicable) its virtual view.
//                        accessibilityFocusLayoutRestoreView = accessFocusedView;
//                        accessibilityFocusLayoutRestoreNode = viewRootImpl
//                        .getAccessibilityFocusedVirtualView();
//                    } else {
//                        // Otherwise, try to maintain focus at the same
//                        // position.
//                        accessibilityFocusPosition = getPositionForView(accessFocusedChild);
//                    }
//                }
//            }
        }
        
        // Pull all children into the RecycleBin.
        // These views will be reused if possible
        const int firstPosition = mFirstPosition;
        RecycleBin recycleBin = mRecycler;
        
        // reset the focus restoration
        shared_ptr<View> focusLayoutRestoreDirectChild;
        
        // Don't put header or footer views into the Recycler. Those are
        // already cached in mHeaderViews;
        if (dataChanged) {
            for (int i = 0; i < childCount; i++) {
                recycleBin.addScrapView(getChildAt(i));
            }
        } else {
            recycleBin.fillActiveViews(childCount, firstPosition);
        }
        
        // take focus back to us temporarily to avoid the eventual
        // call to clear focus when removing the focused child below
        // from messing things up when ViewAncestor assigns focus back
        // to someone else
        const shared_ptr<View> focusedChild = getFocusedChild();
        if (focusedChild != NULL) {
            // TODO: in some cases focusedChild.getParent() == NULL
            
            // we can remember the focused view to restore after relayout if the
            // data hasn't changed, or if the focused position is a header or footer
            if (!dataChanged || isDirectChildHeaderOrFooter(focusedChild)) {
                focusLayoutRestoreDirectChild = focusedChild;
                // remember the specific view that had focus
                focusLayoutRestoreView = findFocus();
                if (focusLayoutRestoreView != NULL) {
                    // tell it we are going to mess with it
                    focusLayoutRestoreView->onStartTemporaryDetach();
                }
            }
            requestFocus();
        }
        
        // Clear out old views
        detachAllViewsFromParent();
        
        switch (mLayoutMode) {
            case LAYOUT_SET_SELECTION:
                if (newSel != NULL) {
                    sel = fillFromSelection(newSel->getTop(), childrenTop, childrenBottom);
                } else {
                    sel = fillFromMiddle(childrenTop, childrenBottom);
                }
                break;
            case LAYOUT_SYNC:
                sel = fillSpecific(mSyncPosition, mSpecificTop);
                break;
            case LAYOUT_FORCE_BOTTOM:
                sel = fillUp(mItemCount - 1, childrenBottom);
                adjustViewsUpOrDown();
                break;
            case LAYOUT_FORCE_TOP:
                mFirstPosition = 0;
                sel = fillFromTop(childrenTop);
                adjustViewsUpOrDown();
                break;
            case LAYOUT_SPECIFIC:
                sel = fillSpecific(reconcileSelectedPosition(), mSpecificTop);
                break;
            case LAYOUT_MOVE_SELECTION:
                sel = moveSelection(oldSel, newSel, delta, childrenTop, childrenBottom);
                break;
            default:
                if (childCount == 0) {
                    if (!mStackFromBottom) {
                        const int position = lookForSelectablePosition(0, true);
                        setSelectedPositionInt(position);
                        sel = fillFromTop(childrenTop);
                    } else {
                        const int position = lookForSelectablePosition(mItemCount - 1, false);
                        setSelectedPositionInt(position);
                        sel = fillUp(mItemCount - 1, childrenBottom);
                    }
                } else {
                    if (mSelectedPosition >= 0 && mSelectedPosition < mItemCount) {
                        sel = fillSpecific(mSelectedPosition,
                                           oldSel == NULL ? childrenTop : oldSel->getTop());
                    } else if (mFirstPosition < mItemCount) {
                        sel = fillSpecific(mFirstPosition,
                                           oldFirst == NULL ? childrenTop : oldFirst->getTop());
                    } else {
                        sel = fillSpecific(0, childrenTop);
                    }
                }
                break;
        }
        
        // Flush any cached views that did not get reused above
        recycleBin.scrapActiveViews();
        
        if (sel != NULL) {
            // the current selected item should get focus if items
            // are focusable
            if (mItemsCanFocus && hasFocus() && !sel->hasFocus()) {
                const bool focusWasTaken = (sel == focusLayoutRestoreDirectChild &&
                                               focusLayoutRestoreView->requestFocus()) || sel->requestFocus();
                if (!focusWasTaken) {
                    // selected item didn't take focus, fine, but still want
                    // to make sure something else outside of the selected view
                    // has focus
                    shared_ptr<View> focused = getFocusedChild();
                    if (focused != NULL) {
                        focused->clearFocus();
                    }
                    positionSelector(sel);
                } else {
                    sel->setSelected(false);
                    mSelectorRect.setEmpty();
                }
            } else {
                positionSelector(sel);
            }
            mSelectedTop = sel->getTop();
        } else {
            mSelectedTop = 0;
            mSelectorRect.setEmpty();
            
            // even if there is not selected position, we may need to restore
            // focus (i.e. something focusable in touch mode)
            if (hasFocus() && focusLayoutRestoreView != NULL) {
                focusLayoutRestoreView->requestFocus();
            }
        }
        
        // Attempt to restore accessibility focus.
// TODO:
//        if (accessibilityFocusLayoutRestoreView != NULL) {
//            const AccessibilityNodeProvider provider =
//            accessibilityFocusLayoutRestoreView.getAccessibilityNodeProvider();
//            if ((accessibilityFocusLayoutRestoreNode != NULL) && (provider != NULL)) {
//                const int virtualViewId = AccessibilityNodeInfo.getVirtualDescendantId(
//                                                                                       accessibilityFocusLayoutRestoreNode.getSourceNodeId());
//                provider.performAction(virtualViewId,
//                                       AccessibilityNodeInfo.ACTION_ACCESSIBILITY_FOCUS, NULL);
//            } else {
//                accessibilityFocusLayoutRestoreView.requestAccessibilityFocus();
//            }
//        } else if (accessibilityFocusPosition != INVALID_POSITION) {
//            // Bound the position within the visible children.
//            const int position = MathUtils.constrain(
//                                                     (accessibilityFocusPosition - mFirstPosition), 0, (getChildCount() - 1));
//            const View restoreView = getChildAt(position);
//            if (restoreView != NULL) {
//                restoreView.requestAccessibilityFocus();
//            }
//        }
        
        // tell focus view we are done mucking with it, if it is still in
        // our view hierarchy.
// TODO:
//        if (focusLayoutRestoreView != NULL
//            && focusLayoutRestoreView->getWindowToken() != NULL) {
//            focusLayoutRestoreView->onFinishTemporaryDetach();
//        }
        
        mLayoutMode = LAYOUT_NORMAL;
        mDataChanged = false;
        mNeedSync = false;
        setNextSelectedPositionInt(mSelectedPosition);
        
        updateScrollIndicators();
        
        if (mItemCount > 0) {
            checkSelectionChanged();
        }
        
        invokeOnItemScrollListener();
    } catch(Exception e) {
        
    }
    
    if (!blockLayoutRequests) {
        mBlockLayoutRequests = false;
    }
}

// TODO:
//    /**
//     * @param focusedView the view that has accessibility focus.
//     * @return the direct child that contains accessibility focus.
//     */
//    private View findAccessibilityFocusedChild(View focusedView) {
//        ViewParent viewParent = focusedView.getParent();
//        while ((viewParent instanceof View) && (viewParent != this)) {
//            focusedView = (View) viewParent;
//            viewParent = viewParent.getParent();
//        }
//        if (!(viewParent instanceof View)) {
//            return NULL;
//        }
//        return focusedView;
//    }

bool ListView::isDirectChildHeaderOrFooter(shared_ptr<View> child) {
    
    
    const vector<FixedViewInfo> headers = mHeaderViewInfos;
    const int numHeaders = headers.size();
    for (int i = 0; i < numHeaders; i++) {
        if (child == headers.at(i).view) {
            return true;
        }
    }
    const vector<FixedViewInfo> footers = mFooterViewInfos;
    const int numFooters = footers.size();
    for (int i = 0; i < numFooters; i++) {
        if (child == footers.at(i).view) {
            return true;
        }
    }
    return false;
}

shared_ptr<View> ListView::makeAndAddView(int position, int y, bool flow, int childrenLeft, bool selected) {
    shared_ptr<View> child;
    
    if (!mDataChanged) {
        // Try to use an existing view for this position
        child = mRecycler.getActiveView(position);
        if (child != NULL) {
            // Found it -- we're using an existing child
            // This just needs to be positioned
            setupChild(child, position, y, flow, childrenLeft, selected, true);
            
            return child;
        }
    }
    
    // Make a new view for this position, or convert an unused view if possible
    child = obtainView(position);
    
    // This needs to be positioned and measured
    setupChild(child, position, y, flow, childrenLeft, selected, false);
    
    return child;
}

void ListView::setupChild(shared_ptr<View> child, int position, int y, bool flowDown, int childrenLeft,
                        bool selected, bool recycled) {
    const bool isSelected = selected && shouldShowSelector();
    const bool updateChildSelected = isSelected != child->isSelected();
    const bool needToMeasure = !recycled || updateChildSelected || child->isLayoutRequested();
    
    // Respect layout params that are already in the view. Otherwise make some up...
    // noinspection unchecked
    ListViewLayoutParams *p = (ListViewLayoutParams*) child->getLayoutParams();
    if (p == NULL) {
        p = new ListViewLayoutParams(LayoutParams::MATCH_PARENT,
                                         LayoutParams::WRAP_CONTENT, 0);
    }
    p->viewType = mAdapter->getItemViewType(position);
    
    if (recycled || (p->recycledHeaderFooter &&
                     p->viewType == ITEM_VIEW_TYPE_HEADER_OR_FOOTER)) {
        attachViewToParent(child, flowDown ? -1 : 0, p);
    } else {
        if (p->viewType == ITEM_VIEW_TYPE_HEADER_OR_FOOTER) {
            p->recycledHeaderFooter = true;
        }
        addViewInLayout(child, flowDown ? -1 : 0, p, true);
    }
    
    if (updateChildSelected) {
        child->setSelected(isSelected);
    }
    
//    if (mChoiceMode != CHOICE_MODE_NONE && mCheckStates != NULL) {
//        if (child instanceof Checkable) {
//            ((Checkable) child).setChecked(mCheckStates.get(position));
//        }
//    }
    
    if (needToMeasure) {
        int childWidthSpec = ViewGroup::getChildMeasureSpec(mWidthMeasureSpec,
                                                           mListPadding.left() + mListPadding.right(), p->width);
        int lpHeight = p->height;
        int childHeightSpec;
        if (lpHeight > 0) {
            childHeightSpec = MeasureSpec::makeMeasureSpec(lpHeight, MeasureSpec::EXACTLY);
        } else {
            childHeightSpec = MeasureSpec::makeMeasureSpec(0, MeasureSpec::UNSPECIFIED);
        }
        child->measure(childWidthSpec, childHeightSpec);
    } else {
        cleanupLayoutState(child);
    }
    
    const int w = child->getMeasuredWidth();
    const int h = child->getMeasuredHeight();
    const int childTop = flowDown ? y : y - h;
    
    if (needToMeasure) {
        const int childRight = childrenLeft + w;
        const int childBottom = childTop + h;
        child->layout(childrenLeft, childTop, childRight, childBottom);
    } else {
        child->offsetLeftAndRight(childrenLeft - child->getLeft());
        child->offsetTopAndBottom(childTop - child->getTop());
    }
    
    if (mCachingStarted && !child->isDrawingCacheEnabled()) {
        child->setDrawingCacheEnabled(true);
    }
}

bool ListView::canAnimate() {
    return AbsListView::canAnimate() && mItemCount > 0;
}

void ListView::setSelection(int position) {
    setSelectionFromTop(position, 0);
}

void ListView::setSelectionFromTop(int position, int y) {
    if (mAdapter == NULL) {
        return;
    }

    if (!isInTouchMode()) {
        position = lookForSelectablePosition(position, true);
        if (position >= 0) {
            setNextSelectedPositionInt(position);
        }
    } else {
        mResurrectToPosition = position;
    }

    if (position >= 0) {
        mLayoutMode = LAYOUT_SPECIFIC;
        mSpecificTop = mListPadding.top() + y;
        
        if (mNeedSync) {
            mSyncPosition = position;
            mSyncRowId = mAdapter->getItemId(position);
        }
// TODO:
//        if (mPositionScroller != NULL) {
//            mPositionScroller.stop();
//        }
        requestLayout();
    }
}

void ListView::setSelectionInt(int position) {
    setNextSelectedPositionInt(position);
    bool awakeScrollbars = false;
    
    const int selectedPosition = mSelectedPosition;
    
    if (selectedPosition >= 0) {
        if (position == selectedPosition - 1) {
            awakeScrollbars = true;
        } else if (position == selectedPosition + 1) {
            awakeScrollbars = true;
        }
    }
    
//    if (mPositionScroller != NULL) {
//        mPositionScroller.stop();
//    }
    
    layoutChildren();
    
//    if (awakeScrollbars) {
//        awakenScrollBars();
//    }
}

int ListView::lookForSelectablePosition(int position, bool lookDown) {
    
    shared_ptr<ListAdapter> &adapter = mAdapter;

    if (adapter == NULL || isInTouchMode()) {
        return INVALID_POSITION;
    }
    
    const int count = adapter->getCount();
    if (!mAreAllItemsSelectable) {
        if (lookDown) {
            position = max(0, position);
            while (position < count && !adapter->isEnabled(position)) {
                position++;
            }
        } else {
            position = min(position, count - 1);
            while (position >= 0 && !adapter->isEnabled(position)) {
                position--;
            }
        }
        
        if (position < 0 || position >= count) {
            return INVALID_POSITION;
        }
        return position;
    } else {
        if (position < 0 || position >= count) {
            return INVALID_POSITION;
        }
        return position;
    }
}

void ListView::setSelectionAfterHeaderView() {
    const int count = mHeaderViewInfos.size();
    if (count > 0) {
        mNextSelectedPosition = 0;
        return;
    }
    
    if (mAdapter != NULL) {
        setSelection(count);
    } else {
        mNextSelectedPosition = count;
        mLayoutMode = LAYOUT_SET_SELECTION;
    }
    
}

// TODO:
//bool ListView::dispatchKeyEvent(KeyEvent event) {
//    // Dispatch in the normal way
//    bool handled = AbsListView::dispatchKeyEvent(event);
//    if (!handled) {
//        // If we didn't handle it...
//        View focused = getFocusedChild();
//        if (focused != NULL && event.getAction() == KeyEvent.ACTION_DOWN) {
//            // ... and our focused child didn't handle it
//            // ... give it to ourselves so we can scroll if necessary
//            handled = onKeyDown(event.getKeyCode(), event);
//        }
//    }
//    return handled;
//}

// TODO:
////@Override
//bool onKeyDown(int keyCode, KeyEvent event) {
//    return commonKey(keyCode, 1, event);
//}
//
////@Override
//bool onKeyMultiple(int keyCode, int repeatCount, KeyEvent event) {
//    return commonKey(keyCode, repeatCount, event);
//}
//
////@Override
//bool onKeyUp(int keyCode, KeyEvent event) {
//    return commonKey(keyCode, 1, event);
//}

// TODO:
//    private bool commonKey(int keyCode, int count, KeyEvent event) {
//        if (mAdapter == NULL || !mIsAttached) {
//            return false;
//        }
//        
//        if (mDataChanged) {
//            layoutChildren();
//        }
//        
//        bool handled = false;
//        int action = event.getAction();
//        
//        if (action != KeyEvent.ACTION_UP) {
//            switch (keyCode) {
//                case KeyEvent.KEYCODE_DPAD_UP:
//                    if (event.hasNoModifiers()) {
//                        handled = resurrectSelectionIfNeeded();
//                        if (!handled) {
//                            while (count-- > 0) {
//                                if (arrowScroll(FOCUS_UP)) {
//                                    handled = true;
//                                } else {
//                                    break;
//                                }
//                            }
//                        }
//                    } else if (event.hasModifiers(KeyEvent.META_ALT_ON)) {
//                        handled = resurrectSelectionIfNeeded() || fullScroll(FOCUS_UP);
//                    }
//                    break;
//                    
//                case KeyEvent.KEYCODE_DPAD_DOWN:
//                    if (event.hasNoModifiers()) {
//                        handled = resurrectSelectionIfNeeded();
//                        if (!handled) {
//                            while (count-- > 0) {
//                                if (arrowScroll(FOCUS_DOWN)) {
//                                    handled = true;
//                                } else {
//                                    break;
//                                }
//                            }
//                        }
//                    } else if (event.hasModifiers(KeyEvent.META_ALT_ON)) {
//                        handled = resurrectSelectionIfNeeded() || fullScroll(FOCUS_DOWN);
//                    }
//                    break;
//                    
//                case KeyEvent.KEYCODE_DPAD_LEFT:
//                    if (event.hasNoModifiers()) {
//                        handled = handleHorizontalFocusWithinListItem(View.FOCUS_LEFT);
//                    }
//                    break;
//                    
//                case KeyEvent.KEYCODE_DPAD_RIGHT:
//                    if (event.hasNoModifiers()) {
//                        handled = handleHorizontalFocusWithinListItem(View.FOCUS_RIGHT);
//                    }
//                    break;
//                    
//                case KeyEvent.KEYCODE_DPAD_CENTER:
//                case KeyEvent.KEYCODE_ENTER:
//                    if (event.hasNoModifiers()) {
//                        handled = resurrectSelectionIfNeeded();
//                        if (!handled
//                            && event.getRepeatCount() == 0 && getChildCount() > 0) {
//                            keyPressed();
//                            handled = true;
//                        }
//                    }
//                    break;
//                    
//                case KeyEvent.KEYCODE_SPACE:
//                    if (mPopup == NULL || !mPopup.isShowing()) {
//                        if (event.hasNoModifiers()) {
//                            handled = resurrectSelectionIfNeeded() || pageScroll(FOCUS_DOWN);
//                        } else if (event.hasModifiers(KeyEvent.META_SHIFT_ON)) {
//                            handled = resurrectSelectionIfNeeded() || pageScroll(FOCUS_UP);
//                        }
//                        handled = true;
//                    }
//                    break;
//                    
//                case KeyEvent.KEYCODE_PAGE_UP:
//                    if (event.hasNoModifiers()) {
//                        handled = resurrectSelectionIfNeeded() || pageScroll(FOCUS_UP);
//                    } else if (event.hasModifiers(KeyEvent.META_ALT_ON)) {
//                        handled = resurrectSelectionIfNeeded() || fullScroll(FOCUS_UP);
//                    }
//                    break;
//                    
//                case KeyEvent.KEYCODE_PAGE_DOWN:
//                    if (event.hasNoModifiers()) {
//                        handled = resurrectSelectionIfNeeded() || pageScroll(FOCUS_DOWN);
//                    } else if (event.hasModifiers(KeyEvent.META_ALT_ON)) {
//                        handled = resurrectSelectionIfNeeded() || fullScroll(FOCUS_DOWN);
//                    }
//                    break;
//                    
//                case KeyEvent.KEYCODE_MOVE_HOME:
//                    if (event.hasNoModifiers()) {
//                        handled = resurrectSelectionIfNeeded() || fullScroll(FOCUS_UP);
//                    }
//                    break;
//                    
//                case KeyEvent.KEYCODE_MOVE_END:
//                    if (event.hasNoModifiers()) {
//                        handled = resurrectSelectionIfNeeded() || fullScroll(FOCUS_DOWN);
//                    }
//                    break;
//                    
//                case KeyEvent.KEYCODE_TAB:
//                    // XXX Sometimes it is useful to be able to TAB through the items in
//                    //     a ListView sequentially.  Unfortunately this can create an
//                    //     asymmetry in TAB navigation order unless the list selection
//                    //     always reverts to the top or bottom when receiving TAB focus from
//                    //     another widget.  Leaving this behavior disabled for now but
//                    //     perhaps it should be configurable (and more comprehensive).
//                    if (false) {
//                        if (event.hasNoModifiers()) {
//                            handled = resurrectSelectionIfNeeded() || arrowScroll(FOCUS_DOWN);
//                        } else if (event.hasModifiers(KeyEvent.META_SHIFT_ON)) {
//                            handled = resurrectSelectionIfNeeded() || arrowScroll(FOCUS_UP);
//                        }
//                    }
//                    break;
//            }
//        }
//        
//        if (handled) {
//            return true;
//        }
//        
//        if (sendToTextFilter(keyCode, count, event)) {
//            return true;
//        }
//        
//        switch (action) {
//            case KeyEvent.ACTION_DOWN:
//                return super.onKeyDown(keyCode, event);
//                
//            case KeyEvent.ACTION_UP:
//                return super.onKeyUp(keyCode, event);
//                
//            case KeyEvent.ACTION_MULTIPLE:
//                return super.onKeyMultiple(keyCode, count, event);
//                
//            default: // shouldn't happen
//                return false;
//        }
//    }

bool ListView::pageScroll(int direction) {
    int nextPage = -1;
    bool down = false;

    if (direction == FOCUS_UP) {
        nextPage = max(0, mSelectedPosition - getChildCount() - 1);
    } else if (direction == FOCUS_DOWN) {
        nextPage = min(mItemCount - 1, mSelectedPosition + getChildCount() - 1);
        down = true;
    }
    
    if (nextPage >= 0) {
        int position = lookForSelectablePosition(nextPage, down);
        if (position >= 0) {
            mLayoutMode = LAYOUT_SPECIFIC;
            mSpecificTop = mPaddingTop + getVerticalFadingEdgeLength();
            
            if (down && position > mItemCount - getChildCount()) {
                mLayoutMode = LAYOUT_FORCE_BOTTOM;
            }
            
            if (!down && position < getChildCount()) {
                mLayoutMode = LAYOUT_FORCE_TOP;
            }
            
            setSelectionInt(position);
            invokeOnItemScrollListener();
            invalidate();
            
            return true;
        }
    }
    
    return false;
}

bool ListView::fullScroll(int direction) {
    bool moved = false;

    if (direction == FOCUS_UP) {
        if (mSelectedPosition != 0) {
            int position = lookForSelectablePosition(0, true);
            if (position >= 0) {
                mLayoutMode = LAYOUT_FORCE_TOP;
                setSelectionInt(position);
                invokeOnItemScrollListener();
            }
            moved = true;
        }
    } else if (direction == FOCUS_DOWN) {
        if (mSelectedPosition < mItemCount - 1) {
            int position = lookForSelectablePosition(mItemCount - 1, true);
            if (position >= 0) {
                mLayoutMode = LAYOUT_FORCE_BOTTOM;
                setSelectionInt(position);
                invokeOnItemScrollListener();
            }
            moved = true;
        }
    }
    
    if (moved) {
        invalidate();
    }
    
    return moved;
}

bool ListView::handleHorizontalFocusWithinListItem(int direction) {

    if (direction != View::FOCUS_LEFT && direction != View::FOCUS_RIGHT)  {
        throw IllegalArgumentException("direction must be one of {View.FOCUS_LEFT, View.FOCUS_RIGHT}");
    }
    
    const int numChildren = getChildCount();
    if (mItemsCanFocus && numChildren > 0 && mSelectedPosition != INVALID_POSITION) {
        View *selectedView = getSelectedView();
// TODO:
//        if (selectedView != NULL && selectedView->hasFocus() &&
//            selectedView instanceof ViewGroup) {
// TODO:
//            const View currentFocus = selectedView->findFocus();
//            const View nextFocus = FocusFinder.getInstance().findNextFocus(
//                                                                           (ViewGroup) selectedView, currentFocus, direction);
//            if (nextFocus != NULL) {
//                // do the math to get interesting rect in next focus' coordinates
//                currentFocus.getFocusedRect(mTempRect);
//                offsetDescendantRectToMyCoords(currentFocus, mTempRect);
//                offsetRectIntoDescendantCoords(nextFocus, mTempRect);
//                if (nextFocus.requestFocus(direction, mTempRect)) {
//                    return true;
//                }
//            }
            // we are blocking the key from being handled (by returning true)
            // if the global result is going to be some other view within this
            // list.  this is to acheive the overall goal of having
            // horizontal d-pad navigation remain in the current item.
// TODO:            View globalNextFocus = FocusFinder.getInstance().findNextFocus((ViewGroup) getRootView(), currentFocus, direction);
//            if (globalNextFocus != NULL) {
//                return isViewAncestorOf(globalNextFocus, this);
//            }
//        }
    }
    return false;
}

bool ListView::arrowScroll(int direction) {
    try {
        mInLayout = true;
        const bool handled = arrowScrollImpl(direction);
        if (handled) {
// TODO:            playSoundEffect(SoundEffectConstants.getContantForFocusDirection(direction));
        }
        return handled;
    } catch(Exception e) {}

}

const int ListView::nextSelectedPositionForDirection(int selectedPos, int direction) {
    int nextSelected;

    if (direction == View::FOCUS_DOWN) {
        nextSelected = selectedPos != INVALID_POSITION && selectedPos >= mFirstPosition ?
        selectedPos + 1 :
        mFirstPosition;
    } else {
        const int lastPos = mFirstPosition + getChildCount() - 1;
        nextSelected = selectedPos != INVALID_POSITION && selectedPos <= lastPos ?
        selectedPos - 1 :
        lastPos;
    }
    
    if (nextSelected < 0 || nextSelected >= mAdapter->getCount()) {
        return INVALID_POSITION;
    }
    
    return lookForSelectablePosition(nextSelected, direction == View::FOCUS_DOWN);
}

bool ListView::arrowScrollImpl(int direction) {
//    if (getChildCount() <= 0) {
//        return false;
//    }
//    
//    View *selectedView = getSelectedView();
//    int selectedPos = mSelectedPosition;
//    
//    int nextSelectedPosition = nextSelectedPositionForDirection(selectedPos, direction);
//    int amountToScroll = amountToScroll(direction, nextSelectedPosition);
//    
//    // if we are moving focus, we may OVERRIDE the default behavior
//    const ArrowScrollFocusResult focusResult = mItemsCanFocus ? arrowScrollFocused(direction) : NULL;
//    if (focusResult != NULL) {
//        nextSelectedPosition = focusResult.getSelectedPosition();
//        amountToScroll = focusResult.getAmountToScroll();
//    }
//    ArrowScrollFocusResult *focusResult;
//    
//    bool needToRedraw = focusResult != NULL;
//    if (nextSelectedPosition != INVALID_POSITION) {
//        handleNewSelectionChange(selectedView, direction, nextSelectedPosition, focusResult != NULL);
//        setSelectedPositionInt(nextSelectedPosition);
//        setNextSelectedPositionInt(nextSelectedPosition);
//        selectedView = getSelectedView();
//        selectedPos = nextSelectedPosition;
//        if (mItemsCanFocus && focusResult == NULL) {
//            // there was no new view found to take focus, make sure we
//            // don't leave focus with the old selection
//
//            shared_ptr<View> focused = getFocusedChild();
//            if (focused != NULL) {
//                focused->clearFocus();
//            }
//        }
//        needToRedraw = true;
//        checkSelectionChanged();
//    }
//    
//    if (amountToScroll > 0) {
//        scrollListItemsBy((direction == View::FOCUS_UP) ? amountToScroll : -amountToScroll);
//        needToRedraw = true;
//    }
//    
//    // if we didn't find a new focusable, make sure any existing focused
//    // item that was panned off screen gives up focus.
//    if (mItemsCanFocus && (focusResult == NULL)
//        && selectedView != NULL && selectedView.hasFocus()) {
//        const View focused = selectedView.findFocus();
//        if (!isViewAncestorOf(focused, this) || distanceToView(focused) > 0) {
//            focused.clearFocus();
//        }
//    }
//    
//    // if  the current selection is panned off, we need to remove the selection
//    if (nextSelectedPosition == INVALID_POSITION && selectedView != NULL
//        && !isViewAncestorOf(selectedView, this)) {
//        selectedView = NULL;
//        hideSelector();
//        
//        // but we don't want to set the ressurect position (that would make subsequent
//        // unhandled key events bring back the item we just scrolled off!)
//        mResurrectToPosition = INVALID_POSITION;
//    }
//    
//    if (needToRedraw) {
//        if (selectedView != NULL) {
//         positionSelector(selectedPos, selectedView);
//            mSelectedTop = selectedView->getTop();
//        }
//        if (!awakenScrollBars()) {
//            View::invalidate();
//        }
//        invokeOnItemScrollListener();
//        return true;
//    }
    
    return false;
}

void ListView::handleNewSelectionChange(View *selectedView, int direction, int newSelectedPosition,
                                        bool newFocusAssigned) {
    if (newSelectedPosition == INVALID_POSITION) {
        throw IllegalArgumentException("newSelectedPosition needs to be valid");
    }
    
    // whether or not we are moving down or up, we want to preserve the
    // top of whatever view is on top:
    // - moving down: the view that had selection
    // - moving up: the view that is getting selection
    shared_ptr<View> topView;
    shared_ptr<View> bottomView;
    int topViewIndex, bottomViewIndex;
    bool topSelected = false;
    const int selectedIndex = mSelectedPosition - mFirstPosition;
    const int nextSelectedIndex = newSelectedPosition - mFirstPosition;

    if (direction == View::FOCUS_UP) {
        topViewIndex = nextSelectedIndex;
        bottomViewIndex = selectedIndex;
        topView = getChildAt(topViewIndex);
        bottomView = shared_ptr<View>(selectedView);
        topSelected = true;
    } else {
        topViewIndex = selectedIndex;
        bottomViewIndex = nextSelectedIndex;
        topView = shared_ptr<View>(selectedView);
        bottomView = getChildAt(bottomViewIndex);
    }
    
    const int numChildren = getChildCount();
    
    // start with top view: is it changing size?
    if (topView != NULL) {
        topView->setSelected(!newFocusAssigned && topSelected);
        measureAndAdjustDown(topView, topViewIndex, numChildren);
    }
    
    // is the bottom view changing size?
    if (bottomView != NULL) {
        bottomView->setSelected(!newFocusAssigned && !topSelected);
        measureAndAdjustDown(bottomView, bottomViewIndex, numChildren);
    }
}

void ListView::measureAndAdjustDown(shared_ptr<View> child, int childIndex, int numChildren) {
    int oldHeight = child->getHeight();
    measureItem(child);
    if (child->getMeasuredHeight() != oldHeight) {
        // lay out the view, preserving its top
        relayoutMeasuredItem(child);
        
        // adjust views below appropriately
        const int heightDelta = child->getMeasuredHeight() - oldHeight;
        for (int i = childIndex + 1; i < numChildren; i++) {
            getChildAt(i)->offsetTopAndBottom(heightDelta);
        }
    }
}

/**
 * Measure a particular list child.
 * TODO: unify with setUpChild.
 * @param child The child.
 */
void ListView::measureItem(shared_ptr<View> child) {
    ListViewLayoutParams *p = (ListViewLayoutParams*) child->getLayoutParams();
    
    if (p == NULL) {
        p = new ListViewLayoutParams(
                                       LayoutParams::MATCH_PARENT,
                                       LayoutParams::WRAP_CONTENT);
    }
    
    int childWidthSpec = ViewGroup::getChildMeasureSpec(mWidthMeasureSpec,
                                                       mListPadding.left() + mListPadding.right(), p->width);
    int lpHeight = p->height;
    int childHeightSpec;
    if (lpHeight > 0) {
        childHeightSpec = MeasureSpec::makeMeasureSpec(lpHeight, MeasureSpec::EXACTLY);
    } else {
        childHeightSpec = MeasureSpec::makeMeasureSpec(0, MeasureSpec::UNSPECIFIED);
    }
    child->measure(childWidthSpec, childHeightSpec);
}

void ListView::relayoutMeasuredItem(shared_ptr<View> child) {
    const int w = child->getMeasuredWidth();
    const int h = child->getMeasuredHeight();
    const int childLeft = mListPadding.left();
    const int childRight = childLeft + w;
    const int childTop = child->getTop();
    const int childBottom = childTop + h;
    child->layout(childLeft, childTop, childRight, childBottom);
}

int ListView::getArrowScrollPreviewLength() {
    return max(MIN_SCROLL_PREVIEW_PIXELS, getVerticalFadingEdgeLength());
}

int ListView::amountToScroll(int direction, int nextSelectedPosition) {
    const int listBottom = getHeight() - mListPadding.bottom();
    const int listTop = mListPadding.top();
    
    int numChildren = getChildCount();

    if (direction == View::FOCUS_DOWN) {
        int indexToMakeVisible = numChildren - 1;
        if (nextSelectedPosition != INVALID_POSITION) {
            indexToMakeVisible = nextSelectedPosition - mFirstPosition;
        }
        while (numChildren <= indexToMakeVisible) {
            // Child to view is not attached yet.
            addViewBelow(getChildAt(numChildren - 1), mFirstPosition + numChildren - 1);
            numChildren++;
        }
        const int positionToMakeVisible = mFirstPosition + indexToMakeVisible;
        const shared_ptr<View> viewToMakeVisible = getChildAt(indexToMakeVisible);
        
        int goalBottom = listBottom;
        if (positionToMakeVisible < mItemCount - 1) {
            goalBottom -= getArrowScrollPreviewLength();
        }
        
        if (viewToMakeVisible->getBottom() <= goalBottom) {
            // item is fully visible.
            return 0;
        }
        
        if (nextSelectedPosition != INVALID_POSITION
            && (goalBottom - viewToMakeVisible->getTop()) >= getMaxScrollAmount()) {
            // item already has enough of it visible, changing selection is good enough
            return 0;
        }
        
        int amountToScroll = (viewToMakeVisible->getBottom() - goalBottom);
        
        if ((mFirstPosition + numChildren) == mItemCount) {
            // last is last in list -> make sure we don't scroll past it
            const int max = getChildAt(numChildren - 1)->getBottom() - listBottom;
            amountToScroll = min(amountToScroll, max);
        }
        
        return min(amountToScroll, getMaxScrollAmount());
    } else {
        int indexToMakeVisible = 0;
        if (nextSelectedPosition != INVALID_POSITION) {
            indexToMakeVisible = nextSelectedPosition - mFirstPosition;
        }
        while (indexToMakeVisible < 0) {
            // Child to view is not attached yet.
            addViewAbove(getChildAt(0), mFirstPosition);
            mFirstPosition--;
            indexToMakeVisible = nextSelectedPosition - mFirstPosition;
        }
        const int positionToMakeVisible = mFirstPosition + indexToMakeVisible;
        const shared_ptr<View> viewToMakeVisible = getChildAt(indexToMakeVisible);
        int goalTop = listTop;
        if (positionToMakeVisible > 0) {
            goalTop += getArrowScrollPreviewLength();
        }
        if (viewToMakeVisible->getTop() >= goalTop) {
            // item is fully visible.
            return 0;
        }
        
        if (nextSelectedPosition != INVALID_POSITION &&
            (viewToMakeVisible->getBottom() - goalTop) >= getMaxScrollAmount()) {
            // item already has enough of it visible, changing selection is good enough
            return 0;
        }
        
        int amountToScroll = (goalTop - viewToMakeVisible->getTop());
        if (mFirstPosition == 0) {
            // first is first in list -> make sure we don't scroll past it
            int max = listTop - getChildAt(0)->getTop();
            amountToScroll = min(amountToScroll,  max);
        }
        return min(amountToScroll, getMaxScrollAmount());
    }
}

int ListView::lookForSelectablePositionOnScreen(int direction) {
    const int firstPosition = mFirstPosition;

    if (direction == View::FOCUS_DOWN) {
        int startPos = (mSelectedPosition != INVALID_POSITION) ?
        mSelectedPosition + 1 :
        firstPosition;
        if (startPos >= mAdapter->getCount()) {
            return INVALID_POSITION;
        }
        if (startPos < firstPosition) {
            startPos = firstPosition;
        }
        
        const int lastVisiblePos = getLastVisiblePosition();
        shared_ptr<ListAdapter> adapter = getAdapter();
        for (int pos = startPos; pos <= lastVisiblePos; pos++) {
            if (adapter->isEnabled(pos)
                && getChildAt(pos - firstPosition)->getVisibility() == View::VISIBLE) {
                return pos;
            }
        }
    } else {
        int last = firstPosition + getChildCount() - 1;
        int startPos = (mSelectedPosition != INVALID_POSITION) ?
        mSelectedPosition - 1 :
        firstPosition + getChildCount() - 1;
        if (startPos < 0 || startPos >= mAdapter->getCount()) {
            return INVALID_POSITION;
        }
        if (startPos > last) {
            startPos = last;
        }
        
        shared_ptr<ListAdapter> adapter = getAdapter();
        for (int pos = startPos; pos >= firstPosition; pos--) {
            if (adapter->isEnabled(pos)
                && getChildAt(pos - firstPosition)->getVisibility() == View::VISIBLE) {
                return pos;
            }
        }
    }
    return INVALID_POSITION;
}

ListView::ArrowScrollFocusResult* ListView::arrowScrollFocused(const int direction) {
    View *selectedView = getSelectedView();
    shared_ptr<View> newFocus;

    if (selectedView != NULL && selectedView->hasFocus()) {
        shared_ptr<View> oldFocus = selectedView->findFocus();
//        newFocus = FocusFinder::getInstance().findNextFocus(this, oldFocus, direction);
    } else {
        if (direction == View::FOCUS_DOWN) {

            const bool topFadingEdgeShowing = (mFirstPosition > 0);
            const int listTop = mListPadding.top() +
            (topFadingEdgeShowing ? getArrowScrollPreviewLength() : 0);
            int ySearchPoint = (selectedView != NULL && selectedView->getTop() > listTop) ?
                selectedView->getTop() : listTop;
                mTempRect.set(0, ySearchPoint, 0, ySearchPoint);
      } else {
            const bool bottomFadingEdgeShowing =
            (mFirstPosition + getChildCount() - 1) < mItemCount;
            int listBottom = getHeight() - mListPadding.bottom() -
            (bottomFadingEdgeShowing ? getArrowScrollPreviewLength() : 0);
            int ySearchPoint = (selectedView != NULL && selectedView->getBottom() < listBottom) ?
            selectedView->getBottom() : listBottom;
            mTempRect.set(0, ySearchPoint, 0, ySearchPoint);
        }
//        newFocus = FocusFinder::getInstance().findNextFocusFromRect(this, mTempRect, direction);
    }
    
    if (newFocus != NULL) {
        int positionOfNewFocusI = positionOfNewFocus(newFocus);
        
        // if the focus change is in a different new position, make sure
        // we aren't jumping over another selectable position
        if (mSelectedPosition != INVALID_POSITION && positionOfNewFocusI != mSelectedPosition) {
            const int selectablePosition = lookForSelectablePositionOnScreen(direction);
            if (selectablePosition != INVALID_POSITION &&
                ((direction == View::FOCUS_DOWN && selectablePosition < positionOfNewFocusI) ||
                 (direction == View::FOCUS_UP && selectablePosition > positionOfNewFocusI))) {
                    return NULL;
                }
        }
        
        int focusScroll = amountToScrollToNewFocus(direction, newFocus, positionOfNewFocusI);

        const int maxScrollAmount = getMaxScrollAmount();
        if (focusScroll < maxScrollAmount) {
            // not moving too far, safe to give next view focus
            newFocus->requestFocus(direction);
            mArrowScrollFocusResult->populate(positionOfNewFocusI, focusScroll);
            return mArrowScrollFocusResult;

        } else if (distanceToView(newFocus) < maxScrollAmount){
            // Case to consider:
            // too far to get entire next focusable on screen, but by going
            // max scroll amount, we are getting it at least partially in view,
            // so give it focus and scroll the max ammount.
            newFocus->requestFocus(direction);
            mArrowScrollFocusResult->populate(positionOfNewFocusI, maxScrollAmount);
            return mArrowScrollFocusResult;
        }
    }

    return NULL;
}

int ListView::positionOfNewFocus(shared_ptr<View> newFocus) {
    const int numChildren = getChildCount();
    for (int i = 0; i < numChildren; i++) {
        const shared_ptr<View> child = getChildAt(i);
        if (isViewAncestorOf(newFocus, child)) {
            return mFirstPosition + i;
        }
    }
    throw IllegalArgumentException("newFocus is not a child of any of the children of the list!");
}

bool ListView::isViewAncestorOf(shared_ptr<View> child, shared_ptr<View> parent) {
    if (child == parent) {
        return true;
    }
    
    shared_ptr<ViewParent> theParent = child->getParent();
    shared_ptr<ViewGroup> parentIsViewGroup = dynamic_pointer_cast<ViewGroup>(theParent);

    return (parentIsViewGroup != NULL) && isViewAncestorOf(dynamic_pointer_cast<View>(theParent), parent);
}

/**
 * Determine how much we need to scroll in order to get newFocus in view.
 * @param direction either {@link android.view.View#FOCUS_UP} or
 *        {@link android.view.View#FOCUS_DOWN}.
 * @param newFocus The view that would take focus.
 * @param positionOfNewFocus The position of the list item containing newFocus
 * @return The amount to scroll.  Note: this is always positive!  Direction
 *   needs to be taken into account when actually scrolling.
 */
int ListView::amountToScrollToNewFocus(int direction, shared_ptr<View> newFocus, int positionOfNewFocus) {
    int amountToScroll = 0;
    newFocus->getDrawingRect(mTempRect);
    offsetDescendantRectToMyCoords(newFocus.get(), mTempRect);
    if (direction == View::FOCUS_UP) {
        if (mTempRect.top() < mListPadding.top()) {
            amountToScroll = mListPadding.top() - mTempRect.top();
            if (positionOfNewFocus > 0) {
                amountToScroll += getArrowScrollPreviewLength();
            }
        }
    } else {
        const int listBottom = getHeight() - mListPadding.bottom();
        if (mTempRect.bottom() > listBottom) {
            amountToScroll = mTempRect.bottom() - listBottom;
            if (positionOfNewFocus < mItemCount - 1) {
                amountToScroll += getArrowScrollPreviewLength();
            }
        }
    }
    return amountToScroll;
}

/**
 * Determine the distance to the nearest edge of a view in a particular
 * direction.
 *
 * @param descendant A descendant of this list.
 * @return The distance, or 0 if the nearest edge is already on screen.
 */
int ListView::distanceToView(shared_ptr<View> descendant) {
    int distance = 0;
    descendant->getDrawingRect(mTempRect);
    offsetDescendantRectToMyCoords(descendant.get(), mTempRect);
    const int listBottom = mBottom - mTop - mListPadding.bottom();
    if (mTempRect.bottom() < mListPadding.top()) {
        distance = mListPadding.top() - mTempRect.bottom();
    } else if (mTempRect.top() > listBottom) {
        distance = mTempRect.top() - listBottom;
    }
    return distance;
}


/**
 * Scroll the children by amount, adding a view at the end and removing
 * views that fall off as necessary.
 *
 * @param amount The amount (positive or negative) to scroll.
 */
void ListView::scrollListItemsBy(int amount) {
    offsetChildrenTopAndBottom(amount);
    
    const int listBottom = getHeight() - mListPadding.bottom();
    const int listTop = mListPadding.top();
    AbsListView::RecycleBin &recycleBin = mRecycler;
    
    if (amount < 0) {
        // shifted items up
        
        // may need to pan views into the bottom space
        int numChildren = getChildCount();
        shared_ptr<View> last = getChildAt(numChildren - 1);
        while (last->getBottom() < listBottom) {
            const int lastVisiblePosition = mFirstPosition + numChildren - 1;
            if (lastVisiblePosition < mItemCount - 1) {
                last = addViewBelow(last, lastVisiblePosition);
                numChildren++;
            } else {
                break;
            }
        }
        
        // may have brought in the last child of the list that is skinnier
        // than the fading edge, thereby leaving space at the end.  need
        // to shift back
        if (last->getBottom() < listBottom) {
            offsetChildrenTopAndBottom(listBottom - last->getBottom());
        }
        
        // top views may be panned off screen
        shared_ptr<View> first = getChildAt(0);
        while (first->getBottom() < listTop) {
            ListViewLayoutParams *layoutParams = (ListViewLayoutParams*) first->getLayoutParams();
            if (recycleBin.shouldRecycleViewType(layoutParams->viewType)) {
                recycleBin.addScrapView(first);
            }
            detachViewFromParent(first);
            first = getChildAt(0);
            mFirstPosition++;
        }
    } else {
        // shifted items down
        shared_ptr<View> first = getChildAt(0);
        
        // may need to pan views into top
        while ((first->getTop() > listTop) && (mFirstPosition > 0)) {
            first = addViewAbove(first, mFirstPosition);
            mFirstPosition--;
        }
        
        // may have brought the very first child of the list in too far and
        // need to shift it back
        if (first->getTop() > listTop) {
            offsetChildrenTopAndBottom(listTop - first->getTop());
        }
        
        int lastIndex = getChildCount() - 1;
        shared_ptr<View> last = getChildAt(lastIndex);
        
        // bottom view may be panned off screen
        while (last->getTop() > listBottom) {

            ListViewLayoutParams *layoutParams = (ListViewLayoutParams*) last->getLayoutParams();
            if (recycleBin.shouldRecycleViewType(layoutParams->viewType)) {
                recycleBin.addScrapView(last);
            }
            detachViewFromParent(last);
            last = getChildAt(--lastIndex);
        }
    }
}

shared_ptr<View> ListView::addViewAbove(shared_ptr<View> theView, int position) {
    int abovePosition = position - 1;
    shared_ptr<View> view = obtainView(abovePosition);
    int edgeOfNewChild = theView->getTop() - mDividerHeight;

    setupChild(view, abovePosition, edgeOfNewChild, false, mListPadding.left(),
               false, false);
    return view;
}

shared_ptr<View> ListView::addViewBelow(shared_ptr<View> theView, int position) {
    int belowPosition = position + 1;

    shared_ptr<View> view = obtainView(belowPosition);

    int edgeOfNewChild = theView->getBottom() + mDividerHeight;
    setupChild(view, belowPosition, edgeOfNewChild, true, mListPadding.left(),
               false, false);
    return view;
}

void ListView::setItemsCanFocus(bool itemsCanFocus) {
    mItemsCanFocus = itemsCanFocus;
    if (!itemsCanFocus) {
        setDescendantFocusability(ViewGroup::FOCUS_BLOCK_DESCENDANTS);
    }
}

bool ListView::getItemsCanFocus() {
    return mItemsCanFocus;
}

bool ListView::isOpaque() {
// TODO:
//    bool retValue = (mCachingActive && mIsCacheColorOpaque && mDividerIsOpaque &&
//                        hasOpaqueScrollbars()) || super.isOpaque();
    bool retValue = false;
// END TODO:

    if (retValue) {
        // only return true if the list items cover the entire area of the view
// TODO:        const int listTop = mListPadding != NULL ? mListPadding.top : m_paddingTop;
        int listTop = 0;
// END TODO:
        shared_ptr<View> first = getChildAt(0);
        if (first == NULL || first->getTop() > listTop) {
            return false;
        }
// TODO:        const int listBottom = getHeight() -
//        (mListPadding != NULL ? mListPadding.bottom : m_paddingBottom);
        int listBottom = 0;
// END TODO:
        shared_ptr<View> last = getChildAt(getChildCount() - 1);
        if (last == NULL || last->getBottom() < listBottom) {
            return false;
        }
    }
    return retValue;
}

//@Override
void ListView::setCacheColorHint(unsigned int color) {
    const bool opaque = (color >> 24) == 0xFF;
    mIsCacheColorOpaque = opaque;
    if (opaque) {
        if (mDividerPaint == NULL) {
            mDividerPaint = new Paint();
        }
        mDividerPaint->setColor(color);
    }
    AbsListView::setCacheColorHint(color);
}

void ListView::drawOverscrollHeader(shared_ptr<Canvas> canvas, shared_ptr<Drawable> drawable, Rect bounds) {
    const int height = drawable->getMinimumHeight();
    
    canvas->save();
    canvas->clipRect(bounds);
    
    const int span = bounds.bottom() - bounds.top();
    if (span < height) {
        bounds.set(bounds.left(), bounds.bottom() - height, bounds.right(), bounds.bottom());
    }
    
    drawable->setBounds(bounds);
    drawable->draw(canvas);
    
    canvas->restore();
}

void ListView::drawOverscrollFooter(shared_ptr<Canvas> canvas, shared_ptr<Drawable> drawable, Rect bounds) {
    const int height = drawable->getMinimumHeight();
    
    canvas->save();
    canvas->clipRect(bounds);
    
    const int span = bounds.bottom() - bounds.top();
    if (span < height) {
        bounds.set(bounds.left(), bounds.height(), bounds.right(), bounds.top() + height);
    }
    
    drawable->setBounds(bounds);
    drawable->draw(canvas);
    
    canvas->restore();
}

void ListView::dispatchDraw(shared_ptr<Canvas> canvas) {
    
    // Draw the dividers
    const int dividerHeight = mDividerHeight;
    
    if (dividerHeight > 0 && mDivider != NULL) {
        // Only modify the top and bottom in the loop, we set the left and right here
        Rect &bounds = mTempRect;
        bounds.fLeft = mPaddingLeft;
        bounds.fRight = mRight - mLeft - mPaddingRight;
        
        const int count = getChildCount();
        const int headerCount = mHeaderViewInfos.size();
        const int footerLimit = mItemCount - mFooterViewInfos.size() - 1;
        const bool headerDividers = mHeaderDividersEnabled;
        const bool footerDividers = mFooterDividersEnabled;
        const int first = mFirstPosition;
        const bool areAllItemsSelectable = mAreAllItemsSelectable;
        const shared_ptr<ListAdapter> &adapter = mAdapter;
        // If the list is opaque *and* the background is not, we want to
        // fill a rect where the dividers would be for non-selectable items
        // If the list is opaque and the background is also opaque, we don't
        // need to draw anything since the background will do it for us
        const bool fillForMissingDividers = isOpaque() && !AbsListView::isOpaque();
        
        if (fillForMissingDividers && mDividerPaint == NULL && mIsCacheColorOpaque) {
            mDividerPaint = new Paint();
            mDividerPaint->setColor(getCacheColorHint());
        }
        Paint *paint = mDividerPaint;
        
        if (!mStackFromBottom) {
            int bottom;
            int listBottom = mBottom - mTop - mListPadding.bottom();
            
            for (int i = 0; i < count; i++) {
                if ((headerDividers || first + i >= headerCount) &&
                    (footerDividers || first + i < footerLimit)) {
                    shared_ptr<View> child = getChildAt(i);
                    bottom = child->getBottom();
                    // Don't draw dividers next to items that are not enabled
                    if (bottom < listBottom) {
                        if ((areAllItemsSelectable ||
                             (adapter->isEnabled(first + i) && (i == count - 1 ||
                                                               adapter->isEnabled(first + i + 1))))) {
                            bounds.fTop = bottom;
                            bounds.fBottom = bottom + dividerHeight;
                            drawDivider(canvas, bounds, i);
                        } else if (fillForMissingDividers) {
                            bounds.fTop = bottom;
                            bounds.fBottom = bottom + dividerHeight;
                            canvas->drawRect(bounds, paint);
                        }
                    }
                }
            }
        } else {
            int top;
            int listTop = mListPadding.top();
            
            for (int i = 0; i < count; i++) {
                if ((headerDividers || first + i >= headerCount) &&
                    (footerDividers || first + i < footerLimit)) {
                    shared_ptr<View> child = getChildAt(i);
                    top = child->getTop();
                    // Don't draw dividers next to items that are not enabled
                    if (top > listTop) {
                        if ((areAllItemsSelectable ||
                             (adapter->isEnabled(first + i) && (i == count - 1 ||
                                                               adapter->isEnabled(first + i + 1))))) {
                            bounds.fTop = top - dividerHeight;
                            bounds.fBottom = top;
                            // Give the method the child ABOVE the divider, so we
                            // subtract one from our child
                            // position. Give -1 when there is no child above the
                            // divider.
                            drawDivider(canvas, bounds, i - 1);
                        } else if (fillForMissingDividers) {
                            bounds.fTop = top - dividerHeight;
                            bounds.fBottom = top;
                            canvas->drawRect(bounds, paint);
                        }
                    }
                }
            }
        }
    }
    
    // Draw the indicators (these should be drawn above the dividers) and children
    AbsListView::dispatchDraw(canvas);
}

bool ListView::drawChild(shared_ptr<Canvas> canvas, shared_ptr<View> child, uint64_t drawingTime) {
    bool more = AbsListView::drawChild(canvas, child, drawingTime);
// TODO:
//    if (mCachingActive && child->mCachingFailed) {
//        mCachingActive = false;
//    }
    return more;
}

void ListView::drawDivider(shared_ptr<Canvas> canvas, Rect bounds, int childIndex) {
    // This widget draws the same divider for all children
    const shared_ptr<Drawable> &divider = mDivider;
    const bool clipDivider = mClipDivider;
    
    if (!clipDivider) {
        divider->setBounds(bounds);
    } else {
        canvas->save();
        canvas->clipRect(bounds);
    }
    
    divider->draw(canvas);
    
    if (clipDivider) {
        canvas->restore();
    }
}

shared_ptr<Drawable> ListView::getDivider() {
    return mDivider;
}

void ListView::setDivider(shared_ptr<Drawable> divider) {
    if (divider != NULL) {
        mDividerHeight = divider->getIntrinsicHeight();
        mClipDivider = dynamic_pointer_cast<ColorDrawable>(divider) != NULL;
    } else {
        mDividerHeight = 0;
        mClipDivider = false;
    }
    mDivider = divider;
    mDividerIsOpaque = divider == NULL || divider->getOpacity() == PIXEL_FORMAT_OPAQUE;
    requestLayoutIfNecessary();
}

int ListView::getDividerHeight() {
    return mDividerHeight;
}

void ListView::setDividerHeight(int height) {
    mDividerHeight = height;
    requestLayout();
    View::invalidate();
}

void ListView::setHeaderDividersEnabled(bool headerDividersEnabled) {
    mHeaderDividersEnabled = headerDividersEnabled;
    View::invalidate();
}

void ListView::setFooterDividersEnabled(bool footerDividersEnabled) {
    mFooterDividersEnabled = footerDividersEnabled;
    View::invalidate();
}

void ListView::setOverscrollHeader(shared_ptr<Drawable> header) {
    mOverScrollHeader = header;
    if (View::mScrollY < 0) {
        View::invalidate();
    }
}

shared_ptr<Drawable> ListView::getOverscrollHeader() {
    return mOverScrollHeader;
}

void ListView::setOverscrollFooter(shared_ptr<Drawable> footer) {
    mOverScrollFooter = footer;
    View::invalidate();
}

shared_ptr<Drawable> ListView::getOverscrollFooter() {
    return mOverScrollFooter;
}

void ListView::onFocusChanged(bool gainFocus, int direction, Rect *previouslyFocusedRect) {
    AbsListView::onFocusChanged(gainFocus, direction, previouslyFocusedRect);
    
    shared_ptr<ListAdapter> &adapter = mAdapter;
    int closetChildIndex = -1;
    int closestChildTop = 0;
    
    if (adapter != NULL && gainFocus && previouslyFocusedRect != NULL) {
        previouslyFocusedRect->offset(mScrollX, mScrollY);
        
        // Don't cache the result of getChildCount or mFirstPosition here,
        // it could change in layoutChildren.
        if (adapter->getCount() < getChildCount() + mFirstPosition) {
            mLayoutMode = LAYOUT_NORMAL;
            layoutChildren();
        }
        
        // figure out which item should be selected based on previously
        // focused rect
        Rect &otherRect = mTempRect;
        int minDistance = numeric_limits<int>::max();
        const int childCount = getChildCount();
        const int firstPosition = mFirstPosition;
        
        for (int i = 0; i < childCount; i++) {
            // only consider selectable views
            if (!adapter->isEnabled(firstPosition + i)) {
                continue;
            }
            
            shared_ptr<View> other = getChildAt(i);
            other->getDrawingRect(otherRect);

            offsetDescendantRectToMyCoords(other.get(), otherRect);
            int distance = getDistance(*previouslyFocusedRect, otherRect, direction);
            
            if (distance < minDistance) {
                minDistance = distance;
                closetChildIndex = i;
                closestChildTop = other->getTop();
            }
        }
    }
    
    if (closetChildIndex >= 0) {
        setSelectionFromTop(closetChildIndex + mFirstPosition, closestChildTop);
    } else {
        requestLayout();
    }
}

void ListView::onFinishInflate() {
    View::onFinishInflate();
    
    int count = getChildCount();
    if (count > 0) {
        for (int i = 0; i < count; ++i) {
            addHeaderView(getChildAt(i));
        }
        removeAllViews();
    }
}

shared_ptr<View> ListView::findViewTraversal(string id) {
    shared_ptr<View> v;
    v = ViewGroup::findViewTraversal(id);
    if (v == NULL) {
        v = findViewInHeadersOrFooters(mHeaderViewInfos, id);
        if (v != NULL) {
            return v;
        }
        v = findViewInHeadersOrFooters(mFooterViewInfos, id);
        if (v != NULL) {
            return v;
        }
    }
    return v;
}

/* (non-Javadoc)
 *
 * Look in the passed in list of headers or footers for the view.
 */
shared_ptr<View> ListView::findViewInHeadersOrFooters(vector<FixedViewInfo> where, string id) {

        int len = where.size();
        shared_ptr<View> v;
        
        for (int i = 0; i < len; i++) {
            v = where.at(i).view;

            if (!v->isRootNamespace()) {
                v = v->findViewTraversal(id);
                
                if (v != NULL) {
                    return v;
                }
            }
        }
    return NULL;
}

/* (non-Javadoc)
 * @see android.view.View#findViewWithTag(Object)
 * First look in our children, then in any header and footer views that may be scrolled off.
 */
//@Override
shared_ptr<View> ListView::findViewWithTagTraversal(shared_ptr<Object> tag) {
    shared_ptr<View> v = ViewGroup::findViewWithTagTraversal(tag);
    if (v == NULL) {
        v = findViewWithTagInHeadersOrFooters(mHeaderViewInfos, tag);
        if (v != NULL) {
            return v;
        }
        
        v = findViewWithTagInHeadersOrFooters(mFooterViewInfos, tag);
        if (v != NULL) {
            return v;
        }
    }
    return v;
}

/* (non-Javadoc)
 *
 * Look in the passed in list of headers or footers for the view with the tag.
 */
shared_ptr<View> ListView::findViewWithTagInHeadersOrFooters(vector<FixedViewInfo> where, shared_ptr<Object> tag) {

    int len = where.size();
    shared_ptr<View> v;
    
    for (int i = 0; i < len; i++) {
        v = where.at(i).view;

        if (!v->isRootNamespace()) {
            v = v->findViewWithTagTraversal(tag);
            
            if (v != NULL) {
                return v;
            }
        }
    }
    return NULL;
}

///**
// * @hide
// * @see android.view.View#findViewByPredicate(Predicate)
// * First look in our children, then in any header and footer views that may be scrolled off.
// */
////@Override
//shared_ptr<View> findViewByPredicateTraversal(Predicate<View> predicate, View childToSkip) {
//    View v;
//    v = super.findViewByPredicateTraversal(predicate, childToSkip);
//    if (v == NULL) {
//        v = findViewByPredicateInHeadersOrFooters(mHeaderViewInfos, predicate, childToSkip);
//        if (v != NULL) {
//            return v;
//        }
//        
//        v = findViewByPredicateInHeadersOrFooters(mFooterViewInfos, predicate, childToSkip);
//        if (v != NULL) {
//            return v;
//        }
//    }
//    return v;
//}

//    /* (non-Javadoc)
//     *
//     * Look in the passed in list of headers or footers for the first view that matches
//     * the predicate.
//     */
//    View findViewByPredicateInHeadersOrFooters(ArrayList<FixedViewInfo> where,
//                                               Predicate<View> predicate, View childToSkip) {
//        if (where != NULL) {
//            int len = where.size();
//            View v;
//            
//            for (int i = 0; i < len; i++) {
//                v = where.get(i).view;
//                
//                if (v != childToSkip && !v.isRootNamespace()) {
//                    v = v.findViewByPredicate(predicate);
//                    
//                    if (v != NULL) {
//                        return v;
//                    }
//                }
//            }
//        }
//        return NULL;
//    }
    
//    /**
//     * Returns the set of checked items ids. The result is only valid if the
//     * choice mode has not been set to {@link #CHOICE_MODE_NONE}.
//     * 
//     * @return A new array which contains the id of each checked item in the
//     *         list.
//     *         
//     * @deprecated Use {@link #getCheckedItemIds()} instead.
//     */
//    @Deprecated
//    public long[] getCheckItemIds() {
//        // Use new behavior that correctly handles stable ID mapping.
//        if (mAdapter != NULL && mAdapter.hasStableIds()) {
//            return getCheckedItemIds();
//        }
//        
//        // Old behavior was buggy, but would sort of work for adapters without stable IDs.
//        // Fall back to it to support legacy apps.
//        if (mChoiceMode != CHOICE_MODE_NONE && mCheckStates != NULL && mAdapter != NULL) {
//            const SparseBooleanArray states = mCheckStates;
//            const int count = states.size();
//            const long[] ids = new long[count];
//            const ListAdapter adapter = mAdapter;
//            
//            int checkedCount = 0;
//            for (int i = 0; i < count; i++) {
//                if (states.valueAt(i)) {
//                    ids[checkedCount++] = adapter.getItemId(states.keyAt(i));
//                }
//            }
//            
//            // Trim array if needed. mCheckStates may contain false values
//            // resulting in checkedCount being smaller than count.
//            if (checkedCount == count) {
//                return ids;
//            } else {
//                const long[] result = new long[checkedCount];
//                System.arraycopy(ids, 0, result, 0, checkedCount);
//                
//                return result;
//            }
//        }
//        return new long[0];
//    }

//    @Override
//    public void onInitializeAccessibilityEvent(AccessibilityEvent event) {
//        super.onInitializeAccessibilityEvent(event);
//        event.setClassName(ListView.class.getName());
//    }
    
//    @Override
//    public void onInitializeAccessibilityNodeInfo(AccessibilityNodeInfo info) {
//        super.onInitializeAccessibilityNodeInfo(info);
//        info.setClassName(ListView.class.getName());
//    }
//}


ANDROID_END
