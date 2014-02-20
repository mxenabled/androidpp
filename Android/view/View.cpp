/*
 * File:   View.cpp
 * Author: saulhoward
 *
 * Created on October 4, 2013, 12:49 PM
 */

#include "Android/graphics/Canvas.h"
#include "Android/graphics/drawable/ColorDrawable.h"
#include "Android/internal/R.h"
#include "Android/view/MotionEvent.h"
#include "Android/view/View.h"
#include "Android/view/ViewGroup.h"
#include "Android/view/ViewTreeObserver.h"
#include "Android/widget/LinearLayout.h"
#include "Android/widget/RelativeLayout.h"
#include "Android/widget/ImageView.h"
#include "Android/widget/TextView.h"
#include "Android/utils/instantiator.h"
#include "Android/utils/System.h"

#include <ctime>
#include <math.h>

#include <ui/PixelFormat.h>

ANDROID_BEGIN

const vector<int> View::PFLAG2_TEXT_DIRECTION_FLAGS = {
    TEXT_DIRECTION_INHERIT << PFLAG2_TEXT_DIRECTION_MASK_SHIFT,
    TEXT_DIRECTION_FIRST_STRONG << PFLAG2_TEXT_DIRECTION_MASK_SHIFT,
    TEXT_DIRECTION_ANY_RTL << PFLAG2_TEXT_DIRECTION_MASK_SHIFT,
    TEXT_DIRECTION_LTR << PFLAG2_TEXT_DIRECTION_MASK_SHIFT,
    TEXT_DIRECTION_RTL << PFLAG2_TEXT_DIRECTION_MASK_SHIFT,
    TEXT_DIRECTION_LOCALE << PFLAG2_TEXT_DIRECTION_MASK_SHIFT
};

const vector<int> View::PFLAG2_TEXT_ALIGNMENT_FLAGS = {
    TEXT_ALIGNMENT_INHERIT << PFLAG2_TEXT_ALIGNMENT_MASK_SHIFT,
    TEXT_ALIGNMENT_GRAVITY << PFLAG2_TEXT_ALIGNMENT_MASK_SHIFT,
    TEXT_ALIGNMENT_TEXT_START << PFLAG2_TEXT_ALIGNMENT_MASK_SHIFT,
    TEXT_ALIGNMENT_TEXT_END << PFLAG2_TEXT_ALIGNMENT_MASK_SHIFT,
    TEXT_ALIGNMENT_CENTER << PFLAG2_TEXT_ALIGNMENT_MASK_SHIFT,
    TEXT_ALIGNMENT_VIEW_START << PFLAG2_TEXT_ALIGNMENT_MASK_SHIFT,
    TEXT_ALIGNMENT_VIEW_END << PFLAG2_TEXT_ALIGNMENT_MASK_SHIFT
};

vector<vector<int>> View::VIEW_STATE_SETS = vector<vector<int>>();
vector<int> View::ENABLED_STATE_SET = vector<int>();

const vector<int> View::VIEW_STATE_IDS({
    R::attr::state_window_focused, VIEW_STATE_WINDOW_FOCUSED,
    R::attr::state_selected, VIEW_STATE_SELECTED,
    R::attr::state_focused, VIEW_STATE_FOCUSED,
    R::attr::state_enabled, VIEW_STATE_ENABLED,
    R::attr::state_pressed, VIEW_STATE_PRESSED,
    R::attr::state_activated, VIEW_STATE_ACTIVATED,
    R::attr::state_accelerated, VIEW_STATE_ACCELERATED,
    R::attr::state_hovered, VIEW_STATE_HOVERED,
    R::attr::state_drag_can_accept, VIEW_STATE_DRAG_CAN_ACCEPT,
    R::attr::state_drag_hovered, VIEW_STATE_DRAG_HOVERED
});

View::StaticBlock View::staticBlock = View::StaticBlock();

const string View::NO_ID("-1");
const int View::VISIBILITY_FLAGS[] = {VISIBLE, INVISIBLE, GONE};

const float View::NONZERO_EPSILON = 0.001f;

View::View() {
    mResources = NULL;
}

View::View(Context *context) :
        mAttachInfo(NULL),
        mParent(NULL),
        m_id(string("none")),
        m_minHeight(0),
        m_minWidth(0)
{
    mContext = context;
    mResources = context != NULL ? context->getResources() : NULL;
    mPrivateFlags2 =
    (LAYOUT_DIRECTION_DEFAULT << PFLAG2_LAYOUT_DIRECTION_MASK_SHIFT) |
    (TEXT_DIRECTION_DEFAULT << PFLAG2_TEXT_DIRECTION_MASK_SHIFT) |
    (PFLAG2_TEXT_DIRECTION_RESOLVED_DEFAULT) |
    (TEXT_ALIGNMENT_DEFAULT << PFLAG2_TEXT_ALIGNMENT_MASK_SHIFT) |
    (PFLAG2_TEXT_ALIGNMENT_RESOLVED_DEFAULT);
    
    m_userPaddingStart = UNDEFINED_PADDING;
    m_userPaddingEnd = UNDEFINED_PADDING;
}

View::View(Context *context, AttributeSet *attrs) : View(context) {
    attrs->loadAttributes();

    int viewFlagValues = 0;
    int viewFlagMasks = 0;

    int leftPadding = -1;
    int topPadding = -1;
    int rightPadding = -1;
    int bottomPadding = -1;
    int startPadding = UNDEFINED_PADDING;
    int endPadding = UNDEFINED_PADDING;

    int padding = -1;

    bool leftPaddingDefined = false;
    bool rightPaddingDefined = false;
    bool startPaddingDefined = false;
    bool endPaddingDefined = false;
    
    float density = context->getResources()->getDisplayMetrics().density;
    
    mTouchSlop = (int) (density * TOUCH_SLOP);
    mMinimumFlingVelocity = (int) (density * MINIMUM_FLING_VELOCITY + 0.5f);
    mMaximumFlingVelocity = (int) (density * MAXIMUM_FLING_VELOCITY + 0.5f);

    for (int i = 0; i < attrs->getAttributeCount(); i++) {

        const char *name = attrs->getAttributeName(i);

        if (strcmp(R::styleable::background, name) == 0) {
            setBackground(attrs->getAttributeDrawableValue(context->getResources(), name));
        } else if (strcmp(R::styleable::id, name) == 0) {
            m_id = string(attrs->getAttributeValue(name));
            size_t found = m_id.find("@+id/");
            if (found != string::npos) {
                m_id = m_id.substr(5);
            }
        } else if (strcmp(R::styleable::visibility, name) == 0) {
            const int visibility = attrs->getAttributeIntValue(name, 0);
            if (visibility != 0) {
                viewFlagValues |= VISIBILITY_FLAGS[visibility];
                viewFlagMasks |= VISIBILITY_MASK;
            }
        } else if (strcmp(R::styleable::padding, name) == 0) {
            padding = attrs->getDimensionValue(context->getResources(), name, -1);
            m_userPaddingLeftInitial = padding;
            m_userPaddingRightInitial = padding;
            leftPaddingDefined = true;
            rightPaddingDefined = true;
        } else if (strcmp(R::styleable::paddingLeft, name) == 0) {
            leftPadding = attrs->getDimensionValue(context->getResources(), name, -1);
            m_userPaddingLeftInitial = leftPadding;
            leftPaddingDefined = true;
        } else if (strcmp(R::styleable::paddingTop, name) == 0) {
            topPadding = attrs->getDimensionValue(context->getResources(), name, -1);
        } else if (strcmp(R::styleable::paddingRight, name) == 0) {
            rightPadding = attrs->getDimensionValue(context->getResources(), name, -1);
            m_userPaddingRightInitial = rightPadding;
            rightPaddingDefined = true;
        } else if (strcmp(R::styleable::paddingBottom, name) == 0) {
            bottomPadding = attrs->getDimensionValue(context->getResources(), name, -1);
        } else if (strcmp(R::styleable::textAlignment, name) == 0) {
            // Clear any text alignment flag already set
            mPrivateFlags2 &= ~PFLAG2_TEXT_ALIGNMENT_MASK;
            // Set the text alignment flag depending on the value of the attribute
            string alignment = attrs->getAttributeValue(name);
            const int textAlignment = getTextAlignment(alignment, TEXT_ALIGNMENT_DEFAULT);
            mPrivateFlags2 |= PFLAG2_TEXT_ALIGNMENT_FLAGS[textAlignment];
        }
    }
    
    // View Layout Direction stuff, we are defaulting since we don't support RTL
    mPrivateFlags2 &= ~(PFLAG2_LAYOUT_DIRECTION_MASK | PFLAG2_LAYOUT_DIRECTION_RESOLVED_MASK);
    // Set the layout direction flags depending on the value of the attribute
    const int value = LAYOUT_DIRECTION_DEFAULT;
    mPrivateFlags2 |= (value << PFLAG2_LAYOUT_DIRECTION_MASK_SHIFT);

    m_userPaddingStart = startPadding;
    m_userPaddingEnd = endPadding;

    if (padding >= 0) {
        leftPadding = padding;
        topPadding = padding;
        rightPadding = padding;
        bottomPadding = padding;
        m_userPaddingLeftInitial = padding;
        m_userPaddingRightInitial = padding;
    }

    const bool hasRelativePadding = startPaddingDefined || endPaddingDefined;

    if (leftPaddingDefined && !hasRelativePadding) {
        m_userPaddingLeftInitial = leftPadding;
    }

    if (rightPaddingDefined && !hasRelativePadding) {
        m_userPaddingRightInitial = rightPadding;
    }

    internalSetPadding(
                m_userPaddingLeftInitial,
                topPadding >= 0 ? topPadding : mPaddingTop,
                m_userPaddingRightInitial,
                bottomPadding >= 0 ? bottomPadding : mPaddingBottom);

    if (viewFlagMasks != 0) {
        setFlags(viewFlagValues, viewFlagMasks);
    }
}

void View::addTouchables(vector<shared_ptr<View>> &views) {
    const int viewFlags = mViewFlags;
    
    if (((viewFlags & CLICKABLE) == CLICKABLE || (viewFlags & LONG_CLICKABLE) == LONG_CLICKABLE)
        && (viewFlags & ENABLED_MASK) == ENABLED) {
        views.push_back(shared_from_this());
    }
}

void View::assignParent(shared_ptr<ViewParent> parent) {

    if (!mParent) {
        mParent = parent;
    } else if (!parent) {
        mParent = NULL;
    } else {
        // This view already has a parent
    }
}

bool View::canHaveDisplayList() {
    return !(!mAttachInfo || !mAttachInfo->m_hardwareRenderer);
}

bool View::canResolveLayoutDirection() {
    switch (getRawLayoutDirection()) {
        case LAYOUT_DIRECTION_INHERIT:
            return (mParent != NULL) && mParent->canResolveLayoutDirection();
        default:
            return true;
    }
}

void View::checkForLongClick(int delayOffset) {
    if ((mViewFlags & LONG_CLICKABLE) == LONG_CLICKABLE) {
        mHasPerformedLongPress = false;
        
        if (m_pendingCheckForLongPress == NULL) {
            m_pendingCheckForLongPress = new CheckForLongPress(shared_from_this());
        }
        m_pendingCheckForLongPress->rememberWindowAttachCount();
        postDelayed(m_pendingCheckForLongPress, DEFAULT_LONG_PRESS_TIMEOUT - delayOffset);
    }
}

void View::cancelLongPress() {
    removeLongPressCallback();
    
    /*
     * The prepressed state handled by the tap callback is a display
     * construct, but the tap callback will post a long press callback
     * less its own timeout. Remove it here.
     */
    removeTapCallback();
}

void View::clearDisplayList() {
    if (mDisplayList != NULL) {
        mDisplayList->clear();
    }
}

void View::computeOpaqueFlags() {
    // Opaque if:
    //   - Has a background
    //   - Background is opaque
    //   - Doesn't have scrollbars or scrollbars overlay
    
    if (m_background != NULL && m_background->getOpacity() == PIXEL_FORMAT_OPAQUE) {
        mPrivateFlags |= PFLAG_OPAQUE_BACKGROUND;
    } else {
        mPrivateFlags &= ~PFLAG_OPAQUE_BACKGROUND;
    }
    
    const int flags = mViewFlags;
    if (((flags & SCROLLBARS_VERTICAL) == 0 && (flags & SCROLLBARS_HORIZONTAL) == 0) ||
        (flags & SCROLLBARS_STYLE_MASK) == SCROLLBARS_INSIDE_OVERLAY ||
        (flags & SCROLLBARS_STYLE_MASK) == SCROLLBARS_OUTSIDE_OVERLAY) {
        mPrivateFlags |= PFLAG_OPAQUE_SCROLLBARS;
    } else {
        mPrivateFlags &= ~PFLAG_OPAQUE_SCROLLBARS;
    }
}

