//
//  ViewTreeObserver.h
//  Androidpp
//
//  Created by Saul Howard on 1/14/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef __Androidpp__ViewTreeObserver__
#define __Androidpp__ViewTreeObserver__

#include "AndroidMacros.h"

#include "Android/graphics/Rect.h"
#include "Android/graphics/Region.h"
#include "Android/utils/Exceptions.h"
#include "Android/view/View.h"

#include <vector>

using namespace std;

ANDROID_BEGIN

/**
 * A view tree observer is used to register listeners that can be notified of global
 * changes in the view tree. Such global events include, but are not limited to,
 * layout of the whole tree, beginning of the drawing pass, touch mode change....
 *
 * A ViewTreeObserver should never be instantiated by applications as it is provided
 * by the views hierarchy. Refer to {@link android.view.View#getViewTreeObserver()}
 * for more information.
 */
class ViewTreeObserver {
    
    friend class AttachInfo;
    friend class View;
    
public:
    
    /**
     * Interface definition for a callback to be invoked when the view hierarchy is
     * attached to and detached from its window.
     */
    class OnWindowAttachListener {
        
    public:
        
        /**
         * Callback method to be invoked when the view hierarchy is attached to a window
         */
        virtual void onWindowAttached() = 0;
        
        /**
         * Callback method to be invoked when the view hierarchy is detached from a window
         */
        virtual void onWindowDetached() = 0;
    };
    
    /**
     * Interface definition for a callback to be invoked when the view hierarchy's window
     * focus state changes.
     */
    class OnWindowFocusChangeListener {
        
    public:
        
        /**
         * Callback method to be invoked when the window focus changes in the view tree.
         *
         * @param hasFocus Set to true if the window is gaining focus, false if it is
         * losing focus.
         */
        virtual void onWindowFocusChanged(bool hasFocus) = 0;
    };
    
    /**
     * Interface definition for a callback to be invoked when the focus state within
     * the view tree changes.
     */
    class OnGlobalFocusChangeListener {
        
    public:
        
        /**
         * Callback method to be invoked when the focus changes in the view tree. When
         * the view tree transitions from touch mode to non-touch mode, oldFocus is NULL.
         * When the view tree transitions from non-touch mode to touch mode, newFocus is
         * NULL. When focus changes in non-touch mode (without transition from or to
         * touch mode) either oldFocus or newFocus can be NULL.
         *
         * @param oldFocus The previously focused view, if any.
         * @param newFocus The newly focused View, if any.
         */
        virtual void onGlobalFocusChanged(shared_ptr<View> oldFocus, shared_ptr<View> newFocus) = 0;
    };
    
    /**
     * Interface definition for a callback to be invoked when the global layout state
     * or the visibility of views within the view tree changes.
     */
    class OnGlobalLayoutListener {
        
    public:
        
        /**
         * Callback method to be invoked when the global layout state or the visibility of views
         * within the view tree changes
         */
        virtual void onGlobalLayout() = 0;
    };
    
    /**
     * Interface definition for a callback to be invoked when the view tree is about to be drawn.
     */
    class OnPreDrawListener {
        
    public:
        
        /**
         * Callback method to be invoked when the view tree is about to be drawn. At this point, all
         * views in the tree have been measured and given a frame. Clients can use this to adjust
         * their scroll bounds or even to request a new layout before drawing occurs.
         *
         * @return Return true to proceed with the current drawing pass, or false to cancel.
         *
         * @see android.view.View#onMeasure
         * @see android.view.View#onLayout
         * @see android.view.View#onDraw
         */
        virtual bool onPreDraw() = 0;
    };
    
    /**
     * Interface definition for a callback to be invoked when the view tree is about to be drawn.
     */
    class OnDrawListener {
        
    public:
        
        /**
         * <p>Callback method to be invoked when the view tree is about to be drawn. At this point,
         * views cannot be modified in any way.</p>
         *
         * <p>Unlike with {@link OnPreDrawListener}, this method cannot be used to cancel the
         * current drawing pass.</p>
         *
         * <p>An {@link OnDrawListener} listener <strong>cannot be added or removed</strong>
         * from this method.</p>
         *
         * @see android.view.View#onMeasure
         * @see android.view.View#onLayout
         * @see android.view.View#onDraw
         */
        virtual void onDraw() = 0;
    };
    
    /**
     * Interface definition for a callback to be invoked when the touch mode changes.
     */
    class OnTouchModeChangeListener {
        
    public:
        
