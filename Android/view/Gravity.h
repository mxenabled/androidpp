/*
 * File:   Gravity::h
 * Author: saulhoward
 *
 * Created on October 17, 2013, 12:47 AM
 */

#ifndef GRAVITY_H
#define	GRAVITY_H

#include "AndroidMacros.h"

#include "Android/view/View.h"
#include "Android/graphics/Rect.h"
#include "Android/utils/System.h"

#include <string>

ANDROID_BEGIN

class Gravity {
    public:
        /** Constant indicating that no gravity has been set **/
    static const int NO_GRAVITY = 0x0000;

    /** Raw bit indicating the gravity for an axis has been specified. */
    static const int AXIS_SPECIFIED = 0x0001;

    /** Raw bit controlling how the left/top edge is placed. */
    static const int AXIS_PULL_BEFORE = 0x0002;
    /** Raw bit controlling how the right/bottom edge is placed. */
    static const int AXIS_PULL_AFTER = 0x0004;
    /** Raw bit controlling whether the right/bottom edge is clipped to its
     * container, based on the gravity direction being applied. */
    static const int AXIS_CLIP = 0x0008;

    /** Bits defining the horizontal axis. */
    static const int AXIS_X_SHIFT = 0;
    /** Bits defining the vertical axis. */
    static const int AXIS_Y_SHIFT = 4;

    /** Push object to the top of its container, not changing its size. */
    static const int TOP = (AXIS_PULL_BEFORE|AXIS_SPECIFIED)<<AXIS_Y_SHIFT;
    /** Push object to the bottom of its container, not changing its size. */
    static const int BOTTOM = (AXIS_PULL_AFTER|AXIS_SPECIFIED)<<AXIS_Y_SHIFT;
    /** Push object to the left of its container, not changing its size. */
    static const int LEFT = (AXIS_PULL_BEFORE|AXIS_SPECIFIED)<<AXIS_X_SHIFT;
    /** Push object to the right of its container, not changing its size. */
    static const int RIGHT = (AXIS_PULL_AFTER|AXIS_SPECIFIED)<<AXIS_X_SHIFT;

    /** Place object in the vertical center of its container, not changing its
     *  size. */
    static const int CENTER_VERTICAL = AXIS_SPECIFIED<<AXIS_Y_SHIFT;
    /** Grow the vertical size of the object if needed so it completely fills
     *  its container. */
    static const int FILL_VERTICAL = TOP|BOTTOM;

    /** Place object in the horizontal center of its container, not changing its
     *  size. */
    static const int CENTER_HORIZONTAL = AXIS_SPECIFIED<<AXIS_X_SHIFT;
    /** Grow the horizontal size of the object if needed so it completely fills
     *  its container. */
    static const int FILL_HORIZONTAL = LEFT|RIGHT;

    /** Place the object in the center of its container in both the vertical
     *  and horizontal axis, not changing its size. */
    static const int CENTER = CENTER_VERTICAL|CENTER_HORIZONTAL;

    /** Grow the horizontal and vertical size of the object if needed so it
     *  completely fills its container. */
    static const int FILL = FILL_VERTICAL|FILL_HORIZONTAL;

    /** Flag to clip the edges of the object to its container along the
     *  vertical axis. */
    static const int CLIP_VERTICAL = AXIS_CLIP<<AXIS_Y_SHIFT;

    /** Flag to clip the edges of the object to its container along the
     *  horizontal axis. */
    static const int CLIP_HORIZONTAL = AXIS_CLIP<<AXIS_X_SHIFT;

    /** Raw bit controlling whether the layout direction is relative or not (START/END instead of
     * absolute LEFT/RIGHT).
     */
    static const int RELATIVE_LAYOUT_DIRECTION = 0x00800000;

    /**
     * Binary mask to get the absolute horizontal gravity of a gravity.
     */
    static const int HORIZONTAL_GRAVITY_MASK = (AXIS_SPECIFIED |
            AXIS_PULL_BEFORE | AXIS_PULL_AFTER) << AXIS_X_SHIFT;
    /**
     * Binary mask to get the vertical gravity of a gravity.
     */
    static const int VERTICAL_GRAVITY_MASK = (AXIS_SPECIFIED |
            AXIS_PULL_BEFORE | AXIS_PULL_AFTER) << AXIS_Y_SHIFT;

    /** Special constant to enable clipping to an overall display along the
     *  vertical dimension.  This is not applied by default by
     *  {@link #apply(int, int, int, Rect, int, int, Rect)}; you must do so
     *  yourself by calling {@link #applyDisplay}.
     */
    static const int DISPLAY_CLIP_VERTICAL = 0x10000000;

