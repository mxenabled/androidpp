//
//  Drawable.cpp
//  cocos2dx
//
//  Created by Saul Howard on 11/13/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#include "Drawable.h"

#include "Android/content/Context.h"
#include "Android/internal/R.h"
#include "Android/graphics/drawable/BitmapDrawable.h"
#include "Android/graphics/drawable/ColorDrawable.h"
#include "Android/graphics/drawable/GradientDrawable.h"
#include "Android/graphics/drawable/StateListDrawable.h"
#include "Android/graphics/drawable/LayerDrawable.h"

ANDROID_BEGIN

Rect Drawable::ZERO_BOUNDS_RECT = Rect();

void Drawable::clearColorFilter() {
    setColorFilter(NULL);
}

//void Drawable::setColorFilter(int color, PorterDuff.Mode mode) {
//    setColorFilter(new PorterDuffColorFilter(color, mode));
//}

/**
 * Specify a bounding Rectangle for the Drawable. This is where the drawable
 * will draw when its draw() method is called.
 */
void Drawable::setBounds(int left, int top, int right, int bottom) {
    Rect &oldBounds = m_bounds;
    
    if (oldBounds == ZERO_BOUNDS_RECT) {
        oldBounds = m_bounds = Rect();
    }
    
    if (oldBounds.left() != left || oldBounds.top() != top ||
        oldBounds.right() != right || oldBounds.bottom() != bottom) {
        if (!oldBounds.isEmpty()) {
            // first invalidate the previous bounds
            invalidateSelf();
        }
        m_bounds.set(left, top, right, bottom);
        onBoundsChange(m_bounds);
    }
}

/**
 * Specify a bounding rectangle for the Drawable. This is where the drawable
 * will draw when its draw() method is called.
 */
void Drawable::setBounds(Rect bounds) {
    setBounds(bounds.left(), bounds.top(), bounds.right(), bounds.bottom());
}

/**
 * Return a copy of the drawable's bounds in the specified rect (allocated
 * by the caller). The bounds specify where this will draw when its draw()
 * method is called.
 *
 * @param bounds rect to receive the drawable's bounds (allocated by the
 *               caller).
 */
const void Drawable::copyBounds(Rect &bounds) {
    bounds.set(m_bounds);
}

/**
 * Return a copy of the drawable's bounds in a new rect. This returns the
 * same values as getBounds(), but the returned object is guaranteed to not
 * be changed later by the drawable (i.e. it retains no reference to this
 * rect). If the caller already has a rect allocated, call copyBounds(rect).
 *
 * @return A copy of the drawable's bounds
 */
const Rect *Drawable::copyBounds() {
    return new Rect(m_bounds);
}

shared_ptr<Drawable> Drawable::createFromXmlInner(Resources *r, CCPullParser &parser) {
    
    AttributeSet *attrs = (AttributeSet*) &parser;
    
    shared_ptr<Drawable> drawable;
    string name(parser.getName());
    
    if (name.compare("selector") == 0) {
        drawable = make_shared<StateListDrawable>();
    } else if (name.compare("layer-list") == 0) {
        drawable = make_shared<LayerDrawable>();
    } else if (name.compare("color") == 0) {
        drawable = make_shared<ColorDrawable>();
    } else if (name.compare("shape") == 0) {
        drawable = make_shared<GradientDrawable>();
    } else if (name.compare("bitmap") == 0) {
        drawable = make_shared<BitmapDrawable>(r);
    }
    
    if (drawable != NULL) {
        drawable->inflate(r, parser, attrs);
    }
    
    return drawable;
}

/**
 * Return the drawable's bounds rect. Note: for efficiency, the returned
 * object may be the same object stored in the drawable (though this is not
 * guaranteed), so if a persistent copy of the bounds is needed, call
 * copyBounds(rect) instead.
 * You should also not change the object returned by this method as it may
 * be the same object stored in the drawable.
 *
 * @return The bounds of the drawable (which may change later, so caller
 *         beware). DO NOT ALTER the returned object as it may change the
 *         stored bounds of this drawable.
 *
 * @see #copyBounds()
 * @see #copyBounds(android.graphics.rect)
 */
const Rect Drawable::getBounds() {
    if (m_bounds == ZERO_BOUNDS_RECT) {
        m_bounds = Rect();
    }
    
    return m_bounds;
}

/**
 * Set a mask of the configuration parameters for which this drawable
 * may change, requiring that it be re-created.
 *
 * @param configs A mask of the changing configuration parameters, as
 * defined by {@link android.content.res.Configuration}.
 *
 * @see android.content.res.Configuration
 */
void Drawable::setChangingConfigurations(int configs) {
    m_changingConfigurations = configs;
}

/**
 * Return a mask of the configuration parameters for which this drawable
 * may change, requiring that it be re-created.  The default implementation
 * returns whatever was provided through
 * {@link #setChangingConfigurations(int)} or 0 by default.  Subclasses
 * may extend this to or in the changing configurations of any other
 * drawables they hold.
 *
 * @return Returns a mask of the changing configuration parameters, as
 * defined by {@link android.content.res.Configuration}.
 *
 * @see android.content.res.Configuration
 */