shared_ptr<View> View::createInstance(const char *name, Context *context, AttributeSet *attrs) {

    ViewType viewType = getViewType(name);

    switch(viewType) {
        case kViewTypeView:
            return make_shared<View>(context, attrs);
        case kViewTypeViewGroup:
            return make_shared<ViewGroup>(context, attrs);
        case kViewTypeLinearLayout:
            return make_shared<LinearLayout>(context, attrs);
        case kViewTypeRelativeLayout:
            return make_shared<RelativeLayout>(context, attrs);
        case kViewTypeImageView:
            return make_shared<ImageView>(context, attrs);
        case kViewTypeTextView:
            return make_shared<TextView>(context, attrs);
        default:
            typedef instantiator<View> IT;
            return IT::Instance().instantiate(string(name), context, attrs);
    }
}

void View::dispatchAttachedToWindow(AttachInfo *info, int visibility) {

    mAttachInfo = info;
    mWindowAttachCount++;
    // We will need to evaluate the drawable state at least once.
    mPrivateFlags |= PFLAG_DRAWABLE_STATE_DIRTY;
    if (mFloatingTreeObserver != NULL) {
        info->mTreeObserver->merge(mFloatingTreeObserver);
        mFloatingTreeObserver = NULL;
    }
//    if ((mPrivateFlags & PFLAG_SCROLL_CONTAINER) != 0) {
//        mAttachInfo->mScrollContainers.add(this);
//        mPrivateFlags |= PFLAG_SCROLL_CONTAINER_ADDED;
//    }
    
    onAttachedToWindow();

    // TODO: OnAttachStateChange listeners fired

    int vis = info->mWindowVisibility;
    if (vis != GONE) {
        onWindowVisibilityChanged(vis);
    }
}

void View::dispatchDetachedFromWindow() {
    AttachInfo *info = mAttachInfo;
    if (info) {
        int vis = info->mWindowVisibility;
        if (vis != GONE) {
            onWindowVisibilityChanged(GONE);
        }
    }

    onDetachedFromWindow();

    mAttachInfo = NULL;
}

bool View::dispatchTouchEvent(MotionEvent *ev) {
    
    if (onTouchEvent(ev)) {
        return true;
    }
    
    return false;
}

void View::dispatchWindowVisibilityChanged(int visibility) {
    onWindowVisibilityChanged(visibility);
}

void View::draw(shared_ptr<Canvas> canvas) {
    
    if (m_background != NULL) {
        const int scrollX = mScrollX;
        const int scrollY = mScrollY;
        
        if (m_backgroundSizeChanged) {
            m_background->setBounds(0, 0,  mRight - mLeft, mBottom - mTop);
            m_backgroundSizeChanged = false;
        }
        
        if ((scrollX | scrollY) == 0) {
            m_background->draw(canvas);
        } else {
            canvas->translate(scrollX, scrollY);
            m_background->draw(canvas);
            canvas->translate(-scrollX, -scrollY);
        }
    }

    onDraw(canvas);

    dispatchDraw(canvas);
}

bool View::draw(shared_ptr<Canvas> canvas, ViewGroup* parent, uint64_t drawingTime) {
    bool more = false;
    const bool childHasIdentityMatrix = hasIdentityMatrix();
    bool concatMatrix = false;
    const int flags = parent->mGroupFlags;
    
    
    // Sets the flag as early as possible to allow draw() implementations
    // to call invalidate() successfully when doing animations
    mPrivateFlags |= PFLAG_DRAWN;
    
    if (!concatMatrix &&
            (flags & (ViewGroup::FLAG_SUPPORT_STATIC_TRANSFORMATIONS |
                    ViewGroup::FLAG_CLIP_CHILDREN)) == ViewGroup::FLAG_CLIP_CHILDREN &&
            canvas->quickReject(mLeft, mTop, mRight, mBottom) &&
            (mPrivateFlags & PFLAG_DRAW_ANIMATION) == 0) {
        mPrivateFlags2 |= PFLAG2_VIEW_QUICK_REJECTED;
        return more;
    }

    mPrivateFlags2 &= ~PFLAG2_VIEW_QUICK_REJECTED;

    m_recreateDisplayList = (mPrivateFlags & PFLAG_INVALIDATED) == PFLAG_INVALIDATED;
    mPrivateFlags &= ~PFLAG_INVALIDATED;


    GLES20DisplayList *displayList = getDisplayList();
    if (!displayList->isValid()) {
        displayList = NULL;
    }

    mPrivateFlags &= ~PFLAG_DIRTY_MASK;
    canvas->drawDisplayList(displayList, NULL, flags);

    m_recreateDisplayList = false;

    return more;
}

/**
 * This function is called whenever the state of the view changes in such
 * a way that it impacts the state of drawables being shown.
 *
 * <p>Be sure to call through to the superclass when overriding this
 * function.
 *
 * @see Drawable#setState(int[])
 */
void View::drawableStateChanged() {
    if (m_background != NULL && m_background->isStateful()) {
        m_background->setState(getDrawableState());
    }
}

int View::getBaseline() {
	return -1;
}

Context *View::getContext() {
    return mContext;
}

void View::dispatchStartTemporaryDetach() {
//    clearAccessibilityFocus();
    clearDisplayList();
    
    onStartTemporaryDetach();
}

void View::getDrawingRect(Rect outRect) {
    outRect.set(mScrollX, mScrollY, mScrollX + (mRight - mLeft), mScrollY + (mBottom - mTop));
}

int View::getDefaultSize(int size, int measureSpec) {
    int result = size;
    int specMode = MeasureSpec::getMode(measureSpec);
    int specSize = MeasureSpec::getSize(measureSpec);

    switch (specMode) {
        case MeasureSpec::UNSPECIFIED:
            result = size;
            break;
        case MeasureSpec::AT_MOST:
        case MeasureSpec::EXACTLY:
            result = specSize;
            break;
    }
    return result;
}

GLES20DisplayList *View::getDisplayList() {
    mDisplayList = getDisplayList(mDisplayList, false);
    return mDisplayList;
}

GLES20DisplayList *View::getDisplayList(GLES20DisplayList *displayList, bool isLayer) {

    if (!canHaveDisplayList()) {

        CCLOG("cannot have displaylists(%s) %d", m_id.c_str(), (!mAttachInfo));
        
        return NULL;
    }

    if (((mPrivateFlags & PFLAG_DRAWING_CACHE_VALID) == 0 ||
            !displayList || !displayList->isValid() ||
            (!isLayer && m_recreateDisplayList))) {
        // Don't need to recreate the display list, just need to tell our
        // children to restore/recreate theirs
        if (displayList && displayList->isValid() && !isLayer && !m_recreateDisplayList) {
            mPrivateFlags |= PFLAG_DRAWN | PFLAG_DRAWING_CACHE_VALID;
            mPrivateFlags &= ~PFLAG_DIRTY_MASK;
            dispatchGetDisplayList();

            return displayList;
        }

        if (!isLayer) {
            // If we got here, we're recreating it. Mark it as such to ensure that
            // we copy in child display lists into ours in drawChild()
            m_recreateDisplayList = true;
        }

        if (!displayList) {
            displayList = mAttachInfo->m_hardwareRenderer->createDisplayList();

            // If we're creating a new display list, make sure our parent gets invalidated
            // since they will need to recreate their display list to account for this
            // new child display list.
            invalidateParentCaches();
        }

        bool caching = false;
        int width = mRight - mLeft;
        int height = mBottom - mTop;

        shared_ptr<Canvas> canvas = displayList->start(width, height);
        if (!isLayer) {
            mPrivateFlags |= PFLAG_DRAWN | PFLAG_DRAWING_CACHE_VALID;
            mPrivateFlags &= ~PFLAG_DIRTY_MASK;
        }

        // Fast path for layouts with no backgrounds
        if ((mPrivateFlags & PFLAG_SKIP_DRAW) == PFLAG_SKIP_DRAW) {
            dispatchDraw(canvas);
        } else {
            draw(canvas);
        }

        displayList->end();
        displayList->setCaching(caching);
        if (isLayer) {
            displayList->setLeftTopRightBottom(0, 0, width, height);
        } else {
            setDisplayListProperties(displayList);
        }

    } else if (!isLayer) {
        mPrivateFlags |= PFLAG_DRAWN | PFLAG_DRAWING_CACHE_VALID;
        mPrivateFlags &= ~PFLAG_DIRTY_MASK;
    }

    return displayList;
}

sp<Handler> View::getHandler() {
    if (mAttachInfo != NULL) {
        return mAttachInfo->mHandler;
    }
    return NULL;
}

void View::getHitRect(Rect &outRect) {
    updateMatrix();
    TransformationInfo &info = mTransformationInfo;
    if (info.mMatrixIsIdentity || mAttachInfo == NULL) {
        outRect.set(mLeft, mTop, mRight, mBottom);
    } else {
        RectF &tmpRect = mAttachInfo->mTmpTransformRect;
        tmpRect.set(0, 0, getWidth(), getHeight());
        info.mMatrix.mapRect(&tmpRect);
        outRect.set((int) tmpRect.fLeft + mLeft, (int) tmpRect.fTop + mTop,
                    (int) tmpRect.fRight + mLeft, (int) tmpRect.fBottom + mTop);
    }
}

/**
 * Returns the start padding of this view depending on its resolved layout direction.
 * If there are inset and enabled scrollbars, this value may include the space
 * required to display the scrollbars as well.
 *
 * @return the start padding in pixels
 */
int View::getPaddingStart() {
    if (!isPaddingResolved()) {
        resolvePadding();
    }
    return (getLayoutDirection() == LAYOUT_DIRECTION_RTL) ?
    mPaddingRight : mPaddingLeft;
}

/**
 * Returns the end padding of this view depending on its resolved layout direction.
 * If there are inset and enabled scrollbars, this value may include the space
 * required to display the scrollbars as well.
 *
 * @return the end padding in pixels
 */
int View::getPaddingEnd() {
    if (!isPaddingResolved()) {
        resolvePadding();
    }
    return (getLayoutDirection() == LAYOUT_DIRECTION_RTL) ?
    mPaddingLeft : mPaddingRight;
}

int View::getRawLayoutDirection() {
    return (mPrivateFlags2 & PFLAG2_LAYOUT_DIRECTION_MASK) >> PFLAG2_LAYOUT_DIRECTION_MASK_SHIFT;
}

int View::getRawTextAlignment() {
    return (mPrivateFlags2 & PFLAG2_TEXT_ALIGNMENT_MASK) >> PFLAG2_TEXT_ALIGNMENT_MASK_SHIFT;
}

/**
 * Returns true if the transform matrix is the identity matrix.
 * Recomputes the matrix if necessary.
 *
 * @return True if the transform matrix is the identity matrix, false otherwise.
 */
bool View::hasIdentityMatrix() {
    updateMatrix();
    return mTransformationInfo.mMatrixIsIdentity;
}

/**
 * Indicates whether the view is currently in pressed state. Unless
 * {@link #setPressed(bool)} is explicitly called, only clickable views can enter
 * the pressed state.
 *
 * @see #setPressed(bool)
 * @see #isClickable()
 * @see #setClickable(bool)
 *
 * @return true if the view is currently pressed, false otherwise
 */
bool View::isPressed() {
    return (mPrivateFlags & PFLAG_PRESSED) == PFLAG_PRESSED;
}

View *View::findViewById(string id) {
    if (id.empty()) {
        return NULL;
    }
    return findViewTraversal(id).get();
}

View *View::findViewWithTag(shared_ptr<Object> tag) {
    if (tag == NULL) {
        return NULL;
    }
    return findViewWithTagTraversal(tag).get();
}

/**
 * {@hide}
 * @param tag the tag of the view to be found
 * @return the view of specified tag, null if cannot be found
 */
shared_ptr<View> View::findViewWithTagTraversal(shared_ptr<Object> tag) {
    if (tag != NULL && tag->equals(mTag.get())) {
        return shared_from_this();
    }
    return NULL;
}

shared_ptr<View> View::findSharedViewById(string id) {
    if (id.empty()) {
        return NULL;
    }
    return findViewTraversal(id);
}

shared_ptr<View> View::findViewTraversal(string id) {
    if (id == m_id) {
        return shared_from_this();
    }
    return NULL;
}

static const char alphanum[] =
"0123456789"
"!@#$%^&*"
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz";
int stringLength = sizeof(alphanum) - 1;

string View::generateViewId() {
    
    srand(time(0));
    string str;
    for (unsigned int i = 0; i < 20; ++i) {
        str += alphanum[rand() % stringLength];
    }
    
    return str;
}

/**
 * Return an array of resource IDs of the drawable states representing the
 * current state of the view.
 *
 * @return The current drawable state
 *
 * @see Drawable#setState(int[])
 * @see #drawableStateChanged()
 * @see #onCreateDrawableState(int)
 */
