/*
 * File:   FrameLayout.cpp
 * Author: saulhoward
 *
 * Created on October 17, 2013, 11:19 PM
 */

#include "FrameLayout.h"

#include "Android/widget/FrameLayoutParams.h"

#include <algorithm>

ANDROID_BEGIN

FrameLayout::FrameLayout(Context *context) : ViewGroup(context) {
}

FrameLayout::FrameLayout(Context* context, AttributeSet* attrs) : ViewGroup(context, attrs) {
    attrs->loadAttributes();
}

FrameLayout::~FrameLayout() {
}

void FrameLayout::setForegroundGravity(int foregroundGravity) {
    if (m_foregroundGravity != foregroundGravity) {
        if ((foregroundGravity & Gravity::RELATIVE_HORIZONTAL_GRAVITY_MASK) == 0) {
            foregroundGravity |= Gravity::START;
        }

        if ((foregroundGravity & Gravity::VERTICAL_GRAVITY_MASK) == 0) {
            foregroundGravity |= Gravity::TOP;
        }

        m_foregroundGravity = foregroundGravity;

        /*if (m_foregroundGravity == Gravity::FILL && m_foreground != null) {
            Rect padding = new Rect();
            if (m_foreground.getPadding(padding)) {
                m_foregroundPaddingLeft = padding.left;
                m_foregroundPaddingTop = padding.top;
                m_foregroundPaddingRight = padding.right;
                m_foregroundPaddingBottom = padding.bottom;
            }
        } else {*/
        m_foregroundPaddingLeft = 0;
        m_foregroundPaddingTop = 0;
        m_foregroundPaddingRight = 0;
        m_foregroundPaddingBottom = 0;

        requestLayout();
    }
}

int FrameLayout::getPaddingLeftWithForeground() {
    return m_foregroundInPadding ? std::max(mPaddingLeft, m_foregroundPaddingLeft) :
            mPaddingLeft + m_foregroundPaddingLeft;
}

int FrameLayout::getPaddingRightWithForeground() {
    return m_foregroundInPadding ? std::max(mPaddingRight, m_foregroundPaddingRight) :
            mPaddingRight + m_foregroundPaddingRight;
}

int FrameLayout::getPaddingTopWithForeground() {
    return m_foregroundInPadding ? std::max(mPaddingTop, m_foregroundPaddingTop) :
            mPaddingTop + m_foregroundPaddingTop;
}

int FrameLayout::getPaddingBottomWithForeground() {
    return m_foregroundInPadding ? std::max(mPaddingBottom, m_foregroundPaddingBottom) :
            mPaddingBottom + m_foregroundPaddingBottom;
}

void FrameLayout::onMeasure(int widthMeasureSpec, int heightMeasureSpec) {

    int count = getChildCount();

    const bool measureMatchParentChildren =
            MeasureSpec::getMode(widthMeasureSpec) != MeasureSpec::EXACTLY ||
                    MeasureSpec::getMode(heightMeasureSpec) != MeasureSpec::EXACTLY;
    m_matchParentChildren.clear();

    int maxHeight = 0;
    int maxWidth = 0;
    int childState = 0;

    for (int i = 0; i < count; i++) {
        shared_ptr<View> child = getChildAt(i);
        if (m_measureAllChildren || child->getVisibility() != View::GONE) {
            measureChildWithMargins(child, widthMeasureSpec, 0, heightMeasureSpec, 0);
            FrameLayoutParams *lp = static_cast<FrameLayoutParams*>(child->getLayoutParams());
            maxWidth = std::max(maxWidth, child->getMeasuredWidth() + lp->leftMargin + lp->rightMargin);
            maxHeight = std::max(maxHeight, child->getMeasuredHeight() + lp->topMargin + lp->bottomMargin);
            childState = combineMeasuredStates(childState, child->getMeasuredState());
            if (measureMatchParentChildren) {
                if (lp->width == LayoutParams::MATCH_PARENT ||
                        lp->height == LayoutParams::MATCH_PARENT) {
                    m_matchParentChildren.push_back(child);
                }
            }
        }
    }

    // Account for padding too
    maxWidth += getPaddingLeftWithForeground() + getPaddingRightWithForeground();
    maxHeight += getPaddingTopWithForeground() + getPaddingBottomWithForeground();

    // Check against our minimum height and width
    maxHeight = std::max(maxHeight, getSuggestedMinimumHeight());
    maxWidth = std::max(maxWidth, getSuggestedMinimumWidth());

    // Check against our foreground's minimum height and width
    /*const Drawable drawable = getForeground();
    if (drawable != null) {
        maxHeight = std::max(maxHeight, drawable.getMinimumHeight());
        maxWidth = std::max(maxWidth, drawable.getMinimumWidth());
    }*/

    setMeasuredDimension(resolveSizeAndState(maxWidth, widthMeasureSpec, childState),
            resolveSizeAndState(maxHeight, heightMeasureSpec,
                    childState << MEASURED_HEIGHT_STATE_SHIFT));

    count = m_matchParentChildren.size();
    if (count > 1) {
        for (int i = 0; i < count; i++) {
            shared_ptr<View> child = m_matchParentChildren[i];
            
            MarginLayoutParams *lp = static_cast<MarginLayoutParams*>(child->getLayoutParams());
            int childWidthMeasureSpec;
            int childHeightMeasureSpec;

            if (lp->width == LayoutParams::MATCH_PARENT) {
                childWidthMeasureSpec = MeasureSpec::makeMeasureSpec(getMeasuredWidth() -
                        getPaddingLeftWithForeground() - getPaddingRightWithForeground() -
                        lp->leftMargin - lp->rightMargin,
                        MeasureSpec::EXACTLY);
            } else {
                childWidthMeasureSpec = getChildMeasureSpec(widthMeasureSpec,
                        getPaddingLeftWithForeground() + getPaddingRightWithForeground() +
                                lp->leftMargin + lp->rightMargin,
                        lp->width);
            }

            if (lp->height == LayoutParams::MATCH_PARENT) {
                childHeightMeasureSpec = MeasureSpec::makeMeasureSpec(getMeasuredHeight() -
                        getPaddingTopWithForeground() - getPaddingBottomWithForeground() -
                        lp->topMargin - lp->bottomMargin,
                        MeasureSpec::EXACTLY);
            } else {
                childHeightMeasureSpec = getChildMeasureSpec(heightMeasureSpec,
                        getPaddingTopWithForeground() + getPaddingBottomWithForeground() +
                                lp->topMargin + lp->bottomMargin,
                        lp->height);
            }

            child->measure(childWidthMeasureSpec, childHeightMeasureSpec);
        }
    }
}