    /** Special constant to enable clipping to an overall display along the
     *  horizontal dimension.  This is not applied by default by
     *  {@link #apply(int, int, int, Rect, int, int, Rect)}; you must do so
     *  yourself by calling {@link #applyDisplay}.
     */
    static const int DISPLAY_CLIP_HORIZONTAL = 0x01000000;

    /** Push object to x-axis position at the start of its container, not changing its size. */
    static const int START = RELATIVE_LAYOUT_DIRECTION | LEFT;

    /** Push object to x-axis position at the end of its container, not changing its size. */
    static const int END = RELATIVE_LAYOUT_DIRECTION | RIGHT;

    /**
     * Binary mask for the horizontal gravity and script specific direction bit.
     */
    static const int RELATIVE_HORIZONTAL_GRAVITY_MASK = START | END;
    
    static inline void apply(int gravity, int w, int h, Rect container,
                      int xAdj, int yAdj, Rect &outRect) {
        switch (gravity&((AXIS_PULL_BEFORE|AXIS_PULL_AFTER)<<AXIS_X_SHIFT)) {
            case 0:
                outRect.fLeft = container.left()
                + ((container.right() - container.left() - w)/2) + xAdj;
                outRect.fRight = outRect.left() + w;
                if ((gravity&(AXIS_CLIP<<AXIS_X_SHIFT))
                    == (AXIS_CLIP<<AXIS_X_SHIFT)) {
                    if (outRect.left() < container.left()) {
                        outRect.fLeft = container.left();
                    }
                    if (outRect.right() > container.right()) {
                        outRect.fRight = container.right();
                    }
                }
                break;
            case AXIS_PULL_BEFORE<<AXIS_X_SHIFT:
                outRect.fLeft = container.left() + xAdj;
                outRect.fRight = outRect.left() + w;
                if ((gravity&(AXIS_CLIP<<AXIS_X_SHIFT))
                    == (AXIS_CLIP<<AXIS_X_SHIFT)) {
                    if (outRect.right() > container.right()) {
                        outRect.fRight = container.right();
                    }
                }
                break;
            case AXIS_PULL_AFTER<<AXIS_X_SHIFT:
                outRect.fRight = container.right() - xAdj;
                outRect.fLeft = outRect.right() - w;
                if ((gravity&(AXIS_CLIP<<AXIS_X_SHIFT))
                    == (AXIS_CLIP<<AXIS_X_SHIFT)) {
                    if (outRect.left() < container.left()) {
                        outRect.fLeft = container.left();
                    }
                }
                break;
            default:
                outRect.fLeft = container.left() + xAdj;
                outRect.fRight = container.right() + xAdj;
                break;
        }
        
        switch (gravity&((AXIS_PULL_BEFORE|AXIS_PULL_AFTER)<<AXIS_Y_SHIFT)) {
            case 0:
                outRect.fTop = container.top()
                + ((container.bottom() - container.top() - h)/2) + yAdj;
                outRect.fBottom = outRect.top() + h;
                if ((gravity&(AXIS_CLIP<<AXIS_Y_SHIFT))
                    == (AXIS_CLIP<<AXIS_Y_SHIFT)) {
                    if (outRect.top() < container.top()) {
                        outRect.fTop = container.top();
                    }
                    if (outRect.bottom() > container.bottom()) {
                        outRect.fBottom = container.bottom();
                    }
                }
                break;
            case AXIS_PULL_BEFORE<<AXIS_Y_SHIFT:
                outRect.fTop = container.top() + yAdj;
                outRect.fBottom = outRect.top() + h;
                if ((gravity&(AXIS_CLIP<<AXIS_Y_SHIFT))
                    == (AXIS_CLIP<<AXIS_Y_SHIFT)) {
                    if (outRect.bottom() > container.bottom()) {
                        outRect.fBottom = container.bottom();
                    }
                }
                break;
            case AXIS_PULL_AFTER<<AXIS_Y_SHIFT:
                outRect.fBottom = container.bottom() - yAdj;
                outRect.fTop = outRect.bottom() - h;
                if ((gravity&(AXIS_CLIP<<AXIS_Y_SHIFT))
                    == (AXIS_CLIP<<AXIS_Y_SHIFT)) {
                    if (outRect.top() < container.top()) {
                        outRect.fTop = container.top();
                    }
                }
                break;
            default:
                outRect.fTop = container.top() + yAdj;
                outRect.fBottom = container.bottom() + yAdj;
                break;
        }
    }

