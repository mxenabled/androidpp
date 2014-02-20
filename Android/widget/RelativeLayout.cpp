//
//  RelativeLayout.cpp
//  cocos2dx
//
//  Created by Saul Howard on 11/8/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#include "RelativeLayout.h"

#include "Android/graphics/BitmapFactory.h"
#include "Android/graphics/Canvas.h"
#include "Android/graphics/Paint.h"

#include <algorithm>

#include <SkImageDecoder.h>

#include "cocos2d.h"

ANDROID_BEGIN

vector<RelativeLayout::Node*> RelativeLayout::Node::sPool(100);

const int RelativeLayout::RULES_VERTICAL[] = {
    RelativeLayoutParams::ABOVE, RelativeLayoutParams::BELOW, RelativeLayoutParams::ALIGN_BASELINE, RelativeLayoutParams::ALIGN_TOP, RelativeLayoutParams::ALIGN_BOTTOM
};

const int RelativeLayout::RULES_HORIZONTAL[] = {
    RelativeLayoutParams::LEFT_OF, RelativeLayoutParams::RIGHT_OF, RelativeLayoutParams::ALIGN_LEFT, RelativeLayoutParams::ALIGN_RIGHT, RelativeLayoutParams::START_OF, RelativeLayoutParams::END_OF, RelativeLayoutParams::ALIGN_START, RelativeLayoutParams::ALIGN_END
};

RelativeLayout::RelativeLayout(Context *context) : ViewGroup(context) {
}

RelativeLayout::RelativeLayout(Context *context, AttributeSet *attrs) : ViewGroup(context, attrs) {
    initFromAttributes(context, attrs);
}

void RelativeLayout::initFromAttributes(Context *context, AttributeSet *attrs) {
    
    m_ignoreGravity = View::NO_ID;
    
    attrs->loadAttributes();
    
    for (int i = 0; i < attrs->getAttributeCount(); i++) {
        
        const char *name = attrs->getAttributeName(i);
        
		if (strcmp(R::styleable::gravity, name) == 0) {
            string gravity(attrs->getAttributeValue(name));
            m_gravity = Gravity::getGravity(gravity, m_gravity);
		}
	}
}

bool RelativeLayout::shouldDelayChildPressedState() {
    return false;
}

/**
 * Defines which View is ignored when the gravity is applied. This setting has no
 * effect if the gravity is <code>Gravity::START | Gravity::TOP</code>.
 *
 * @param viewId The id of the View to be ignored by gravity, or 0 if no View
 *        should be ignored.
 *
 * @see #setGravity(int)
 *
 * @attr ref android.R.styleable#RelativeLayout_ignoreGravity
 */
void RelativeLayout::setIgnoreGravity(string viewId) {
    m_ignoreGravity = viewId;
}

/**
 * Describes how the child views are positioned.
 *
 * @return the Gravity::
 *
 * @see #setGravity(int)
 * @see android.view.Gravity
 *
 * @attr ref android.R.styleable#RelativeLayout_gravity
 */
int RelativeLayout::getGravity() {
    return m_gravity;
}

/**
 * Describes how the child views are positioned. Defaults to
 * <code>Gravity::START | Gravity::TOP</code>.
 *
 * <p>Note that since RelativeLayout considers the positioning of each child
 * relative to one another to be significant, setting gravity will affect
 * the positioning of all children as a single unit within the parent.
 * This happens after children have been relatively positioned.</p>
 *
 * @param gravity See {@link android.view.Gravity}
 *
 * @see #setHorizontalGravity(int)
 * @see #setVerticalGravity(int)
 *
 * @attr ref android.R.styleable#RelativeLayout_gravity
 */
void RelativeLayout::setGravity(int gravity) {
    if (m_gravity != gravity) {
        if ((gravity & Gravity::RELATIVE_HORIZONTAL_GRAVITY_MASK) == 0) {
            gravity |= Gravity::START;
        }
        
        if ((gravity & Gravity::VERTICAL_GRAVITY_MASK) == 0) {
            gravity |= Gravity::TOP;
        }
        
        m_gravity = gravity;
        requestLayout();
    }
}

void RelativeLayout::setHorizontalGravity(int horizontalGravity) {
    const int gravity = horizontalGravity & Gravity::RELATIVE_HORIZONTAL_GRAVITY_MASK;
    if ((m_gravity & Gravity::RELATIVE_HORIZONTAL_GRAVITY_MASK) != gravity) {
        m_gravity = (m_gravity & ~Gravity::RELATIVE_HORIZONTAL_GRAVITY_MASK) | gravity;
        requestLayout();
    }
}

