//
//  AbsListView::cpp
//  Androidpp
//
//  Created by Saul Howard on 2/7/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#include "AbsListView.h"

#include "Android/graphics/drawable/TransitionDrawable.h"
#include "Android/text/String.h"
#include "Android/text/TextUtils.h"
#include "Android/utils/System.h"
#include "Android/view/KeyEvent.h"
#include "Android/view/MotionEvent.h"
#include "Android/view/VelocityTracker.h"
#include "Android/widget/Filterable.h"

#include <algorithm>
#include <cmath>

ANDROID_BEGIN

AbsListView::AbsListView(Context *context) : AdapterView<ListAdapter>(context, NULL) {
    initAbsListView();
    
    setVerticalScrollBarEnabled(true);
    initializeScrollbars(NULL);
}

AbsListView::AbsListView(Context *context, AttributeSet *attrs) : AdapterView<ListAdapter>(context, attrs) {

    initAbsListView();
    
//    shared_ptr<Drawable> d = attrs->getAttributeDrawableValue(context->getResources(), name); // a.getDrawable(com.android.internal.R.styleable.AbsListView_listSelector);
//    if (d != NULL) {
//        setSelector(d);
//    }
//    
//    mDrawSelectorOnTop = a.getBoolean(com.android.internal.R.styleable.AbsListView_drawSelectorOnTop, false);
//    
//    bool stackFromBottom = a.getBoolean(R.styleable.AbsListView_stackFromBottom, false);
//    setStackFromBottom(stackFromBottom);
//    
//    bool scrollingCacheEnabled = a.getBoolean(R.styleable.AbsListView_scrollingCache, true);
//    setScrollingCacheEnabled(scrollingCacheEnabled);
//    
//    bool useTextFilter = a.getBoolean(R.styleable.AbsListView_textFilterEnabled, false);
//    setTextFilterEnabled(useTextFilter);
//    
//    int transcriptMode = a.getInt(R.styleable.AbsListView_transcriptMode,
//                                  TRANSCRIPT_MODE_DISABLED);
//    setTranscriptMode(transcriptMode);
//    
//    int color = a.getColor(R.styleable.AbsListView_cacheColorHint, 0);
//    setCacheColorHint(color);
//    
//    bool enableFastScroll = a.getBoolean(R.styleable.AbsListView_fastScrollEnabled, false);
//    setFastScrollEnabled(enableFastScroll);
//    
//    bool smoothScrollbar = a.getBoolean(R.styleable.AbsListView_smoothScrollbar, true);
//    setSmoothScrollbarEnabled(smoothScrollbar);
}

void AbsListView::initAbsListView() {
    // Setting focusable in touch mode will set the focusable property to true
//    setFocusableInTouchMode(true);
    setWillNotDraw(false);
    setAlwaysDrawnWithCacheEnabled(false);
    setScrollingCacheEnabled(true);
    
//    const ViewConfiguration configuration = ViewConfiguration.get(mContext);
//    mTouchSlop = configuration.getScaledTouchSlop();
//    mMinimumVelocity = configuration.getScaledMinimumFlingVelocity();
//    mMaximumVelocity = configuration.getScaledMaximumFlingVelocity();
    mDensityScale = getContext()->getResources()->getDisplayMetrics().density;
}

void AbsListView::setFastScrollEnabled(bool enabled) {
//    mFastScrollEnabled = enabled;
//    if (enabled) {
//        if (mFastScroller == NULL) {
//            mFastScroller = new FastScroller(getContext(), this);
//        }
//    } else {
//        if (mFastScroller != NULL) {
//            mFastScroller.stop();
//            mFastScroller = NULL;
//        }
//    }
}

bool AbsListView::isFastScrollEnabled() {
    return mFastScrollEnabled;
}

bool AbsListView::isVerticalScrollBarHidden() {
    return false;//mFastScroller != NULL && mFastScroller.isVisible();
}

void AbsListView::setSmoothScrollbarEnabled(bool enabled) {
    mSmoothScrollbarEnabled = enabled;
}

bool AbsListView::isSmoothScrollbarEnabled() {
    return mSmoothScrollbarEnabled;
}

void AbsListView::setOnScrollListener(OnScrollListener *l) {
    mOnScrollListener = l;
    invokeOnItemScrollListener();
}

void AbsListView::invokeOnItemScrollListener() {
    if (mFastScroller != NULL) {
        //mFastScroller.onScroll(this, mFirstPosition, getChildCount(), mItemCount);
    }
    if (mOnScrollListener != NULL) {
        mOnScrollListener->onScroll(this, mFirstPosition, getChildCount(), mItemCount);
    }
}

bool AbsListView::isScrollingCacheEnabled() {
    return mScrollingCacheEnabled;
}

void AbsListView::setScrollingCacheEnabled(bool enabled) {
    if (mScrollingCacheEnabled && !enabled) {
        clearScrollingCache();
    }
    mScrollingCacheEnabled = enabled;
}

void AbsListView::setTextFilterEnabled(bool textFilterEnabled) {
    mTextFilterEnabled = textFilterEnabled;
}

bool AbsListView::isTextFilterEnabled() {
    return mTextFilterEnabled;
}

void AbsListView::getFocusedRect(Rect r) {
    View *view = getSelectedView();
    if (view != NULL && view->getParent().get() == this) {
        // the focused rectangle of the selected view offset into the
        // coordinate space of this view->
        view->getFocusedRect(r);
        offsetDescendantRectToMyCoords(view, r);
    } else {
        // otherwise, just the norm
        AdapterView<ListAdapter>::getFocusedRect(r);
    }
}

void AbsListView::useDefaultSelector() {
    setSelector(getResources()->getDrawable("com.android.internal.R.drawable.list_selector_background"));
}

bool AbsListView::isStackFromBottom() {
    return mStackFromBottom;
}

void AbsListView::setStackFromBottom(bool stackFromBottom) {
    if (mStackFromBottom != stackFromBottom) {
        mStackFromBottom = stackFromBottom;
        requestLayoutIfNecessary();
    }
}

void AbsListView::requestLayoutIfNecessary() {
    if (getChildCount() > 0) {
        resetList();
        requestLayout();
        invalidate();
    }
}

bool AbsListView::acceptFilter() {
    return mTextFilterEnabled;// && getAdapter() instanceof Filterable && ((Filterable) getAdapter()).getFilter() != NULL;
}

void AbsListView::setFilterText(shared_ptr<String> filterText) {
    // TODO: Should we check for acceptFilter()?
    if (mTextFilterEnabled && !TextUtils::isEmpty(filterText)) {
//        createTextFilter(false);
//        // This is going to call our listener onTextChanged, but we might not
//        // be ready to bring up a window yet
//        mTextFilter->setText(filterText);
//        mTextFilter->setSelection(filterText.length());
//        if (mAdapter instanceof Filterable) {
//            // if mPopup is non-NULL, then onTextChanged will do the filtering
//            if (mPopup == NULL) {
//                Filter f = ((Filterable) mAdapter).getFilter();
//                f.filter(filterText);
//            }
//            // Set filtered to true so we will display the filter window when our main
//            // window is ready
//            mFiltered = true;
//            mDataSetObserver.clearSavedState();
//        }
    }
}

CharSequence *AbsListView::getTextFilter() {
//    if (mTextFilterEnabled && mTextFilter != NULL) {
//        return mTextFilter->getText();
//    }
    return NULL;
}

void AbsListView::onFocusChanged(bool gainFocus, int direction, Rect *previouslyFocusedRect) {
    AdapterView<ListAdapter>::onFocusChanged(gainFocus, direction, previouslyFocusedRect);
    if (gainFocus && mSelectedPosition < 0 && !isInTouchMode()) {
        resurrectSelection();
    }
}

void AbsListView::requestLayout() {
    if (!mBlockLayoutRequests && !mInLayout) {
        AdapterView<ListAdapter>::requestLayout();
    }
}

void AbsListView::resetList() {
    removeAllViewsInLayout();
    mFirstPosition = 0;
    mDataChanged = false;
    mNeedSync = false;
    mOldSelectedPosition = INVALID_POSITION;
    mOldSelectedRowId = INVALID_ROW_ID;
    setSelectedPositionInt(INVALID_POSITION);
    setNextSelectedPositionInt(INVALID_POSITION);
    mSelectedTop = 0;
    mSelectorRect.setEmpty();
    invalidate();
}

int AbsListView::computeVerticalScrollExtent() {
    const int count = getChildCount();
    if (count > 0) {
        if (mSmoothScrollbarEnabled) {
            int extent = count * 100;
            
            shared_ptr<View> view = getChildAt(0);
            const int top = view->getTop();
            int height = view->getHeight();
            if (height > 0) {
                extent += (top * 100) / height;
            }
            
            view = getChildAt(count - 1);
            const int bottom = view->getBottom();
            height = view->getHeight();
            if (height > 0) {
                extent -= ((bottom - getHeight()) * 100) / height;
            }
            
            return extent;
        } else {
            return 1;
        }
    }
    return 0;
}

int AbsListView::computeVerticalScrollOffset() {
    const int firstPosition = mFirstPosition;
    const int childCount = getChildCount();
    if (firstPosition >= 0 && childCount > 0) {
        if (mSmoothScrollbarEnabled) {
            const shared_ptr<View> view = getChildAt(0);
            const int top = view->getTop();
            int height = view->getHeight();
            if (height > 0) {
                return max(firstPosition * 100 - (top * 100) / height, 0);
            }
        } else {
            int index;
            const int count = mItemCount;
            if (firstPosition == 0) {
                index = 0;
            } else if (firstPosition + childCount == count) {
                index = count;
            } else {
                index = firstPosition + childCount / 2;
            }
            return (int) (firstPosition + childCount * (index / (float) count));
        }
    }
    return 0;
}

int AbsListView::computeVerticalScrollRange() {
    return mSmoothScrollbarEnabled ? max(mItemCount * 100, 0) : mItemCount;
}