        /**
         * Callback method to be invoked when the touch mode changes.
         *
         * @param isInTouchMode True if the view hierarchy is now in touch mode, false  otherwise.
         */
        virtual void onTouchModeChanged(bool isInTouchMode) = 0;
    };
    
    /**
     * Interface definition for a callback to be invoked when
     * something in the view tree has been scrolled.
     */
    class OnScrollChangedListener {
        
    public:
        
        /**
         * Callback method to be invoked when something in the view tree
         * has been scrolled.
         */
        virtual void onScrollChanged() = 0;
    };
    
    /**
     * Parameters used with OnComputeInternalInsetsListener.
     *
     * We are not yet ready to commit to this API and support it, so
     * @hide
     */
    class InternalInsetsInfo {
        
    public:
        
        /**
         * Offsets from the frame of the window at which the content of
         * windows behind it should be placed.
         */
        Rect contentInsets;
        
        /**
         * Offsets from the frame of the window at which windows behind it
         * are visible.
         */
        Rect visibleInsets;
        
        /**
         * Touchable region defined relative to the origin of the frame of the window.
         * Only used when {@link #setTouchableInsets(int)} is called with
         * the option {@link #TOUCHABLE_INSETS_REGION}.
         */
        Region touchableRegion;
        
        /**
         * Option for {@link #setTouchableInsets(int)}: the entire window frame
         * can be touched.
         */
        static const int TOUCHABLE_INSETS_FRAME = 0;
        
        /**
         * Option for {@link #setTouchableInsets(int)}: the area inside of
         * the content insets can be touched.
         */
        static const int TOUCHABLE_INSETS_CONTENT = 1;
        
        /**
         * Option for {@link #setTouchableInsets(int)}: the area inside of
         * the visible insets can be touched.
         */
        static const int TOUCHABLE_INSETS_VISIBLE = 2;
        
        /**
         * Option for {@link #setTouchableInsets(int)}: the area inside of
         * the provided touchable region in {@link #touchableRegion} can be touched.
         */
        static const int TOUCHABLE_INSETS_REGION = 3;
        
        /**
         * Set which parts of the window can be touched: either
         * {@link #TOUCHABLE_INSETS_FRAME}, {@link #TOUCHABLE_INSETS_CONTENT},
         * {@link #TOUCHABLE_INSETS_VISIBLE}, or {@link #TOUCHABLE_INSETS_REGION}.
         */
        void setTouchableInsets(int val) {
            mTouchableInsets = val;
        }
        
        int hashCode();
        
        bool equals(InternalInsetsInfo *other);
        
    private:
        
        int mTouchableInsets;
        
        void reset();
        
        void set(InternalInsetsInfo other);
    };
    
    /**
     * Interface definition for a callback to be invoked when layout has
     * completed and the client can compute its interior insets.
     *
     * We are not yet ready to commit to this API and support it, so
     * @hide
     */
    class OnComputeInternalInsetsListener {
        
    public:
        
        /**
         * Callback method to be invoked when layout has completed and the
         * client can compute its interior insets.
         *
         * @param inoutInfo Should be filled in by the implementation with
         * the information about the insets of the window.  This is called
         * with whatever values the previous OnComputeInternalInsetsListener
         * returned, if there are multiple such listeners in the window.
         */
        virtual void onComputeInternalInsets(InternalInsetsInfo inoutInfo) = 0;
    };
    
private:
    
    // Recursive listeners use vector
    vector<OnWindowFocusChangeListener*> mOnWindowFocusListeners;
    vector<OnWindowAttachListener*> mOnWindowAttachListeners;
    vector<OnGlobalFocusChangeListener*> mOnGlobalFocusListeners;
    vector<OnTouchModeChangeListener*> mOnTouchModeChangeListeners;
    
    // Non-recursive listeners use vector
    // Any listener invoked from ViewRootImpl.performTraversals() should not be recursive
    vector<OnGlobalLayoutListener*> mOnGlobalLayoutListeners;
    vector<OnComputeInternalInsetsListener*> mOnComputeInternalInsetsListeners;
    vector<OnScrollChangedListener*> mOnScrollChangedListeners;
    vector<OnPreDrawListener*> mOnPreDrawListeners;
    
    // These listeners cannot be mutated during dispatch
    vector<OnDrawListener*> mOnDrawListeners;
    
    bool mAlive = true;
    
    /**
     * Creates a new ViewTreeObserver. This constructor should not be called
     */
    ViewTreeObserver() {
    }
    
    /**
     * Merges all the listeners registered on the specified observer with the listeners
     * registered on this object. After this method is invoked, the specified observer
     * will return false in {@link #isAlive()} and should not be used anymore.
     *
     * @param observer The ViewTreeObserver whose listeners must be added to this observer
     */
    void merge(ViewTreeObserver *observer);
    
public:
    