void RelativeLayout::setVerticalGravity(int verticalGravity) {
    const int gravity = verticalGravity & Gravity::VERTICAL_GRAVITY_MASK;
    if ((m_gravity & Gravity::VERTICAL_GRAVITY_MASK) != gravity) {
        m_gravity = (m_gravity & ~Gravity::VERTICAL_GRAVITY_MASK) | gravity;
        requestLayout();
    }
}

int RelativeLayout::getBaseline() {
    return m_baselineView != NULL ? m_baselineView->getBaseline() : View::getBaseline();
}

void RelativeLayout::requestLayout() {
    ViewGroup::requestLayout();
    m_dirtyHierarchy = true;
}

void RelativeLayout::sortChildren() {
    int count = getChildCount();
    if (m_sortedVerticalChildren.size() != count) m_sortedVerticalChildren.resize(count);
    if (m_sortedHorizontalChildren.size() != count) m_sortedHorizontalChildren.resize(count);
    
    m_graph.clear();
    
    for (int i = 0; i < count; i++) {
        shared_ptr<View> child = getChildAt(i);
        m_graph.add(child);
    }
    
    m_graph.getSortedViews(m_sortedVerticalChildren, RULES_VERTICAL, 5);
    m_graph.getSortedViews(m_sortedHorizontalChildren, RULES_HORIZONTAL, 8);
}