    static inline void apply(int gravity, int w, int h, Rect container,
               Rect &outRect, int layoutDirection) {
        int absGravity = getAbsoluteGravity(gravity, layoutDirection);
        apply(absGravity, w, h, container, 0, 0, outRect);
    }
    
    static inline int getAbsoluteGravity(int gravity, int layoutDirection) {
        int result = gravity;
        // If layout is script specific and gravity is horizontal relative (START or END)
        if ((result & RELATIVE_LAYOUT_DIRECTION) > 0) {
            if ((result & Gravity::START) == Gravity::START) {
                // Remove the START bit
                result &= ~START;
                if (layoutDirection == View::LAYOUT_DIRECTION_RTL) {
                    // Set the RIGHT bit
                    result |= RIGHT;
                } else {
                    // Set the LEFT bit
                    result |= LEFT;
                }
            } else if ((result & Gravity::END) == Gravity::END) {
                // Remove the END bit
                result &= ~END;
                if (layoutDirection == View::LAYOUT_DIRECTION_RTL) {
                    // Set the LEFT bit
                    result |= LEFT;
                } else {
                    // Set the RIGHT bit
                    result |= RIGHT;
                }
            }
            // Don't need the script specific bit any more, so remove it as we are converting to
            // absolute values (LEFT or RIGHT)
            result &= ~RELATIVE_LAYOUT_DIRECTION;
        }
        return result;
    }
    
    static inline int getGravity(std::string name, int defaultGravity) {
        
        if (name.empty()) return defaultGravity;
        
        int gravity = 0;
        
        if (System::find(name, STR_NO_GRAVITY) != -1) {
            gravity |= NO_GRAVITY;
        }
        if (System::find(name, STR_AXIS_SPECIFIED) != -1) {
            gravity |= AXIS_SPECIFIED;
        }
        if (System::find(name, STR_AXIS_PULL_BEFORE) != -1) {
            gravity |= AXIS_PULL_BEFORE;
        }
        if (System::find(name, STR_AXIS_PULL_AFTER) != -1) {
            gravity |= AXIS_PULL_AFTER;
        }
        if (System::find(name, STR_AXIS_CLIP) != -1) {
            gravity |= AXIS_CLIP;
        }
        if (System::find(name, STR_AXIS_X_SHIFT) != -1) {
            gravity |= AXIS_X_SHIFT;
        }
        if (System::find(name, STR_AXIS_Y_SHIFT) != -1) {
            gravity |= AXIS_Y_SHIFT;
        }
        if (System::find(name, STR_TOP) != -1) {
            gravity |= TOP;
        }
        if (System::find(name, STR_BOTTOM) != -1) {
            gravity |= BOTTOM;
        }
        if (System::find(name, STR_LEFT) != -1) {
            gravity |= LEFT;
        }
        if (System::find(name, STR_RIGHT) != -1) {
            gravity |= RIGHT;
        }
        if (System::find(name, STR_CENTER_VERTICAL) != -1) {
            gravity |= CENTER_VERTICAL;
        }
        if (System::find(name, STR_FILL_VERTICAL) != -1) {
            gravity |= FILL_VERTICAL;
        }
        if (System::find(name, STR_CENTER_HORIZONTAL) != -1) {
            gravity |= CENTER_HORIZONTAL;
        }
        if (System::find(name, STR_FILL_HORIZONTAL) != -1) {
            gravity |= FILL_HORIZONTAL;
        }
        if (System::find(name, STR_CENTER) != -1) {
            gravity |= CENTER;
        }
        if (System::find(name, STR_FILL) != -1) {
            gravity |= FILL;
        }
        
        return gravity;
    }
    
private:
    
    static const string STR_NO_GRAVITY;
    static const string STR_AXIS_SPECIFIED;
    static const string STR_AXIS_PULL_BEFORE;
    static const string STR_AXIS_PULL_AFTER;
    static const string STR_AXIS_CLIP;
    static const string STR_AXIS_X_SHIFT;
    static const string STR_AXIS_Y_SHIFT;
    static const string STR_TOP;
    static const string STR_BOTTOM;
    static const string STR_LEFT;
    static const string STR_RIGHT;
    static const string STR_CENTER_VERTICAL;
    static const string STR_FILL_VERTICAL;
    static const string STR_CENTER_HORIZONTAL;
    static const string STR_FILL_HORIZONTAL;
    static const string STR_CENTER;
    static const string STR_FILL;
};

ANDROID_END

#endif	/* GRAVITY_H */