float AbsListView::getTopFadingEdgeStrength() {
    const int count = getChildCount();
    const float fadeEdge = AdapterView<ListAdapter>::getTopFadingEdgeStrength();
    if (count == 0) {
        return fadeEdge;
    } else {
        if (mFirstPosition > 0) {
            return 1.0f;
        }
        
        const int top = getChildAt(0)->getTop();
        const float fadeLength = (float) getVerticalFadingEdgeLength();
        return top < mPaddingTop ? (float) -(top - mPaddingTop) / fadeLength : fadeEdge;
    }
}

float AbsListView::getBottomFadingEdgeStrength() {
    const int count = getChildCount();
    const float fadeEdge = AdapterView<ListAdapter>::getBottomFadingEdgeStrength();
    if (count == 0) {
        return fadeEdge;
    } else {
        if (mFirstPosition + count - 1 < mItemCount - 1) {
            return 1.0f;
        }
        
        const int bottom = getChildAt(count - 1)->getBottom();
        const int height = getHeight();
        const float fadeLength = (float) getVerticalFadingEdgeLength();
        return bottom > height - mPaddingBottom ?
        (float) (bottom - height + mPaddingBottom) / fadeLength : fadeEdge;
    }
}

void AbsListView::onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
    if (mSelector == NULL) {
        useDefaultSelector();
    }
    Rect listPadding = mListPadding;
    listPadding.fLeft = mSelectionLeftPadding + mPaddingLeft;
    listPadding.fTop = mSelectionTopPadding + mPaddingTop;
    listPadding.fRight = mSelectionRightPadding + mPaddingRight;
    listPadding.fBottom = mSelectionBottomPadding + mPaddingBottom;
}

void AbsListView::onLayout(bool changed, int l, int t, int r, int b) {
    AdapterView<ListAdapter>::onLayout(changed, l, t, r, b);
    mInLayout = true;
    layoutChildren();
    mInLayout = false;
}

bool AbsListView::setFrame(int left, int top, int right, int bottom) {
    const bool changed = AdapterView<ListAdapter>::setFrame(left, top, right, bottom);
    
    if (changed) {
        // Reposition the popup when the frame has changed. This includes
        // translating the widget, not just changing its dimension. The
        // filter popup needs to follow the widget.
        const bool visible = getWindowVisibility() == View::VISIBLE;
//        if (mFiltered && visible && mPopup != NULL && mPopup->isShowing()) {
//            positionPopup();
//        }
    }
    
    return changed;
}

void AbsListView::layoutChildren() {
}

void AbsListView::updateScrollIndicators() {
    if (mScrollUp != NULL) {
        bool canScrollUp;
        // 0th element is not visible
        canScrollUp = mFirstPosition > 0;
        
        // ... Or top of 0th element is not visible
        if (!canScrollUp) {
            if (getChildCount() > 0) {
                shared_ptr<View> child = getChildAt(0);
                canScrollUp = child->getTop() < mListPadding.fTop;
            }
        }
        
        mScrollUp->setVisibility(canScrollUp ? View::VISIBLE : View::INVISIBLE);
    }
    
    if (mScrollDown != NULL) {
        bool canScrollDown;
        int count = getChildCount();
        
        // Last item is not visible
        canScrollDown = (mFirstPosition + count) < mItemCount;
        
        // ... Or bottom of the last element is not visible
        if (!canScrollDown && count > 0) {
            shared_ptr<View> child = getChildAt(count - 1);
            canScrollDown = child->getBottom() > mBottom - mListPadding.fBottom;
        }
        
        mScrollDown->setVisibility(canScrollDown ? View::VISIBLE : View::INVISIBLE);
    }
}

View *AbsListView::getSelectedView() {
    if (mItemCount > 0 && mSelectedPosition >= 0) {
        return getChildAt(mSelectedPosition - mFirstPosition).get();
    } else {
        return NULL;
    }
}

int AbsListView::getListPaddingTop() {
    return mListPadding.fTop;
}

int AbsListView::getListPaddingBottom() {
    return mListPadding.fBottom;
}

int AbsListView::getListPaddingLeft() {
    return mListPadding.fLeft;
}

int AbsListView::getListPaddingRight() {
    return mListPadding.fRight;
}

shared_ptr<View> AbsListView::obtainView(int position) {
    shared_ptr<View> scrapView;
    
    scrapView = mRecycler.getScrapView(position);
    
    shared_ptr<View> child;
    if (scrapView != NULL) {
        
        child = mAdapter->getView(position, scrapView, this);
        
        if (child != scrapView) {
            mRecycler.addScrapView(scrapView);
            if (mCacheColorHint != 0) {
                child->setDrawingCacheBackgroundColor(mCacheColorHint);
            }
        }
    } else {
        child = mAdapter->getView(position, NULL, this);
        if (mCacheColorHint != 0) {
            child->setDrawingCacheBackgroundColor(mCacheColorHint);
        }
    }
    
    return child;
}

void AbsListView::positionSelector(shared_ptr<View> sel) {
    Rect selectorRect = mSelectorRect;
    selectorRect.set(sel->getLeft(), sel->getTop(), sel->getRight(), sel->getBottom());
    positionSelector(selectorRect.fLeft, selectorRect.fTop, selectorRect.fRight,
                     selectorRect.fBottom);
    
    const bool isChildViewEnabled = mIsChildViewEnabled;
    if (sel->isEnabled() != isChildViewEnabled) {
        mIsChildViewEnabled = !isChildViewEnabled;
        refreshDrawableState();
    }
}

void AbsListView::positionSelector(int l, int t, int r, int b) {
    mSelectorRect.set(l - mSelectionLeftPadding, t - mSelectionTopPadding, r
                      + mSelectionRightPadding, b + mSelectionBottomPadding);
}

void AbsListView::dispatchDraw(shared_ptr<Canvas> canvas) {
    int saveCount = 0;
    const bool clipToPadding = (mGroupFlags & CLIP_TO_PADDING_MASK) == CLIP_TO_PADDING_MASK;
    if (clipToPadding) {
        saveCount = canvas->save();
        const int scrollX = mScrollX;
        const int scrollY = mScrollY;
        canvas->clipRect(scrollX + mPaddingLeft, scrollY + mPaddingTop,
                        scrollX + mRight - mLeft - mPaddingRight,
                        scrollY + mBottom - mTop - mPaddingBottom);
        mGroupFlags &= ~CLIP_TO_PADDING_MASK;
    }
    
    const bool drawSelectorOnTop = mDrawSelectorOnTop;
    if (!drawSelectorOnTop) {
        drawSelector(canvas);
    }
    
    AdapterView<ListAdapter>::dispatchDraw(canvas);
    
    if (drawSelectorOnTop) {
        drawSelector(canvas);
    }
    
    if (clipToPadding) {
        canvas->restoreToCount(saveCount);
        mGroupFlags |= CLIP_TO_PADDING_MASK;
    }
}

void AbsListView::onSizeChanged(int w, int h, int oldw, int oldh) {
    if (getChildCount() > 0) {
        mDataChanged = true;
        rememberSyncState();
    }
    
    if (mFastScroller != NULL) {
//        mFastScroller.onSizeChanged(w, h, oldw, oldh);
    }
}

bool AbsListView::touchModeDrawsInPressedState() {
    // FIXME use isPressed for this
    switch (mTouchMode) {
        case TOUCH_MODE_TAP:
        case TOUCH_MODE_DONE_WAITING:
            return true;
        default:
            return false;
    }
}

bool AbsListView::shouldShowSelector() {
    return (hasFocus() && !isInTouchMode()) || touchModeDrawsInPressedState();
}

void AbsListView::drawSelector(shared_ptr<Canvas> canvas) {
    if (shouldShowSelector() && &mSelectorRect != NULL && !mSelectorRect.isEmpty()) {
        const shared_ptr<Drawable> &selector = mSelector;
        selector->setBounds(mSelectorRect);
        selector->draw(canvas);
    }
}

void AbsListView::setDrawSelectorOnTop(bool onTop) {
    mDrawSelectorOnTop = onTop;
}

void AbsListView::setSelector(string resID) {
    setSelector(getResources()->getDrawable(resID));
}

void AbsListView::setSelector(shared_ptr<Drawable> sel) {
    if (mSelector != NULL) {
        mSelector->setCallback(NULL);
        unscheduleDrawable(mSelector.get());
    }
    mSelector = sel;
    Rect padding = Rect();
    sel->getPadding(padding);
    mSelectionLeftPadding = padding.fLeft;
    mSelectionTopPadding = padding.fTop;
    mSelectionRightPadding = padding.fRight;
    mSelectionBottomPadding = padding.fBottom;
    sel->setCallback(this);
    sel->setState(getDrawableState());
}

shared_ptr<Drawable> AbsListView::getSelector() {
    return mSelector;
}

void AbsListView::keyPressed() {
    shared_ptr<Drawable> &selector = mSelector;
    Rect &selectorRect = mSelectorRect;
    if (selector != NULL && (isFocused() || touchModeDrawsInPressedState())
        && &selectorRect != NULL && !selectorRect.isEmpty()) {
        
        const shared_ptr<View> &v = getChildAt(mSelectedPosition - mFirstPosition);
        
        if (v != NULL) {
            if (v->hasFocusable()) return;
            v->setPressed(true);
        }
        setPressed(true);
        
        const bool longClickable = isLongClickable();
        shared_ptr<Drawable> d = selector->getCurrent();
        shared_ptr<TransitionDrawable> td = static_pointer_cast<TransitionDrawable>(d);
        if (d != NULL && td != NULL) {
            if (longClickable) {
                td->startTransition(DEFAULT_LONG_PRESS_TIMEOUT);
            } else {
                td->resetTransition();
            }
        }
        if (longClickable && !mDataChanged) {
            if (mPendingCheckForKeyLongPress == NULL) {
                mPendingCheckForKeyLongPress = new CheckForKeyLongPress(this);
            }
            mPendingCheckForKeyLongPress->rememberWindowAttachCount();
            postDelayed(mPendingCheckForKeyLongPress, DEFAULT_LONG_PRESS_TIMEOUT);
        }
    }
}

void AbsListView::setScrollIndicators(View *up, View *down) {
    mScrollUp = up;
    mScrollDown = down;
}

void AbsListView::drawableStateChanged() {
    AdapterView<ListAdapter>::drawableStateChanged();
    if (mSelector != NULL) {
        mSelector->setState(getDrawableState());
    }
}