// TODO: we need to find another way to implement RelativeLayout
// This implementation cannot handle every case
void RelativeLayout::onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
    if (m_dirtyHierarchy) {
        m_dirtyHierarchy = false;
        sortChildren();
    }
    
    int myWidth = -1;
    int myHeight = -1;
    
    int width = 0;
    int height = 0;
    
    const int widthMode = MeasureSpec::getMode(widthMeasureSpec);
    const int heightMode = MeasureSpec::getMode(heightMeasureSpec);
    const int widthSize = MeasureSpec::getSize(widthMeasureSpec);
    const int heightSize = MeasureSpec::getSize(heightMeasureSpec);
    
    // Record our dimensions if they are known;
    if (widthMode != MeasureSpec::UNSPECIFIED) {
        myWidth = widthSize;
    }
    
    if (heightMode != MeasureSpec::UNSPECIFIED) {
        myHeight = heightSize;
    }
    
    if (widthMode == MeasureSpec::EXACTLY) {
        width = myWidth;
    }
    
    if (heightMode == MeasureSpec::EXACTLY) {
        height = myHeight;
    }
    
    m_hasBaselineAlignedChild = false;
    
    shared_ptr<View> ignore;
    int gravity = m_gravity & Gravity::RELATIVE_HORIZONTAL_GRAVITY_MASK;
    const bool horizontalGravity = gravity != Gravity::START && gravity != 0;
    gravity = m_gravity & Gravity::VERTICAL_GRAVITY_MASK;
    const bool verticalGravity = gravity != Gravity::TOP && gravity != 0;
    
    int left = INT_MAX;
    int top = INT_MAX;
    int right = INT_MIN;
    int bottom = INT_MIN;
    
    bool offsetHorizontalAxis = false;
    bool offsetVerticalAxis = false;
    
    if ((horizontalGravity || verticalGravity) && m_ignoreGravity != View::NO_ID) {
        ignore = findSharedViewById(m_ignoreGravity);
    }
    
    const bool isWrapContentWidth = widthMode != MeasureSpec::EXACTLY;
    const bool isWrapContentHeight = heightMode != MeasureSpec::EXACTLY;
    
    // We need to know our size for doing the correct computation of children positioning in RTL
    // mode but there is no practical way to get it instead of running the code RelativeLayoutParams::BELOW.
    // So, instead of running the code twice, we just set the width to a "default display width"
    // before the computation and then, as a last pass, we will update their real position with
    // an offset equals to "DEFAULT_WIDTH - width".
    const int layoutDirection = getLayoutDirection();
    if (isLayoutRtl() && myWidth == -1) {
        myWidth = DEFAULT_WIDTH;
    }
    
    const vector<shared_ptr<View>> &views = m_sortedHorizontalChildren;
    int count = views.size();
    
    typedef vector<shared_ptr<View>>::const_iterator iter;
    for (iter it = views.begin(); it != views.end(); ++it) {
        shared_ptr<View> child = *it;
        if (child != NULL && child->getVisibility() != GONE) {
            RelativeLayoutParams *params = (RelativeLayoutParams*) child->getLayoutParams();
            vector<int64_t> rules = params->getRules(layoutDirection);
            
            applyHorizontalSizeRules(params, myWidth, rules);
            measureChildHorizontal(child, params, myWidth, myHeight);
            
            if (positionChildHorizontal(child, params, myWidth, isWrapContentWidth)) {
                offsetHorizontalAxis = true;
            }
        }
    }
    
    const vector<shared_ptr<View>> &views1 = m_sortedVerticalChildren;
    count = views1.size();
    
    typedef vector<shared_ptr<View>>::const_iterator iter;
    for (iter it = views1.begin(); it != views1.end(); ++it) {
        shared_ptr<View> child = *it;
        if (child != NULL && child->getVisibility() != GONE) {
            RelativeLayoutParams *params = (RelativeLayoutParams*) child->getLayoutParams();
            
            applyVerticalSizeRules(params, myHeight);
            measureChild(child, params, myWidth, myHeight);
            if (positionChildVertical(child, params, myHeight, isWrapContentHeight)) {
                offsetVerticalAxis = true;
            }
            
            if (isWrapContentWidth) {
                if (isLayoutRtl()) {
                    width = max(width, myWidth - params->mLeft - params->leftMargin);
                } else {
                    width = max(width, params->mRight + params->rightMargin);
                }
            }
            
            if (isWrapContentHeight) {
                height = max(height, params->mBottom + params->bottomMargin);
            }
            
            if (child != ignore || verticalGravity) {
                left = min(left, params->mLeft - params->leftMargin);
                top = min(top, params->mTop - params->topMargin);
            }
            
            if (child != ignore || horizontalGravity) {
                right = max(right, params->mRight + params->rightMargin);
                bottom = max(bottom, params->mBottom + params->bottomMargin);
            }
        }
    }
    
    if (m_hasBaselineAlignedChild) {
        for (int i = 0; i < count; i++) {
            shared_ptr<View> child = getChildAt(i);
            if (child->getVisibility() != GONE) {
                RelativeLayoutParams *params = (RelativeLayoutParams*) child->getLayoutParams();
                alignBaseline(child, params);
                
                if (child != ignore || verticalGravity) {
                    left = min(left, params->mLeft - params->leftMargin);
                    top = min(top, params->mTop - params->topMargin);
                }
                
                if (child != ignore || horizontalGravity) {
                    right = max(right, params->mRight + params->rightMargin);
                    bottom = max(bottom, params->mBottom + params->bottomMargin);
                }
            }
        }
    }
    
    if (isWrapContentWidth) {
        // Width already has left padding in it since it was calculated by looking at
        // the right of each child view
        width += mPaddingRight;
        
        if (mLayoutParams != NULL && mLayoutParams->width >= 0) {
            width = max(width, mLayoutParams->width);
        }
        
        width = max(width, getSuggestedMinimumWidth());
        width = resolveSize(width, widthMeasureSpec);
        
        if (offsetHorizontalAxis) {
            for (int i = 0; i < count; i++) {
                shared_ptr<View> child = getChildAt(i);
                if (child->getVisibility() != GONE) {
                    RelativeLayoutParams *params = static_cast<RelativeLayoutParams*>(child->getLayoutParams());
                    vector<int64_t> rules = params->getRules(layoutDirection);
                    if (rules[RelativeLayoutParams::CENTER_IN_PARENT] != 0 || rules[RelativeLayoutParams::CENTER_HORIZONTAL] != 0) {
                        centerHorizontal(child, params, width);
                    } else if (rules[RelativeLayoutParams::ALIGN_PARENT_RIGHT] != 0) {
                        const int childWidth = child->getMeasuredWidth();
                        params->mLeft = width - mPaddingRight - childWidth;
                        params->mRight = params->mLeft + childWidth;
                    }
                }
            }
        }
    }
    
    if (isWrapContentHeight) {
        // Height already has top padding in it since it was calculated by looking at
        // the bottom of each child view
        height += mPaddingBottom;
        
        if (mLayoutParams != NULL && mLayoutParams->height >= 0) {
            height = max(height, mLayoutParams->height);
        }
        
        height = max(height, getSuggestedMinimumHeight());
        height = resolveSize(height, heightMeasureSpec);
        
        if (offsetVerticalAxis) {
            for (int i = 0; i < count; i++) {
                shared_ptr<View> child = getChildAt(i);
                if (child->getVisibility() != GONE) {
                    RelativeLayoutParams *params = (RelativeLayoutParams*) child->getLayoutParams();
                    vector<int64_t> rules = params->getRules(layoutDirection);
                    if (rules[RelativeLayoutParams::CENTER_IN_PARENT] != 0 || rules[RelativeLayoutParams::CENTER_VERTICAL] != 0) {
                        centerVertical(child, params, height);
                    } else if (rules[RelativeLayoutParams::ALIGN_PARENT_BOTTOM] != 0) {
                        const int childHeight = child->getMeasuredHeight();
                        params->mTop = height - mPaddingBottom - childHeight;
                        params->mBottom = params->mTop + childHeight;
                    }
                }
            }
        }
    }
    
    if (horizontalGravity || verticalGravity) {
        Rect &selfBounds = m_selfBounds;
        selfBounds.fLeft = mPaddingLeft;
        selfBounds.fTop = mPaddingTop;
        selfBounds.fRight = width - mPaddingRight;
        selfBounds.fBottom = height - mPaddingBottom;
        
        Gravity::apply(m_gravity, right - left, bottom - top, selfBounds, m_contentBounds,
                      layoutDirection);
        
        const int horizontalOffset = m_contentBounds.left() - left;
        const int verticalOffset = m_contentBounds.top() - top;
        if (horizontalOffset != 0 || verticalOffset != 0) {
            for (int i = 0; i < count; i++) {
                shared_ptr<View> child = getChildAt(i);
                if (child->getVisibility() != GONE && child != ignore) {
                    RelativeLayoutParams *params = (RelativeLayoutParams*) child->getLayoutParams();
                    if (horizontalGravity) {
                        params->mLeft += horizontalOffset;
                        params->mRight += horizontalOffset;
                    }
                    if (verticalGravity) {
                        params->mTop += verticalOffset;
                        params->mBottom += verticalOffset;
                    }
                }
            }
        }
    }
    
    if (isLayoutRtl()) {
        const int offsetWidth = myWidth - width;
        for (int i = 0; i < count; i++) {
            shared_ptr<View> child = getChildAt(i);
            if (child->getVisibility() != GONE) {
                RelativeLayoutParams *params = (RelativeLayoutParams*) child->getLayoutParams();
                params->mLeft -= offsetWidth;
                params->mRight -= offsetWidth;
            }
        }
        
    }
    
    setMeasuredDimension(width, height);
}

