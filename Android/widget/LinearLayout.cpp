/*
 * File:   LinearLayout.cpp
 * Author: saulhoward
 *
 * Created on October 9, 2013, 11:40 AM
 */

#include "LinearLayout.h"

#include "Android/widget/LinearLayoutParams.h"

#include <algorithm>

ANDROID_BEGIN

LinearLayout::LinearLayout(Context *context) : ViewGroup(context) {
}

LinearLayout::LinearLayout(Context *context, AttributeSet *attrs) : ViewGroup(context, attrs) {
    attrs->loadAttributes();

    for (int i = 0; i < attrs->getAttributeCount(); i++) {

        const char *name = attrs->getAttributeName(i);

		if (strcmp(R::styleable::orientation, name) == 0) {
			const char *value = attrs->getAttributeValue(name);
			if (strcmp(R::styleable::horizontal, value) == 0) {
				setOrientation(HORIZONTAL);
			} else {
				setOrientation(VERTICAL);
			}
		} else if (strcmp(R::styleable::weightSum, name) == 0) {
			m_weightSum = attrs->getAttributeIntValue(name, -1.0);
		}
	}
}

LinearLayout::~LinearLayout() {
}

void LinearLayout::forceUniformHeight(int count, int widthMeasureSpec) {
    // Pretend that the linear layout has an exact size. This is the measured height of
    // ourselves. The measured height should be the max height of the children, changed
    // to accommodate the heightMeasureSpec from the parent
    int uniformMeasureSpec = MeasureSpec::makeMeasureSpec(getMeasuredHeight(),
            MeasureSpec::EXACTLY);
    for (int i = 0; i < count; ++i) {
        shared_ptr<View> child = getVirtualChildAt(i);
        if (child->getVisibility() != View::GONE) {
            LinearLayoutParams *lp = static_cast<LinearLayoutParams*>(child->getLayoutParams());

            if (lp->height == LayoutParams::MATCH_PARENT) {
                // Temporarily force children to reuse their old measured width
                // FIXME: this may not be right for something like wrapping text?
                int oldWidth = lp->width;
                lp->width = child->getMeasuredWidth();

                // Remeasure with new dimensions
                measureChildWithMargins(child, widthMeasureSpec, 0, uniformMeasureSpec, 0);
                lp->width = oldWidth;
            }
        }
    }
}

void LinearLayout::forceUniformWidth(int count, int heightMeasureSpec) {
    // Pretend that the linear layout has an exact size.
    int uniformMeasureSpec = MeasureSpec::makeMeasureSpec(getMeasuredWidth(),
            MeasureSpec::EXACTLY);
    for (int i = 0; i< count; ++i) {
        shared_ptr<View> child = getVirtualChildAt(i);
        if (child->getVisibility() != View::GONE) {
            LinearLayoutParams *lp = static_cast<LinearLayoutParams*>(child->getLayoutParams());

            if (lp->width == LayoutParams::MATCH_PARENT) {
                // Temporarily force children to reuse their old measured height
                // FIXME: this may not be right for something like wrapping text?
                int oldHeight = lp->height;
                lp->height = child->getMeasuredHeight();

                // Remeasue with new dimensions
                measureChildWithMargins(child, uniformMeasureSpec, 0, heightMeasureSpec, 0);
                lp->height = oldHeight;
            }
        }
    }
}

LayoutParams *LinearLayout::generateLayoutParams(AttributeSet *attrs) {
    return new LinearLayoutParams(getContext(), attrs);
}

LayoutParams *LinearLayout::generateDefaultLayoutParams() {
    if (m_orientation == HORIZONTAL) {
        return new LayoutParams(LayoutParams::WRAP_CONTENT, LayoutParams::WRAP_CONTENT);
    } else if (m_orientation == VERTICAL) {
        return new LayoutParams(LayoutParams::MATCH_PARENT, LayoutParams::WRAP_CONTENT);
    }
    return NULL;
}

LayoutParams *LinearLayout::generateLayoutParams(LayoutParams *p) {
    return new LayoutParams(p);
}

bool LinearLayout::checkLayoutParams(LayoutParams *p) {
    return (dynamic_cast<LinearLayoutParams*>(p) != 0);
}

shared_ptr<View> LinearLayout::getVirtualChildAt(int index) {
	return getChildAt(index);
}

int LinearLayout::getVirtualChildCount() {
	return getChildCount();
}

