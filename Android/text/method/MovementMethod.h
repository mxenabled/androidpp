//
//  MovementMethod.h
//  Androidpp
//
//  Created by Saul Howard on 1/28/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef Androidpp_MovementMethod_h
#define Androidpp_MovementMethod_h

#include "AndroidMacros.h"

#include "Android/utils/Object.h"

#include <memory>

ANDROID_BEGIN

class TextView;
class Spannable;
class KeyEvent;
class MotionEvent;

/**
 * Provides cursor positioning, scrolling and text selection functionality in a {@link TextView}.
 * <p>
 * The {@link TextView} delegates handling of key events, trackball motions and touches to
 * the movement method for purposes of content navigation.  The framework automatically
 * selects an appropriate movement method based on the content of the {@link TextView}.
 * </p><p>
 * This interface is intended for use by the framework; it should not be implemented
 * directly by applications.
 * </p>
 */
class MovementMethod : public virtual Object {
    
public:
    
    virtual void initialize(TextView *widget, shared_ptr<Spannable> text) = 0;
    virtual bool onKeyDown(TextView *widget, shared_ptr<Spannable> text, int keyCode, KeyEvent *event) = 0;
    virtual bool onKeyUp(TextView *widget, shared_ptr<Spannable> text, int keyCode, KeyEvent *event) = 0;
    
    /**
     * If the key listener wants to other kinds of key events, return true,
     * otherwise return false and the caller (i.e. the widget host)
     * will handle the key.
     */
    virtual bool onKeyOther(TextView *view, shared_ptr<Spannable> text, KeyEvent *event) = 0;
    
    virtual void onTakeFocus(TextView *widget, shared_ptr<Spannable> text, int direction) = 0;
    virtual bool onTrackballEvent(TextView *widget, shared_ptr<Spannable> text, MotionEvent *event) = 0;
    virtual bool onTouchEvent(TextView *widget, shared_ptr<Spannable> text, MotionEvent *event) = 0;
    virtual bool onGenericMotionEvent(TextView *widget, shared_ptr<Spannable> text, MotionEvent *event) = 0;
    
    /**
     * Returns true if this movement method allows arbitrary selection
     * of any text; false if it has no selection (like a movement method
     * that only scrolls) or a constrained selection (for example
     * limited to links.  The "Select All" menu item is disabled
     * if arbitrary selection is not allowed.
     */
    virtual bool canSelectArbitrarily() = 0;
};

ANDROID_END

#endif