int Drawable::getChangingConfigurations() {
    return m_changingConfigurations;
}

/**
 * Bind a {@link Callback} object to this Drawable.  Required for clients
 * that want to support animated drawables.
 *
 * @param cb The client's Callback implementation.
 *
 * @see #getCallback()
 */
void Drawable::setCallback(Callback *cb) {
    m_callback = cb;
}

/**
 * Return the current {@link Callback} implementation attached to this
 * Drawable.
 *
 * @return A {@link Callback} instance or NULL if no callback was set.
 *
 * @see #setCallback(android.graphics.drawable.Drawable.Callback)
 */
Drawable::Callback *Drawable::getCallback() {
    return m_callback;
}

/**
 * Use the current {@link Callback} implementation to have this Drawable
 * redrawn.  Does nothing if there is no Callback attached to the
 * Drawable.
 *
 * @see Callback#invalidateDrawable
 * @see #getCallback()
 * @see #setCallback(android.graphics.drawable.Drawable.Callback)
 */
void Drawable::invalidateSelf() {
    Callback *callback = getCallback();
    if (callback != NULL) {
        callback->invalidateDrawable(shared_from_this());
    }
}

/**
 * Use the current {@link Callback} implementation to have this Drawable
 * scheduled.  Does nothing if there is no Callback attached to the
 * Drawable.
 *
 * @param what The action being scheduled.
 * @param when The time (in milliseconds) to run.
 *
 * @see Callback#scheduleDrawable
 */
void Drawable::scheduleSelf(sp<Runnable> &what, uint32_t when) {
    Callback *callback = getCallback();
    if (callback != NULL) {
        callback->scheduleDrawable(this, what, when);
    }
}

/**
 * Use the current {@link Callback} implementation to have this Drawable
 * unscheduled.  Does nothing if there is no Callback attached to the
 * Drawable.
 *
 * @param what The runnable that you no longer want called.
 *
 * @see Callback#unscheduleDrawable
 */
void Drawable::unscheduleSelf(sp<Runnable> &what) {
    Callback *callback = getCallback();
    if (callback != NULL) {
        callback->unscheduleDrawable(this, what);
    }
}

/**
 * Returns the resolved layout direction for this Drawable.
 *
 * @return One of {@link android.view.View#LAYOUT_DIRECTION_LTR},
 *   {@link android.view.View#LAYOUT_DIRECTION_RTL}
 *
 * @hide
 */
int Drawable::getLayoutDirection() {
    return m_layoutDirection;
}

void Drawable::inflate(Resources *r, CCPullParser &parser, AttributeSet *attrs) {
    attrs->loadAttributes();
    inflateWithAttributes(r, parser, attrs, R::styleable::visible);
}

void Drawable::inflateWithAttributes(Resources *r, CCPullParser &parser, AttributeSet *attrs, const char* visibleAttr) {
    m_visible = attrs->getAttributeBooleanValue(visibleAttr, m_visible);
}

/**
 * Set the layout direction for this drawable. Should be a resolved direction as the
 * Drawable as no capacity to do the resolution on his own.
 *
 * @param layoutDirection One of {@link android.view.View#LAYOUT_DIrectION_LTR},
 *   {@link android.view.View#LAYOUT_DIrectION_RTL}
 *
 * @hide
 */
void Drawable::setLayoutDirection(int layoutDirection) {
    if (getLayoutDirection() != layoutDirection) {
        m_layoutDirection = layoutDirection;
    }
}

/**
 * Indicates whether this view will change its appearance based on state.
 * Clients can use this to determine whether it is necessary to calculate
 * their state and call setState.
 *
 * @return True if this view changes its appearance based on state, false
 *         otherwise.
 *
 * @see #setState(int[])
 */
bool Drawable::isStateful() {
    return false;
}

/**
 * Specify a set of states for the drawable. These are use-case specific,
 * so see the relevant documentation. As an example, the background for
 * widgets like Button understand the following states:
 * [{@link android.R.attr#state_focused},
 *  {@link android.R.attr#state_pressed}].
 *
 * <p>If the new state you are supplying causes the appearance of the
 * Drawable to change, then it is responsible for calling
 * {@link #invalidateSelf} in order to have itself redrawn, <em>and</em>
 * true will be returned from this function.
 *
 * <p>Note: The Drawable holds a reference on to <var>stateSet</var>
 * until a new state array is given to it, so you must not modify this
 * array during that time.</p>
 *
 * @param stateSet The new set of states to be displayed.
 *
 * @return Returns true if this change in state has caused the appearance
 * of the Drawable to change (hence requiring an invalidate), otherwise
 * returns false.
 */
bool Drawable::setState(const vector<int> stateSet) {
    if (m_stateSet != stateSet) {
        m_stateSet = stateSet;
        return onStateChange(stateSet);
    }
    return false;
}