void RelativeLayout::alignBaseline(shared_ptr<View> child, RelativeLayoutParams *params) {
    const int layoutDirection = getLayoutDirection();
    vector<int64_t> rules = params->getRules(layoutDirection);
    int anchorBaseline = getRelatedViewBaseline(rules, RelativeLayoutParams::ALIGN_BASELINE);
    
    if (anchorBaseline != -1) {
        RelativeLayoutParams *anchorParams = getRelatedViewParams(rules, RelativeLayoutParams::ALIGN_BASELINE);
        if (anchorParams != NULL) {
            int offset = anchorParams->mTop + anchorBaseline;
            int baseline = child->getBaseline();
            if (baseline != -1) {
                offset -= baseline;
            }
            int height = params->mBottom - params->mTop;
            params->mTop = offset;
            params->mBottom = params->mTop + height;
        }
    }
    
    if (m_baselineView == NULL) {
        m_baselineView = child;
    } else {
        RelativeLayoutParams *lp = (RelativeLayoutParams*) m_baselineView->getLayoutParams();
        if (params->mTop < lp->mTop || (params->mTop == lp->mTop && params->mLeft < lp->mLeft)) {
            m_baselineView = child;
        }
    }
}

/**
 * Measure a child-> The child should have left, top, right and bottom information
 * stored in its Layoutparams-> If any of these values is -1 it means that the view
 * can extend up to the corresponding edge.
 *
 * @param child Child to measure
 * @param params LayoutParams associated with child
 * @param myWidth Width of the the RelativeLayout
 * @param myHeight Height of the RelativeLayout
 */
void RelativeLayout::measureChild(shared_ptr<View> child, RelativeLayoutParams *params, int myWidth, int myHeight) {
    int childWidthMeasureSpec = getChildMeasureSpec(params->mLeft,
                                                    params->mRight, params->width,
                                                    params->leftMargin, params->rightMargin,
                                                    mPaddingLeft, mPaddingRight,
                                                    myWidth);
    int childHeightMeasureSpec = getChildMeasureSpec(params->mTop,
                                                     params->mBottom, params->height,
                                                     params->topMargin, params->bottomMargin,
                                                     mPaddingTop, mPaddingBottom,
                                                     myHeight);
    child->measure(childWidthMeasureSpec, childHeightMeasureSpec);
}