vector<int> View::getDrawableState() {
    if ((mPrivateFlags & PFLAG_DRAWABLE_STATE_DIRTY) == 0) {
        return m_drawableState;
    } else {
        m_drawableState = onCreateDrawableState(0);
        mPrivateFlags &= ~PFLAG_DRAWABLE_STATE_DIRTY;
        return m_drawableState;
    }
}

int View::getFadeTop(bool offsetRequired) {
    int top = mPaddingTop;
    if (offsetRequired) top += getTopPaddingOffset();
    return top;
}

int View::getFadeHeight(bool offsetRequired) {
    int padding = mPaddingTop;
    if (offsetRequired) padding += getTopPaddingOffset();
    return mBottom - mTop - mPaddingBottom - padding;
}

LayoutParams* View::getLayoutParams() {
    return mLayoutParams;
}

void View::getLocationOnScreen(vector<int> &location) {
    getLocationInWindow(location);
    
    const AttachInfo *info = mAttachInfo;
    if (info != NULL) {
        location[0] += info->mWindowLeft;
        location[1] += info->mWindowTop;
    }
}

void View::getLocationInWindow(vector<int> &location) {
    if (location.size() < 2) {
        throw IllegalArgumentException("location must be an array of two integers");
    }
    
    if (mAttachInfo == NULL) {
        // When the view is not attached to a window, this method does not make sense
        location[0] = location[1] = 0;
        return;
    }
    
    vector<float> &position = mAttachInfo->mTmpTransformLocation;
    position[0] = position[1] = 0.0f;
    
    if (!hasIdentityMatrix()) {
        getMatrix().mapPoints(position);
    }
    
    position[0] += mLeft;
    position[1] += mTop;
    
    shared_ptr<ViewParent> &viewParent = mParent;
    shared_ptr<View> view = dynamic_pointer_cast<View>(viewParent);
    while (view != NULL) {
        
        position[0] -= view->mScrollX;
        position[1] -= view->mScrollY;
        
        if (!view->hasIdentityMatrix()) {
            view->getMatrix().mapPoints(position);
        }
        
        position[0] += view->mLeft;
        position[1] += view->mTop;
        
        viewParent = view->mParent;
        view = dynamic_pointer_cast<View>(viewParent);
    }
    
//    shared_ptr<ViewRootImpl> viewRootImpl = static_pointer_cast<ViewRootImpl>(viewParent);
//    if (viewRootImpl != NULL) {
//        position[1] -= viewRootImpl->mCurScrollY;
//    }
    
    location[0] = (int) (position[0] + 0.5f);
    location[1] = (int) (position[1] + 0.5f);
}

/**
 * The transform matrix of this view, which is calculated based on the current
 * roation, scale, and pivot properties.
 *
 * @see #getRotation()
 * @see #getScaleX()
 * @see #getScaleY()
 * @see #getPivotX()
 * @see #getPivotY()
 * @return The current transform matrix for the view
 */
Matrix View::getMatrix() {
    updateMatrix();
    return mTransformationInfo.mMatrix;
}

int View::getMeasuredHeight() {
    return m_measuredHeight & MEASURED_SIZE_MASK;
}

int View::getMeasuredState() {
    return (m_measuredWidth&MEASURED_STATE_MASK)
            | ((m_measuredHeight>>MEASURED_HEIGHT_STATE_SHIFT)
                    & (MEASURED_STATE_MASK>>MEASURED_HEIGHT_STATE_SHIFT));
}

int View::getMeasuredWidth() {
    return m_measuredWidth & MEASURED_SIZE_MASK;
}

int View::getSuggestedMinimumHeight() {
    return m_minHeight;
}

int View::getSuggestedMinimumWidth() {
    return m_minWidth;
}

int View::getTextAlignment() {
    return (mPrivateFlags2 & PFLAG2_TEXT_ALIGNMENT_RESOLVED_MASK) >> PFLAG2_TEXT_ALIGNMENT_RESOLVED_MASK_SHIFT;
}

int View::getTextAlignment(string alignment, int defaultAlignment) {
    
    if (alignment.empty()) return defaultAlignment;
    
    if (alignment.compare("center") == 0) {
        return TEXT_ALIGNMENT_CENTER;
    }
    
    if (alignment.compare("gravity") == 0) {
        return TEXT_ALIGNMENT_GRAVITY;
    }
    
    if (alignment.compare("inherit") == 0) {
        return TEXT_ALIGNMENT_INHERIT;
    }
    
    if (alignment.compare("textEnd") == 0) {
        return TEXT_ALIGNMENT_TEXT_END;
    }
    
    if (alignment.compare("textStart") == 0) {
        return TEXT_ALIGNMENT_TEXT_START;
    }
    
    if (alignment.compare("viewEnd") == 0) {
        return TEXT_ALIGNMENT_VIEW_END;
    }
    
    if (alignment.compare("viewStart") == 0) {
        return TEXT_ALIGNMENT_VIEW_START;
    }
    
    return defaultAlignment;
}

shared_ptr<ViewParent> View::getParent() {
    return mParent;
}

int View::getVerticalFadingEdgeLength() {
    if (isVerticalFadingEdgeEnabled()) {
//        ScrollabilityCache cache = mScrollCache;
//        if (cache != NULL) {
//            return cache.fadingEdgeLength;
//        }
    }
    return 0;
}

int View::getVerticalScrollbarWidth() {
//    ScrollabilityCache cache = mScrollCache;
//    if (cache != NULL) {
//        ScrollBarDrawable scrollBar = cache.scrollBar;
//        if (scrollBar != NULL) {
//            int size = scrollBar.getSize(true);
//            if (size <= 0) {
//                size = cache.scrollBarSize;
//            }
//            return size;
//        }
//        return 0;
//    }
    return 0;
}

ViewRootImpl *View::getViewRootImpl() {
    if (mAttachInfo) {
        return mAttachInfo->mViewRootImpl;
    }

    return NULL;
}

ViewTreeObserver *View::getViewTreeObserver() {
    if (mAttachInfo != NULL) {
        return mAttachInfo->mTreeObserver;
    }
    if (mFloatingTreeObserver == NULL) {
        mFloatingTreeObserver = new ViewTreeObserver();
    }
    return mFloatingTreeObserver;
}

ViewType View::getViewType(const char* name) {

    if (strcmp(name, "View") == 0) {
        return kViewTypeView;
    }

    if (strcmp(name, "ViewGroup") == 0) {
        return kViewTypeViewGroup;
    }

    if (strcmp(name, "LinearLayout") == 0) {
        return kViewTypeLinearLayout;
    }
    
    if (strcmp(name, "RelativeLayout") == 0) {
        return kViewTypeRelativeLayout;
    }
    
    if (strcmp(name, "ImageView") == 0) {
        return kViewTypeImageView;
    }

    if (strcmp(name, "TextView") == 0) {
        return kViewTypeTextView;
    }

    return kViewTypeNone;
}

int View::getVisibility() {
    return mViewFlags & View::VISIBILITY_MASK;
}

int View::getWindowVisibility() {
    return mAttachInfo != NULL ? mAttachInfo->mWindowVisibility : GONE;
}

bool View::hasWindowFocus() {
    return mAttachInfo != NULL && mAttachInfo->mHasWindowFocus;
}

/**
 * @hide
 */
void View::internalSetPadding(int left, int top, int right, int bottom) {
    m_userPaddingLeft = left;
    m_userPaddingRight = right;
    m_userPaddingBottom = bottom;
    
    bool changed = false;
    
    if (mPaddingLeft != left) {
        changed = true;
        mPaddingLeft = left;
    }
    if (mPaddingTop != top) {
        changed = true;
        mPaddingTop = top;
    }
    if (mPaddingRight != right) {
        changed = true;
        mPaddingRight = right;
    }
    if (mPaddingBottom != bottom) {
        changed = true;
        mPaddingBottom = bottom;
    }
    
    if (changed) {
        requestLayout();
    }
}

void View::initScrollCache() {
//    if (mScrollCache == NULL) {
//        mScrollCache = new ScrollabilityCache(ViewConfiguration.get(mContext), this);
//    }
}

void View::initializeScrollbars(AttributeSet *attrs) {
    initScrollCache();
    
//    const ScrollabilityCache scrollabilityCache = mScrollCache;
//    
//    if (scrollabilityCache.scrollBar == NULL) {
//        scrollabilityCache.scrollBar = new ScrollBarDrawable();
//    }
//    
//    const bool fadeScrollbars = a.getBoolean(R.styleable.View_fadeScrollbars, true);
//    
//    if (!fadeScrollbars) {
//        scrollabilityCache.state = ScrollabilityCache.ON;
//    }
//    scrollabilityCache.fadeScrollBars = fadeScrollbars;
//    
//    
//    scrollabilityCache.scrollBarFadeDuration = a.getInt(
//                                                        R.styleable.View_scrollbarFadeDuration, ViewConfiguration
//                                                        .getScrollBarFadeDuration());
//    scrollabilityCache.scrollBarDefaultDelayBeforeFade = a.getInt(
//                                                                  R.styleable.View_scrollbarDefaultDelayBeforeFade,
//                                                                  ViewConfiguration.getScrollDefaultDelay());
//    
//    
//    scrollabilityCache.scrollBarSize = a.getDimensionPixelSize(
//                                                               com.android.internal.R.styleable.View_scrollbarSize,
//                                                               ViewConfiguration.get(mContext).getScaledScrollBarSize());
//    
//    Drawable track = a.getDrawable(R.styleable.View_scrollbarTrackHorizontal);
//    scrollabilityCache.scrollBar.setHorizontalTrackDrawable(track);
//    
//    Drawable thumb = a.getDrawable(R.styleable.View_scrollbarThumbHorizontal);
//    if (thumb != NULL) {
//        scrollabilityCache.scrollBar.setHorizontalThumbDrawable(thumb);
//    }
//    
//    bool alwaysDraw = a.getBoolean(R.styleable.View_scrollbarAlwaysDrawHorizontalTrack,
//                                      false);
//    if (alwaysDraw) {
//        scrollabilityCache.scrollBar.setAlwaysDrawHorizontalTrack(true);
//    }
//    
//    track = a.getDrawable(R.styleable.View_scrollbarTrackVertical);
//    scrollabilityCache.scrollBar.setVerticalTrackDrawable(track);
//    
//    thumb = a.getDrawable(R.styleable.View_scrollbarThumbVertical);
//    if (thumb != NULL) {
//        scrollabilityCache.scrollBar.setVerticalThumbDrawable(thumb);
//    }
//    
//    alwaysDraw = a.getBoolean(R.styleable.View_scrollbarAlwaysDrawVerticalTrack,
//                              false);
//    if (alwaysDraw) {
//        scrollabilityCache.scrollBar.setAlwaysDrawVerticalTrack(true);
//    }
//    
//    // Apply layout direction to the new Drawables if needed
//    const int layoutDirection = getLayoutDirection();
//    if (track != NULL) {
//        track.setLayoutDirection(layoutDirection);
//    }
//    if (thumb != NULL) {
//        thumb.setLayoutDirection(layoutDirection);
//    }
    
    // Re-apply user/background padding so that scrollbar(s) get added
    resolvePadding();
}

/**
 * Invalidate the whole view. If the view is visible,
 * {@link #onDraw(android.graphics.Canvas)} will be called at some point in
 * the future. This must be called from a UI thread. To call from a non-UI thread,
 * call {@link #postInvalidate()}.
 */
void View::invalidate() {
    invalidate(true);
}

void View::invalidate(bool invalidateCache) {
    if (skipInvalidate()) {
        return;
    }
    if ((mPrivateFlags & (PFLAG_DRAWN | PFLAG_HAS_BOUNDS)) == (PFLAG_DRAWN | PFLAG_HAS_BOUNDS) ||
        (invalidateCache && (mPrivateFlags & PFLAG_DRAWING_CACHE_VALID) == PFLAG_DRAWING_CACHE_VALID) ||
        (mPrivateFlags & PFLAG_INVALIDATED) != PFLAG_INVALIDATED || isOpaque() != m_lastIsOpaque) {
        m_lastIsOpaque = isOpaque();
        mPrivateFlags &= ~PFLAG_DRAWN;
        mPrivateFlags |= PFLAG_DIRTY;
        if (invalidateCache) {
            mPrivateFlags |= PFLAG_INVALIDATED;
            mPrivateFlags &= ~PFLAG_DRAWING_CACHE_VALID;
        }
        
        if (mParent && mAttachInfo) {
            Rect &r = mAttachInfo->mTmpInvalRect;
            r.set(0, 0, mRight - mLeft, mBottom - mTop);
            // Don't call invalidate -- we don't want to internally scroll
            // our own bounds
            mParent->invalidateChild(shared_from_this(), r);
        }
    }
}