void FrameLayout::onLayout(bool changed, int left, int top, int right, int bottom) {

    CCLOG("onLayout(%s) %d, %d, %d, %d", m_id.c_str(), left, top, right, bottom);
    const int count = getChildCount();

    const int parentLeft = getPaddingLeftWithForeground();
    const int parentRight = right - left - getPaddingRightWithForeground();

    const int parentTop = getPaddingTopWithForeground();
    const int parentBottom = bottom - top - getPaddingBottomWithForeground();

    m_foregroundBoundsChanged = true;

    for (int i = 0; i < count; i++) {
        shared_ptr<View> child = getChildAt(i);
        if (child->getVisibility() != View::GONE) {
            FrameLayoutParams *lp = static_cast<FrameLayoutParams*>(child->getLayoutParams());

            const int width = child->getMeasuredWidth();
            const int height = child->getMeasuredHeight();

            int childLeft;
            int childTop;

            int gravity = lp->gravity;
            if (gravity == -1) {
                gravity = DEFAULT_CHILD_GRAVITY;
            }

            const int layoutDirection = getLayoutDirection();
            const int absoluteGravity = Gravity::getAbsoluteGravity(gravity, layoutDirection);
            const int verticalGravity = gravity & Gravity::VERTICAL_GRAVITY_MASK;

            switch (absoluteGravity & Gravity::HORIZONTAL_GRAVITY_MASK) {
                case Gravity::LEFT:
                    childLeft = parentLeft + lp->leftMargin;
                    break;
                case Gravity::CENTER_HORIZONTAL:
                    childLeft = parentLeft + (parentRight - parentLeft - width) / 2 +
                            lp->leftMargin - lp->rightMargin;
                    break;
                case Gravity::RIGHT:
                    childLeft = parentRight - width - lp->rightMargin;
                    break;
                default:
                    childLeft = parentLeft + lp->leftMargin;
            }

            switch (verticalGravity) {
                case Gravity::TOP:
                    childTop = parentTop + lp->topMargin;
                    break;
                case Gravity::CENTER_VERTICAL:
                    childTop = parentTop + (parentBottom - parentTop - height) / 2 +
                            lp->topMargin - lp->bottomMargin;
                    break;
                case Gravity::BOTTOM:
                    childTop = parentBottom - height - lp->bottomMargin;
                    break;
                default:
                    childTop = parentTop + lp->topMargin;
            }

            child->layout(childLeft, childTop, childLeft + width, childTop + height);
        }
    }
}

void FrameLayout::onSizeChanged(int w, int h, int oldw, int oldh) {
    View::onSizeChanged(w, h, oldw, oldh);
    m_foregroundBoundsChanged = true;
}

void FrameLayout::setMeasureAllChildren(bool measureAll) {
    m_measureAllChildren = measureAll;
}

bool FrameLayout::getMeasureAllChildren() {
    return m_measureAllChildren;
}

LayoutParams *FrameLayout::generateDefaultLayoutParams() {
    return new LayoutParams(LayoutParams::MATCH_PARENT, LayoutParams::MATCH_PARENT);
}

LayoutParams *FrameLayout::generateLayoutParams(AttributeSet *attrs) {
    return new FrameLayoutParams(getContext(), attrs);
}

bool FrameLayout::checkLayoutParams(LayoutParams *p) {
    return (dynamic_cast<FrameLayoutParams*>(p) != 0);
}

LayoutParams *FrameLayout::generateLayoutParams(LayoutParams *p) {
    return new FrameLayoutParams(p);
}

ANDROID_END