void RelativeLayout::measureChildHorizontal(shared_ptr<View> child, RelativeLayoutParams *params, int myWidth, int myHeight) {
    int childWidthMeasureSpec = getChildMeasureSpec(params->mLeft,
                                                    params->mRight, params->width,
                                                    params->leftMargin, params->rightMargin,
                                                    mPaddingLeft, mPaddingRight,
                                                    myWidth);
    int maxHeight = myHeight;
    if (m_measureVerticalWithPaddingMargin) {
        maxHeight = max(0, myHeight - mPaddingTop - mPaddingBottom -
                             params->topMargin - params->bottomMargin);
    }
    int childHeightMeasureSpec;
    if (myHeight < 0 && !m_allowBrokenMeasureSpecs) {
        if (params->height >= 0) {
            childHeightMeasureSpec = MeasureSpec::makeMeasureSpec(
                                                                 params->height, MeasureSpec::EXACTLY);
        } else {
            // Negative values in a mySize/myWidth/myWidth value in RelativeLayout measurement
            // is code for, "we got an unspecified mode in the RelativeLayout's MeasureSpec::"
            // Carry it forward.
            childHeightMeasureSpec = MeasureSpec::makeMeasureSpec(0, MeasureSpec::UNSPECIFIED);
        }
    } else if (params->width == LayoutParams::MATCH_PARENT) {
        childHeightMeasureSpec = MeasureSpec::makeMeasureSpec(maxHeight, MeasureSpec::EXACTLY);
    } else {
        childHeightMeasureSpec = MeasureSpec::makeMeasureSpec(maxHeight, MeasureSpec::AT_MOST);
    }
    child->measure(childWidthMeasureSpec, childHeightMeasureSpec);
}

/**
 * Get a measure spec that accounts for all of the constraints on this view.
 * This includes size constraints imposed by the RelativeLayout as well as
 * the View's desired dimension.
 *
 * @param childStart The left or top field of the child's layout params
 * @param childEnd The right or bottom field of the child's layout params
 * @param childSize The child's desired size (the width or height field of
 *        the child's layout params)
 * @param startMargin The left or top margin
 * @param endMargin The right or bottom margin
 * @param startPadding mPaddingLeft or mPaddingTop
 * @param endPadding mPaddingRight or mPaddingBottom
 * @param mySize The width or height of this view (the RelativeLayout)
 * @return MeasureSpec for the child
 */
int RelativeLayout::getChildMeasureSpec(int childStart, int childEnd,
                                int childSize, int startMargin, int endMargin, int startPadding,
                                int endPadding, int mySize) {
    if (mySize < 0 && !m_allowBrokenMeasureSpecs) {
        if (childSize >= 0) {
            return MeasureSpec::makeMeasureSpec(childSize, MeasureSpec::EXACTLY);
        }
        // Negative values in a mySize/myWidth/myWidth value in RelativeLayout measurement
        // is code for, "we got an unspecified mode in the RelativeLayout's MeasureSpec::"
        // Carry it forward.
        return MeasureSpec::makeMeasureSpec(0, MeasureSpec::UNSPECIFIED);
    }
    
    int childSpecMode = 0;
    int childSpecSize = 0;
    
    // Figure out start and end bounds.
    int tempStart = childStart;
    int tempEnd = childEnd;
    
    // If the view did not express a layout constraint for an edge, use
    // view's margins and our padding
    if (tempStart < 0) {
        tempStart = startPadding + startMargin;
    }
    if (tempEnd < 0) {
        tempEnd = mySize - endPadding - endMargin;
    }
    
    // Figure out maximum size available to this view
    int maxAvailable = tempEnd - tempStart;
    
    if (childStart >= 0 && childEnd >= 0) {
        // Constraints fixed both edges, so child must be an exact size
        childSpecMode = MeasureSpec::EXACTLY;
        childSpecSize = maxAvailable;
    } else {
        if (childSize >= 0) {
            // Child wanted an exact size. Give as much as possible
            childSpecMode = MeasureSpec::EXACTLY;
            
            if (maxAvailable >= 0) {
                // We have a maxmum size in this dimension.
                childSpecSize = min(maxAvailable, childSize);
            } else {
                // We can grow in this dimension.
                childSpecSize = childSize;
            }
        } else if (childSize == LayoutParams::MATCH_PARENT) {
            // Child wanted to be as big as possible. Give all available
            // space
            childSpecMode = MeasureSpec::EXACTLY;
            childSpecSize = maxAvailable;
        } else if (childSize == LayoutParams::WRAP_CONTENT) {
            // Child wants to wrap content. Use AT_MOST
            // to communicate available space if we know
            // our max size
            if (maxAvailable >= 0) {
                // We have a maximum size in this dimension.
                childSpecMode = MeasureSpec::AT_MOST;
                childSpecSize = maxAvailable;
            } else {
                // We can grow in this dimension. Child can be as big as it
                // wants
                childSpecMode = MeasureSpec::UNSPECIFIED;
                childSpecSize = 0;
            }
        }
    }
    
    return MeasureSpec::makeMeasureSpec(childSpecSize, childSpecMode);
}

