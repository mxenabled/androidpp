//
//  Drawable.h
//  cocos2dx
//
//  Created by Saul Howard on 11/13/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef __cocos2dx__Drawable__
#define __cocos2dx__Drawable__

#include "AndroidMacros.h"

#include "Android/content/res/Resources.h"
#include "Android/graphics/Canvas.h"
#include "Android/graphics/ColorFilter.h"
#include "Android/graphics/Insets.h"
#include "Android/graphics/Rect.h"
#include "Android/utils/AttributeSet.h"
#include "Android/utils/CCPullParser.h"

#include <ui/PixelFormat.h>

#include <mindroid/os/Runnable.h>
#include <mindroid/os/Ref.h>

#include <vector>
#include <memory>

ANDROID_BEGIN

using namespace mindroid;
using namespace std;

class Context;

class Drawable : public enable_shared_from_this<Drawable> {
    
public:
    
    class ConstantState {
        
    public:
        /**
         * Create a new drawable without supplying resources the caller
         * is running in.  Note that using this means the density-dependent
         * drawables (like bitmaps) will not be able to update their target
         * density correctly. One should use {@link #newDrawable(Resources)}
         * instead to provide a resource.
         */
        virtual shared_ptr<Drawable> newDrawable() = 0;
        /**
         * Create a new Drawable instance from its constant state.  This
         * must be implemented for drawables that change based on the target
         * density of their caller (that is depending on whether it is
         * in compatibility mode).
         */
        shared_ptr<Drawable> newDrawable(Resources *res) { return newDrawable(); }
        /**
         * Return a bit mask of configuration changes that will impact
         * this drawable (and thus require completely reloading it).
         */
        virtual int getChangingConfigurations() = 0;
    };
    
    /**
     * Implement this interface if you want to create an animated drawable that
     * extends {@link android.graphics.drawable.Drawable Drawable}.
     * Upon retrieving a drawable, use
     * {@link Drawable#setCallback(android.graphics.drawable.Drawable.Callback)}
     * to supply your implementation of the interface to the drawable; it uses
     * this interface to schedule and execute animation changes.
     */
    class Callback {
    public:
        /**
         * Called when the drawable needs to be redrawn.  A view at this point
         * should invalidate itself (or at least the part of itself where the
         * drawable appears).
         *
         * @param who The drawable that is requesting the update.
         */
        virtual void invalidateDrawable(shared_ptr<Drawable> who) = 0;
        
        /**
         * A Drawable can call this to schedule the next frame of its
         * animation.  An implementation can generally simply call
         * {@link android.os.Handler#postAtTime(Runnable, Object, long)} with
         * the parameters <var>(what, who, when)</var> to perform the
         * scheduling.
         *
         * @param who The drawable being scheduled.
         * @param what The action to execute.
         * @param when The time (in milliseconds) to run.  The timebase is
         *             {@link android.os.SystemClock#uptimeMillis}
         */
        virtual void scheduleDrawable(Drawable *who, const sp<Runnable> &what, uint32_t when) = 0;
        
        /**
         * A Drawable can call this to unschedule an action previously
         * scheduled with {@link #scheduleDrawable}.  An implementation can
         * generally simply call
         * {@link android.os.Handler#removeCallbacks(Runnable, Object)} with
         * the parameters <var>(what, who)</var> to unschedule the drawable.
         *
         * @param who The drawable being unscheduled.
         * @param what The action being unscheduled.
         */
        virtual void unscheduleDrawable(Drawable *who, const sp<Runnable> &what) = 0;
    };
    
    virtual ~Drawable() {}
    
    void clearColorFilter();
    const Rect *copyBounds();
    const void copyBounds(Rect &bounds);
    static shared_ptr<Drawable> createFromXmlInner(Resources *r, CCPullParser &parser);
    
    /**
     * Draw in its bounds (set via setBounds) respecting optional effects such
     * as alpha (set via setAlpha) and color filter (set via setColorFilter).
     *
     * @param canvas The canvas to draw into
     */
    virtual void draw(shared_ptr<Canvas> canvas) = 0;
    
    const Rect getBounds();
    Callback *getCallback();
    virtual int getChangingConfigurations();
    virtual ConstantState *getConstantState() { return NULL; }
    virtual shared_ptr<Drawable> getCurrent();
    
    /**
     * Return the intrinsic height of the underlying drawable object. Returns
     * -1 if it has no intrinsic height, such as with a solid color.
     */
    virtual int getIntrinsicHeight() { return -1; }
    virtual int getIntrinsicWidth() { return -1; }
    int getLayoutDirection();
    const int getLevel();
    virtual int getMinimumHeight();
    virtual int getMinimumWidth();
    