    /**
     * Register a callback to be invoked when the view hierarchy is attached to a window.
     *
     * @param listener The callback to add
     *
     * @throws IllegalStateException If {@link #isAlive()} returns false
     */
    void addOnWindowAttachListener(OnWindowAttachListener *listener);
    
    /**
     * Remove a previously installed window attach callback.
     *
     * @param victim The callback to remove
     *
     * @throws IllegalStateException If {@link #isAlive()} returns false
     *
     * @see #addOnWindowAttachListener(android.view.ViewTreeObserver.OnWindowAttachListener)
     */
    void removeOnWindowAttachListener(OnWindowAttachListener *victim);
    
    /**
     * Register a callback to be invoked when the window focus state within the view tree changes.
     *
     * @param listener The callback to add
     *
     * @throws IllegalStateException If {@link #isAlive()} returns false
     */
    void addOnWindowFocusChangeListener(OnWindowFocusChangeListener *listener);
    
    /**
     * Remove a previously installed window focus change callback.
     *
     * @param victim The callback to remove
     *
     * @throws IllegalStateException If {@link #isAlive()} returns false
     *
     * @see #addOnWindowFocusChangeListener(android.view.ViewTreeObserver.OnWindowFocusChangeListener)
     */
    void removeOnWindowFocusChangeListener(OnWindowFocusChangeListener *victim);
    
    /**
     * Register a callback to be invoked when the focus state within the view tree changes.
     *
     * @param listener The callback to add
     *
     * @throws IllegalStateException If {@link #isAlive()} returns false
     */
    void addOnGlobalFocusChangeListener(OnGlobalFocusChangeListener *listener);
    
    /**
     * Remove a previously installed focus change callback.
     *
     * @param victim The callback to remove
     *
     * @throws IllegalStateException If {@link #isAlive()} returns false
     *
     * @see #addOnGlobalFocusChangeListener(OnGlobalFocusChangeListener)
     */
    void removeOnGlobalFocusChangeListener(OnGlobalFocusChangeListener *victim);
    
    /**
     * Register a callback to be invoked when the global layout state or the visibility of views
     * within the view tree changes
     *
     * @param listener The callback to add
     *
     * @throws IllegalStateException If {@link #isAlive()} returns false
     */
    void addOnGlobalLayoutListener(OnGlobalLayoutListener *listener);
    
    /**
     * Remove a previously installed global layout callback
     *
     * @param victim The callback to remove
     *
     * @throws IllegalStateException If {@link #isAlive()} returns false
     *
     * @see #addOnGlobalLayoutListener(OnGlobalLayoutListener)
     */
    void removeOnGlobalLayoutListener(OnGlobalLayoutListener *victim);
    
    /**
     * Register a callback to be invoked when the view tree is about to be drawn
     *
     * @param listener The callback to add
     *
     * @throws IllegalStateException If {@link #isAlive()} returns false
     */
    void addOnPreDrawListener(OnPreDrawListener *listener);
    
    /**
     * Remove a previously installed pre-draw callback
     *
     * @param victim The callback to remove
     *
     * @throws IllegalStateException If {@link #isAlive()} returns false
     *
     * @see #addOnPreDrawListener(OnPreDrawListener)
     */
    void removeOnPreDrawListener(OnPreDrawListener *victim);
    
    /**
     * <p>Register a callback to be invoked when the view tree is about to be drawn.</p>
     * <p><strong>Note:</strong> this method <strong>cannot</strong> be invoked from
     * {@link android.view.ViewTreeObserver.OnDrawListener#onDraw()}.</p>
     *
     * @param listener The callback to add
     *
     * @throws IllegalStateException If {@link #isAlive()} returns false
     */
    void addOnDrawListener(OnDrawListener *listener);
    
    /**
     * <p>Remove a previously installed pre-draw callback.</p>
     * <p><strong>Note:</strong> this method <strong>cannot</strong> be invoked from
     * {@link android.view.ViewTreeObserver.OnDrawListener#onDraw()}.</p>
     *
     * @param victim The callback to remove
     *
     * @throws IllegalStateException If {@link #isAlive()} returns false
     *
     * @see #addOnDrawListener(OnDrawListener)
     */
    void removeOnDrawListener(OnDrawListener *victim);
    
    /**
     * Register a callback to be invoked when a view has been scrolled.
     *
     * @param listener The callback to add
     *
     * @throws IllegalStateException If {@link #isAlive()} returns false
     */
    void addOnScrollChangedListener(OnScrollChangedListener *listener);
    