vector<int> AbsListView::onCreateDrawableState(int extraSpace) {
    // If the child view is enabled then do the default behavior.
    if (mIsChildViewEnabled) {
        // Common case
        return AdapterView<ListAdapter>::onCreateDrawableState(extraSpace);
    }
    
    // The selector uses this View's drawable state. The selected child view
    // is disabled, so we need to remove the enabled state from the drawable
    // states.
    const int enabledState = ENABLED_STATE_SET[0];
    
    // If we don't have any extra space, it will return one of the static state arrays,
    // and clearing the enabled state on those arrays is a bad thing!  If we specify
    // we need extra space, it will create+copy into a new array that safely mutable.
    vector<int> state = AdapterView<ListAdapter>::onCreateDrawableState(extraSpace + 1);
    int enabledPos = -1;
    for (int i = state.size() - 1; i >= 0; i--) {
        if (state[i] == enabledState) {
            enabledPos = i;
            break;
        }
    }
    
    // Remove the enabled state
    if (enabledPos >= 0) {
        System::arraycopy(state, enabledPos + 1, state, enabledPos,
                         state.size() - enabledPos - 1);
    }
    
    return state;
}

bool AbsListView::verifyDrawable(shared_ptr<Drawable> dr) {
    return mSelector == dr || AdapterView<ListAdapter>::verifyDrawable(dr);
}

void AbsListView::onAttachedToWindow() {
    AdapterView<ListAdapter>::onAttachedToWindow();
    
    ViewTreeObserver *treeObserver = getViewTreeObserver();
    if (treeObserver != NULL) {
        treeObserver->addOnTouchModeChangeListener(this);
        if (mTextFilterEnabled && mPopup != NULL && !mGlobalLayoutListenerAddedFilter) {
            treeObserver->addOnGlobalLayoutListener(this);
        }
    }
}

void AbsListView::onDetachedFromWindow() {
    AdapterView<ListAdapter>::onDetachedFromWindow();
    
    // Dismiss the popup in case onSaveInstanceState() was not invoked
    dismissPopup();
    
    ViewTreeObserver *treeObserver = getViewTreeObserver();
    if (treeObserver != NULL) {
        treeObserver->removeOnTouchModeChangeListener(this);
        if (mTextFilterEnabled && mPopup != NULL) {
            treeObserver->removeOnGlobalLayoutListener(this);
            mGlobalLayoutListenerAddedFilter = false;
        }
    }
}

void AbsListView::onWindowFocusChanged(bool hasWindowFocus) {
    AdapterView<ListAdapter>::onWindowFocusChanged(hasWindowFocus);
    
    const int touchMode = isInTouchMode() ? TOUCH_MODE_ON : TOUCH_MODE_OFF;
    
    if (!hasWindowFocus) {
        setChildrenDrawingCacheEnabled(false);
        removeCallbacks(mFlingRunnable);
        // Always hide the type filter
        dismissPopup();
        
        if (touchMode == TOUCH_MODE_OFF) {
            // Remember the last selected element
            mResurrectToPosition = mSelectedPosition;
        }
    } else {
        if (mFiltered) {
            // Show the type filter only if a filter is in effect
            showPopup();
        }
        
        // If we changed touch mode since the last time we had focus
        if (touchMode != mLastTouchMode && mLastTouchMode != TOUCH_MODE_UNKNOWN) {
            // If we come back in trackball mode, we bring the selection back
            if (touchMode == TOUCH_MODE_OFF) {
                // This will trigger a layout
                resurrectSelection();
                
                // If we come back in touch mode, then we want to hide the selector
            } else {
                hideSelector();
                mLayoutMode = LAYOUT_NORMAL;
                layoutChildren();
            }
        }
    }
    
    mLastTouchMode = touchMode;
}

//ContextMenuInfo *AbsListView::createContextMenuInfo(View *view, int position, long id) {
//    return new AdapterContextMenuInfo(view, position, id);
//}

void AbsListView::WindowRunnnable::rememberWindowAttachCount() {
    mOriginalAttachCount = mAbsListView->getWindowAttachCount();
}

bool AbsListView::WindowRunnnable::sameWindow() {
    return mAbsListView->hasWindowFocus() && mAbsListView->getWindowAttachCount() == mOriginalAttachCount;
}

void AbsListView::PerformClick::run() {
    // The data has changed since we posted this action in the event queue,
    // bail out before bad things happen
    if (mAbsListView->mDataChanged) return;
    
    if (mAbsListView->mAdapter != NULL && mAbsListView->mItemCount > 0 &&
        mClickMotionPosition < mAbsListView->mAdapter->getCount() && sameWindow()) {
        mAbsListView->performItemClick(mChild.get(), mClickMotionPosition, mAbsListView->getAdapter()->getItemId(mClickMotionPosition));
    }
}

void AbsListView::CheckForLongPress::run() {
    const int motionPosition = mAbsListView->mMotionPosition;
    const shared_ptr<View> child = mAbsListView->getChildAt(motionPosition - mAbsListView->mFirstPosition);
    if (child != NULL) {
        const int longPressPosition = mAbsListView->mMotionPosition;
        const long longPressId = mAbsListView->mAdapter->getItemId(mAbsListView->mMotionPosition);
        
        bool handled = false;
        if (sameWindow() && !mAbsListView->mDataChanged) {
            handled = mAbsListView->performLongPress(child, longPressPosition, longPressId);
        }
        if (handled) {
            mAbsListView->mTouchMode = TOUCH_MODE_REST;
            mAbsListView->setPressed(false);
            child->setPressed(false);
        } else {
            mAbsListView->mTouchMode = TOUCH_MODE_DONE_WAITING;
        }
        
    }
}

void AbsListView::CheckForKeyLongPress::run() {
    if (mAbsListView->isPressed() && mAbsListView->mSelectedPosition >= 0) {
        int index = mAbsListView->mSelectedPosition - mAbsListView->mFirstPosition;
        shared_ptr<View> v = mAbsListView->getChildAt(index);
        
        if (!mAbsListView->mDataChanged) {
            bool handled = false;
            if (sameWindow()) {
                handled = mAbsListView->performLongPress(v, mAbsListView->mSelectedPosition, mAbsListView->mSelectedRowId);
            }
            if (handled) {
                mAbsListView->setPressed(false);
                v->setPressed(false);
            }
        } else {
            mAbsListView->setPressed(false);
            if (v != NULL) v->setPressed(false);
        }
    }
}

bool AbsListView::performLongPress(shared_ptr<View> child,
                      const int longPressPosition, const long longPressId) {
    bool handled = false;
    
    if (mOnItemLongClickListener != NULL) {
        handled = mOnItemLongClickListener->onItemLongClick(this, child.get(),
                                                           longPressPosition, longPressId);
    }
    if (!handled) {
//        mContextMenuInfo = createContextMenuInfo(child, longPressPosition, longPressId);
//        handled = AdapterView<ListAdapter>::showContextMenuForChild(this);
    }
    if (handled) {
//        performHapticFeedback(HapticFeedbackConstants.LONG_PRESS);
    }
    return handled;
}

//ContextMenuInfo *AbsListView::getContextMenuInfo() {
//    return mContextMenuInfo;
//}

//bool AbsListView::showContextMenuForChild(View *originalView) {
//    const int longPressPosition = getPositionForView(originalView);
//    if (longPressPosition >= 0) {
//        const long longPressId = mAdapter->getItemId(longPressPosition);
//        bool handled = false;
//        
//        if (mOnItemLongClickListener != NULL) {
//            handled = mOnItemLongClickListener.onItemLongClick(AbsListView::this, originalView,
//                                                               longPressPosition, longPressId);
//        }
//        if (!handled) {
//            mContextMenuInfo = createContextMenuInfo(
//                                                     getChildAt(longPressPosition - mFirstPosition),
//                                                     longPressPosition, longPressId);
//            handled = AdapterView<ListAdapter>::showContextMenuForChild(originalView);
//        }
//        
//        return handled;
//    }
//    return false;
//}

bool AbsListView::onKeyUp(int keyCode, KeyEvent *event) {
//    switch (keyCode) {
//        case KeyEvent::KEYCODE_DPAD_CENTER:
//        case KeyEvent->KEYCODE_ENTER:
//            if (isPressed() && mSelectedPosition >= 0 && mAdapter != NULL &&
//                mSelectedPosition < mAdapter->getCount()) {
//                const shared_ptr<View> view = getChildAt(mSelectedPosition - mFirstPosition);
//                performItemClick(view.get(), mSelectedPosition, mSelectedRowId);
//                setPressed(false);
//                if (view != NULL) view->setPressed(false);
//                return true;
//            }
//    }
    return AdapterView<ListAdapter>::onKeyUp(keyCode, event);
}

void AbsListView::dispatchSetPressed(bool pressed) {
    // Don't dispatch setPressed to our children. We call setPressed on ourselves to
    // get the selector in the right state, but we don't want to press each child->
}

int AbsListView::pointToPosition(int x, int y) {
    Rect &frame = mTouchFrame;
    if (&frame == NULL) {
        mTouchFrame = Rect();
        frame = mTouchFrame;
    }
    
    const int count = getChildCount();
    for (int i = count - 1; i >= 0; i--) {
        const shared_ptr<View> child = getChildAt(i);
        if (child->getVisibility() == View::VISIBLE) {
            child->getHitRect(frame);
            if (frame.contains(x, y)) {
                return mFirstPosition + i;
            }
        }
    }
    return INVALID_POSITION;
}

long AbsListView::pointToRowId(int x, int y) {
    int position = pointToPosition(x, y);
    if (position >= 0) {
        return mAdapter->getItemId(position);
    }
    return INVALID_ROW_ID;
}