void View::invalidate(int l, int t, int r, int b) {
    if (skipInvalidate()) {
        return;
    }
    if ((mPrivateFlags & (PFLAG_DRAWN | PFLAG_HAS_BOUNDS)) == (PFLAG_DRAWN | PFLAG_HAS_BOUNDS) ||
        (mPrivateFlags & PFLAG_DRAWING_CACHE_VALID) == PFLAG_DRAWING_CACHE_VALID ||
        (mPrivateFlags & PFLAG_INVALIDATED) != PFLAG_INVALIDATED) {
        
        mPrivateFlags &= ~PFLAG_DRAWING_CACHE_VALID;
        mPrivateFlags |= PFLAG_INVALIDATED;
        mPrivateFlags |= PFLAG_DIRTY;
        
        if (mParent && mAttachInfo && l < r && t < b) {
            const int scrollX = mScrollX;
            const int scrollY = mScrollY;
            Rect &tmpr = mAttachInfo->mTmpInvalRect;
            tmpr.set(l - scrollX, t - scrollY, r - scrollX, b - scrollY);
            mParent->invalidateChild(shared_from_this(), tmpr);
        }
    }
}

void View::invalidateViewProperty(bool invalidateParent, bool forceRedraw) {
    if (mDisplayList == NULL || (mPrivateFlags & PFLAG_DRAW_ANIMATION) == PFLAG_DRAW_ANIMATION) {
        if (invalidateParent) {
            invalidateParentCaches();
        }
        if (forceRedraw) {
            mPrivateFlags |= PFLAG_DRAWN; // force another invalidation with the new orientation
        }
        invalidate(false);
    } else {
        const AttachInfo *ai = mAttachInfo;
        const shared_ptr<ViewParent> &p = mParent;
        if (p != NULL && ai != NULL) {
            Rect r = ai->mTmpInvalRect;
            r.set(0, 0, mRight - mLeft, mBottom - mTop);
            shared_ptr<ViewGroup> viewGroup = static_pointer_cast<ViewGroup>(mParent);
            if (viewGroup != NULL) {
                viewGroup->invalidateChildFast(shared_from_this(), r);
            } else {
                mParent->invalidateChild(shared_from_this(), r);
            }
        }
    }
}

bool View::skipInvalidate() {
    
    shared_ptr<ViewGroup> parent;
    if (mParent && mParent->isView()) parent = static_pointer_cast<ViewGroup>(mParent);
    return (mViewFlags & VISIBILITY_MASK) != VISIBLE;
}

void View::invalidateDrawable(shared_ptr<Drawable> who) {
    
    if (verifyDrawable(who)) {
        const Rect dirty = who->getBounds();
        const int scrollX = mScrollX;
        const int scrollY = mScrollY;
        
        invalidate(dirty.left() + scrollX, dirty.top() + scrollY,
                   dirty.right() + scrollX, dirty.bottom() + scrollY);
    }
}

void View::invalidateParentCaches() {
    if (mParent && mParent->isView()) {
        shared_ptr<ViewGroup> parent = static_pointer_cast<ViewGroup>(mParent);
        parent->mPrivateFlags |= PFLAG_INVALIDATED;
    }
}

bool View::isHorizontalFadingEdgeEnabled() {
    return (mViewFlags & FADING_EDGE_HORIZONTAL) == FADING_EDGE_HORIZONTAL;
}

bool View::isInScrollingContainer() {
    shared_ptr<ViewParent> p = getParent();
    while (p && p->isView()) {
        shared_ptr<ViewGroup> parent = static_pointer_cast<ViewGroup>(mParent);
        if (parent->shouldDelayChildPressedState()) {
            return true;
        }
        p = p->getParent();
    }
    return false;
}

/**
 * Returns true if this view has been through at least one layout since it
 * was last attached to or detached from a window.
 */
bool View::isLaidOut() {
    return (mPrivateFlags3 & PFLAG3_IS_LAID_OUT) == PFLAG3_IS_LAID_OUT;
}

bool View::isLayoutRequested() {
    return (mPrivateFlags & PFLAG_FORCE_LAYOUT) == PFLAG_FORCE_LAYOUT;
}

/**
 * Indicates whether this View is opaque. An opaque View guarantees that it will
 * draw all the pixels overlapping its bounds using a fully opaque color.
 *
 * Subclasses of View should override this method whenever possible to indicate
 * whether an instance is opaque. Opaque Views are treated in a special way by
 * the View hierarchy, possibly allowing it to perform optimizations during
 * invalidate/draw passes.
 *
 * @return True if this View is guaranteed to be fully opaque, false otherwise.
 */
bool View::isOpaque() {
    return (mPrivateFlags & PFLAG_OPAQUE_MASK) == PFLAG_OPAQUE_MASK &&
    (mTransformationInfo.mAlpha >= 1.0f);
}

void View::jumpDrawablesToCurrentState() {
    if (m_background != NULL) {
        m_background->jumpToCurrentState();
    }
}

void View::layout(int left, int top, int right, int bottom) {
    
    if ((mPrivateFlags3 & PFLAG3_MEASURE_NEEDED_BEFORE_LAYOUT) != 0) {
        onMeasure(m_oldWidthMeasureSpec, m_oldHeightMeasureSpec);
        mPrivateFlags3 &= ~PFLAG3_MEASURE_NEEDED_BEFORE_LAYOUT;
    }
    
    bool changed = setFrame(left, top, right, bottom);
    
    if (true) {
        onLayout(changed, left, top, right, bottom);

        // TODO: onLayoutChanged listener
    }
    
    mPrivateFlags &= ~PFLAG_FORCE_LAYOUT;
    mPrivateFlags3 |= PFLAG3_IS_LAID_OUT;
}

void View::measure(int widthMeasureSpec, int heightMeasureSpec) {
    
    uint64_t key = (uint64_t) widthMeasureSpec << 32 | ((uint64_t) heightMeasureSpec & 0xffffffffL);
    
    if ((mPrivateFlags & PFLAG_FORCE_LAYOUT) == PFLAG_FORCE_LAYOUT ||
                widthMeasureSpec != m_oldWidthMeasureSpec ||
                heightMeasureSpec != m_oldHeightMeasureSpec) {

        // first clears the measured dimension flag
        mPrivateFlags &= ~PFLAG_MEASURED_DIMENSION_SET;

        bool skipCache = (mPrivateFlags & PFLAG_FORCE_LAYOUT) == PFLAG_FORCE_LAYOUT;
        
        map<uint64_t, uint64_t>::iterator it = m_measureCache.find(key);
        
        if (skipCache || it == m_measureCache.end()) {
            // measure ourselves, this should set the measured dimension flag back
            onMeasure(widthMeasureSpec, heightMeasureSpec);
            mPrivateFlags3 &= ~PFLAG3_MEASURE_NEEDED_BEFORE_LAYOUT;
        } else {
            const uint64_t value = it->second;
            // Casting a long to int drops the high 32 bits, no mask needed
            setMeasuredDimension((int) (value >> 32), (int) value);
            mPrivateFlags3 |= PFLAG3_MEASURE_NEEDED_BEFORE_LAYOUT;
        }

        mPrivateFlags |= PFLAG_LAYOUT_REQUIRED;
    }

    m_oldWidthMeasureSpec = widthMeasureSpec;
    m_oldHeightMeasureSpec = heightMeasureSpec;
    
    uint64_t value = ((uint64_t) m_measuredWidth) << 32 | ((uint64_t) m_measuredHeight & 0xffffffffL);
    m_measureCache[key] = value;
}

vector<int> View::mergeDrawableStates(vector<int> baseState, vector<int> additionalState) {
    const int N = baseState.size();
    int i = N - 1;
    while (i >= 0 && baseState[i] == 0) {
        i--;
    }
    
    System::arraycopy(additionalState, 0, baseState, i + 1, additionalState.size());
    return baseState;
}

bool View::nonzero(float value) {
    return (value < -NONZERO_EPSILON || value > NONZERO_EPSILON);
}

void View::offsetTopAndBottom(int offset) {
    if (offset != 0) {
        updateMatrix();
        const bool matrixIsIdentity = true;//mTransformationInfo == NULL || mTransformationInfo.mMatrixIsIdentity;
        if (matrixIsIdentity) {
            if (mDisplayList != NULL) {
                invalidateViewProperty(false, false);
            } else {
                const shared_ptr<ViewParent> &p = mParent;
                if (p != NULL && mAttachInfo != NULL) {
                    Rect r = mAttachInfo->mTmpInvalRect;
                    int minTop;
                    int maxBottom;
                    int yLoc;
                    if (offset < 0) {
                        minTop = mTop + offset;
                        maxBottom = mBottom;
                        yLoc = offset;
                    } else {
                        minTop = mTop;
                        maxBottom = mBottom + offset;
                        yLoc = 0;
                    }
                    r.set(0, yLoc, mRight - mLeft, maxBottom - minTop);
                    p->invalidateChild(shared_from_this(), r);
                }
            }
        } else {
            invalidateViewProperty(false, false);
        }
        
        mTop += offset;
        mBottom += offset;
        if (mDisplayList != NULL) {
            mDisplayList->offsetTopBottom(offset);
            invalidateViewProperty(false, false);
        } else {
            if (!matrixIsIdentity) {
                invalidateViewProperty(false, true);
            }
            invalidateParentIfNeeded();
        }
    }
}

void View::offsetLeftAndRight(int offset) {
    if (offset != 0) {
        updateMatrix();
        const bool matrixIsIdentity = true;//mTransformationInfo == NULL || mTransformationInfo.mMatrixIsIdentity;
        if (matrixIsIdentity) {
            if (mDisplayList != NULL) {
                invalidateViewProperty(false, false);
            } else {
                const shared_ptr<ViewParent> &p = mParent;
                if (p != NULL && mAttachInfo != NULL) {
                    Rect r = mAttachInfo->mTmpInvalRect;
                    int minLeft;
                    int maxRight;
                    if (offset < 0) {
                        minLeft = mLeft + offset;
                        maxRight = mRight;
                    } else {
                        minLeft = mLeft;
                        maxRight = mRight + offset;
                    }
                    r.set(0, 0, maxRight - minLeft, mBottom - mTop);
                    p->invalidateChild(shared_from_this(), r);
                }
            }
        } else {
            invalidateViewProperty(false, false);
        }
        
        mLeft += offset;
        mRight += offset;
        if (mDisplayList != NULL) {
            mDisplayList->offsetLeftRight(offset);
            invalidateViewProperty(false, false);
        } else {
            if (!matrixIsIdentity) {
                invalidateViewProperty(false, true);
            }
            invalidateParentIfNeeded();
        }
    }
}

void View::onAttachedToWindow() {
    mPrivateFlags3 &= ~PFLAG3_IS_LAID_OUT;
}

/**
 * Generate the new {@link android.graphics.drawable.Drawable} state for
 * this view. This is called by the view
 * system when the cached Drawable state is determined to be invalid.  To
 * retrieve the current state, you should use {@link #getDrawableState}.
 *
 * @param extraSpace if non-zero, this is the number of extra entries you
 * would like in the returned array in which you can place your own
 * states.
 *
 * @return Returns an array holding the current {@link Drawable} state of
 * the view.
 *
 * @see #mergeDrawableStates(int[], int[])
 */
vector<int> View::onCreateDrawableState(int extraSpace) {
    if ((mViewFlags & DUPLICATE_PARENT_STATE) == DUPLICATE_PARENT_STATE && mParent->isView()) {
        shared_ptr<ViewGroup> parent = static_pointer_cast<ViewGroup>(mParent);
        return parent->onCreateDrawableState(extraSpace);
    }
    
    vector<int> drawableState;
    
    int viewStateIndex = 0;
    if ((mPrivateFlags & PFLAG_PRESSED) != 0) viewStateIndex |= VIEW_STATE_PRESSED;
    if ((mViewFlags & ENABLED_MASK) == ENABLED) viewStateIndex |= VIEW_STATE_ENABLED;
    if (isFocused()) viewStateIndex |= VIEW_STATE_FOCUSED;
    if ((mPrivateFlags & PFLAG_SELECTED) != 0) viewStateIndex |= VIEW_STATE_SELECTED;
//    if (hasWindowFocus()) viewStateIndex |= VIEW_STATE_WINDOW_FOCUSED;
    if ((mPrivateFlags & PFLAG_ACTIVATED) != 0) viewStateIndex |= VIEW_STATE_ACTIVATED;

    if ((mPrivateFlags & PFLAG_HOVERED) != 0) viewStateIndex |= VIEW_STATE_HOVERED;
    
    if ((mPrivateFlags2 & PFLAG2_DRAG_CAN_ACCEPT) != 0) viewStateIndex |= VIEW_STATE_DRAG_CAN_ACCEPT;
    if ((mPrivateFlags2 & PFLAG2_DRAG_HOVERED) != 0) viewStateIndex |= VIEW_STATE_DRAG_HOVERED;
    
    drawableState = VIEW_STATE_SETS[viewStateIndex];
    
    if (extraSpace == 0) {
        return drawableState;
    }
    
    vector<int> fullState;
    if (!drawableState.empty()) {
        fullState = drawableState;
    } else {
        fullState = vector<int>(extraSpace);
    }
    
    return fullState;
}