void LinearLayout::layoutVertical(int left, int top, int right, int bottom) {
    const int paddingLeft = mPaddingLeft;

    int childTop;
    int childLeft;

    // Where right end of child should go
    const int width = right - left;
    int childRight = width - mPaddingRight;

    // Space available for child
    int childSpace = width - paddingLeft - mPaddingRight;

    const int count = getVirtualChildCount();

    const int majorGravity = m_gravity & Gravity::VERTICAL_GRAVITY_MASK;
    const int minorGravity = m_gravity & Gravity::RELATIVE_HORIZONTAL_GRAVITY_MASK;

    switch (majorGravity) {
        case Gravity::BOTTOM:
            // m_totalLength contains the padding already
            childTop = mPaddingTop + bottom - top - m_totalLength;
            break;

        // m_totalLength contains the padding already
        case Gravity::CENTER_VERTICAL:
            childTop = mPaddingTop + (bottom - top - m_totalLength) / 2;
            break;

        case Gravity::TOP:
        default:
            childTop = mPaddingTop;
            break;
    }

    for (int i = 0; i < count; i++) {
        shared_ptr<View> child = getVirtualChildAt(i);
        if (child == NULL) {
            childTop += measureNullChild(i);
        } else if (child->getVisibility() != View::GONE) {
            const int childWidth = child->getMeasuredWidth();
            const int childHeight = child->getMeasuredHeight();
            
            LinearLayoutParams *lp = static_cast<LinearLayoutParams*>(child->getLayoutParams());

            int gravity = lp->gravity;
            if (gravity < 0) {
                gravity = minorGravity;
            }
            const int layoutDirection = getLayoutDirection();
            const int absoluteGravity = Gravity::getAbsoluteGravity(gravity, layoutDirection);
            switch (absoluteGravity & Gravity::HORIZONTAL_GRAVITY_MASK) {
                case Gravity::CENTER_HORIZONTAL:
                    childLeft = paddingLeft + ((childSpace - childWidth) / 2)
                            + lp->leftMargin - lp->rightMargin;
                    break;

                case Gravity::RIGHT:
                    childLeft = childRight - childWidth - lp->rightMargin;
                    break;

                case Gravity::LEFT:
                default:
                    childLeft = paddingLeft + lp->leftMargin;
                    break;
            }

            /*if (hasDividerBeforeChildAt(i)) {
                childTop += mDividerHeight;
            }*/

            childTop += lp->topMargin;
            setChildFrame(child, childLeft, childTop + getLocationOffset(child),
                    childWidth, childHeight);
            childTop += childHeight + lp->bottomMargin + getNextLocationOffset(child);

            i += getChildrenSkipCount(child, i);
        }
    }
}

/**
 * Position the children during a layout pass if the orientation of this
 * LinearLayout is set to {@link #HORIZONTAL}.
 *
 * @see #getOrientation()
 * @see #setOrientation(int)
 * @see #onLayout(bool, int, int, int, int)
 * @param left
 * @param top
 * @param right
 * @param bottom
 */
void LinearLayout::layoutHorizontal(int left, int top, int right, int bottom) {
    const bool isLayoutRtl = false;
    const int paddingTop = mPaddingTop;

    int childTop;
    int childLeft;

    // Where bottom of child should go
    const int height = bottom - top;
    int childBottom = height - mPaddingBottom;

    // Space available for child
    int childSpace = height - paddingTop - mPaddingBottom;

    const int count = getVirtualChildCount();

    const int majorGravity = m_gravity & Gravity::RELATIVE_HORIZONTAL_GRAVITY_MASK;
    const int minorGravity = m_gravity & Gravity::VERTICAL_GRAVITY_MASK;

    const bool baselineAligned = m_baselineAligned;

    int *maxAscent = m_maxAscent;
    int *maxDescent = m_maxDescent;

    const int layoutDirection = getLayoutDirection();
    switch (Gravity::getAbsoluteGravity(majorGravity, layoutDirection)) {
        case Gravity::RIGHT:
            // m_totalLength contains the padding already
            childLeft = mPaddingLeft + right - left - m_totalLength;
            break;

        case Gravity::CENTER_HORIZONTAL:
            // m_totalLength contains the padding already
            childLeft = mPaddingLeft + (right - left - m_totalLength) / 2;
            break;

        case Gravity::LEFT:
        default:
            childLeft = mPaddingLeft;
            break;
    }

    int start = 0;
    int dir = 1;
    //In case of RTL, start drawing from the last child->
    if (isLayoutRtl) {
        start = count - 1;
        dir = -1;
    }

    for (int i = 0; i < count; i++) {
        int childIndex = start + dir * i;
        shared_ptr<View> child = getVirtualChildAt(childIndex);

        if (child == NULL) {
            childLeft += measureNullChild(childIndex);
        } else if (child->getVisibility() != View::GONE) {
            const int childWidth = child->getMeasuredWidth();
            const int childHeight = child->getMeasuredHeight();
            int childBaseline = -1;
            
            LinearLayoutParams *lp = static_cast<LinearLayoutParams*>(child->getLayoutParams());

            if (baselineAligned && lp->height != LayoutParams::MATCH_PARENT) {
                childBaseline = child->getBaseline();
            }

            int gravity = lp->gravity;
            if (gravity < 0) {
                gravity = minorGravity;
            }

            switch (gravity & Gravity::VERTICAL_GRAVITY_MASK) {
                case Gravity::TOP:
                    childTop = paddingTop + lp->topMargin;
                    if (childBaseline != -1) {
                        childTop += maxAscent[INDEX_TOP] - childBaseline;
                    }
                    break;

                case Gravity::CENTER_VERTICAL:
                    // Removed support for baseline alignment when layout_gravity or
                    // gravity == center_vertical. See bug #1038483.
                    // Keep the code around if we need to re-enable this feature
                    // if (childBaseline != -1) {
                    //     // Align baselines vertically only if the child is smaller than us
                    //     if (childSpace - childHeight > 0) {
                    //         childTop = paddingTop + (childSpace / 2) - childBaseline;
                    //     } else {
                    //         childTop = paddingTop + (childSpace - childHeight) / 2;
                    //     }
                    // } else {
                    childTop = paddingTop + ((childSpace - childHeight) / 2)
                            + lp->topMargin - lp->bottomMargin;
                    break;

                case Gravity::BOTTOM:
                    childTop = childBottom - childHeight - lp->bottomMargin;
                    if (childBaseline != -1) {
                        int descent = child->getMeasuredHeight() - childBaseline;
                        childTop -= (maxDescent[INDEX_BOTTOM] - descent);
                    }
                    break;
                default:
                    childTop = paddingTop;
                    break;
            }

            /*if (hasDividerBeforeChildAt(childIndex)) {
                childLeft += mDividerWidth;
            }*/

            childLeft += lp->leftMargin;
            setChildFrame(child, childLeft + getLocationOffset(child), childTop,
                    childWidth, childHeight);
            childLeft += childWidth + lp->rightMargin +
                    getNextLocationOffset(child);

            i += getChildrenSkipCount(child, childIndex);
        }
    }
}