void AbsListView::CheckForTap::run() {
    if (mAbsListView->mTouchMode == TOUCH_MODE_DOWN) {
        mAbsListView->mTouchMode = TOUCH_MODE_TAP;
        const shared_ptr<View> child = mAbsListView->getChildAt(mAbsListView->mMotionPosition - mAbsListView->mFirstPosition);
        if (child != NULL && !child->hasFocusable()) {
            mAbsListView->mLayoutMode = LAYOUT_NORMAL;
            
            if (!mAbsListView->mDataChanged) {
                mAbsListView->layoutChildren();
                child->setPressed(true);
                mAbsListView->positionSelector(child);
                mAbsListView->setPressed(true);
                
                const int longPressTimeout = View::DEFAULT_LONG_PRESS_TIMEOUT;
                const bool longClickable = mAbsListView->isLongClickable();
                
                if (mAbsListView->mSelector != NULL) {
                    shared_ptr<Drawable> d = mAbsListView->mSelector->getCurrent();
                    shared_ptr<TransitionDrawable> td = static_pointer_cast<TransitionDrawable>(d);
                    if (d != NULL && td != NULL) {
                        if (longClickable) {
                            td->startTransition(longPressTimeout);
                        } else {
                            td->resetTransition();
                        }
                    }
                }
                
                if (longClickable) {
                    if (mAbsListView->mPendingCheckForLongPress == NULL) {
                        mAbsListView->mPendingCheckForLongPress = new CheckForLongPress(mAbsListView);
                    }
                    mAbsListView->mPendingCheckForLongPress->rememberWindowAttachCount();
                    mAbsListView->postDelayed(mAbsListView->mPendingCheckForLongPress, longPressTimeout);
                } else {
                    mAbsListView->mTouchMode = TOUCH_MODE_DONE_WAITING;
                }
            } else {
                mAbsListView->mTouchMode = TOUCH_MODE_DONE_WAITING;
            }
        }
    }
}

bool AbsListView::startScrollIfNeeded(int deltaY) {
    // Check if we have moved far enough that it looks more like a
    // scroll than a tap
    const int distance = abs(deltaY);
    if (distance > mTouchSlop) {
        createScrollingCache();
        mTouchMode = TOUCH_MODE_SCROLL;
        mMotionCorrection = deltaY;
        sp<Handler> handler = getHandler();
        // Handler should not be NULL unless the AbsListView is not attached to a
        // window, which would make it very hard to scroll it... but the monkeys
        // say it's possible.
        if (handler != NULL) {
            handler->removeCallbacks(mPendingCheckForLongPress);
        }
        setPressed(false);
        shared_ptr<View> motionView = getChildAt(mMotionPosition - mFirstPosition);
        if (motionView != NULL) {
            motionView->setPressed(false);
        }
        reportScrollStateChange(OnScrollListener::SCROLL_STATE_TOUCH_SCROLL);
        // Time to start stealing events! Once we've stolen them, don't let anyone
        // steal from us
        requestDisallowInterceptTouchEvent(true);
        return true;
    }
    
    return false;
}

void AbsListView::onTouchModeChanged(bool isInTouchMode) {
    if (isInTouchMode) {
        // Get rid of the selection when we enter touch mode
        hideSelector();
        // Layout, but only if we already have done so previously.
        // (Otherwise may clobber a LAYOUT_SYNC layout that was requested to restore
        // state.)
        if (getHeight() > 0 && getChildCount() > 0) {
            // We do not lose focus initiating a touch (since AbsListView is focusable in
            // touch mode). Force an initial layout to get rid of the selection.
            mLayoutMode = LAYOUT_NORMAL;
            layoutChildren();
        }
    }
}

bool AbsListView::onTouchEvent(MotionEvent *ev) {
//    if (mFastScroller != NULL) {
//        bool intercepted = mFastScroller.onTouchEvent(ev);
//        if (intercepted) {
//            return true;
//        }
//    }
    
    const int action = ev->getAction();
    const int x = (int) ev->getX();
    const int y = (int) ev->getY();
    
    shared_ptr<View> v;
    int deltaY;
    
    if (mVelocityTracker == NULL) {
        mVelocityTracker = VelocityTracker::obtain();
    }
    mVelocityTracker->addMovement(ev);
    
    switch (action) {
        case MotionEvent::ACTION_DOWN: {
            int motionPosition = pointToPosition(x, y);
            if (!mDataChanged) {
                if ((mTouchMode != TOUCH_MODE_FLING) && (motionPosition >= 0)
                    && (getAdapter()->isEnabled(motionPosition))) {
                    // User clicked on an actual view (and was not stopping a fling). It might be a
                    // click or a scroll. Assume it is a click until proven otherwise
                    mTouchMode = TOUCH_MODE_DOWN;
                    // FIXME Debounce
                    if (mPendingCheckForTap == NULL) {
                        mPendingCheckForTap = new CheckForTap(this);
                    }
                    postDelayed(mPendingCheckForTap, TAP_TIMEOUT);
                } else {
//                    if (ev->getEdgeFlags() != 0 && motionPosition < 0) {
//                        // If we couldn't find a view to click on, but the down event was touching
//                        // the edge, we will bail out and try again. This allows the edge correcting
//                        // code in ViewRoot to try to find a nearby view to select
//                        return false;
//                    }
                    // User clicked on whitespace, or stopped a fling. It is a scroll.
                    createScrollingCache();
                    mTouchMode = TOUCH_MODE_SCROLL;
                    mMotionCorrection = 0;
                    motionPosition = findMotionRow(y);
                    reportScrollStateChange(OnScrollListener::SCROLL_STATE_TOUCH_SCROLL);
                }
            }
            
            if (motionPosition >= 0) {
                // Remember where the motion event started
                v = getChildAt(motionPosition - mFirstPosition);
                mMotionViewOriginalTop = v->getTop();
                mMotionX = x;
                mMotionY = y;
                mMotionPosition = motionPosition;
            }
            mLastY = INT_MIN;
            break;
        }
            
        case MotionEvent::ACTION_MOVE: {
            deltaY = y - mMotionY;
            switch (mTouchMode) {
                case TOUCH_MODE_DOWN:
                case TOUCH_MODE_TAP:
                case TOUCH_MODE_DONE_WAITING:
                    // Check if we have moved far enough that it looks more like a
                    // scroll than a tap
                    startScrollIfNeeded(deltaY);
                    break;
                case TOUCH_MODE_SCROLL:
                    if (PROFILE_SCROLLING) {
                        if (!mScrollProfilingStarted) {
//                            Debug.startMethodTracing("AbsListViewScroll");
                            mScrollProfilingStarted = true;
                        }
                    }
                    
                    if (y != mLastY) {
                        deltaY -= mMotionCorrection;
                        int incrementalDeltaY = mLastY != INT_MIN ? y - mLastY : deltaY;
                        trackMotionScroll(deltaY, incrementalDeltaY);
                        
                        // Check to see if we have bumped into the scroll limit
                        shared_ptr<View> motionView = getChildAt(mMotionPosition - mFirstPosition);
                        if (motionView != NULL) {
                            // Check if the top of the motion view is where it is
                            // supposed to be
                            if (motionView->getTop() != mMotionViewNewTop) {
                                // We did not scroll the full amount. Treat this essentially like the
                                // start of a new touch scroll
                                const int motionPosition = findMotionRow(y);
                                
                                mMotionCorrection = 0;
                                motionView = getChildAt(motionPosition - mFirstPosition);
                                mMotionViewOriginalTop = motionView->getTop();
                                mMotionY = y;
                                mMotionPosition = motionPosition;
                            }
                        }
                        mLastY = y;
                    }
                    break;
            }
            
            break;
        }
            
        case MotionEvent::ACTION_UP: {
            switch (mTouchMode) {
                case TOUCH_MODE_DOWN:
                case TOUCH_MODE_TAP:
                case TOUCH_MODE_DONE_WAITING:
                {
                    const int motionPosition = mMotionPosition;
                    const shared_ptr<View> child = getChildAt(motionPosition - mFirstPosition);
                    if (child != NULL && !child->hasFocusable()) {
                        if (mTouchMode != TOUCH_MODE_DOWN) {
                            child->setPressed(false);
                        }
                        
                        if (mPerformClick == NULL) {
                            mPerformClick = new PerformClick(this);
                        }
                        
                        const sp<AbsListView::PerformClick> &performClick = mPerformClick;
                        performClick->mChild = child;
                        performClick->mClickMotionPosition = motionPosition;
                        performClick->rememberWindowAttachCount();
                        
                        mResurrectToPosition = motionPosition;
                        
                        if (mTouchMode == TOUCH_MODE_DOWN || mTouchMode == TOUCH_MODE_TAP) {
                            const sp<Handler> &handler = getHandler();
                            if (handler != NULL) {
                                sp<Runnable> toRun = mPendingCheckForLongPress;
                                if (mTouchMode == TOUCH_MODE_DOWN) toRun = mPendingCheckForTap;
                                handler->removeCallbacks(toRun);
                            }
                            mLayoutMode = LAYOUT_NORMAL;
                            mTouchMode = TOUCH_MODE_TAP;
                            if (!mDataChanged) {
                                setSelectedPositionInt(mMotionPosition);
                                layoutChildren();
                                child->setPressed(true);
                                positionSelector(child);
                                setPressed(true);
                                if (mSelector != NULL) {
                                    shared_ptr<Drawable> d = mSelector->getCurrent();
                                    shared_ptr<TransitionDrawable> td = static_pointer_cast<TransitionDrawable>(d);
                                    if (d != NULL && td != NULL) {
                                        td->resetTransition();
                                    }
                                }
                                sp<PressedRunnable> pressedRunnable = new PressedRunnable(this, child, performClick);
                                postDelayed(pressedRunnable, PRESSED_STATE_DURATION);
                            }
                            return true;
                        } else {
                            if (!mDataChanged) {
                                post(performClick);
                            }
                        }
                    }
                    mTouchMode = TOUCH_MODE_REST;
                    break;
                }
                case TOUCH_MODE_SCROLL:
                {
                    VelocityTracker *velocityTracker = mVelocityTracker;
                    velocityTracker->computeCurrentVelocity(1000, mMaximumVelocity);
                    const int initialVelocity = (int) velocityTracker->getYVelocity();
                    if (abs(initialVelocity) > mMinimumVelocity && (getChildCount() > 0)) {
                        if (mFlingRunnable == NULL) {
                            mFlingRunnable = new FlingRunnable(this);
                        }
                        reportScrollStateChange(OnScrollListener::SCROLL_STATE_FLING);
                        mFlingRunnable->start(-initialVelocity);
                    } else {
                        mTouchMode = TOUCH_MODE_REST;
                        reportScrollStateChange(OnScrollListener::SCROLL_STATE_IDLE);
                    }
                }
            }
            
            setPressed(false);
            
            // Need to redraw since we probably aren't drawing the selector anymore
            invalidate();
            
            const sp<Handler> &handler = getHandler();
            if (handler != NULL) {
                handler->removeCallbacks(mPendingCheckForLongPress);
            }
            
            if (mVelocityTracker != NULL) {
                mVelocityTracker->recycle();
                mVelocityTracker = NULL;
            }
            
            if (PROFILE_SCROLLING) {
                if (mScrollProfilingStarted) {
//                    Debug.stopMethodTracing();
                    mScrollProfilingStarted = false;
                }
            }
            break;
        }
            
        case MotionEvent::ACTION_CANCEL: {
            mTouchMode = TOUCH_MODE_REST;
            setPressed(false);
            shared_ptr<View> motionView = getChildAt(mMotionPosition - mFirstPosition);
            if (motionView != NULL) {
                motionView->setPressed(false);
            }
            clearScrollingCache();
            
            const sp<Handler> &handler = getHandler();
            if (handler != NULL) {
                handler->removeCallbacks(mPendingCheckForLongPress);
            }
            
            if (mVelocityTracker != NULL) {
                mVelocityTracker->recycle();
                mVelocityTracker = NULL;
            }
        }
            
    }
    
    return true;
}

