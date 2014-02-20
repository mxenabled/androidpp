/*
 * File:   DecorView.h
 * Author: saulhoward
 *
 * Created on October 18, 2013, 6:24 PM
 */

#ifndef DECORVIEW_H
#define	DECORVIEW_H

#include "AndroidMacros.h"

#include "Android/widget/FrameLayout.h"

ANDROID_BEGIN

class MotionEvent;

class DecorView : public FrameLayout {
public:
    DecorView(Context *context, int featureId);
    virtual ~DecorView();
    
    virtual bool canResolveLayoutDirection() { return View::canResolveLayoutDirection(); }
    bool superDispatchTouchEvent(MotionEvent *ev);
    
private:
    int m_featureId;
};

ANDROID_END

#endif	/* DECORVIEW_H */