bool RelativeLayout::positionChildHorizontal(shared_ptr<View> child, RelativeLayoutParams *params, int myWidth,
                                        bool wrapContent) {
    
    const int layoutDirection = getLayoutDirection();
    vector<int64_t> rules = params->getRules(layoutDirection);
    
    if (params->mLeft < 0 && params->mRight >= 0) {
        // Right is fixed, but left varies
        params->mLeft = params->mRight - child->getMeasuredWidth();
    } else if (params->mLeft >= 0 && params->mRight < 0) {
        // Left is fixed, but right varies
        params->mRight = params->mLeft + child->getMeasuredWidth();
    } else if (params->mLeft < 0 && params->mRight < 0) {
        // Both left and right vary
        if (rules[RelativeLayoutParams::CENTER_IN_PARENT] != 0 || rules[RelativeLayoutParams::CENTER_HORIZONTAL] != 0) {
            if (!wrapContent) {
                centerHorizontal(child, params, myWidth);
            } else {
                params->mLeft = mPaddingLeft + params->leftMargin;
                params->mRight = params->mLeft + child->getMeasuredWidth();
            }
            return true;
        } else {
            // This is the default case. For RTL we start from the right and for LTR we start
            // from the left. This will give LEFT/TOP for LTR and RIGHT/TOP for RTL.
            if (isLayoutRtl()) {
                params->mRight = myWidth - mPaddingRight- params->rightMargin;
                params->mLeft = params->mRight - child->getMeasuredWidth();
            } else {
                params->mLeft = mPaddingLeft + params->leftMargin;
                params->mRight = params->mLeft + child->getMeasuredWidth();
            }
        }
    }
    return rules[RelativeLayoutParams::ALIGN_PARENT_END] != 0;
}

bool RelativeLayout::positionChildVertical(shared_ptr<View> child, RelativeLayoutParams *params, int myHeight,
                                      bool wrapContent) {
    
    vector<int64_t> rules = params->getRules();
    
    if (params->mTop < 0 && params->mBottom >= 0) {
        // Bottom is fixed, but top varies
        params->mTop = params->mBottom - child->getMeasuredHeight();
    } else if (params->mTop >= 0 && params->mBottom < 0) {
        // Top is fixed, but bottom varies
        params->mBottom = params->mTop + child->getMeasuredHeight();
    } else if (params->mTop < 0 && params->mBottom < 0) {
        // Both top and bottom vary
        if (rules[RelativeLayoutParams::CENTER_IN_PARENT] != 0 || rules[RelativeLayoutParams::CENTER_VERTICAL] != 0) {
            if (!wrapContent) {
                centerVertical(child, params, myHeight);
            } else {
                params->mTop = mPaddingTop + params->topMargin;
                params->mBottom = params->mTop + child->getMeasuredHeight();
            }
            return true;
        } else {
            params->mTop = mPaddingTop + params->topMargin;
            params->mBottom = params->mTop + child->getMeasuredHeight();
        }
    }
    return rules[RelativeLayoutParams::ALIGN_PARENT_BOTTOM] != 0;
}