void AbsListView::draw(shared_ptr<Canvas> canvas) {
    AdapterView<ListAdapter>::draw(canvas);
//    if (mFastScroller != NULL) {
//        mFastScroller->draw(canvas);
//    }
}

bool AbsListView::onInterceptTouchEvent(MotionEvent *ev) {
    int action = ev->getAction();
    int x = (int) ev->getX();
    int y = (int) ev->getY();
    shared_ptr<View> v;
    
    if (mFastScroller != NULL) {
//        bool intercepted = mFastScroller.onInterceptTouchEvent(ev);
//        if (intercepted) {
//            return true;
//        }
    }
    
    switch (action) {
        case MotionEvent::ACTION_DOWN: {
            int motionPosition = findMotionRow(y);
            if (mTouchMode != TOUCH_MODE_FLING && motionPosition >= 0) {
                // User clicked on an actual view (and was not stopping a fling).
                // Remember where the motion event started
                v = getChildAt(motionPosition - mFirstPosition);
                mMotionViewOriginalTop = v->getTop();
                mMotionX = x;
                mMotionY = y;
                mMotionPosition = motionPosition;
                mTouchMode = TOUCH_MODE_DOWN;
                clearScrollingCache();
            }
            mLastY = INT_MIN;
            break;
        }
            
        case MotionEvent::ACTION_MOVE: {
            switch (mTouchMode) {
                case TOUCH_MODE_DOWN:
                    if (startScrollIfNeeded(y - mMotionY)) {
                        return true;
                    }
                    break;
            }
            break;
        }
            
        case MotionEvent::ACTION_UP: {
            mTouchMode = TOUCH_MODE_REST;
            reportScrollStateChange(OnScrollListener::SCROLL_STATE_IDLE);
            break;
        }
    }
    
    return false;
}

void AbsListView::addTouchables(vector<shared_ptr<View>> &views) {
    const int count = getChildCount();
    const int firstPosition = mFirstPosition;
    shared_ptr<ListAdapter> &adapter = mAdapter;
    
    if (adapter == NULL) {
        return;
    }
    
    for (int i = 0; i < count; i++) {
        const shared_ptr<View> child = getChildAt(i);
        if (adapter->isEnabled(firstPosition + i)) {
            views.push_back(child);
        }
        child->addTouchables(views);
    }
}

void AbsListView::reportScrollStateChange(int newState) {
    if (newState != mLastScrollState) {
        if (mOnScrollListener != NULL) {
            mOnScrollListener->onScrollStateChanged(this, newState);
            mLastScrollState = newState;
        }
    }
}
    
AbsListView::FlingRunnable::FlingRunnable(AbsListView *absListView) {
    mAbsListView = absListView;
//    mScroller = new Scroller(getContext());
}

void AbsListView::FlingRunnable::start(int initialVelocity) {
    int initialY = initialVelocity < 0 ? INT_MAX : 0;
    mLastFlingY = initialY;
//    mScroller.fling(0, initialY, 0, initialVelocity,
//                    0, INT_MAX, 0, INT_MAX);
    mAbsListView->mTouchMode = TOUCH_MODE_FLING;
    mAbsListView->post(this);
    
    if (PROFILE_FLINGING) {
        if (!mAbsListView->mFlingProfilingStarted) {
//            Debug.startMethodTracing("AbsListViewFling");
            mAbsListView->mFlingProfilingStarted = true;
        }
    }
}

void AbsListView::FlingRunnable::endFling() {
    mAbsListView->mTouchMode = TOUCH_MODE_REST;
    mAbsListView->reportScrollStateChange(OnScrollListener::SCROLL_STATE_IDLE);
    mAbsListView->clearScrollingCache();
}

void AbsListView::FlingRunnable::run() {
    if (mAbsListView->mTouchMode != TOUCH_MODE_FLING) {
        return;
    }
    
    if (mAbsListView->mItemCount == 0 || mAbsListView->getChildCount() == 0) {
        endFling();
        return;
    }
    
//    const Scroller scroller = mAbsListView->mScroller;
    bool more = false;//scroller.computeScrollOffset();
    const int y = 0;//scroller.getCurrY();
    
    // Flip sign to convert finger direction to list items direction
    // (e.g. finger moving down means list is moving towards the top)
    int delta = mLastFlingY - y;
    
    // Pretend that each frame of a fling scroll is a touch scroll
    if (delta > 0) {
        // List is moving towards the top. Use first view as mMotionPosition
        mAbsListView->mMotionPosition = mAbsListView->mFirstPosition;
        const shared_ptr<View> &firstView = mAbsListView->getChildAt(0);
        mAbsListView->mMotionViewOriginalTop = firstView->getTop();
        
        // Don't fling more than 1 screen
        delta = min(mAbsListView->getHeight() - mAbsListView->mPaddingBottom - mAbsListView->mPaddingTop - 1, delta);
    } else {
        // List is moving towards the bottom. Use last view as mMotionPosition
        int offsetToLast = mAbsListView->getChildCount() - 1;
        mAbsListView->mMotionPosition = mAbsListView->mFirstPosition + offsetToLast;
        
        const shared_ptr<View> lastView = mAbsListView->getChildAt(offsetToLast);
        mAbsListView->mMotionViewOriginalTop = lastView->getTop();
        
        // Don't fling more than 1 screen
        delta = max(-(mAbsListView->getHeight() - mAbsListView->mPaddingBottom - mAbsListView->mPaddingTop - 1), delta);
    }
    
    mAbsListView->trackMotionScroll(delta, delta);
    
    // Check to see if we have bumped into the scroll limit
    shared_ptr<View> motionView = mAbsListView->getChildAt(mAbsListView->mMotionPosition - mAbsListView->mFirstPosition);
    if (motionView != NULL) {
        // Check if the top of the motion view is where it is
        // supposed to be
        if (motionView->getTop() != mAbsListView->mMotionViewNewTop) {
            more = false;
        }
    }
    
    if (more) {
        mAbsListView->invalidate();
        mLastFlingY = y;
        mAbsListView->post(this);
    } else {
        endFling();
        if (PROFILE_FLINGING) {
            if (mAbsListView->mFlingProfilingStarted) {
//                Debug.stopMethodTracing();
                mAbsListView->mFlingProfilingStarted = false;
            }
        }
    }
}

void AbsListView::createScrollingCache() {
    if (mScrollingCacheEnabled && !mCachingStarted) {
        setChildrenDrawnWithCacheEnabled(true);
        setChildrenDrawingCacheEnabled(true);
        mCachingStarted = true;
    }
}

void AbsListView::clearScrollingCache() {
    if (mClearScrollingCache == NULL) {
        mClearScrollingCache = new ScrollingCacheRunnable(this);
    }
    post(mClearScrollingCache);
}

void AbsListView::trackMotionScroll(int deltaY, int incrementalDeltaY) {
    const int childCount = getChildCount();
    if (childCount == 0) {
        return;
    }
    
    const int firstTop = getChildAt(0)->getTop();
    const int lastBottom = getChildAt(childCount - 1)->getBottom();
    
    const Rect listPadding = mListPadding;
    
    // FIXME account for grid vertical spacing too?
    const int spaceAbove = listPadding.fTop - firstTop;
    const int end = getHeight() - listPadding.fBottom;
    const int spaceBelow = lastBottom - end;
    
    const int height = getHeight() - mPaddingBottom - mPaddingTop;
    if (deltaY < 0) {
        deltaY = max(-(height - 1), deltaY);
    } else {
        deltaY = min(height - 1, deltaY);
    }
    
    if (incrementalDeltaY < 0) {
        incrementalDeltaY = max(-(height - 1), incrementalDeltaY);
    } else {
        incrementalDeltaY = min(height - 1, incrementalDeltaY);
    }
    
    const int absIncrementalDeltaY = abs(incrementalDeltaY);
    
    if (spaceAbove >= absIncrementalDeltaY && spaceBelow >= absIncrementalDeltaY) {
        hideSelector();
        offsetChildrenTopAndBottom(incrementalDeltaY);
        invalidate();
        mMotionViewNewTop = mMotionViewOriginalTop + deltaY;
    } else {
        const int firstPosition = mFirstPosition;
        
        if (firstPosition == 0 && firstTop >= listPadding.fTop && deltaY > 0) {
            // Don't need to move views down if the top of the first position is already visible
            return;
        }
        
        if (firstPosition + childCount == mItemCount && lastBottom <= end && deltaY < 0) {
            // Don't need to move views up if the bottom of the last position is already visible
            return;
        }
        
        const bool down = incrementalDeltaY < 0;
        
        hideSelector();
        
        const int headerViewsCount = getHeaderViewsCount();
        const int footerViewsStart = mItemCount - getFooterViewsCount();
        
        int start = 0;
        int count = 0;
        
        if (down) {
            const int top = listPadding.fTop - incrementalDeltaY;
            for (int i = 0; i < childCount; i++) {
                const shared_ptr<View> child = getChildAt(i);
                if (child->getBottom() >= top) {
                    break;
                } else {
                    count++;
                    int position = firstPosition + i;
                    if (position >= headerViewsCount && position < footerViewsStart) {
                        mRecycler.addScrapView(child);
                    }
                }
            }
        } else {
            const int bottom = getHeight() - listPadding.fBottom - incrementalDeltaY;
            for (int i = childCount - 1; i >= 0; i--) {
                const shared_ptr<View> child = getChildAt(i);
                if (child->getTop() <= bottom) {
                    break;
                } else {
                    start = i;
                    count++;
                    int position = firstPosition + i;
                    if (position >= headerViewsCount && position < footerViewsStart) {
                        mRecycler.addScrapView(child);
                    }
                }
            }
        }
        
        mMotionViewNewTop = mMotionViewOriginalTop + deltaY;
        
        mBlockLayoutRequests = true;
        detachViewsFromParent(start, count);
        offsetChildrenTopAndBottom(incrementalDeltaY);
        
        if (down) {
            mFirstPosition += count;
        }
        
        invalidate();
        fillGap(down);
        mBlockLayoutRequests = false;
        
        invokeOnItemScrollListener();
    }
}