void LinearLayout::measureChildBeforeLayout(shared_ptr<View> child, int childIndex,
                            int widthMeasureSpec, int totalWidth, int heightMeasureSpec,
                            int totalHeight) {
    measureChildWithMargins(child, widthMeasureSpec, totalWidth, heightMeasureSpec, totalHeight);
}

void LinearLayout::measureChildWithMargins(shared_ptr<View> child,
                            int parentWidthMeasureSpec, int widthUsed,
                            int parentHeightMeasureSpec, int heightUsed) {
    MarginLayoutParams *lp = static_cast<MarginLayoutParams*>(child->getLayoutParams());

    const int childWidthMeasureSpec = getChildMeasureSpec(parentWidthMeasureSpec, mPaddingLeft + mPaddingRight + lp->leftMargin + lp->rightMargin
             + widthUsed, lp->width);
    const int childHeightMeasureSpec = getChildMeasureSpec(parentHeightMeasureSpec, mPaddingTop + mPaddingBottom + lp->topMargin + lp->bottomMargin
             + heightUsed, lp->height);

    child->measure(childWidthMeasureSpec, childHeightMeasureSpec);
}


void LinearLayout::measureHorizontal(int widthMeasureSpec, int heightMeasureSpec) {
    m_totalLength = 0;
    int maxHeight = 0;
    int childState = 0;
    int alternativeMaxHeight = 0;
    int weightedMaxHeight = 0;
    bool allFillParent = true;
    float totalWeight = 0;

    const int count = getVirtualChildCount();

    const int widthMode = MeasureSpec::getMode(widthMeasureSpec);
    const int heightMode = MeasureSpec::getMode(heightMeasureSpec);

    bool matchHeight = false;

    if (!m_maxAscent || !m_maxDescent) {
        m_maxAscent = new int[VERTICAL_GRAVITY_COUNT];
        m_maxDescent = new int[VERTICAL_GRAVITY_COUNT];
    }

    int *maxAscent = m_maxAscent;
    int *maxDescent = m_maxDescent;

    maxAscent[0] = maxAscent[1] = maxAscent[2] = maxAscent[3] = -1;
    maxDescent[0] = maxDescent[1] = maxDescent[2] = maxDescent[3] = -1;

    const bool baselineAligned = m_baselineAligned;
    const bool useLargestChild = m_useLargestChild;

    const bool isExactly = widthMode == MeasureSpec::EXACTLY;

    int largestChildWidth = INT_MIN;

    // See how wide everyone is. Also remember max height.
    for (int i = 0; i < count; ++i) {
        shared_ptr<View> child = getVirtualChildAt(i);

        if (child == NULL) {
            m_totalLength += measureNullChild(i);
            continue;
        }

        if (child->getVisibility() == View::GONE) {
            i += getChildrenSkipCount(child, i);
            continue;
        }

        /*if (hasDividerBeforeChildAt(i)) {
            m_totalLength += mDividerWidth;
        }*/

        LinearLayoutParams *lp = static_cast<LinearLayoutParams*>(child->getLayoutParams());

        totalWeight += lp->weight;

        if (widthMode == MeasureSpec::EXACTLY && lp->width == 0 && lp->weight > 0) {
            // Optimization: don't bother measuring children who are going to use
            // leftover space. These views will get measured again down below if
            // there is any leftover space.
            if (isExactly) {
                m_totalLength += lp->leftMargin + lp->rightMargin;
            } else {
                const int totalLength = m_totalLength;
                m_totalLength = std::max(totalLength, totalLength +
                        lp->leftMargin + lp->rightMargin);
            }

            // Baseline alignment requires to measure widgets to obtain the
            // baseline offset (in particular for TextViews). The following
            // defeats the optimization mentioned above. Allow the child to
            // use as much space as it wants because we can shrink things
            // later (and re-measure).
            if (baselineAligned) {
                const int freeSpec = MeasureSpec::makeMeasureSpec(0, MeasureSpec::UNSPECIFIED);
                child->measure(freeSpec, freeSpec);
            }
        } else {
            int oldWidth = INT_MIN;

            if (lp->width == 0 && lp->weight > 0) {
                // widthMode is either UNSPECIFIED or AT_MOST, and this
                // child
                // wanted to stretch to fill available space. Translate that to
                // WRAP_CONTENT so that it does not end up with a width of 0
                oldWidth = 0;
                lp->width = LayoutParams::WRAP_CONTENT;
            }

            // Determine how big this child would like to be. If this or
            // previous children have given a weight, then we allow it to
            // use all available space (and we will shrink things later
            // if needed).
            measureChildBeforeLayout(child, i, widthMeasureSpec,
                    totalWeight == 0 ? m_totalLength : 0,
                    heightMeasureSpec, 0);

            if (oldWidth != INT_MIN) {
                lp->width = oldWidth;
            }

            const int childWidth = child->getMeasuredWidth();
            if (isExactly) {
                m_totalLength += childWidth + lp->leftMargin + lp->rightMargin +
                        getNextLocationOffset(child);
            } else {
                const int totalLength = m_totalLength;
                m_totalLength = std::max(totalLength, totalLength + childWidth + lp->leftMargin +
                        lp->rightMargin + getNextLocationOffset(child));
            }

            if (useLargestChild) {
                largestChildWidth = std::max(childWidth, largestChildWidth);
            }
        }

        bool matchHeightLocally = false;
        if (heightMode != MeasureSpec::EXACTLY && lp->height == LayoutParams::MATCH_PARENT) {
            // The height of the linear layout will scale, and at least one
            // child said it wanted to match our height. Set a flag indicating that
            // we need to remeasure at least that view when we know our height.
            matchHeight = true;
            matchHeightLocally = true;
        }

        const int margin = lp->topMargin + lp->bottomMargin;
        const int childHeight = child->getMeasuredHeight() + margin;
        childState = combineMeasuredStates(childState, child->getMeasuredState());

        if (baselineAligned) {
            const int childBaseline = child->getBaseline();
            if (childBaseline != -1) {
                // Translates the child's vertical gravity into an index
                // in the range 0..VERTICAL_GRAVITY_COUNT
                const int gravity = (lp->gravity < 0 ? m_gravity : lp->gravity)
                        & Gravity::VERTICAL_GRAVITY_MASK;
                const int index = ((gravity >> Gravity::AXIS_Y_SHIFT)
                        & ~Gravity::AXIS_SPECIFIED) >> 1;

                maxAscent[index] = std::max(maxAscent[index], childBaseline);
                maxDescent[index] = std::max(maxDescent[index], childHeight - childBaseline);
            }
        }

        maxHeight = std::max(maxHeight, childHeight);

        allFillParent = allFillParent && lp->height == LayoutParams::MATCH_PARENT;
        if (lp->weight > 0) {
            /*
             * Heights of weighted Views are bogus if we end up
             * re-measuring, so keep them separate.
             */
            weightedMaxHeight = std::max(weightedMaxHeight,
                    matchHeightLocally ? margin : childHeight);
        } else {
            alternativeMaxHeight = std::max(alternativeMaxHeight,
                    matchHeightLocally ? margin : childHeight);
        }

        i += getChildrenSkipCount(child, i);
    }

    /*if (m_totalLength > 0 && hasDividerBeforeChildAt(count)) {
        m_totalLength += mDividerWidth;
    }*/

    // Check m_maxAscent[INDEX_TOP] first because it maps to Gravity::TOP,
    // the most common case
    if (maxAscent[INDEX_TOP] != -1 ||
            maxAscent[INDEX_CENTER_VERTICAL] != -1 ||
            maxAscent[INDEX_BOTTOM] != -1 ||
            maxAscent[INDEX_FILL] != -1) {
        const int ascent = std::max(maxAscent[INDEX_FILL],
                std::max(maxAscent[INDEX_CENTER_VERTICAL],
                        std::max(maxAscent[INDEX_TOP], maxAscent[INDEX_BOTTOM])));
        const int descent = std::max(maxDescent[INDEX_FILL],
                std::max(maxDescent[INDEX_CENTER_VERTICAL],
                        std::max(maxDescent[INDEX_TOP], maxDescent[INDEX_BOTTOM])));
        maxHeight = std::max(maxHeight, ascent + descent);
    }

    if (useLargestChild &&
            (widthMode == MeasureSpec::AT_MOST || widthMode == MeasureSpec::UNSPECIFIED)) {
        m_totalLength = 0;

        for (int i = 0; i < count; ++i) {
            shared_ptr<View> child = getVirtualChildAt(i);

            if (child == NULL) {
                m_totalLength += measureNullChild(i);
                continue;
            }

            if (child->getVisibility() == View::GONE) {
                i += getChildrenSkipCount(child, i);
                continue;
            }

            LinearLayoutParams *lp = static_cast<LinearLayoutParams*>(child->getLayoutParams());
            if (isExactly) {
                m_totalLength += largestChildWidth + lp->leftMargin + lp->rightMargin +
                        getNextLocationOffset(child);
            } else {
                const int totalLength = m_totalLength;
                m_totalLength = std::max(totalLength, totalLength + largestChildWidth +
                        lp->leftMargin + lp->rightMargin + getNextLocationOffset(child));
            }
        }
    }

    // Add in our padding
    m_totalLength += mPaddingLeft + mPaddingRight;

    int widthSize = m_totalLength;

    // Check against our minimum width
    widthSize = std::max(widthSize, getSuggestedMinimumWidth());

    // Reconcile our calculated size with the widthMeasureSpec
    int widthSizeAndState = resolveSizeAndState(widthSize, widthMeasureSpec, 0);
    widthSize = widthSizeAndState & MEASURED_SIZE_MASK;

    // Either expand children with weight to take up available space or
    // shrink them if they extend beyond our current bounds
    int delta = widthSize - m_totalLength;
    if (delta != 0 && totalWeight > 0.0f) {
        float weightSum = m_weightSum > 0.0f ? m_weightSum : totalWeight;

        maxAscent[0] = maxAscent[1] = maxAscent[2] = maxAscent[3] = -1;
        maxDescent[0] = maxDescent[1] = maxDescent[2] = maxDescent[3] = -1;
        maxHeight = -1;

        m_totalLength = 0;

        for (int i = 0; i < count; ++i) {
            shared_ptr<View> child = getVirtualChildAt(i);

            if (child == NULL || child->getVisibility() == View::GONE) {
                continue;
            }

            LinearLayoutParams *lp = static_cast<LinearLayoutParams*>(child->getLayoutParams());

            float childExtra = lp->weight;
            if (childExtra > 0) {
                // Child said it could absorb extra space -- give him his share
                int share = (int) (childExtra * delta / weightSum);
                weightSum -= childExtra;
                delta -= share;

                const int childHeightMeasureSpec = getChildMeasureSpec(
                        heightMeasureSpec,
                        mPaddingTop + mPaddingBottom + lp->topMargin + lp->bottomMargin,
                        lp->height);

                // TODO: Use a field like lp->isMeasured to figure out if this
                // child has been previously measured
                if ((lp->width != 0) || (widthMode != MeasureSpec::EXACTLY)) {
                    // child was measured once already above ... base new measurement
                    // on stored values
                    int childWidth = child->getMeasuredWidth() + share;
                    if (childWidth < 0) {
                        childWidth = 0;
                    }

                    child->measure(
                            MeasureSpec::makeMeasureSpec(childWidth, MeasureSpec::EXACTLY),
                            childHeightMeasureSpec);
                } else {
                    // child was skipped in the loop above. Measure for this first time here
                    child->measure(MeasureSpec::makeMeasureSpec(
                            share > 0 ? share : 0, MeasureSpec::EXACTLY),
                            childHeightMeasureSpec);
                }

                // Child may now not fit in horizontal dimension.
                childState = combineMeasuredStates(childState,
                        child->getMeasuredState() & MEASURED_STATE_MASK);
            }

            if (isExactly) {
                m_totalLength += child->getMeasuredWidth() + lp->leftMargin + lp->rightMargin +
                        getNextLocationOffset(child);
            } else {
                const int totalLength = m_totalLength;
                m_totalLength = std::max(totalLength, totalLength + child->getMeasuredWidth() +
                        lp->leftMargin + lp->rightMargin + getNextLocationOffset(child));
            }

            bool matchHeightLocally = heightMode != MeasureSpec::EXACTLY &&
                    lp->height == LayoutParams::MATCH_PARENT;

            const int margin = lp->topMargin + lp->bottomMargin;
            int childHeight = child->getMeasuredHeight() + margin;
            maxHeight = std::max(maxHeight, childHeight);
            alternativeMaxHeight = std::max(alternativeMaxHeight,
                    matchHeightLocally ? margin : childHeight);

            allFillParent = allFillParent && lp->height == LayoutParams::MATCH_PARENT;

            if (baselineAligned) {
                const int childBaseline = child->getBaseline();
                if (childBaseline != -1) {
                    // Translates the child's vertical gravity into an index in the range 0..2
                    const int gravity = (lp->gravity < 0 ? m_gravity : lp->gravity)
                            & Gravity::VERTICAL_GRAVITY_MASK;
                    const int index = ((gravity >> Gravity::AXIS_Y_SHIFT)
                            & ~Gravity::AXIS_SPECIFIED) >> 1;

                    maxAscent[index] = std::max(maxAscent[index], childBaseline);
                    maxDescent[index] = std::max(maxDescent[index],
                            childHeight - childBaseline);
                }
            }
        }

        // Add in our padding
        m_totalLength += mPaddingLeft + mPaddingRight;
        // TODO: Should we update widthSize with the new total length?

        // Check m_maxAscent[INDEX_TOP] first because it maps to Gravity::TOP,
        // the most common case
        if (maxAscent[INDEX_TOP] != -1 ||
                maxAscent[INDEX_CENTER_VERTICAL] != -1 ||
                maxAscent[INDEX_BOTTOM] != -1 ||
                maxAscent[INDEX_FILL] != -1) {
            const int ascent = std::max(maxAscent[INDEX_FILL],
                    std::max(maxAscent[INDEX_CENTER_VERTICAL],
                            std::max(maxAscent[INDEX_TOP], maxAscent[INDEX_BOTTOM])));
            const int descent = std::max(maxDescent[INDEX_FILL],
                    std::max(maxDescent[INDEX_CENTER_VERTICAL],
                            std::max(maxDescent[INDEX_TOP], maxDescent[INDEX_BOTTOM])));
            maxHeight = std::max(maxHeight, ascent + descent);
        }
    } else {
        alternativeMaxHeight = std::max(alternativeMaxHeight, weightedMaxHeight);

        // We have no limit, so make all weighted views as wide as the largest child->
        // Children will have already been measured once.
        if (useLargestChild && widthMode != MeasureSpec::EXACTLY) {
            for (int i = 0; i < count; i++) {
                shared_ptr<View> child = getVirtualChildAt(i);

                if (child == NULL || child->getVisibility() == View::GONE) {
                    continue;
                }

                LinearLayoutParams *lp = static_cast<LinearLayoutParams*>(child->getLayoutParams());

                float childExtra = lp->weight;
                if (childExtra > 0) {
                    child->measure(
                            MeasureSpec::makeMeasureSpec(largestChildWidth, MeasureSpec::EXACTLY),
                            MeasureSpec::makeMeasureSpec(child->getMeasuredHeight(),
                                    MeasureSpec::EXACTLY));
                }
            }
        }
    }

    if (!allFillParent && heightMode != MeasureSpec::EXACTLY) {
        maxHeight = alternativeMaxHeight;
    }

    maxHeight += mPaddingTop + mPaddingBottom;

    // Check against our minimum height
    maxHeight = std::max(maxHeight, getSuggestedMinimumHeight());

    setMeasuredDimension(widthSizeAndState | (childState&MEASURED_STATE_MASK),
            resolveSizeAndState(maxHeight, heightMeasureSpec,
                    (childState<<MEASURED_HEIGHT_STATE_SHIFT)));

    if (matchHeight) {
        forceUniformHeight(count, widthMeasureSpec);
    }
}