void RelativeLayout::applyHorizontalSizeRules(RelativeLayoutParams *childParams, int myWidth, vector<int64_t> rules) {
    RelativeLayoutParams *anchorParams = NULL;
    
    // -1 indicated a "soft requirement" in that direction. For example:
    // left=10, right=-1 means the view must start at 10, but can go as far as it wants to the right
    // left =-1, right=10 means the view must end at 10, but can go as far as it wants to the left
    // left=10, right=20 means the left and right ends are both fixed
    childParams->mLeft = -1;
    childParams->mRight = -1;
    
    anchorParams = getRelatedViewParams(rules, RelativeLayoutParams::LEFT_OF);
    if (anchorParams != NULL) {
        childParams->mRight = anchorParams->mLeft - (anchorParams->leftMargin +
                                                   childParams->rightMargin);
    } else if (childParams->alignWithParent && rules[RelativeLayoutParams::LEFT_OF] != 0) {
        if (myWidth >= 0) {
            childParams->mRight = myWidth - mPaddingRight - childParams->rightMargin;
        } else {
            // FIXME uh oh...
        }
    }
    
    anchorParams = getRelatedViewParams(rules, RelativeLayoutParams::RIGHT_OF);
    if (anchorParams != NULL) {
        childParams->mLeft = anchorParams->mRight + (anchorParams->rightMargin +
                                                   childParams->leftMargin);
    } else if (childParams->alignWithParent && rules[RelativeLayoutParams::RIGHT_OF] != 0) {
        childParams->mLeft = mPaddingLeft + childParams->leftMargin;
    }
    
    anchorParams = getRelatedViewParams(rules, RelativeLayoutParams::ALIGN_LEFT);
    if (anchorParams != NULL) {
        childParams->mLeft = anchorParams->mLeft + childParams->leftMargin;
    } else if (childParams->alignWithParent && rules[RelativeLayoutParams::ALIGN_LEFT] != 0) {
        childParams->mLeft = mPaddingLeft + childParams->leftMargin;
    }
    
    anchorParams = getRelatedViewParams(rules, RelativeLayoutParams::ALIGN_RIGHT);
    if (anchorParams != NULL) {
        childParams->mRight = anchorParams->mRight - childParams->rightMargin;
    } else if (childParams->alignWithParent && rules[RelativeLayoutParams::ALIGN_RIGHT] != 0) {
        if (myWidth >= 0) {
            childParams->mRight = myWidth - mPaddingRight - childParams->rightMargin;
        } else {
            // FIXME uh oh...
        }
    }
    
    if (0 != rules[RelativeLayoutParams::ALIGN_PARENT_LEFT]) {
        childParams->mLeft = mPaddingLeft + childParams->leftMargin;
    }
    
    if (0 != rules[RelativeLayoutParams::ALIGN_PARENT_RIGHT]) {
        if (myWidth >= 0) {
            childParams->mRight = myWidth - mPaddingRight - childParams->rightMargin;
        } else {
            // FIXME uh oh...
        }
    }
}

void RelativeLayout::applyVerticalSizeRules(RelativeLayoutParams *childParams, int myHeight) {
    vector<int64_t> rules = childParams->getRules();
    RelativeLayoutParams *anchorParams = NULL;
    
    childParams->mTop = -1;
    childParams->mBottom = -1;
    
    anchorParams = getRelatedViewParams(rules, RelativeLayoutParams::ABOVE);
    if (anchorParams != NULL) {
        childParams->mBottom = anchorParams->mTop - (anchorParams->topMargin +
                                                   childParams->bottomMargin);
    } else if (childParams->alignWithParent && rules[RelativeLayoutParams::ABOVE] != 0) {
        if (myHeight >= 0) {
            childParams->mBottom = myHeight - mPaddingBottom - childParams->bottomMargin;
        } else {
            // FIXME uh oh...
        }
    }
    
    anchorParams = getRelatedViewParams(rules, RelativeLayoutParams::BELOW);
    if (anchorParams != NULL) {
        childParams->mTop = anchorParams->mBottom + (anchorParams->bottomMargin +
                                                   childParams->topMargin);
    } else if (childParams->alignWithParent && rules[RelativeLayoutParams::BELOW] != 0) {
        childParams->mTop = mPaddingTop + childParams->topMargin;
    }
    
    anchorParams = getRelatedViewParams(rules, RelativeLayoutParams::ALIGN_TOP);
    if (anchorParams != NULL) {
        childParams->mTop = anchorParams->mTop + childParams->topMargin;
    } else if (childParams->alignWithParent && rules[RelativeLayoutParams::ALIGN_TOP] != 0) {
        childParams->mTop = mPaddingTop + childParams->topMargin;
    }
    
    anchorParams = getRelatedViewParams(rules, RelativeLayoutParams::ALIGN_BOTTOM);
    if (anchorParams != NULL) {
        childParams->mBottom = anchorParams->mBottom - childParams->bottomMargin;
    } else if (childParams->alignWithParent && rules[RelativeLayoutParams::ALIGN_BOTTOM] != 0) {
        if (myHeight >= 0) {
            childParams->mBottom = myHeight - mPaddingBottom - childParams->bottomMargin;
        } else {
            // FIXME uh oh...
        }
    }
    
    if (0 != rules[RelativeLayoutParams::ALIGN_PARENT_TOP]) {
        childParams->mTop = mPaddingTop + childParams->topMargin;
    }
    
    if (0 != rules[RelativeLayoutParams::ALIGN_PARENT_BOTTOM]) {
        if (myHeight >= 0) {
            childParams->mBottom = myHeight - mPaddingBottom - childParams->bottomMargin;
        } else {
            // FIXME uh oh...
        }
    }
    
    if (rules[RelativeLayoutParams::ALIGN_BASELINE] != 0) {
        m_hasBaselineAlignedChild = true;
    }
}