int AbsListView::getHeaderViewsCount() {
    return 0;
}

int AbsListView::getFooterViewsCount() {
    return 0;
}

void AbsListView::hideSelector() {
    if (mSelectedPosition != INVALID_POSITION) {
        mResurrectToPosition = mSelectedPosition;
        if (mNextSelectedPosition >= 0 && mNextSelectedPosition != mSelectedPosition) {
            mResurrectToPosition = mNextSelectedPosition;
        }
        setSelectedPositionInt(INVALID_POSITION);
        setNextSelectedPositionInt(INVALID_POSITION);
        mSelectedTop = 0;
        mSelectorRect.setEmpty();
    }
}

int AbsListView::reconcileSelectedPosition() {
    int position = mSelectedPosition;
    if (position < 0) {
        position = mResurrectToPosition;
    }
    position = max(0, position);
    position = min(position, mItemCount - 1);
    return position;
}

void AbsListView::invalidateViews() {
    mDataChanged = true;
    rememberSyncState();
    requestLayout();
    invalidate();
}

bool AbsListView::resurrectSelection() {
    const int childCount = getChildCount();
    
    if (childCount <= 0) {
        return false;
    }
    
    int selectedTop = 0;
    int selectedPos;
    int childrenTop = mListPadding.fTop;
    int childrenBottom = mBottom - mTop - mListPadding.fBottom;
    const int firstPosition = mFirstPosition;
    const int toPosition = mResurrectToPosition;
    bool down = true;
    
    if (toPosition >= firstPosition && toPosition < firstPosition + childCount) {
        selectedPos = toPosition;
        
        const shared_ptr<View> &selected = getChildAt(selectedPos - mFirstPosition);
        selectedTop = selected->getTop();
        int selectedBottom = selected->getBottom();
        
        // We are scrolled, don't get in the fade
        if (selectedTop < childrenTop) {
            selectedTop = childrenTop + getVerticalFadingEdgeLength();
        } else if (selectedBottom > childrenBottom) {
            selectedTop = childrenBottom - selected->getMeasuredHeight()
            - getVerticalFadingEdgeLength();
        }
    } else {
        if (toPosition < firstPosition) {
            // Default to selecting whatever is first
            selectedPos = firstPosition;
            for (int i = 0; i < childCount; i++) {
                const shared_ptr<View> &v = getChildAt(i);
                const int top = v->getTop();
                
                if (i == 0) {
                    // Remember the position of the first item
                    selectedTop = top;
                    // See if we are scrolled at all
                    if (firstPosition > 0 || top < childrenTop) {
                        // If we are scrolled, don't select anything that is
                        // in the fade region
                        childrenTop += getVerticalFadingEdgeLength();
                    }
                }
                if (top >= childrenTop) {
                    // Found a view whose top is fully visisble
                    selectedPos = firstPosition + i;
                    selectedTop = top;
                    break;
                }
            }
        } else {
            const int itemCount = mItemCount;
            down = false;
            selectedPos = firstPosition + childCount - 1;
            
            for (int i = childCount - 1; i >= 0; i--) {
                const shared_ptr<View> &v = getChildAt(i);
                const int top = v->getTop();
                const int bottom = v->getBottom();
                
                if (i == childCount - 1) {
                    selectedTop = top;
                    if (firstPosition + childCount < itemCount || bottom > childrenBottom) {
                        childrenBottom -= getVerticalFadingEdgeLength();
                    }
                }
                
                if (bottom <= childrenBottom) {
                    selectedPos = firstPosition + i;
                    selectedTop = top;
                    break;
                }
            }
        }
    }
    
    mResurrectToPosition = INVALID_POSITION;
    removeCallbacks(mFlingRunnable);
    mTouchMode = TOUCH_MODE_REST;
    clearScrollingCache();
    mSpecificTop = selectedTop;
    selectedPos = lookForSelectablePosition(selectedPos, down);
    if (selectedPos >= firstPosition && selectedPos <= getLastVisiblePosition()) {
        mLayoutMode = LAYOUT_SPECIFIC;
        setSelectionInt(selectedPos);
        invokeOnItemScrollListener();
    } else {
        selectedPos = INVALID_POSITION;
    }
    reportScrollStateChange(OnScrollListener::SCROLL_STATE_IDLE);
    
    return selectedPos >= 0;
}

void AbsListView::handleDataChanged() {
    int count = mItemCount;
    if (count > 0) {
        
        int newPos;
        
        int selectablePos;
        
        // Find the row we are supposed to sync to
        if (mNeedSync) {
            // Update this first, since setNextSelectedPositionInt inspects it
            mNeedSync = false;
            
            if (mTranscriptMode == TRANSCRIPT_MODE_ALWAYS_SCROLL ||
                (mTranscriptMode == TRANSCRIPT_MODE_NORMAL &&
                 mFirstPosition + getChildCount() >= mOldItemCount)) {
                    mLayoutMode = LAYOUT_FORCE_BOTTOM;
                    return;
                }
            
            switch (mSyncMode) {
                case SYNC_SELECTED_POSITION:
                    if (isInTouchMode()) {
                        // We saved our state when not in touch mode. (We know this because
                        // mSyncMode is SYNC_SELECTED_POSITION.) Now we are trying to
                        // restore in touch mode. Just leave mSyncPosition as it is (possibly
                        // adjusting if the available range changed) and return.
                        mLayoutMode = LAYOUT_SYNC;
                        mSyncPosition = min(max(0, mSyncPosition), count - 1);
                        
                        return;
                    } else {
                        // See if we can find a position in the new data with the same
                        // id as the old selection. This will change mSyncPosition.
                        newPos = findSyncPosition();
                        if (newPos >= 0) {
                            // Found it. Now verify that new selection is still selectable
                            selectablePos = lookForSelectablePosition(newPos, true);
                            if (selectablePos == newPos) {
                                // Same row id is selected
                                mSyncPosition = newPos;
                                
                                if (mSyncHeight == getHeight()) {
                                    // If we are at the same height as when we saved state, try
                                    // to restore the scroll position too.
                                    mLayoutMode = LAYOUT_SYNC;
                                } else {
                                    // We are not the same height as when the selection was saved, so
                                    // don't try to restore the exact position
                                    mLayoutMode = LAYOUT_SET_SELECTION;
                                }
                                
                                // Restore selection
                                setNextSelectedPositionInt(newPos);
                                return;
                            }
                        }
                    }
                    break;
                case SYNC_FIRST_POSITION:
                    // Leave mSyncPosition as it is -- just pin to available range
                    mLayoutMode = LAYOUT_SYNC;
                    mSyncPosition = min(max(0, mSyncPosition), count - 1);
                    
                    return;
            }
        }
        
        if (!isInTouchMode()) {
            // We couldn't find matching data -- try to use the same position
            newPos = getSelectedItemPosition();
            
            // Pin position to the available range
            if (newPos >= count) {
                newPos = count - 1;
            }
            if (newPos < 0) {
                newPos = 0;
            }
            
            // Make sure we select something selectable -- first look down
            selectablePos = lookForSelectablePosition(newPos, true);
            
            if (selectablePos >= 0) {
                setNextSelectedPositionInt(selectablePos);
                return;
            } else {
                // Looking down didn't work -- try looking up
                selectablePos = lookForSelectablePosition(newPos, false);
                if (selectablePos >= 0) {
                    setNextSelectedPositionInt(selectablePos);
                    return;
                }
            }
        } else {
            
            // We already know where we want to resurrect the selection
            if (mResurrectToPosition >= 0) {
                return;
            }
        }
        
    }
    
    // Nothing is selected. Give up and reset everything.
    mLayoutMode = mStackFromBottom ? LAYOUT_FORCE_BOTTOM : LAYOUT_FORCE_TOP;
    mSelectedPosition = INVALID_POSITION;
    mSelectedRowId = INVALID_ROW_ID;
    mNextSelectedPosition = INVALID_POSITION;
    mNextSelectedRowId = INVALID_ROW_ID;
    mNeedSync = false;
    checkSelectionChanged();
}

void AbsListView::dismissPopup() {
    if (mPopup != NULL) {
//        mPopup->dismiss();
    }
}

void AbsListView::showPopup() {
    // Make sure we have a window before showing the popup
    if (getWindowVisibility() == View::VISIBLE) {
//        createTextFilter(true);
        positionPopup();
        // Make sure we get focus if we are showing the popup
        checkFocus();
    }
}

