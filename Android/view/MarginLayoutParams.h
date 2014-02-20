/*
 * File:   MarginLayoutParams.h
 * Author: saulhoward
 *
 * Created on October 16, 2013, 10:24 PM
 */

#ifndef MARGINLAYOUTPARAMS_H
#define	MARGINLAYOUTPARAMS_H

#include "AndroidMacros.h"

#include "Android/view/LayoutParams.h"

ANDROID_BEGIN

class MarginLayoutParams : public LayoutParams {

public:
    
    MarginLayoutParams(Context *context, AttributeSet *attrs) : LayoutParams(context, attrs) {

        int margin = attrs->getDimensionValue(context->getResources(), R::styleable::layout_margin, -1);

        if (margin >= 0) {
            leftMargin = margin;
            rightMargin = margin;
            topMargin = margin;
            bottomMargin = margin;
        } else {
            leftMargin = attrs->getDimensionValue(context->getResources(), R::styleable::layout_marginLeft, UNDEFINED_MARGIN);
            if (leftMargin == UNDEFINED_MARGIN) {
                m_marginFlags |= LEFT_MARGIN_UNDEFINED_MASK;
                leftMargin = DEFAULT_MARGIN_RESOLVED;
            }
            rightMargin = attrs->getDimensionValue(context->getResources(), R::styleable::layout_marginRight, UNDEFINED_MARGIN);
            if (rightMargin == UNDEFINED_MARGIN) {
                m_marginFlags |= RIGHT_MARGIN_UNDEFINED_MASK;
                rightMargin = DEFAULT_MARGIN_RESOLVED;
            }

            topMargin = attrs->getDimensionValue(context->getResources(), R::styleable::layout_marginTop, DEFAULT_MARGIN_RESOLVED);
            bottomMargin = attrs->getDimensionValue(context->getResources(), R::styleable::layout_marginBottom, DEFAULT_MARGIN_RESOLVED);

            startMargin = attrs->getDimensionValue(context->getResources(), R::styleable::layout_marginStart, DEFAULT_MARGIN_RELATIVE);
            endMargin = attrs->getDimensionValue(context->getResources(), R::styleable::layout_marginEnd, DEFAULT_MARGIN_RELATIVE);

            if (isMarginRelative()) {
                m_marginFlags |= NEED_RESOLUTION_MASK;
            }
        }
    }

    MarginLayoutParams(int width, int height) : LayoutParams(width, height) {

        m_marginFlags |= LEFT_MARGIN_UNDEFINED_MASK;
        m_marginFlags |= RIGHT_MARGIN_UNDEFINED_MASK;

        m_marginFlags &= ~NEED_RESOLUTION_MASK;
        m_marginFlags &= ~RTL_COMPATIBILITY_MODE_MASK;
    }

    MarginLayoutParams(MarginLayoutParams *source) {
        width = source->width;
        height = source->height;

        leftMargin = source->leftMargin;
        topMargin = source->topMargin;
        rightMargin = source->rightMargin;
        bottomMargin = source->bottomMargin;
        startMargin = source->startMargin;
        endMargin = source->endMargin;

        m_marginFlags = source->m_marginFlags;
    }

    MarginLayoutParams(LayoutParams *source) : LayoutParams(source) {

        m_marginFlags |= LEFT_MARGIN_UNDEFINED_MASK;
        m_marginFlags |= RIGHT_MARGIN_UNDEFINED_MASK;

        m_marginFlags &= ~NEED_RESOLUTION_MASK;
        m_marginFlags &= ~RTL_COMPATIBILITY_MODE_MASK;
    }

    virtual ~MarginLayoutParams() {}

    int leftMargin = 0;

    /**
     * The top margin in pixels of the child.
     * Call {@link ViewGroup#setLayoutParams(LayoutParams)} after reassigning a new value
     * to this field.
     */
    int topMargin = 0;

    /**
     * The right margin in pixels of the child.
     * Call {@link ViewGroup#setLayoutParams(LayoutParams)} after reassigning a new value
     * to this field.
     */
    int rightMargin = 0;

    /**
     * The bottom margin in pixels of the child.
     * Call {@link ViewGroup#setLayoutParams(LayoutParams)} after reassigning a new value
     * to this field.
     */
    int bottomMargin = 0;

    /**
     * The default start and end margin.
     * @hide
     */
    static const int DEFAULT_MARGIN_RELATIVE = INT_MIN;

    void setMargins(int left, int top, int right, int bottom) {
        leftMargin = left;
        topMargin = top;
        rightMargin = right;
        bottomMargin = bottom;
        m_marginFlags &= ~LEFT_MARGIN_UNDEFINED_MASK;
        m_marginFlags &= ~RIGHT_MARGIN_UNDEFINED_MASK;
        if (isMarginRelative()) {
            m_marginFlags |= NEED_RESOLUTION_MASK;
        } else {
            m_marginFlags &= ~NEED_RESOLUTION_MASK;
        }
    }

    void setMarginsRelative(int start, int top, int end, int bottom) {
        startMargin = start;
        topMargin = top;
        endMargin = end;
        bottomMargin = bottom;
        m_marginFlags |= NEED_RESOLUTION_MASK;
    }

    void setMarginStart(int start) {
        startMargin = start;
        m_marginFlags |= NEED_RESOLUTION_MASK;
    }