shared_ptr<View> RelativeLayout::getRelatedView(vector<int64_t> rules, int relation) {
    int64_t id = rules[relation];
    if (id != 0) {
        Node *node = m_graph.m_keyNodes[id];
        if (node == NULL) return NULL;
        shared_ptr<View> v = node->view;
        
        // Find the first non-GONE view up the chain
        while (v->getVisibility() == View::GONE) {
            RelativeLayoutParams *lp = (RelativeLayoutParams*) v->getLayoutParams();
            rules = lp->getRules(v->getLayoutDirection());
            node = m_graph.m_keyNodes[(rules[relation])];
            if (node == NULL) return NULL;
            v = node->view;
        }
        
        return v;
    }
    
    return NULL;
}

RelativeLayoutParams *RelativeLayout::getRelatedViewParams(vector<int64_t> rules, int relation) {
    shared_ptr<View> v = getRelatedView(rules, relation);
    if (v != NULL) {
        RelativeLayoutParams* params = (RelativeLayoutParams*) v->getLayoutParams();
        if (params != 0) {
            return params;
        }
    }
    return NULL;
}

int RelativeLayout::getRelatedViewBaseline(vector<int64_t> rules, int relation) {
    shared_ptr<View> v = getRelatedView(rules, relation);
    if (v != NULL) {
        return v->getBaseline();
    }
    return -1;
}

void RelativeLayout::centerHorizontal(shared_ptr<View> child, RelativeLayoutParams *params, int myWidth) {
    int childWidth = child->getMeasuredWidth();
    int left = (myWidth - childWidth) / 2;
    
    params->mLeft = left;
    params->mRight = left + childWidth;
}

void RelativeLayout::centerVertical(shared_ptr<View> child, RelativeLayoutParams *params, int myHeight) {
    int childHeight = child->getMeasuredHeight();
    int top = (myHeight - childHeight) / 2;
    
    params->mTop = top;
    params->mBottom = top + childHeight;
}

void RelativeLayout::onLayout(bool changed, int l, int t, int r, int b) {
    
    CCLOG("onLayout(%s) %d, %d, %d, %d Children %d", m_id.c_str(), l, t, r, b, getChildCount());
    
    //  The layout has actually already been performed and the positions
    //  cached.  Apply the cached values to the children.
    const int count = getChildCount();
    
    for (int i = 0; i < count; i++) {
        shared_ptr<View> child = getChildAt(i);
        if (child->getVisibility() != GONE) {
            RelativeLayoutParams *st = static_cast<RelativeLayoutParams*>(child->getLayoutParams());
            child->layout(st->mLeft, st->mTop, st->mRight, st->mBottom);
        }
    }
}

LayoutParams *RelativeLayout::generateLayoutParams(AttributeSet *attrs) {
    return new RelativeLayoutParams(getContext(), attrs);
}

/**
 * Returns a set of layout parameters with a width of
 * {@link android.view.ViewGroup.LayoutParams#WRAP_CONTENT},
 * a height of {@link android.view.ViewGroup.LayoutParams#WRAP_CONTENT} and no spanning.
 */
LayoutParams *RelativeLayout::generateDefaultLayoutParams() {
    return new LayoutParams(LayoutParams::WRAP_CONTENT, LayoutParams::WRAP_CONTENT);
}

LayoutParams *RelativeLayout::generateLayoutParams(LayoutParams *p) {
    return new RelativeLayoutParams(p);
}

// Override to allow type-checking of Layoutparams->
bool RelativeLayout::checkLayoutParams(LayoutParams *p) {
    return dynamic_cast<RelativeLayoutParams*>(p) != 0;
}

ANDROID_END