void AbsListView::positionPopup() {
    int screenHeight = getResources()->getDisplayMetrics().heightPixels;
    vector<int> xy = vector<int>(2);
    getLocationOnScreen(xy);
    // TODO: The 20 below should come from the theme
    // TODO: And the gravity should be defined in the theme as well
    const int bottomGap = screenHeight - xy[1] - getHeight() + (int) (mDensityScale * 20);
//    if (!mPopup->isShowing()) {
//        mPopup->showAtLocation(this, Gravity::BOTTOM | Gravity::CENTER_HORIZONTAL, xy[0], bottomGap);
//    } else {
//        mPopup->update(xy[0], bottomGap, -1, -1);
//    }
}

int AbsListView::getDistance(Rect &source, Rect &dest, int direction) {
    int sX, sY; // source x, y
    int dX, dY; // dest x, y
    switch (direction) {
        case View::FOCUS_RIGHT:
            sX = source.fRight;
            sY = source.fTop + source.height() / 2;
            dX = dest.fLeft;
            dY = dest.fTop + dest.height() / 2;
            break;
        case View::FOCUS_DOWN:
            sX = source.fLeft + source.width() / 2;
            sY = source.fBottom;
            dX = dest.fLeft + dest.width() / 2;
            dY = dest.fTop;
            break;
        case View::FOCUS_LEFT:
            sX = source.fLeft;
            sY = source.fTop + source.height() / 2;
            dX = dest.fRight;
            dY = dest.fTop + dest.height() / 2;
            break;
        case View::FOCUS_UP:
            sX = source.fLeft + source.width() / 2;
            sY = source.fTop;
            dX = dest.fLeft + dest.width() / 2;
            dY = dest.fBottom;
            break;
        default:
            throw IllegalArgumentException("direction must be one of {FOCUS_UP, FOCUS_DOWN, FOCUS_LEFT, FOCUS_RIGHT}.");
    }
    int deltaX = dX - sX;
    int deltaY = dY - sY;
    return deltaY * deltaY + deltaX * deltaX;
}

bool AbsListView::isInFilterMode() {
    return mFiltered;
}

bool AbsListView::sendToTextFilter(int keyCode, int count, KeyEvent *event) {
    if (!acceptFilter()) {
        return false;
    }
    
    bool handled = false;
//    bool okToSend = true;
//    switch (keyCode) {
//        case KeyEvent::KEYCODE_DPAD_UP:
//        case KeyEvent::KEYCODE_DPAD_DOWN:
//        case KeyEvent::KEYCODE_DPAD_LEFT:
//        case KeyEvent::KEYCODE_DPAD_RIGHT:
//        case KeyEvent::KEYCODE_DPAD_CENTER:
//        case KeyEvent::KEYCODE_ENTER:
//            okToSend = false;
//            break;
//        case KeyEvent::KEYCODE_BACK:
//            if (mFiltered && mPopup != NULL && mPopup->isShowing() &&
//                event.getAction() == KeyEvent->ACTION_DOWN) {
//                handled = true;
//                mTextFilter->setText("");
//            }
//            okToSend = false;
//            break;
//        case KeyEvent->KEYCODE_SPACE:
//            // Only send spaces once we are filtered
//            okToSend = mFiltered = true;
//            break;
//    }
//    
//    if (okToSend) {
//        createTextFilter(true);
//        
//        KeyEvent *forwardEvent = event;
//        if (forwardEvent->getRepeatCount() > 0) {
//            forwardEvent = KeyEvent->changeTimeRepeat(event, event.getEventTime(), 0);
//        }
//        
//        int action = event.getAction();
//        switch (action) {
//            case KeyEvent->ACTION_DOWN:
//                handled = mTextFilter->onKeyDown(keyCode, forwardEvent);
//                break;
//                
//            case KeyEvent->ACTION_UP:
//                handled = mTextFilter->onKeyUp(keyCode, forwardEvent);
//                break;
//                
//            case KeyEvent->ACTION_MULTIPLE:
//                handled = mTextFilter->onKeyMultiple(keyCode, count, event);
//                break;
//        }
//    }
    return handled;
}

//InputConnection *AbsListView::onCreateInputConnection(EditorInfo *outAttrs) {
//    if (isTextFilterEnabled()) {
//        // XXX we need to have the text filter created, so we can get an
//        // InputConnection to proxy to.  Unfortunately this means we pretty
//        // much need to make it as soon as a list view gets focus.
//        createTextFilter(false);
//        if (mPublicInputConnection == NULL) {
//            mDefInputConnection = new BaseInputConnection(this, false);
//            mPublicInputConnection = new InputConnectionWrapper(
//                                                                mTextFilter->onCreateInputConnection(outAttrs), true) {
//                                public bool reportFullscreenMode(bool enabled) {
//                    // Use our own input connection, since it is
//                    // the "real" one the IME is talking with.
//                    return mDefInputConnection.reportFullscreenMode(enabled);
//                }
//                
//                                public bool performEditorAction(int editorAction) {
//                    // The editor is off in its own window; we need to be
//                    // the one that does this.
//                    if (editorAction == EditorInfo.IME_ACTION_DONE) {
//                        InputMethodManager imm = (InputMethodManager)
//                        getContext()->getSystemService(
//                                                      Context.INPUT_METHOD_SERVICE);
//                        if (imm != NULL) {
//                            imm.hideSoftInputFromWindow(getWindowToken(), 0);
//                        }
//                        return true;
//                    }
//                    return false;
//                }
//                
//                                public bool sendKeyEvent(KeyEvent event) {
//                    // Use our own input connection, since the filter
//                    // text view may not be shown in a window so has
//                    // no ViewRoot to dispatch events with.
//                    return mDefInputConnection.sendKeyEvent(event);
//                }
//            };
//        }
//        outAttrs.inputType = EditorInfo.TYPE_CLASS_TEXT
//        | EditorInfo.TYPE_TEXT_VARIATION_FILTER;
//        outAttrs.imeOptions = EditorInfo.IME_ACTION_DONE;
//        return mPublicInputConnection;
//    }
//    return NULL;
//}
//
//bool AbsListView::checkInputConnectionProxy(View *view) {
//    return view == mTextFilter;
//}

//void AbsListView::createTextFilter(bool animateEntrance) {
//    if (mPopup == NULL) {
//        Context *c = getContext();
//        PopupWindow p = new PopupWindow(c);
//        LayoutInflater layoutInflater = (LayoutInflater) c.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
//        mTextFilter = (EditText) layoutInflater.inflate(
//                                                        com.android.internal.R.layout.typing_filter, NULL);
//        // For some reason setting this as the "real" input type changes
//        // the text view in some way that it doesn't work, and I don't
//        // want to figure out why this is.
//        mTextFilter->setRawInputType(EditorInfo.TYPE_CLASS_TEXT
//                                    | EditorInfo.TYPE_TEXT_VARIATION_FILTER);
//        mTextFilter->setImeOptions(EditorInfo.IME_FLAG_NO_EXTRACT_UI);
//        mTextFilter->addTextChangedListener(this);
//        p.setFocusable(false);
//        p.setTouchable(false);
//        p.setInputMethodMode(PopupWindow.INPUT_METHOD_NOT_NEEDED);
//        p.setContentView(mTextFilter);
//        p.setWidth(LayoutParams.WRAP_CONTENT);
//        p.setHeight(LayoutParams.WRAP_CONTENT);
//        p.setBackgroundDrawable(NULL);
//        mPopup = p;
//        getViewTreeObserver().addOnGlobalLayoutListener(this);
//        mGlobalLayoutListenerAddedFilter = true;
//    }
//    if (animateEntrance) {
//        mPopup->setAnimationStyle(com.android.internal.R.style.Animation_TypingFilter);
//    } else {
//        mPopup->setAnimationStyle(com.android.internal.R.style.Animation_TypingFilterRestore);
//    }
//}

void AbsListView::clearTextFilter() {
    if (mFiltered) {
//        mTextFilter->setText("");
//        mFiltered = false;
//        if (mPopup != NULL && mPopup->isShowing()) {
//            dismissPopup();
//        }
    }
}

bool AbsListView::hasTextFilter() {
    return mFiltered;
}

void AbsListView::onGlobalLayout() {
//    if (isShown()) {
//        // Show the popup if we are filtered
//        if (mFiltered && mPopup != NULL && !mPopup->isShowing()) {
//            showPopup();
//        }
//    } else {
//        // Hide the popup when we are no longer visible
//        if (mPopup->isShowing()) {
//            dismissPopup();
//        }
//    }
}

void AbsListView::beforeTextChanged(shared_ptr<CharSequence> s, int start, int count, int after) {
}

void AbsListView::onTextChanged(shared_ptr<CharSequence> s, int start, int before, int count) {
    if (mPopup != NULL && isTextFilterEnabled()) {
        int length = s->length();
        bool showing = false;//mPopup->isShowing();
        if (!showing && length > 0) {
            // Show the filter popup if necessary
            showPopup();
            mFiltered = true;
        } else if (showing && length == 0) {
            // Remove the filter popup if the user has cleared all text
            dismissPopup();
            mFiltered = false;
        }
        shared_ptr<Filterable> filterable = dynamic_pointer_cast<Filterable>(mAdapter);
        if (filterable != NULL) {
            shared_ptr<Filter> f = filterable->getFilter();
            // Filter should not be NULL when we reach this part
            if (f != NULL) {
                f->filter(s, this);
            } else {
                throw IllegalStateException("You cannot call onTextChanged with a non filterable adapter");
            }
        }
    }
}

void AbsListView::afterTextChanged(Editable *s) {
}

void AbsListView::onFilterComplete(int count) {
    if (mSelectedPosition < 0 && count > 0) {
        mResurrectToPosition = INVALID_POSITION;
        resurrectSelection();
    }
}

LayoutParams *AbsListView::generateLayoutParams(LayoutParams *p) {
    return new LayoutParams(p);
}

LayoutParams *AbsListView::generateLayoutParams(AttributeSet *attrs) {
    return new ListViewLayoutParams(getContext(), attrs);
}

bool AbsListView::checkLayoutParams(LayoutParams *p) {
    return static_cast<ListViewLayoutParams*>(p) != NULL;
}

void AbsListView::setTranscriptMode(int mode) {
    mTranscriptMode = mode;
}

int AbsListView::getTranscriptMode() {
    return mTranscriptMode;
}

int AbsListView::getSolidColor() {
    return mCacheColorHint;
}