/**
 * Describes the current state, as a union of primitve states, such as
 * {@link android.R.attr#state_focused},
 * {@link android.R.attr#state_selected}, etc.
 * Some drawables may modify their imagery based on the selected state.
 * @return An array of resource Ids describing the current state.
 */
vector<int> Drawable::getState() {
    return m_stateSet;
}

/**
 * @return The current drawable that will be used by this drawable. For simple drawables, this
 *         is just the drawable itself. For drawables that change state like
 *         {@link StateListDrawable} and {@link LevelListDrawable} this will be the child drawable
 *         currently in use.
 */
shared_ptr<Drawable> Drawable::getCurrent() {
    return shared_from_this();
}

/**
 * Specify the level for the drawable.  This allows a drawable to vary its
 * imagery based on a continuous controller, for example to show progress
 * or volume level.
 *
 * <p>If the new level you are supplying causes the appearance of the
 * Drawable to change, then it is responsible for calling
 * {@link #invalidateSelf} in order to have itself redrawn, <em>and</em>
 * true will be returned from this function.
 *
 * @param level The new level, from 0 (minimum) to 10000 (maximum).
 *
 * @return Returns true if this change in level has caused the appearance
 * of the Drawable to change (hence requiring an invalidate), otherwise
 * returns false.
 */
const bool Drawable::setLevel(int level) {
    if (m_level != level) {
        m_level = level;
        return onLevelChange(level);
    }
    return false;
}

/**
 * Retrieve the current level.
 *
 * @return int Current level, from 0 (minimum) to 10000 (maximum).
 */
const int Drawable::getLevel() {
    return m_level;
}

/**
 * Set whether this Drawable is visible.  This generally does not impact
 * the Drawable's behavior, but is a hint that can be used by some
 * Drawables, for example, to decide whether run animations.
 *
 * @param visible Set to true if visible, false if not.
 * @param restart You can supply true here to force the drawable to behave
 *                as if it has just become visible, even if it had last
 *                been set visible.  Used for example to force animations
 *                to restart.
 *
 * @return bool Returns true if the new visibility is different than
 *         its previous state.
 */
bool Drawable::setVisible(bool visible, bool restart) {
    bool changed = m_visible != visible;
    if (changed) {
        m_visible = visible;
        invalidateSelf();
    }
    return changed;
}

const bool Drawable::isVisible() {
    return m_visible;
}

/**
 * Return the appropriate opacity value for two source opacities.  If
 * either is UNKNOWN, that is returned; else, if either is TRANSLUCENT,
 * that is returned; else, if either is TRANSPARENT, that is returned;
 * else, OPAQUE is returned.
 *
 * <p>This is to help in implementing {@link #getOpacity}.
 *
 * @param op1 One opacity value.
 * @param op2 Another opacity value.
 *
 * @return int The combined opacity value.
 *
 * @see #getOpacity
 */
int Drawable::resolveOpacity(int op1, int op2) {
    if (op1 == op2) {
        return op1;
    }
    if (op1 == PIXEL_FORMAT_UNKNOWN || op2 == PIXEL_FORMAT_UNKNOWN) {
        return PIXEL_FORMAT_UNKNOWN;
    }
    if (op1 == PIXEL_FORMAT_TRANSLUCENT || op2 == PIXEL_FORMAT_TRANSLUCENT) {
        return PIXEL_FORMAT_TRANSLUCENT;
    }
    if (op1 == PIXEL_FORMAT_TRANSPARENT || op2 == PIXEL_FORMAT_TRANSPARENT) {
        return PIXEL_FORMAT_TRANSPARENT;
    }
    return PIXEL_FORMAT_OPAQUE;
}

/**
 * Returns the minimum width suggested by this Drawable. If a View uses this
 * Drawable as a background, it is suggested that the View use at least this
 * value for its width. (There will be some scenarios where this will not be
 * possible.) This value should INCLUDE any padding.
 *
 * @return The minimum width suggested by this Drawable. If this Drawable
 *         doesn't have a suggested minimum width, 0 is returned.
 */
int Drawable::getMinimumWidth() {
    const int intrinsicWidth = getIntrinsicWidth();
    return intrinsicWidth > 0 ? intrinsicWidth : 0;
}

/**
 * Returns the minimum height suggested by this Drawable. If a View uses this
 * Drawable as a background, it is suggested that the View use at least this
 * value for its height. (There will be some scenarios where this will not be
 * possible.) This value should INCLUDE any padding.
 *
 * @return The minimum height suggested by this Drawable. If this Drawable
 *         doesn't have a suggested minimum height, 0 is returned.
 */
int Drawable::getMinimumHeight() {
    const int intrinsicHeight = getIntrinsicHeight();
    return intrinsicHeight > 0 ? intrinsicHeight : 0;
}

/**
 * Return in padding the insets suggested by this Drawable for placing
 * content inside the drawable's bounds. Positive values move toward the
 * center of the Drawable (set rect.inset). Returns true if this drawable
 * actually has a padding, else false. When false is returned, the padding
 * is always set to 0.
 */
bool Drawable::getPadding(Rect &padding) {
    padding.set(0, 0, 0, 0);
    return false;
}

ANDROID_END