void LinearLayout::measureVertical(int widthMeasureSpec, int heightMeasureSpec) {
	m_totalLength = 0;
	int maxWidth = 0;
	int childState = 0;
	int alternativeMaxWidth = 0;
	int weightedMaxWidth = 0;
	bool allFillParent = true;
	float totalWeight = 0;

	const int count = getVirtualChildCount();

	const int widthMode = MeasureSpec::getMode(widthMeasureSpec);
	const int heightMode = MeasureSpec::getMode(heightMeasureSpec);

	bool matchWidth = false;

	const int baselineChildIndex = m_baselineAlignedChildIndex;
	const bool useLargestChild = m_useLargestChild;

	int largestChildHeight = INT_MIN;

	// See how tall everyone is. Also remember max width.
	for (int i = 0; i < count; ++i) {
		shared_ptr<View> child = getVirtualChildAt(i);

		if (child == NULL) {
			m_totalLength += measureNullChild(i);
			continue;
		}

		if (child->getVisibility() == View::GONE) {
			i += getChildrenSkipCount(child, i);
			continue;
		}

		// if (hasDividerBeforeChildAt(i)) {
// 			m_totalLength += mDividerHeight;
// 		}

		LinearLayoutParams *lp = static_cast<LinearLayoutParams*>(child->getLayoutParams());

		totalWeight += lp->weight;

		if (heightMode == MeasureSpec::EXACTLY && lp->height == 0 && lp->weight > 0) {
			// Optimization: don't bother measuring children who are going to use
			// leftover space. These views will get measured again down below if
			// there is any leftover space.
			const int totalLength = m_totalLength;
			m_totalLength = std::max(totalLength, totalLength + lp->topMargin + lp->bottomMargin);
		} else {
			int oldHeight = INT_MIN;

			if (lp->height == 0 && lp->weight > 0) {
				// heightMode is either UNSPECIFIED or AT_MOST, and this
				// child wanted to stretch to fill available space.
				// Translate that to WRAP_CONTENT so that it does not end up
				// with a height of 0
				oldHeight = 0;
				lp->height = LayoutParams::WRAP_CONTENT;
			}

			// Determine how big this child would like to be. If this or
			// previous children have given a weight, then we allow it to
			// use all available space (and we will shrink things later
			// if needed).
			measureChildBeforeLayout(
					child, i, widthMeasureSpec, 0, heightMeasureSpec,
					totalWeight == 0 ? m_totalLength : 0);

			if (oldHeight != INT_MIN) {
				lp->height = oldHeight;
			}

			const int childHeight = child->getMeasuredHeight();
			const int totalLength = m_totalLength;
			m_totalLength = std::max(totalLength, totalLength + childHeight + lp->topMargin +
					lp->bottomMargin + getNextLocationOffset(child));

			if (useLargestChild) {
				largestChildHeight = std::max(childHeight, largestChildHeight);
			}
		}

		/**
		 * If applicable, compute the additional offset to the child's baseline
		 * we'll need later when asked {@link #getBaseline}.
		 */
		if ((baselineChildIndex >= 0) && (baselineChildIndex == i + 1)) {
			m_baselineChildTop = m_totalLength;
		}

		// if we are trying to use a child index for our baseline, the above
		// book keeping only works if there are no children above it with
		// weight.  fail fast to aid the developer.
		if (i < baselineChildIndex && lp->weight > 0) {
			/* throw new RuntimeException("A child of LinearLayout with index "
					+ "less than m_baselineAlignedChildIndex has weight > 0, which "
					+ "won't work.  Either remove the weight, or don't set "
					+ "m_baselineAlignedChildIndex."); */
		}

		bool matchWidthLocally = false;
		if (widthMode != MeasureSpec::EXACTLY && lp->width == LayoutParams::MATCH_PARENT) {
			// The width of the linear layout will scale, and at least one
			// child said it wanted to match our width. Set a flag
			// indicating that we need to remeasure at least that view when
			// we know our width.
			matchWidth = true;
			matchWidthLocally = true;
		}

		const int margin = lp->leftMargin + lp->rightMargin;
		const int measuredWidth = child->getMeasuredWidth() + margin;
		maxWidth = std::max(maxWidth, measuredWidth);
		childState = combineMeasuredStates(childState, child->getMeasuredState());

		allFillParent = allFillParent && lp->width == LayoutParams::MATCH_PARENT;
		if (lp->weight > 0) {
			/*
			 * Widths of weighted Views are bogus if we end up
			 * remeasuring, so keep them separate.
			 */
			weightedMaxWidth = std::max(weightedMaxWidth,
					matchWidthLocally ? margin : measuredWidth);
		} else {
			alternativeMaxWidth = std::max(alternativeMaxWidth,
					matchWidthLocally ? margin : measuredWidth);
		}

		i += getChildrenSkipCount(child, i);
	}

	if (useLargestChild &&
			(heightMode == MeasureSpec::AT_MOST || heightMode == MeasureSpec::UNSPECIFIED)) {
		m_totalLength = 0;

		for (int i = 0; i < count; ++i) {
			shared_ptr<View> child = getVirtualChildAt(i);

			if (child == NULL) {
				m_totalLength += measureNullChild(i);
				continue;
			}

			if (child->getVisibility() == View::GONE) {
				i += getChildrenSkipCount(child, i);
				continue;
			}

			LinearLayoutParams *lp = static_cast<LinearLayoutParams*>(child->getLayoutParams());
			// Account for negative margins
			const int totalLength = m_totalLength;
			m_totalLength = std::max(totalLength, totalLength + largestChildHeight +
					lp->topMargin + lp->bottomMargin + getNextLocationOffset(child));
		}
	}

	// Add in our padding
	m_totalLength += mPaddingTop + mPaddingBottom;

	int heightSize = m_totalLength;

	// Check against our minimum height
	heightSize = std::max(heightSize, getSuggestedMinimumHeight());

	// Reconcile our calculated size with the heightMeasureSpec
	int heightSizeAndState = resolveSizeAndState(heightSize, heightMeasureSpec, 0);
	heightSize = heightSizeAndState & MEASURED_SIZE_MASK;

	// Either expand children with weight to take up available space or
	// shrink them if they extend beyond our current bounds
	int delta = heightSize - m_totalLength;
	if (delta != 0 && totalWeight > 0.0f) {
		float weightSum = m_weightSum > 0.0f ? m_weightSum : totalWeight;

		m_totalLength = 0;

		for (int i = 0; i < count; ++i) {
			shared_ptr<View> child = getVirtualChildAt(i);

			if (child != NULL && child->getVisibility() == View::GONE) {
				continue;
			}

			LinearLayoutParams *lp = static_cast<LinearLayoutParams*>(child->getLayoutParams());

			float childExtra = lp->weight;
			if (childExtra > 0) {
				// Child said it could absorb extra space -- give him his share
				int share = (int) (childExtra * delta / weightSum);
				weightSum -= childExtra;
				delta -= share;

				const int childWidthMeasureSpec = getChildMeasureSpec(widthMeasureSpec,
						mPaddingLeft + mPaddingRight +
								lp->leftMargin + lp->rightMargin, lp->width);

				// TODO: Use a field like lp->isMeasured to figure out if this
				// child has been previously measured
				if ((lp->height != 0) || (heightMode != MeasureSpec::EXACTLY)) {
					// child was measured once already above...
					// base new measurement on stored values
					int childHeight = child->getMeasuredHeight() + share;
					if (childHeight < 0) {
						childHeight = 0;
					}

					child->measure(childWidthMeasureSpec,
							MeasureSpec::makeMeasureSpec(childHeight, MeasureSpec::EXACTLY));
				} else {
					// child was skipped in the loop above.
					// Measure for this first time here
					child->measure(childWidthMeasureSpec,
							MeasureSpec::makeMeasureSpec(share > 0 ? share : 0,
									MeasureSpec::EXACTLY));
				}

				// Child may now not fit in vertical dimension.
				childState = combineMeasuredStates(childState, child->getMeasuredState()
						& (MEASURED_STATE_MASK>>MEASURED_HEIGHT_STATE_SHIFT));
			}

			const int margin =  lp->leftMargin + lp->rightMargin;
			const int measuredWidth = child->getMeasuredWidth() + margin;
			maxWidth = std::max(maxWidth, measuredWidth);

			bool matchWidthLocally = widthMode != MeasureSpec::EXACTLY &&
					lp->width == LayoutParams::MATCH_PARENT;

			alternativeMaxWidth = std::max(alternativeMaxWidth,
					matchWidthLocally ? margin : measuredWidth);

			allFillParent = allFillParent && lp->width == LayoutParams::MATCH_PARENT;

			const int totalLength = m_totalLength;
			m_totalLength = std::max(totalLength, totalLength + child->getMeasuredHeight() +
					lp->topMargin + lp->bottomMargin + getNextLocationOffset(child));
		}

		// Add in our padding
		m_totalLength += mPaddingTop + mPaddingBottom;
		// TODO: Should we recompute the heightSpec based on the new total length?
	} else {
		alternativeMaxWidth = std::max(alternativeMaxWidth,
				weightedMaxWidth);


		// We have no limit, so make all weighted views as tall as the largest child->
		// Children will have already been measured once.
		if (useLargestChild && heightMode != MeasureSpec::EXACTLY) {
			for (int i = 0; i < count; i++) {
				shared_ptr<View> child = getVirtualChildAt(i);

				if (child == NULL || child->getVisibility() == View::GONE) {
					continue;
				}

				LinearLayoutParams *lp = static_cast<LinearLayoutParams*>(child->getLayoutParams());

				float childExtra = lp->weight;
				if (childExtra > 0) {
					child->measure(
							MeasureSpec::makeMeasureSpec(child->getMeasuredWidth(),
									MeasureSpec::EXACTLY),
							MeasureSpec::makeMeasureSpec(largestChildHeight,
									MeasureSpec::EXACTLY));
				}
			}
		}
	}

	if (!allFillParent && widthMode != MeasureSpec::EXACTLY) {
		maxWidth = alternativeMaxWidth;
	}

	maxWidth += mPaddingLeft + mPaddingRight;

	// Check against our minimum width
	maxWidth = std::max(maxWidth, getSuggestedMinimumWidth());

	setMeasuredDimension(resolveSizeAndState(maxWidth, widthMeasureSpec, childState),
			heightSizeAndState);

	if (matchWidth) {
		forceUniformWidth(count, heightMeasureSpec);
	}
}

void LinearLayout::onLayout(bool changed, int l, int t, int r, int b) {

    CCLOG("onLayout(%s) %d, %d, %d, %d", m_id.c_str(), l, t, r, b);
    if (m_orientation == VERTICAL) {
        layoutVertical(l, t, r, b);
    } else {
        layoutHorizontal(l, t, r, b);
    }
}

void LinearLayout::onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
    
    CCLOG("onMeasure(%s) Children: %d", m_id.c_str(), getChildCount());

	if (m_orientation == VERTICAL) {
		measureVertical(widthMeasureSpec, heightMeasureSpec);
	} else {
		measureHorizontal(widthMeasureSpec, heightMeasureSpec);
	}
}

void LinearLayout::setChildFrame(shared_ptr<View> child, int left, int top, int width, int height) {
    child->layout(left, top, left + width, top + height);
}

void LinearLayout::setOrientation(int orientation) {
	if (m_orientation != orientation) {
		m_orientation = orientation;
		requestLayout();
	}
}

ANDROID_END