    /**
     * Return the opacity/transparency of this Drawable.  The returned value is
     * one of the abstract format constants in
     * {@link android.graphics.PixelFormat}:
     * {@link android.graphics.PixelFormat#UNKNOWN},
     * {@link android.graphics.PixelFormat#TRANSLUCENT},
     * {@link android.graphics.PixelFormat#TRANSPARENT}, or
     * {@link android.graphics.PixelFormat#OPAQUE}.
     *
     * <p>Generally a Drawable should be as conservative as possible with the
     * value it returns.  For example, if it contains multiple child drawables
     * and only shows one of them at a time, if only one of the children is
     * TRANSLUCENT and the others are OPAQUE then TRANSLUCENT should be
     * returned.  You can use the method {@link #resolveOpacity} to perform a
     * standard reduction of two opacities to the appropriate single output.
     *
     * <p>Note that the returned value does <em>not</em> take into account a
     * custom alpha or color filter that has been applied by the client through
     * the {@link #setAlpha} or {@link #setColorFilter} methods.
     *
     * @return int The opacity class of the Drawable.
     *
     * @see android.graphics.PixelFormat
     */
    virtual int getOpacity() = 0;
    virtual Insets getOpticalInsets() { return Insets::NONE; }
    virtual bool getPadding(Rect &padding);
    vector<int> getState();
    
    /**
     * Returns a Region representing the part of the Drawable that is completely
     * transparent.  This can be used to perform drawing operations, identifying
     * which parts of the target will not change when rendering the Drawable.
     * The default implementation returns NULL, indicating no transparent
     * region; subclasses can optionally override this to return an actual
     * Region if they want to supply this optimization information, but it is
     * not required that they do so.
     *
     * @return Returns NULL if the Drawables has no transparent region to
     * report, else a Region holding the parts of the Drawable's bounds that
     * are transparent.
     */
    Region *getTransparentRegion() { return NULL; }
    
    virtual void inflate(Resources *r, CCPullParser &parser, AttributeSet *attrs);
    void invalidateSelf();
    
    virtual bool isStateful();
    const bool isVisible();
    
    /**
     * If this Drawable does transition animations between states, ask that
     * it immediately jump to the current state and skip any active animations.
     */
    virtual void jumpToCurrentState() {}
    virtual shared_ptr<Drawable> mutate() { return shared_from_this(); }
    
    static int resolveOpacity(int op1, int op2);
    void scheduleSelf(sp<Runnable> &what, uint32_t when);
    
    virtual void setAlpha(int alpha) = 0;
    void setBounds(int left, int top, int right, int bottom);
    void setBounds(Rect bounds);
    void setCallback(Callback *cb);
    void setChangingConfigurations(int configs);
    virtual void setColorFilter(ColorFilter *cf) = 0;
//    void setColorFilter(int color, PorterDuff.Mode mode);
    
    /**
     * Set to true to have the drawable dither its colors when drawn to a device
     * with fewer than 8-bits per color component. This can improve the look on
     * those devices, but can also slow down the drawing a little.
     */
    virtual void setDither(bool dither) {}
    
    /**
     * Set to true to have the drawable filter its bitmap when scaled or rotated
     * (for drawables that use bitmaps). If the drawable does not use bitmaps,
     * this call is ignored. This can improve the look when scaled or rotated,
     * but also slows down the drawing.
     */
    virtual void setFilterBitmap(bool filter) {}
    void setLayoutDirection(int layoutDirection);
    const bool setLevel(int level);
    bool setState(const vector<int> stateSet);
    bool setVisible(bool visible, bool restart);
    void unscheduleSelf(sp<Runnable> &what);
    
protected:
    
    virtual void onBoundsChange(Rect bounds) {}
    virtual bool onLevelChange(int level) { return false; }
    virtual bool onStateChange(vector<int> state) { return false; }
    
    virtual void inflateWithAttributes(Resources *r, CCPullParser &parser, AttributeSet *attrs, const char* visibleAttr);
    
private:
    
    static Rect ZERO_BOUNDS_RECT;
    
    Rect m_bounds;
    Callback *m_callback = NULL;
    int m_layoutDirection = 0;
    int m_level = 0;
    int m_changingConfigurations = 0;
    vector<int> m_stateSet;
    bool m_visible = false;
};

ANDROID_END

#endif /* defined(__cocos2dx__Drawable__) */