    /**
     * Remove a previously installed scroll-changed callback
     *
     * @param victim The callback to remove
     *
     * @throws IllegalStateException If {@link #isAlive()} returns false
     *
     * @see #addOnScrollChangedListener(OnScrollChangedListener)
     */
    void removeOnScrollChangedListener(OnScrollChangedListener *victim);
    
    /**
     * Register a callback to be invoked when the invoked when the touch mode changes.
     *
     * @param listener The callback to add
     *
     * @throws IllegalStateException If {@link #isAlive()} returns false
     */
    void addOnTouchModeChangeListener(OnTouchModeChangeListener *listener);
    
    /**
     * Remove a previously installed touch mode change callback
     *
     * @param victim The callback to remove
     *
     * @throws IllegalStateException If {@link #isAlive()} returns false
     *
     * @see #addOnTouchModeChangeListener(OnTouchModeChangeListener)
     */
    void removeOnTouchModeChangeListener(OnTouchModeChangeListener *victim);
    
    /**
     * Register a callback to be invoked when the invoked when it is time to
     * compute the window's internal insets.
     *
     * @param listener The callback to add
     *
     * @throws IllegalStateException If {@link #isAlive()} returns false
     *
     * We are not yet ready to commit to this API and support it, so
     * @hide
     */
    void addOnComputeInternalInsetsListener(OnComputeInternalInsetsListener *listener);
    
    /**
     * Remove a previously installed internal insets computation callback
     *
     * @param victim The callback to remove
     *
     * @throws IllegalStateException If {@link #isAlive()} returns false
     *
     * @see #addOnComputeInternalInsetsListener(OnComputeInternalInsetsListener)
     *
     * We are not yet ready to commit to this API and support it, so
     * @hide
     */
    void removeOnComputeInternalInsetsListener(OnComputeInternalInsetsListener *victim);
    
    /**
     * Indicates whether this ViewTreeObserver is alive. When an observer is not alive,
     * any call to a method (except this one) will throw an exception.
     *
     * If an application keeps a long-lived reference to this ViewTreeObserver, it should
     * always check for the result of this method before calling any other method.
     *
     * @return True if this object is alive and be used, false otherwise.
     */
    bool isAlive() { return mAlive; }
    
private:
    
    void checkIsAlive();
    
    /**
     * Marks this ViewTreeObserver as not alive. After invoking this method, invoking
     * any other method but {@link #isAlive()} and {@link #kill()} will throw an Exception.
     *
     * @hide
     */
    void kill() { mAlive = false; }
    
    /**
     * Notifies registered listeners that window has been attached/detached.
     */
    void dispatchOnWindowAttachedChange(bool attached);
    
    /**
     * Notifies registered listeners that window focus has changed.
     */
    void dispatchOnWindowFocusChange(bool hasFocus);
    
    /**
     * Notifies registered listeners that focus has changed.
     */
    void dispatchOnGlobalFocusChange(shared_ptr<View> oldFocus, shared_ptr<View> newFocus);
    
public:
    
    /**
     * Notifies registered listeners that a global layout happened. This can be called
     * manually if you are forcing a layout on a View or a hierarchy of Views that are
     * not attached to a Window or in the GONE state.
     */
    void dispatchOnGlobalLayout();
    
    /**
     * Notifies registered listeners that the drawing pass is about to start. If a
     * listener returns true, then the drawing pass is canceled and rescheduled. This can
     * be called manually if you are forcing the drawing on a View or a hierarchy of Views
     * that are not attached to a Window or in the GONE state.
     *
     * @return True if the current draw should be canceled and resceduled, false otherwise.
     */
    bool dispatchOnPreDraw();
    
    /**
     * Notifies registered listeners that the drawing pass is about to start.
     */
    void dispatchOnDraw();
    
private:
    
    /**
     * Notifies registered listeners that the touch mode has changed.
     *
     * @param inTouchMode True if the touch mode is now enabled, false otherwise.
     */
    void dispatchOnTouchModeChanged(bool inTouchMode);
    
    /**
     * Notifies registered listeners that something has scrolled.
     */
    void dispatchOnScrollChanged();
    
    /**
     * Returns whether there are listeners for computing internal insets.
     */
    bool hasComputeInternalInsetsListeners();
    
    /**
     * Calls all listeners to compute the current insets.
     */
    void dispatchOnComputeInternalInsets(InternalInsetsInfo inoutInfo);
};

ANDROID_END

#endif /* defined(__Androidpp__ViewTreeObserver__) */