void View::onDetachedFromWindow() {
    
    mPrivateFlags3 &= ~PFLAG3_IS_LAID_OUT;
    
    if (mAttachInfo) {
//        mAttachInfo->mViewRootImpl->cancelInvalidate(this);
    }
}

void View::onDraw(shared_ptr<Canvas> canvas) {
}

void View::onFocusChanged(bool gainFocus, int direction, Rect *previouslyFocusedRect) {
    if (gainFocus) {
//        if (AccessibilityManager.getInstance(mContext).isEnabled()) {
//            sendAccessibilityEvent(AccessibilityEvent.TYPE_VIEW_FOCUSED);
//        }
    }
    
//    InputMethodManager imm = InputMethodManager.peekInstance();
    if (!gainFocus) {
        if (isPressed()) {
            setPressed(false);
        }
//        if (imm != NULL && mAttachInfo != NULL
//            && mAttachInfo.mHasWindowFocus) {
//            imm.focusOut(this);
//        }
//        onFocusLost();
    }
//    else if (imm != NULL && mAttachInfo != NULL
//               && mAttachInfo.mHasWindowFocus) {
//        imm.focusIn(this);
//    }
    
    invalidate(true);
//    ListenerInfo li = mListenerInfo;
//    if (li != NULL && li.mOnFocusChangeListener != NULL) {
//        li.mOnFocusChangeListener.onFocusChange(this, gainFocus);
//    }
//    
//    if (mAttachInfo != NULL) {
//        mAttachInfo->mKeyDispatchState.reset(this);
//    }
}

void View::onLayout(bool changed, int left, int top, int right, int bottom) {
    CCLOG("onLayout(%s) %d, %d, %d, %d", m_id.c_str(), left, top, right, bottom);
}

void View::onMeasure(int widthMeasureSpec, int heightMeasureSpec) {

    CCLOG("onMeasure(%s)", m_id.c_str());
    
    setMeasuredDimension(getDefaultSize(getSuggestedMinimumWidth(), widthMeasureSpec),
                getDefaultSize(getSuggestedMinimumHeight(), heightMeasureSpec));
}

bool View::onKeyDown(int keyCode, KeyEvent *event) {
    bool result = false;
    
//    switch (keyCode) {
//        case KeyEvent::KEYCODE_DPAD_CENTER:
//        case KeyEvent::KEYCODE_ENTER: {
//            if ((mViewFlags & ENABLED_MASK) == DISABLED) {
//                return true;
//            }
//            // Long clickable items don't necessarily have to be clickable
//            if (((mViewFlags & CLICKABLE) == CLICKABLE ||
//                 (mViewFlags & LONG_CLICKABLE) == LONG_CLICKABLE) &&
//                (event.getRepeatCount() == 0)) {
//                setPressed(true);
//                checkForLongClick(0);
//                return true;
//            }
//            break;
//        }
//    }
    return result;
}

bool View::onKeyUp(int keyCode, KeyEvent *event) {
    bool result = false;
    
//    switch (keyCode) {
//        case KeyEvent::KEYCODE_DPAD_CENTER:
//        case KeyEvent::KEYCODE_ENTER: {
//            if ((mViewFlags & ENABLED_MASK) == DISABLED) {
//                return true;
//            }
//            if ((mViewFlags & CLICKABLE) == CLICKABLE && isPressed()) {
//                setPressed(false);
//                
//                if (!mHasPerformedLongPress) {
//                    // This is a tap, so remove the longpress check
//                    removeLongPressCallback();
//                    
//                    result = performClick();
//                }
//            }
//            break;
//        }
//    }
    return result;
}

void View::onResolveDrawables(int layoutDirection) {
}

void View::onScrollChanged(int l, int t, int oldl, int oldt) {
//    if (AccessibilityManager.getInstance(mContext).isEnabled()) {
//        postSendViewScrolledAccessibilityEventCallback();
//    }
    
    m_backgroundSizeChanged = true;
    
    AttachInfo *ai = mAttachInfo;
    if (ai != NULL) {
        ai->mViewScrollChanged = true;
    }
}

void View::onStartTemporaryDetach() {
    removeUnsetPressCallback();
    mPrivateFlags |= PFLAG_CANCEL_NEXT_UP_EVENT;
}

bool View::onTouchEvent(androidcpp::MotionEvent *event) {
    
    const int viewFlags = mViewFlags;
    
    if ((viewFlags & ENABLED_MASK) == DISABLED) {
        if (event->getAction() == MotionEvent::ACTION_UP && (mPrivateFlags & PFLAG_PRESSED) != 0) {
            setPressed(false);
        }
        // A disabled view that is clickable still consumes the touch
        // events, it just doesn't respond to them.
        return (((viewFlags & CLICKABLE) == CLICKABLE ||
                 (viewFlags & LONG_CLICKABLE) == LONG_CLICKABLE));
    }
    
//    if (mTouchDelegate != NULL) {
//        if (mTouchDelegate.onTouchEvent(event)) {
//            return true;
//        }
//    }
    
    if (((viewFlags & CLICKABLE) == CLICKABLE || (viewFlags & LONG_CLICKABLE) == LONG_CLICKABLE)) {
        switch (event->getAction()) {
            case MotionEvent::ACTION_UP:
            {
                bool prepressed = (mPrivateFlags & PFLAG_PREPRESSED) != 0;
                
                if ((mPrivateFlags & PFLAG_PRESSED) != 0 || prepressed) {
                    // take focus if we don't have it already and we should in
                    // touch mode.
                    bool focusTaken = false;
//                    if (isFocusable() && isFocusableInTouchMode() && !isFocused()) {
//                        focusTaken = requestFocus();
//                    }
                    
                    if (prepressed) {
                        // The button is being released before we actually
                        // showed it as pressed.  Make it show the pressed
                        // state now (before scheduling the click) to ensure
                        // the user sees it.
                        setPressed(true);
                    }
                    
                    if (!mHasPerformedLongPress) {
                        // This is a tap, so remove the longpress check
                        removeLongPressCallback();
                        
                        // Only perform take click actions if we were in the pressed state
                        if (!focusTaken) {
                            // Use a Runnable and post this rather than calling
                            // performClick directly. This lets other visual state
                            // of the view update before click actions start.
                            if (m_performClick == NULL) {
                                m_performClick = new PerformClick(shared_from_this());
                            }
                            
                            if (!post(m_performClick)) {
                                performClick();
                            }
                        }
                    }
                    
                    if (m_unsetPressedState == NULL) {
                        m_unsetPressedState = new UnsetPressedState(shared_from_this());
                    }
                    
                    if (prepressed) {
                        postDelayed(m_unsetPressedState, PRESSED_STATE_DURATION);
                    } else if (!post(m_unsetPressedState)) {
                        // If the post failed, unpress right now
                        m_unsetPressedState->run();
                    }
                    removeTapCallback();
                }
                break;
            }
                
            case MotionEvent::ACTION_DOWN:
            {
                mHasPerformedLongPress = false;
                
//                if (performButtonActionOnTouchDown(event)) {
//                    break;
//                }
                
                // Walk up the hierarchy to determine if we're inside a scrolling container.
                bool inScrollingContainer = isInScrollingContainer();
                
                // For views inside a scrolling container, delay the pressed feedback for
                // a short period in case this is a scroll.
                if (inScrollingContainer) {
                    mPrivateFlags |= PFLAG_PREPRESSED;
                    if (m_pendingCheckForTap == NULL) {
                        m_pendingCheckForTap = new CheckForTap(shared_from_this());
                    }
                    
                    postDelayed(m_pendingCheckForTap, TAP_TIMEOUT);
                } else {
                    // Not inside a scrolling container, so show the feedback right away
                    setPressed(true);
                    checkForLongClick(0);
                }
                break;
            }
                
            case MotionEvent::ACTION_CANCEL:
            {
                setPressed(false);
                removeTapCallback();
                removeLongPressCallback();
                break;
            }
                
            case MotionEvent::ACTION_MOVE:
            {
                const int x = event->getX();
                const int y = event->getY();
                
                // Be lenient about moving outside of buttons
                if (!pointInView(x, y, mTouchSlop)) {
                    // Outside button
                    removeTapCallback();
                    if ((mPrivateFlags & PFLAG_PRESSED) != 0) {
                        // Remove any future long press/tap checks
                        removeLongPressCallback();
                        
                        setPressed(false);
                    }
                }
                break;
            }
        }
        return true;
    }
    
    return false;
}

void View::onWindowFocusChanged(bool hasWindowFocus) {
//    InputMethodManager imm = InputMethodManager.peekInstance();
    if (!hasWindowFocus) {
        if (isPressed()) {
            setPressed(false);
        }
//        if (imm != NULL && (mPrivateFlags & PFLAG_FOCUSED) != 0) {
//            imm.focusOut(this);
//        }
        removeLongPressCallback();
        removeTapCallback();
//        onFocusLost();
    }
//    else if (imm != NULL && (mPrivateFlags & PFLAG_FOCUSED) != 0) {
//        imm.focusIn(this);
//    }
    refreshDrawableState();
}

void View::onWindowVisibilityChanged(int visibility) {
    if (visibility == View::VISIBLE) {
        // initialAwakenScrollBars();
    }
}

// TODO: implement
bool View::performClick() {
    if (m_onClickListener) {
        m_onClickListener(*this);
        return true;
    }
    
    return false;
}

// TODO: implement
bool View::performLongClick() {
    if (m_onLongClickListener) {
        m_onLongClickListener(*this);
        return true;
    }
    return false;
}

bool View::pointInView(float localX, float localY) const {
    return localX >= 0 && localX < (mRight - mLeft) && localY >= 0 && localY < (mBottom - mTop);
}

bool View::pointInView(float localX, float localY, float slop) const {
    return localX >= -slop && localY >= -slop && localX < ((mRight - mLeft) + slop) &&
        localY < ((mBottom - mTop) + slop);
}

bool View::post(const sp<Runnable> &runnable) {
    const AttachInfo *info = mAttachInfo;
    if (info) {
        return info->mHandler->post(runnable);
    }
    
    return false;
}

bool View::post(function<void ()> function) {
    
    const AttachInfo *info = mAttachInfo;
    if (info && function) {
        return info->mHandler->post(function);
    }
    
    return false;
}

bool View::postDelayed(const sp<Runnable> &runnable, uint32_t delay) {
    const AttachInfo *info = mAttachInfo;
    if (info) {
        return info->mHandler->postDelayed(runnable, delay);
    }
    
    return false;
}

bool View::postDelayed(function<void ()> function, uint32_t delay) {
    
    const AttachInfo *info = mAttachInfo;
    if (info && function) {
        return info->mHandler->postDelayed(function, delay);
    }
    
    return false;
}

/**
 * Call this to force a view to update its drawable state. This will cause
 * drawableStateChanged to be called on this view. Views that are interested
 * in the new state should call getDrawableState.
 *
 * @see #drawableStateChanged
 * @see #getDrawableState
 */
void View::refreshDrawableState() {
    mPrivateFlags |= PFLAG_DRAWABLE_STATE_DIRTY;
    drawableStateChanged();
    
    if (mParent) {
        mParent->childDrawableStateChanged(shared_from_this());
    }
}

bool View::removeCallbacks(const sp<Runnable> &runnable) {
    if (runnable != NULL) {
        const AttachInfo *info = mAttachInfo;
        if (info) {
            return info->mHandler->removeCallbacks(runnable);
        }
    }
    
    return false;
}

void View::removeLongPressCallback() {
    if (m_pendingCheckForLongPress != NULL) {
        removeCallbacks(m_pendingCheckForLongPress);
    }
}

void View::removeTapCallback() {
    if (m_pendingCheckForTap != NULL) {
        mPrivateFlags &= ~PFLAG_PREPRESSED;
        removeCallbacks(m_pendingCheckForTap);
    }
}

void View::removeUnsetPressCallback() {
    if ((mPrivateFlags & PFLAG_PRESSED) != 0 && m_unsetPressedState != NULL) {
        setPressed(false);
        removeCallbacks(m_unsetPressedState);
    }
}

void View::clearFocus() {

    if ((mPrivateFlags & PFLAG_FOCUSED) != 0) {
        mPrivateFlags &= ~PFLAG_FOCUSED;
        
        if (mParent != NULL) {
            mParent->clearChildFocus(shared_from_this());
        }
        
        onFocusChanged(false, 0, NULL);
        
        refreshDrawableState();
        
        if (!rootViewRequestFocus()) {
            notifyGlobalFocusCleared(shared_from_this());
        }
        
//        if (AccessibilityManager.getInstance(mContext).isEnabled()) {
//            notifyAccessibilityStateChanged();
//        }
    }
}