void AbsListView::setCacheColorHint(int color) {
    mCacheColorHint = color;
}

int AbsListView::getCacheColorHint() {
    return mCacheColorHint;
}

void AbsListView::reclaimViews(vector<shared_ptr<View>> &views) {
    int childCount = getChildCount();
    RecyclerListener *listener = mRecycler.mRecyclerListener;
    
    // Reclaim views on screen
    for (int i = 0; i < childCount; i++) {
        shared_ptr<View> child = getChildAt(i);
        ListViewLayoutParams *lp = (ListViewLayoutParams*) child->getLayoutParams();
        // Don't reclaim header or footer views, or views that should be ignored
        if (lp != NULL && mRecycler.shouldRecycleViewType(lp->viewType)) {
            views.push_back(child);
            if (listener != NULL) {
                // Pretend they went through the scrap heap
                listener->onMovedToScrapHeap(child);
            }
        }
    }
    mRecycler.reclaimScrapViews(views);
    removeAllViewsInLayout();
}

//bool AbsListView::onConsistencyCheck(int consistency) {
//    bool result = AdapterView<ListAdapter>::onConsistencyCheck(consistency);
//    
//    const bool checkLayout = (consistency & ViewDebug.CONSISTENCY_LAYOUT) != 0;
//    
//    if (checkLayout) {
//        // The active recycler must be empty
//        const View[] activeViews = mRecycler.mActiveViews;
//        int count = activeViews.length;
//        for (int i = 0; i < count; i++) {
//            if (activeViews[i] != NULL) {
//                result = false;
//            }
//        }
//        
//        // All views in the recycler must NOT be on screen and must NOT have a parent
//        const ArrayList<View> scrap = mRecycler.mCurrentScrap;
//        if (!checkScrap(scrap)) result = false;
//        const ArrayList<View>[] scraps = mRecycler.mScrapViews;
//        count = scraps.length;
//        for (int i = 0; i < count; i++) {
//            if (!checkScrap(scraps[i])) result = false;
//        }
//    }
//    
//    return result;
//}

bool AbsListView::checkScrap(vector<shared_ptr<View>> scrap) {
    bool result = true;
    
    const int count = scrap.size();
    for (int i = 0; i < count; i++) {
        const shared_ptr<View> view = scrap[i];
        if (view->getParent() != NULL) {
            result = false;
        }
        if (indexOfChild(view) != mChildren.end()) {
            result = false;
        }
    }
    
    return result;
}

void AbsListView::setRecyclerListener(RecyclerListener *listener) {
    mRecycler.mRecyclerListener = listener;
}
    
AbsListView::ListViewLayoutParams::ListViewLayoutParams(Context *c, AttributeSet *attrs) :LayoutParams(c, attrs) {
}

AbsListView::ListViewLayoutParams::ListViewLayoutParams(int w, int h) : LayoutParams(w, h) {
}

AbsListView::ListViewLayoutParams::ListViewLayoutParams(int w, int h, int viewType) : LayoutParams(w, h) {
    viewType = viewType;
}

AbsListView::ListViewLayoutParams::ListViewLayoutParams(LayoutParams *source) : LayoutParams(source) {
}
    
void AbsListView::RecycleBin::setViewTypeCount(int viewTypeCount) {
    if (viewTypeCount < 1) {
        throw IllegalArgumentException("Can't have a viewTypeCount < 1");
    }
    //noinspection unchecked
    vector<vector<shared_ptr<View>>> scrapViews = vector<vector<shared_ptr<View>>>(viewTypeCount);
    for (int i = 0; i < viewTypeCount; i++) {
        scrapViews[i] = vector<shared_ptr<View>>();
    }
    mViewTypeCount = viewTypeCount;
    mCurrentScrap = scrapViews[0];
    mScrapViews = scrapViews;
}

bool AbsListView::RecycleBin::shouldRecycleViewType(int viewType) {
    return viewType >= 0;
}
    
void AbsListView::RecycleBin::clear() {
    if (mViewTypeCount == 1) {
        vector<shared_ptr<View>> &scrap = mCurrentScrap;
        const int scrapCount = scrap.size();
        for (int i = 0; i < scrapCount; i++) {
            int index = scrapCount - 1 - i;
            shared_ptr<View> removed = scrap[index];
            mAbsListView->removeDetachedView(removed, false);
            scrap.erase(scrap.begin() + index);
        }
    } else {
        const int typeCount = mViewTypeCount;
        for (int i = 0; i < typeCount; i++) {
            vector<shared_ptr<View>> &scrap = mScrapViews[i];
            const int scrapCount = scrap.size();
            for (int j = 0; j < scrapCount; j++) {
                int index = scrapCount - 1 - j;
                shared_ptr<View> removed = scrap[index];
                mAbsListView->removeDetachedView(removed, false);
                scrap.erase(scrap.begin() + index);
            }
        }
    }
}

void AbsListView::RecycleBin::fillActiveViews(int childCount, int firstActivePosition) {
    if (mActiveViews.size() < childCount) {
        mActiveViews = vector<shared_ptr<View>>(childCount);
    }
    mFirstActivePosition = firstActivePosition;
    
    vector<shared_ptr<View>> &activeViews = mActiveViews;
    for (int i = 0; i < childCount; i++) {
        shared_ptr<View> child = mAbsListView->getChildAt(i);
        ListViewLayoutParams *lp = (ListViewLayoutParams*) child->getLayoutParams();
        // Don't put header or footer views into the scrap heap
        if (lp != NULL && lp->viewType != AdapterView::ITEM_VIEW_TYPE_HEADER_OR_FOOTER) {
            // Note:  We do place AdapterView::ITEM_VIEW_TYPE_IGNORE in active views.
            //        However, we will NOT place them into scrap views.
            activeViews[i] = child;
        }
    }
}

shared_ptr<View> AbsListView::RecycleBin::getActiveView(int position) {
    int index = position - mFirstActivePosition;
    vector<shared_ptr<View>> &activeViews = mActiveViews;
    if (index >=0 && index < activeViews.size()) {
        const shared_ptr<View> match = activeViews[index];
        activeViews[index] = NULL;
        return match;
    }
    return NULL;
}

shared_ptr<View> AbsListView::RecycleBin::getScrapView(int position) {
    vector<shared_ptr<View>> scrapViews;
    if (mViewTypeCount == 1) {
        scrapViews = mCurrentScrap;
        int size = scrapViews.size();
        if (size > 0) {
            int index = size - 1;
            shared_ptr<View> removed = scrapViews[index];
            scrapViews.erase(scrapViews.begin() + index);
            return removed;
        } else {
            return NULL;
        }
    } else {
        int whichScrap = mAbsListView->mAdapter->getItemViewType(position);
        if (whichScrap >= 0 && whichScrap < mScrapViews.size()) {
            scrapViews = mScrapViews[whichScrap];
            int size = scrapViews.size();
            if (size > 0) {
                int index = size - 1;
                shared_ptr<View> removed = scrapViews[index];
                scrapViews.erase(scrapViews.begin() + index);
                return removed;
            }
        }
    }
    return NULL;
}

void AbsListView::RecycleBin::addScrapView(shared_ptr<View> scrap) {
    ListViewLayoutParams *lp = (ListViewLayoutParams*) scrap->getLayoutParams();
    if (lp == NULL) {
        return;
    }
    
    // Don't put header or footer views or views that should be ignored
    // into the scrap heap
    int viewType = lp->viewType;
    if (!shouldRecycleViewType(viewType)) {
        return;
    }
    
    if (mViewTypeCount == 1) {
        mCurrentScrap.push_back(scrap);
    } else {
        mScrapViews[viewType].push_back(scrap);
    }
    
    if (mRecyclerListener != NULL) {
        mRecyclerListener->onMovedToScrapHeap(scrap);
    }
}

void AbsListView::RecycleBin::scrapActiveViews() {
    vector<shared_ptr<View>> &activeViews = mActiveViews;
    const bool hasListener = mRecyclerListener != NULL;
    const bool multipleScraps = mViewTypeCount > 1;
    
    vector<shared_ptr<View>> &scrapViews = mCurrentScrap;
    const int count = activeViews.size();
    for (int i = 0; i < count; ++i) {
        const shared_ptr<View> &victim = activeViews[i];
        if (victim != NULL) {
            int whichScrap = ((ListViewLayoutParams*)
                              victim->getLayoutParams())->viewType;
            
            activeViews[i] = NULL;
            
            if (whichScrap == AdapterView::ITEM_VIEW_TYPE_IGNORE) {
                // Do not move views that should be ignored
                continue;
            }
            
            if (multipleScraps) {
                scrapViews = mScrapViews[whichScrap];
            }
            scrapViews.push_back(victim);
            
            if (hasListener) {
                mRecyclerListener->onMovedToScrapHeap(victim);
            }
        }
    }
    
    pruneScrapViews();
}

void AbsListView::RecycleBin::pruneScrapViews() {
    const int maxViews = mActiveViews.size();
    const int viewTypeCount = mViewTypeCount;
    const vector<vector<shared_ptr<View>>> &scrapViews = mScrapViews;
    for (int i = 0; i < viewTypeCount; ++i) {
        vector<shared_ptr<View>> scrapPile = scrapViews[i];
        int size = scrapPile.size();
        const int extras = size - maxViews;
        size--;
        for (int j = 0; j < extras; j++) {
            int index = size--;
            shared_ptr<View> removed = scrapPile[index];
            mAbsListView->removeDetachedView(removed, false);
            scrapPile.erase(scrapPile.begin() + index);
        }
    }
}

void AbsListView::RecycleBin::reclaimScrapViews(vector<shared_ptr<View>> &views) {
    if (mViewTypeCount == 1) {
        views.insert(views.begin(), mCurrentScrap.begin(), mCurrentScrap.end());
    } else {
        const int viewTypeCount = mViewTypeCount;
        vector<vector<shared_ptr<View>>> &scrapViews = mScrapViews;
        for (int i = 0; i < viewTypeCount; ++i) {
            vector<shared_ptr<View>> &scrapPile = scrapViews[i];
            views.insert(views.begin(), scrapPile.begin(), scrapPile.end());
        }
    }
}

ANDROID_END