    void setMarginEnd(int end) {
        endMargin = end;
        m_marginFlags |= NEED_RESOLUTION_MASK;
    }

    int getMarginEnd() {
        if (endMargin != DEFAULT_MARGIN_RELATIVE) return endMargin;
        if ((m_marginFlags & NEED_RESOLUTION_MASK) == NEED_RESOLUTION_MASK) {
            doResolveMargins();
        }
        switch(m_marginFlags & LAYOUT_DIRECTION_MASK) {
            case LAYOUT_DIRECTION_RTL:
                return leftMargin;
            case LAYOUT_DIRECTION_LTR:
            default:
                return rightMargin;
        }
    }

    bool isMarginRelative() {
        return (startMargin != DEFAULT_MARGIN_RELATIVE || endMargin != DEFAULT_MARGIN_RELATIVE);
    }

    void setLayoutDirection(int layoutDirection) {
        if (layoutDirection != LAYOUT_DIRECTION_LTR &&
                layoutDirection != LAYOUT_DIRECTION_RTL) return;
        if (layoutDirection != (m_marginFlags & LAYOUT_DIRECTION_MASK)) {
            m_marginFlags &= ~LAYOUT_DIRECTION_MASK;
            m_marginFlags |= (layoutDirection & LAYOUT_DIRECTION_MASK);
            if (isMarginRelative()) {
                m_marginFlags |= NEED_RESOLUTION_MASK;
            } else {
                m_marginFlags &= ~NEED_RESOLUTION_MASK;
            }
        }
    }

    int getLayoutDirection() {
        return (m_marginFlags & LAYOUT_DIRECTION_MASK);
    }

    virtual void resolveLayoutDirection(int layoutDirection) {
        setLayoutDirection(layoutDirection);

        // No relative margin or pre JB-MR1 case or no need to resolve, just dont do anything
        // Will use the left and right margins if no relative margin is defined.
        if (!isMarginRelative() ||
                (m_marginFlags & NEED_RESOLUTION_MASK) != NEED_RESOLUTION_MASK) return;

        // Proceed with resolution
        doResolveMargins();
    }
    bool isLayoutRtl() {
        return ((m_marginFlags & LAYOUT_DIRECTION_MASK) == LAYOUT_DIRECTION_RTL);
    }

private:
    char m_marginFlags;

    static const int LAYOUT_DIRECTION_MASK = 0x00000003;
    static const int LEFT_MARGIN_UNDEFINED_MASK = 0x00000004;
    static const int RIGHT_MARGIN_UNDEFINED_MASK = 0x00000008;
    static const int NEED_RESOLUTION_MASK = 0x00000020;
    static const int RTL_COMPATIBILITY_MODE_MASK = 0x00000010;

    static const int DEFAULT_MARGIN_RESOLVED = 0;
    static const int UNDEFINED_MARGIN = DEFAULT_MARGIN_RELATIVE;

    /**
     * The start margin in pixels of the child.
     * Call {@link ViewGroup#setLayoutParams(LayoutParams)} after reassigning a new value
     * to this field.
     */
    int startMargin = DEFAULT_MARGIN_RELATIVE;

    /**
     * The end margin in pixels of the child.
     * Call {@link ViewGroup#setLayoutParams(LayoutParams)} after reassigning a new value
     * to this field.
     */
    int endMargin = DEFAULT_MARGIN_RELATIVE;

    void doResolveMargins() {
        if ((m_marginFlags & RTL_COMPATIBILITY_MODE_MASK) == RTL_COMPATIBILITY_MODE_MASK) {
            // if left or right margins are not defined and if we have some start or end margin
            // defined then use those start and end margins.
            if ((m_marginFlags & LEFT_MARGIN_UNDEFINED_MASK) == LEFT_MARGIN_UNDEFINED_MASK
                    && startMargin > DEFAULT_MARGIN_RELATIVE) {
                leftMargin = startMargin;
            }
            if ((m_marginFlags & RIGHT_MARGIN_UNDEFINED_MASK) == RIGHT_MARGIN_UNDEFINED_MASK
                    && endMargin > DEFAULT_MARGIN_RELATIVE) {
                rightMargin = endMargin;
            }
        } else {
            // We have some relative margins (either the start one or the end one or both). So use
            // them and override what has been defined for left and right margins. If either start
            // or end margin is not defined, just set it to default "0".
            switch(m_marginFlags & LAYOUT_DIRECTION_MASK) {
                case LAYOUT_DIRECTION_RTL:
                    leftMargin = (endMargin > DEFAULT_MARGIN_RELATIVE) ?
                            endMargin : DEFAULT_MARGIN_RESOLVED;
                    rightMargin = (startMargin > DEFAULT_MARGIN_RELATIVE) ?
                            startMargin : DEFAULT_MARGIN_RESOLVED;
                    break;
                case LAYOUT_DIRECTION_LTR:
                default:
                    leftMargin = (startMargin > DEFAULT_MARGIN_RELATIVE) ?
                            startMargin : DEFAULT_MARGIN_RESOLVED;
                    rightMargin = (endMargin > DEFAULT_MARGIN_RELATIVE) ?
                            endMargin : DEFAULT_MARGIN_RESOLVED;
                    break;
            }
        }
        m_marginFlags &= ~NEED_RESOLUTION_MASK;
    }
};

ANDROID_END

#endif	/* MARGINLAYOUTPARAMS_H */