void View::unFocus() {
    
    if ((mPrivateFlags & PFLAG_FOCUSED) != 0) {
        mPrivateFlags &= ~PFLAG_FOCUSED;
        
        onFocusChanged(false, 0, NULL);
        refreshDrawableState();
        
//        if (AccessibilityManager.getInstance(mContext).isEnabled()) {
//            notifyAccessibilityStateChanged();
//        }
    }
}

void View::setSelected(bool selected) {
    if (((mPrivateFlags & PFLAG_SELECTED) != 0) != selected) {
        mPrivateFlags = (mPrivateFlags & ~PFLAG_SELECTED) | (selected ? PFLAG_SELECTED : 0);
        if (!selected) resetPressedState();
        invalidate(true);
        refreshDrawableState();
        dispatchSetSelected(selected);
//        if (AccessibilityManager.getInstance(mContext).isEnabled()) {
//            notifyAccessibilityStateChanged();
//        }
    }
}

void View::resetPressedState() {
    if ((mViewFlags & ENABLED_MASK) == DISABLED) {
        return;
    }
    
    if (isPressed()) {
        setPressed(false);
        
        if (!mHasPerformedLongPress) {
            removeLongPressCallback();
        }
    }
}

bool View::rootViewRequestFocus() {
    shared_ptr<View> root = getRootView();
    if (root != NULL) {
        return root->requestFocus();
    }
    return false;
}

void View::notifyGlobalFocusCleared(shared_ptr<View> oldFocus) {
    if (oldFocus != NULL && mAttachInfo != NULL) {
        mAttachInfo->mTreeObserver->dispatchOnGlobalFocusChange(oldFocus, NULL);
    }
}

bool View::requestFocusNoSearch(int direction, Rect *previouslyFocusedRect) {
    // need to be focusable
    if ((mViewFlags & FOCUSABLE_MASK) != FOCUSABLE ||
        (mViewFlags & VISIBILITY_MASK) != VISIBLE) {
        return false;
    }
    
    // need to be focusable in touch mode if in touch mode
    if (isInTouchMode() &&
        (FOCUSABLE_IN_TOUCH_MODE != (mViewFlags & FOCUSABLE_IN_TOUCH_MODE))) {
        return false;
    }
    
    // need to not have any parents blocking us
    if (hasAncestorThatBlocksDescendantFocus()) {
        return false;
    }
    
    handleFocusGainInternal(direction, previouslyFocusedRect);
    return true;
}

/**
 * @return Whether any ancestor of this view blocks descendant focus.
 */
bool View::hasAncestorThatBlocksDescendantFocus() {
    shared_ptr<ViewParent> &ancestor = mParent;
    shared_ptr<ViewGroup> vgAncestor = dynamic_pointer_cast<ViewGroup>(ancestor);
    while (vgAncestor != NULL) {
        if (vgAncestor->getDescendantFocusability() == ViewGroup::FOCUS_BLOCK_DESCENDANTS) {
            return true;
        } else {
            ancestor = vgAncestor->getParent();
        }
        vgAncestor = dynamic_pointer_cast<ViewGroup>(ancestor);
    }
    return false;
}

void View::handleFocusGainInternal(int direction, Rect *previouslyFocusedRect) {
    
    if ((mPrivateFlags & PFLAG_FOCUSED) == 0) {
        mPrivateFlags |= PFLAG_FOCUSED;
        
        shared_ptr<View> oldFocus = (mAttachInfo != NULL) ? getRootView()->findFocus() : NULL;
        
        if (mParent != NULL) {
            mParent->requestChildFocus(shared_from_this(), shared_from_this());
        }
        
        if (mAttachInfo != NULL) {
            mAttachInfo->mTreeObserver->dispatchOnGlobalFocusChange(oldFocus, shared_from_this());
        }
        
        onFocusChanged(true, direction, previouslyFocusedRect);
        refreshDrawableState();
        
//        if (AccessibilityManager.getInstance(mContext).isEnabled()) {
//            notifyAccessibilityStateChanged();
//        }
    }
}

shared_ptr<View> View::getRootView() {
    if (mAttachInfo != NULL) {
        shared_ptr<View> v = mAttachInfo->mRootView;
        if (v != NULL) {
            return v;
        }
    }
    
    shared_ptr<View> parent = shared_from_this();
    shared_ptr<View> pparent;
    while (parent->mParent != NULL &&  (pparent = dynamic_pointer_cast<View>(parent->mParent)) != NULL) {
        parent = pparent;
    }
    
    return parent;
}

void View::requestLayout() {

    m_measureCache.clear();
    
    if (mAttachInfo && mAttachInfo->mViewRequestingLayout == NULL) {
        ViewRootImpl *viewRoot = getViewRootImpl();

        if (viewRoot != NULL && viewRoot->isInLayout()) {
            if (!viewRoot->requestLayoutDuringLayout(shared_from_this())) {
                return;
            }
        }

        mAttachInfo->mViewRequestingLayout = shared_from_this();
    }

    mPrivateFlags |= PFLAG_FORCE_LAYOUT;
    mPrivateFlags |= PFLAG_INVALIDATED;

    if (mParent && !mParent->isLayoutRequested()) {
        mParent->requestLayout();
    }

    if (mAttachInfo && mAttachInfo->mViewRequestingLayout == shared_from_this()) {
        mAttachInfo->mViewRequestingLayout = NULL;
    }
}

void View::resetResolvedPadding() {
    mPrivateFlags2 &= ~PFLAG2_PADDING_RESOLVED;
}

void View::resetRtlProperties() {
    resetResolvedLayoutDirection();
    resetResolvedTextDirection();
    resetResolvedTextAlignment();
    resetResolvedPadding();
    resetResolvedDrawables();
}

void View::resolveDrawables() {
    if (canResolveLayoutDirection()) {
        if (m_background != NULL) {
            m_background->setLayoutDirection(getLayoutDirection());
        }
        mPrivateFlags2 |= PFLAG2_DRAWABLE_RESOLVED;
        onResolveDrawables(getLayoutDirection());
    }
}

bool View::resolveLayoutDirection() {
    // Clear any previous layout direction resolution
    mPrivateFlags2 &= ~PFLAG2_LAYOUT_DIRECTION_RESOLVED_MASK;
    
    if (hasRtlSupport()) {
        // Set resolved depending on layout direction
        switch ((mPrivateFlags2 & PFLAG2_LAYOUT_DIRECTION_MASK) >>
                PFLAG2_LAYOUT_DIRECTION_MASK_SHIFT) {
            case LAYOUT_DIRECTION_INHERIT:
                // We cannot resolve yet. LTR is by default and let the resolution happen again
                // later to get the correct resolved value
                if (!canResolveLayoutDirection()) return false;
                
                // Parent has not yet resolved, LTR is still the default
                if (!mParent->isLayoutDirectionResolved()) return false;
                
                if (mParent->getLayoutDirection() == LAYOUT_DIRECTION_RTL) {
                    mPrivateFlags2 |= PFLAG2_LAYOUT_DIRECTION_RESOLVED_RTL;
                }
                break;
            case LAYOUT_DIRECTION_RTL:
                mPrivateFlags2 |= PFLAG2_LAYOUT_DIRECTION_RESOLVED_RTL;
                break;
//            case LAYOUT_DIRECTION_LOCALE:
//                if((LAYOUT_DIRECTION_RTL ==
//                    TextUtils::getLayoutDirectionFromLocale(Locale.getDefault()))) {
//                    mPrivateFlags2 |= PFLAG2_LAYOUT_DIRECTION_RESOLVED_RTL;
//                }
//                break;
            default:
            {
                // Nothing to do, LTR by default
            }
        }
    }
    
    // Set to resolved
    mPrivateFlags2 |= PFLAG2_LAYOUT_DIRECTION_RESOLVED;
    return true;
}

void View::resolveLayoutParams() {
    if (mLayoutParams != NULL) {
        mLayoutParams->resolveLayoutDirection(getLayoutDirection());
    }
}

/**
 * Resolve padding depending on layout direction.
 *
 * @hide
 */
void View::resolvePadding() {
//    if (!isRtlCompatibilityMode()) {
//        // Post Jelly Bean MR1 case: we need to take the resolved layout direction into account.
//        // If start / end padding are defined, they will be resolved (hence overriding) to
//        // left / right or right / left depending on the resolved layout direction.
//        // If start / end padding are not defined, use the left / right ones.
//        int resolvedLayoutDirection = getLayoutDirection();
//        switch (resolvedLayoutDirection) {
//            case LAYOUT_DIRECTION_RTL:
//                if (m_userPaddingStart != UNDEFINED_PADDING) {
//                    m_userPaddingRight = m_userPaddingStart;
//                } else {
//                    m_userPaddingRight = m_userPaddingRightInitial;
//                }
//                if (m_userPaddingEnd != UNDEFINED_PADDING) {
//                    m_userPaddingLeft = m_userPaddingEnd;
//                } else {
//                    m_userPaddingLeft = m_userPaddingLeftInitial;
//                }
//                break;
//            case LAYOUT_DIRECTION_LTR:
//            default:
//                if (m_userPaddingStart != UNDEFINED_PADDING) {
//                    m_userPaddingLeft = m_userPaddingStart;
//                } else {
//                    m_userPaddingLeft = m_userPaddingLeftInitial;
//                }
//                if (m_userPaddingEnd != UNDEFINED_PADDING) {
//                    m_userPaddingRight = m_userPaddingEnd;
//                } else {
//                    m_userPaddingRight = m_userPaddingRightInitial;
//                }
//        }
//        
//        m_userPaddingBottom = (m_userPaddingBottom >= 0) ? m_userPaddingBottom : mPaddingBottom;
//        
//        internalSetPadding(m_userPaddingLeft, mPaddingTop, m_userPaddingRight,
//                           m_userPaddingBottom);
//        onRtlPropertiesChanged(resolvedLayoutDirection);
//    }
    
    mPrivateFlags2 |= PFLAG2_PADDING_RESOLVED;
}

bool View::resolveRtlPropertiesIfNeeded() {
    if (!needRtlPropertiesResolution()) return false;
    
    // Order is important here: LayoutDirection MUST be resolved first
    if (!isLayoutDirectionResolved()) {
        resolveLayoutDirection();
        resolveLayoutParams();
    }
    // ... then we can resolve the others properties depending on the resolved LayoutDirection.
    if (!isTextDirectionResolved()) {
        resolveTextDirection();
    }
    if (!isTextAlignmentResolved()) {
        resolveTextAlignment();
    }
    if (!isPaddingResolved()) {
        resolvePadding();
    }
    if (!isDrawablesResolved()) {
        resolveDrawables();
    }
    onRtlPropertiesChanged(getLayoutDirection());
    return true;
}

int View::resolveSize(int size, int measureSpec) {
    return resolveSizeAndState(size, measureSpec, 0) & MEASURED_SIZE_MASK;
}

int View::resolveSizeAndState(int size, int measureSpec, int childMeasuredState) {
    int result = size;
    int specMode = MeasureSpec::getMode(measureSpec);
    int specSize =  MeasureSpec::getSize(measureSpec);
    switch (specMode) {
        case MeasureSpec::UNSPECIFIED:
            result = size;
            break;
        case MeasureSpec::AT_MOST:
            if (specSize < size) {
                result = specSize | MEASURED_STATE_TOO_SMALL;
            } else {
                result = size;
            }
            break;
        case MeasureSpec::EXACTLY:
            result = specSize;
            break;
    }
    return result | (childMeasuredState&MEASURED_STATE_MASK);
}

void View::scheduleDrawable(Drawable *who, const sp<Runnable> &what, uint32_t when) {
    // TODO: implement
}

void View::scrollTo(int x, int y) {
    if (mScrollX != x || mScrollY != y) {
        int oldX = mScrollX;
        int oldY = mScrollY;
        mScrollX = x;
        mScrollY = y;
        invalidateParentCaches();
        onScrollChanged(mScrollX, mScrollY, oldX, oldY);
//        if (!awakenScrollBars()) {
//            postInvalidateOnAnimation();
//        }
    }
}

void View::setBackgroundResource(string resid) {
    if (!resid.empty() && resid == mBackgroundResource) {
        return;
    }
    
    shared_ptr<Drawable> d;
    if (resid != 0) {
        d = mResources->getDrawable(resid);
    }
    setBackground(d);
    
    mBackgroundResource = resid;
}

void View::setBackground(shared_ptr<Drawable> background) {
    
    computeOpaqueFlags();
    
    if (background == m_background) {
        return;
    }
    
    bool shouldRequestLayout = false;
    
    mBackgroundResource = "";
    
    /*
     * Regardless of whether we're setting a new background or not, we want
     * to clear the previous drawable.
     */
    if (m_background != NULL) {
        m_background->setCallback(NULL);
        unscheduleDrawable(m_background.get());
    }
    
    if (background != NULL) {
        resetResolvedDrawables();
        background->setLayoutDirection(getLayoutDirection());
        if (background->getPadding(mPadding)) {
            resetResolvedPadding();
            switch (background->getLayoutDirection()) {
                case LAYOUT_DIRECTION_RTL:
                    m_userPaddingLeftInitial = mPadding.right();
                    m_userPaddingRightInitial = mPadding.left();
                    internalSetPadding(mPadding.right(), mPadding.top(), mPadding.left(), mPadding.bottom());
                    break;
                case LAYOUT_DIRECTION_LTR:
                default:
                    m_userPaddingLeftInitial = mPadding.left();
                    m_userPaddingRightInitial = mPadding.right();
                    internalSetPadding(mPadding.left(), mPadding.top(), mPadding.right(), mPadding.bottom());
            }
        }
        
        // Compare the minimum sizes of the old Drawable and the new.  If there isn't an old or
        // if it has a different minimum size, we should layout again
        if (m_background == NULL || m_background->getMinimumHeight() != background->getMinimumHeight() ||
            m_background->getMinimumWidth() != background->getMinimumWidth()) {
            shouldRequestLayout = true;
        }
        
        background->setCallback(this);
        if (background->isStateful()) {
            background->setState(getDrawableState());
        }
        background->setVisible(getVisibility() == VISIBLE, false);
        m_background = background;
        
        if ((mPrivateFlags & PFLAG_SKIP_DRAW) != 0) {
            mPrivateFlags &= ~PFLAG_SKIP_DRAW;
            mPrivateFlags |= PFLAG_ONLY_DRAWS_BACKGROUND;
            shouldRequestLayout = true;
        }
    } else {
        /* Remove the background */
        m_background = NULL;
        
        if ((mPrivateFlags & PFLAG_ONLY_DRAWS_BACKGROUND) != 0) {
            /*
             * This view ONLY drew the background before and we're removing
             * the background, so now it won't draw anything
             * (hence we SKIP_DRAW)
             */
            mPrivateFlags &= ~PFLAG_ONLY_DRAWS_BACKGROUND;
            mPrivateFlags |= PFLAG_SKIP_DRAW;
        }
        
        /*
         * When the background is set, we try to apply its padding to this
         * View. When the background is removed, we don't touch this View's
         * padding-> This is noted in the Javadocs. Hence, we don't need to
         * requestLayout(), the invalidate() below is sufficient.
         */
        
        // The old background's minimum size could have affected this
        // View's layout, so let's requestLayout
        shouldRequestLayout = true;
    }
    
    computeOpaqueFlags();
    
    if (shouldRequestLayout) {
        requestLayout();
    }
    
    m_backgroundSizeChanged = true;
    invalidate(true);
}

void View::sizeChange(int newWidth, int newHeight, int oldWidth, int oldHeight) {
    onSizeChanged(newWidth, newHeight, oldWidth, oldHeight);
}

void View::setDisplayListProperties(GLES20DisplayList *displayList) {
    if (displayList) {
        displayList->setLeftTopRightBottom(mLeft, mTop, mRight, mBottom);
        displayList->setHasOverlappingRendering(hasOverlappingRendering());
        if (mParent->isView()) {
            shared_ptr<ViewGroup> parent = static_pointer_cast<ViewGroup>(mParent);
            displayList->setClipChildren((parent->mGroupFlags & ViewGroup::FLAG_CLIP_CHILDREN) != 0);
        }
    }
}

/**
 * Set the enabled state of this view. The interpretation of the enabled
 * state varies by subclass.
 *
 * @param enabled True if this view is enabled, false otherwise.
 */
void View::setEnabled(bool enabled) {
    if (enabled == isEnabled()) return;
    
    setFlags(enabled ? ENABLED : DISABLED, ENABLED_MASK);
    
    /*
     * The View most likely has to change its appearance, so refresh
     * the drawable state.
     */
    refreshDrawableState();
    
    // Invalidate too, since the default behavior for views is to be
    // be drawn at 50% alpha rather than to change the drawable.
    invalidate(true);
}

void View::setFlags(int flags, int mask) {
    int old = mViewFlags;
    mViewFlags = (mViewFlags & ~mask) | (flags & mask);

    int changed = mViewFlags ^ old;
    if (changed == 0) {
        return;
    }

    if ((flags & VISIBILITY_MASK) == VISIBLE) {
        if ((changed & VISIBILITY_MASK) != 0) {
            /*
             * If this view is becoming visible, invalidate it in case it changed while
             * it was not visible. Marking it drawn ensures that the invalidation will
             * go through.
             */
            invalidate(true);
        }
    }

    /* Check if the GONE bit has changed */
    if ((changed & GONE) != 0) {
        requestLayout();

        if (((mViewFlags & VISIBILITY_MASK) == GONE)) {

            mParent->invalidate(true);
            // Mark the view drawn to ensure that it gets invalidated properly the next
            // time it is visible and gets invalidated
        }
        if (mAttachInfo) {
            mAttachInfo->mViewVisibilityChanged = true;
        }
    }

    /* Check if the VISIBLE bit has changed */
    if ((changed & INVISIBLE) != 0) {
        /*
         * If this view is becoming invisible, set the DRAWN flag so that
         * the next invalidate() will not be skipped.
         */
        if (mAttachInfo) {
            mAttachInfo->mViewVisibilityChanged = true;
        }
    }

    if ((changed & DRAWING_CACHE_ENABLED) != 0) {
//        destroyDrawingCache();
        mPrivateFlags &= ~PFLAG_DRAWING_CACHE_VALID;
        invalidateParentCaches();
    }

    if ((changed & DRAWING_CACHE_QUALITY_MASK) != 0) {
//        destroyDrawingCache();
        mPrivateFlags &= ~PFLAG_DRAWING_CACHE_VALID;
    }
}

bool View::setFrame(int left, int top, int right, int bottom) {
    bool changed = false;

    if (mLeft != left || mRight != right || mTop != top || mBottom != bottom) {
        changed = true;

        int oldWidth = mRight - mLeft;
        int oldHeight = mBottom - mTop;
        int newWidth = right - left;
        int newHeight = bottom - top;
        bool sizeChanged = (newWidth != oldWidth) || (newHeight != oldHeight);

        // Invalidate our old position
        invalidate(sizeChanged);

        mLeft = left;
        mTop = top;
        mRight = right;
        mBottom = bottom;

        if (sizeChanged) {
            sizeChange(newWidth, newHeight, oldWidth, oldHeight);
        }
    }

    return changed;
}

void View::setHorizontalFadingEdgeEnabled(bool horizontalFadingEdgeEnabled) {
    if (isHorizontalFadingEdgeEnabled() != horizontalFadingEdgeEnabled) {
        if (horizontalFadingEdgeEnabled) {
            initScrollCache();
        }
        
        mViewFlags ^= FADING_EDGE_HORIZONTAL;
    }
}

void View::setVerticalFadingEdgeEnabled(bool verticalFadingEdgeEnabled) {
    if (isVerticalFadingEdgeEnabled() != verticalFadingEdgeEnabled) {
        if (verticalFadingEdgeEnabled) {
            initScrollCache();
        }
        
        mViewFlags ^= FADING_EDGE_VERTICAL;
    }
}

void View::setId(string id) {
    m_id = id;
    if (m_id == NO_ID) {
        m_id = generateViewId();
    }
}

void View::setIsRootNamespace(bool isRoot) {
    if (isRoot) {
        mPrivateFlags |= PFLAG_IS_ROOT_NAMESPACE;
    } else {
        mPrivateFlags &= ~PFLAG_IS_ROOT_NAMESPACE;
    }
}

void View::setLayoutDirection(int layoutDirection) {
    if (getRawLayoutDirection() != layoutDirection) {
        // Reset the current layout direction and the resolved one
        mPrivateFlags2 &= ~PFLAG2_LAYOUT_DIRECTION_MASK;
        resetRtlProperties();
        // Set the new layout direction (filtered)
        mPrivateFlags2 |=
        ((layoutDirection << PFLAG2_LAYOUT_DIRECTION_MASK_SHIFT) & PFLAG2_LAYOUT_DIRECTION_MASK);
        // We need to resolve all RTL properties as they all depend on layout direction
        resolveRtlPropertiesIfNeeded();
        requestLayout();
        invalidate(true);
    }
}

void View::setLayoutParams(LayoutParams *params) {
    if (params) {
        mLayoutParams = params;
        requestLayout();
    }
}

void View::setMeasuredDimension(int measuredWidth, int measuredHeight) {
    
    CCLOG("MeasuredDimensions(%s) %d, %d", m_id.c_str(), measuredWidth, measuredHeight);
    
    m_measuredWidth = measuredWidth;
    m_measuredHeight = measuredHeight;
    
    mPrivateFlags |= PFLAG_MEASURED_DIMENSION_SET;
}

void View::setOnClickListener(function<void (View &)> onClickListener) {
    if (!isClickable()) {
        setClickable(true);
    }
    
    m_onClickListener = onClickListener;
}

void View::setOnLongClickListener(function<void (View &)> onLongClickListener) {
    if (!isLongClickable()) {
        setLongClickable(true);
    }
    
    m_onLongClickListener = onLongClickListener;
}

void View::setPadding(int left, int top, int right, int bottom) {
    resetResolvedPadding();
    
    m_userPaddingStart = UNDEFINED_PADDING;
    m_userPaddingEnd = UNDEFINED_PADDING;
    
    m_userPaddingLeftInitial = left;
    m_userPaddingRightInitial = right;
    
    internalSetPadding(left, top, right, bottom);
}

void View::setPaddingRelative(int start, int top, int end, int bottom) {
    resetResolvedPadding();
    
    m_userPaddingStart = start;
    m_userPaddingEnd = end;
    
    switch(getLayoutDirection()) {
        case LAYOUT_DIRECTION_RTL:
            m_userPaddingLeftInitial = end;
            m_userPaddingRightInitial = start;
            internalSetPadding(end, top, start, bottom);
            break;
        case LAYOUT_DIRECTION_LTR:
        default:
            m_userPaddingLeftInitial = start;
            m_userPaddingRightInitial = end;
            internalSetPadding(start, top, end, bottom);
    }
}

/**
 * Sets the pressed state for this view.
 *
 * @see #isClickable()
 * @see #setClickable(bool)
 *
 * @param pressed Pass true to set the View's internal state to "pressed", or false to reverts
 *        the View's internal state from a previously set "pressed" state.
 */
void View::setPressed(bool pressed) {
    
    const bool needsRefresh = pressed != ((mPrivateFlags & PFLAG_PRESSED) == PFLAG_PRESSED);
    
    if (pressed) {
        mPrivateFlags |= PFLAG_PRESSED;
    } else {
        mPrivateFlags &= ~PFLAG_PRESSED;
    }
    
    if (needsRefresh) {
        refreshDrawableState();
    }
    
    dispatchSetPressed(pressed);
}

void View::setTextAlignment(int textAlignment) {
    if (textAlignment != getRawTextAlignment()) {
        // Reset the current and resolved text alignment
        mPrivateFlags2 &= ~PFLAG2_TEXT_ALIGNMENT_MASK;
        resetResolvedTextAlignment();
        // Set the new text alignment
        mPrivateFlags2 |=
        ((textAlignment << PFLAG2_TEXT_ALIGNMENT_MASK_SHIFT) & PFLAG2_TEXT_ALIGNMENT_MASK);
        // Do resolution
        resolveTextAlignment();
        // Notify change
//        onRtlPropertiesChanged(getLayoutDirection());
        // Refresh
        requestLayout();
        invalidate(true);
    }
}

/**
 * Utility method to transform a given Rect by the current matrix of this view.
 */
void View::transformRect(Rect &rect) {
    if (!getMatrix().isIdentity()) {
        RectF boundingRect = mAttachInfo->mTmpTransformRect;
        boundingRect.set(rect);
        getMatrix().mapRect(&boundingRect);
        rect.set((int) floor(boundingRect.left()),
                 (int) floor(boundingRect.top()),
                 (int) ceil(boundingRect.right()),
                 (int) ceil(boundingRect.bottom()));
    }
}

void View::setHorizontalScrollBarEnabled(bool horizontalScrollBarEnabled) {
    if (isHorizontalScrollBarEnabled() != horizontalScrollBarEnabled) {
        mViewFlags ^= SCROLLBARS_HORIZONTAL;
        computeOpaqueFlags();
        resolvePadding();
    }
}

void View::setVerticalScrollBarEnabled(bool verticalScrollBarEnabled) {
    if (isVerticalScrollBarEnabled() != verticalScrollBarEnabled) {
        mViewFlags ^= SCROLLBARS_VERTICAL;
        computeOpaqueFlags();
        resolvePadding();
    }
}

bool View::resolveTextAlignment() {
    // Reset any previous text alignment resolution
    mPrivateFlags2 &= ~(PFLAG2_TEXT_ALIGNMENT_RESOLVED | PFLAG2_TEXT_ALIGNMENT_RESOLVED_MASK);
    
//    if (hasRtlSupport()) {
//        // Set resolved text alignment flag depending on text alignment flag
//        final int textAlignment = getRawTextAlignment();
//        switch (textAlignment) {
//            case TEXT_ALIGNMENT_INHERIT:
//                // Check if we can resolve the text alignment
//                if (!canResolveTextAlignment()) {
//                    // We cannot do the resolution if there is no parent so use the default
//                    mPrivateFlags2 |= PFLAG2_TEXT_ALIGNMENT_RESOLVED_DEFAULT;
//                    // Resolution will need to happen again later
//                    return false;
//                }
//                
//                // Parent has not yet resolved, so we still return the default
//                if (!mParent.isTextAlignmentResolved()) {
//                    mPrivateFlags2 |= PFLAG2_TEXT_ALIGNMENT_RESOLVED_DEFAULT;
//                    // Resolution will need to happen again later
//                    return false;
//                }
//                
//                final int parentResolvedTextAlignment = mParent.getTextAlignment();
//                switch (parentResolvedTextAlignment) {
//                    case TEXT_ALIGNMENT_GRAVITY:
//                    case TEXT_ALIGNMENT_TEXT_START:
//                    case TEXT_ALIGNMENT_TEXT_END:
//                    case TEXT_ALIGNMENT_CENTER:
//                    case TEXT_ALIGNMENT_VIEW_START:
//                    case TEXT_ALIGNMENT_VIEW_END:
//                        // Resolved text alignment is the same as the parent resolved
//                        // text alignment
//                        mPrivateFlags2 |=
//                        (parentResolvedTextAlignment << PFLAG2_TEXT_ALIGNMENT_RESOLVED_MASK_SHIFT);
//                        break;
//                    default:
//                        // Use default resolved text alignment
//                        mPrivateFlags2 |= PFLAG2_TEXT_ALIGNMENT_RESOLVED_DEFAULT;
//                }
//                break;
//            case TEXT_ALIGNMENT_GRAVITY:
//            case TEXT_ALIGNMENT_TEXT_START:
//            case TEXT_ALIGNMENT_TEXT_END:
//            case TEXT_ALIGNMENT_CENTER:
//            case TEXT_ALIGNMENT_VIEW_START:
//            case TEXT_ALIGNMENT_VIEW_END:
//                // Resolved text alignment is the same as text alignment
//                mPrivateFlags2 |= (textAlignment << PFLAG2_TEXT_ALIGNMENT_RESOLVED_MASK_SHIFT);
//                break;
//            default:
//                // Use default resolved text alignment
//                mPrivateFlags2 |= PFLAG2_TEXT_ALIGNMENT_RESOLVED_DEFAULT;
//        }
//    } else {
        // Use default resolved text alignment
        mPrivateFlags2 |= PFLAG2_TEXT_ALIGNMENT_RESOLVED_DEFAULT;
//    }
    
    // Set the resolved
    mPrivateFlags2 |= PFLAG2_TEXT_ALIGNMENT_RESOLVED;
    return true;
}

bool View::canResolveTextAlignment() {
    switch (getRawTextAlignment()) {
        case TEXT_DIRECTION_INHERIT:
            return (mParent != NULL) && mParent->canResolveTextAlignment();
        default:
            return true;
    }
}

void View::resetResolvedTextAlignment() {
    // Reset any previous text alignment resolution
    mPrivateFlags2 &= ~(PFLAG2_TEXT_ALIGNMENT_RESOLVED | PFLAG2_TEXT_ALIGNMENT_RESOLVED_MASK);
    // Set to default
    mPrivateFlags2 |= PFLAG2_TEXT_ALIGNMENT_RESOLVED_DEFAULT;
}

bool View::isTextAlignmentInherited() {
    return (getRawTextAlignment() == TEXT_ALIGNMENT_INHERIT);
}

bool View::isTextAlignmentResolved() {
    return (mPrivateFlags2 & PFLAG2_TEXT_ALIGNMENT_RESOLVED) == PFLAG2_TEXT_ALIGNMENT_RESOLVED;
}

int View::getRawTextDirection() {
    return (mPrivateFlags2 & PFLAG2_TEXT_DIRECTION_MASK) >> PFLAG2_TEXT_DIRECTION_MASK_SHIFT;
}

void View::setTextDirection(int textDirection) {
    if (getRawTextDirection() != textDirection) {
        // Reset the current text direction and the resolved one
        mPrivateFlags2 &= ~PFLAG2_TEXT_DIRECTION_MASK;
        resetResolvedTextDirection();
        // Set the new text direction
        mPrivateFlags2 |= ((textDirection << PFLAG2_TEXT_DIRECTION_MASK_SHIFT) & PFLAG2_TEXT_DIRECTION_MASK);
        // Do resolution
        resolveTextDirection();
        // Notify change
//        onRtlPropertiesChanged(getLayoutDirection());
        // Refresh
        requestLayout();
        invalidate(true);
    }
}

int View::getTextDirection() {
    return (mPrivateFlags2 & PFLAG2_TEXT_DIRECTION_RESOLVED_MASK) >> PFLAG2_TEXT_DIRECTION_RESOLVED_MASK_SHIFT;
}

bool View::resolveTextDirection() {
    // Reset any previous text direction resolution
    mPrivateFlags2 &= ~(PFLAG2_TEXT_DIRECTION_RESOLVED | PFLAG2_TEXT_DIRECTION_RESOLVED_MASK);
    
//    if (hasRtlSupport()) {
//        // Set resolved text direction flag depending on text direction flag
//        const int textDirection = getRawTextDirection();
//        switch(textDirection) {
//            case TEXT_DIRECTION_INHERIT:
//                if (!canResolveTextDirection()) {
//                    // We cannot do the resolution if there is no parent, so use the default one
//                    mPrivateFlags2 |= PFLAG2_TEXT_DIRECTION_RESOLVED_DEFAULT;
//                    // Resolution will need to happen again later
//                    return false;
//                }
//                
//                // Parent has not yet resolved, so we still return the default
//                if (!mParent.isTextDirectionResolved()) {
//                    mPrivateFlags2 |= PFLAG2_TEXT_DIRECTION_RESOLVED_DEFAULT;
//                    // Resolution will need to happen again later
//                    return false;
//                }
//                
//                // Set current resolved direction to the same value as the parent's one
//                final int parentResolvedDirection = mParent.getTextDirection();
//                switch (parentResolvedDirection) {
//                    case TEXT_DIRECTION_FIRST_STRONG:
//                    case TEXT_DIRECTION_ANY_RTL:
//                    case TEXT_DIRECTION_LTR:
//                    case TEXT_DIRECTION_RTL:
//                    case TEXT_DIRECTION_LOCALE:
//                        mPrivateFlags2 |=
//                        (parentResolvedDirection << PFLAG2_TEXT_DIRECTION_RESOLVED_MASK_SHIFT);
//                        break;
//                    default:
//                        // Default resolved direction is "first strong" heuristic
//                        mPrivateFlags2 |= PFLAG2_TEXT_DIRECTION_RESOLVED_DEFAULT;
//                }
//                break;
//            case TEXT_DIRECTION_FIRST_STRONG:
//            case TEXT_DIRECTION_ANY_RTL:
//            case TEXT_DIRECTION_LTR:
//            case TEXT_DIRECTION_RTL:
//            case TEXT_DIRECTION_LOCALE:
//                // Resolved direction is the same as text direction
//                mPrivateFlags2 |= (textDirection << PFLAG2_TEXT_DIRECTION_RESOLVED_MASK_SHIFT);
//                break;
//            default:
//                // Default resolved direction is "first strong" heuristic
//                mPrivateFlags2 |= PFLAG2_TEXT_DIRECTION_RESOLVED_DEFAULT;
//        }
//    } else {
        // Default resolved direction is "first strong" heuristic
        mPrivateFlags2 |= PFLAG2_TEXT_DIRECTION_RESOLVED_DEFAULT;
//    }
    
    // Set to resolved
    mPrivateFlags2 |= PFLAG2_TEXT_DIRECTION_RESOLVED;
    return true;
}

bool View::canResolveTextDirection() {
    switch (getRawTextDirection()) {
        case TEXT_DIRECTION_INHERIT:
            return (mParent != NULL) && mParent->canResolveTextDirection();
        default:
            return true;
    }
}

void View::resetResolvedTextDirection() {
    // Reset any previous text direction resolution
    mPrivateFlags2 &= ~(PFLAG2_TEXT_DIRECTION_RESOLVED | PFLAG2_TEXT_DIRECTION_RESOLVED_MASK);
    // Set to default value
    mPrivateFlags2 |= PFLAG2_TEXT_DIRECTION_RESOLVED_DEFAULT;
}

bool View::isTextDirectionInherited() {
    return (getRawTextDirection() == TEXT_DIRECTION_INHERIT);
}

bool View::isTextDirectionResolved() {
    return (mPrivateFlags2 & PFLAG2_TEXT_DIRECTION_RESOLVED) == PFLAG2_TEXT_DIRECTION_RESOLVED;
}

void View::setVisibility(int visibility) {
    setFlags(visibility, View::VISIBILITY_MASK);
}

void View::unscheduleDrawable(Drawable *who, const sp<Runnable> &what) {
    // TODO: implement
}

void View::unscheduleDrawable(Drawable *who) {
    if (mAttachInfo && who != NULL) {
//        mAttachInfo->mViewRootImpl.mChoreographer.removeCallbacks(Choreographer.CALLBACK_ANIMATION, NULL, who);
    }
}

/**
 * When a View's drawing cache is enabled, drawing is redirected to an
 * offscreen bitmap. Some views, like an ImageView, must be able to
 * bypass this mechanism if they already draw a single bitmap, to avoid
 * unnecessary usage of the memory.
 *
 * @param willNotCacheDrawing true if this view does not cache its
 *        drawing, false otherwise
 */
void View::setWillNotCacheDrawing(bool willNotCacheDrawing) {
    setFlags(willNotCacheDrawing ? WILL_NOT_CACHE_DRAWING : 0, WILL_NOT_CACHE_DRAWING);
}

/**
 * If this view doesn't do any drawing on its own, set this flag to
 * allow further optimizations. By default, this flag is not set on
 * View, but could be set on some View subclasses such as ViewGroup.
 *
 * Typically, if you override {@link #onDraw(android.graphics.Canvas)}
 * you should clear this flag.
 *
 * @param willNotDraw whether or not this View draw on its own
 */
void View::setWillNotDraw(bool willNotDraw) {
    setFlags(willNotDraw ? WILL_NOT_DRAW : 0, DRAW_MASK);
}

/**
 * Returns whether or not this View draws on its own.
 *
 * @return true if this view has nothing to draw, false otherwise
 */
bool View::willNotDraw() {
    return (mViewFlags & DRAW_MASK) == WILL_NOT_DRAW;
}

/**
 * Recomputes the transform matrix if necessary.
 */
void View::updateMatrix() {
    
    TransformationInfo &info = mTransformationInfo;
    
    if (info.mMatrixDirty) {
        // transform-related properties have changed since the last time someone
        // asked for the matrix; recalculate it with the current values
        
        // Figure out if we need to update the pivot point
        if ((mPrivateFlags & PFLAG_PIVOT_EXPLICITLY_SET) == 0) {
            if ((mRight - mLeft) != info.mPrevWidth || (mBottom - mTop) != info.mPrevHeight) {
                info.mPrevWidth = mRight - mLeft;
                info.mPrevHeight = mBottom - mTop;
                info.mPivotX = info.mPrevWidth / 2.0f;
                info.mPivotY = info.mPrevHeight / 2.0f;
            }
        }
        info.mMatrix.reset();
        if (!nonzero(info.mRotationX) && !nonzero(info.mRotationY)) {
            info.mMatrix.setTranslate(info.mTranslationX, info.mTranslationY);
            info.mMatrix.preRotate(info.mRotation, info.mPivotX, info.mPivotY);
            info.mMatrix.preScale(info.mScaleX, info.mScaleY, info.mPivotX, info.mPivotY);
        } else {
            if (info.mCamera == NULL) {
                info.mCamera = new Camera();
                info.matrix3D = new Matrix();
            }
            info.mCamera->save();
            info.mMatrix.preScale(info.mScaleX, info.mScaleY, info.mPivotX, info.mPivotY);
            info.mCamera->rotate(info.mRotationX, info.mRotationY, -info.mRotation);
            info.mCamera->getMatrix(info.matrix3D);
            info.matrix3D->preTranslate(-info.mPivotX, -info.mPivotY);
            info.matrix3D->postTranslate(info.mPivotX + info.mTranslationX,
                                        info.mPivotY + info.mTranslationY);
            info.mMatrix.postConcat(info.get3DMatrix());
            info.mCamera->restore();
        }
        info.mMatrixDirty = false;
        info.mMatrixIsIdentity = info.mMatrix.isIdentity();
        info.mInverseMatrixDirty = true;
    }
}

ANDROID_END