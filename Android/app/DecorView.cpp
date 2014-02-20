/*
 * File:   DecorView.cpp
 * Author: saulhoward
 *
 * Created on October 18, 2013, 6:24 PM
 */

#include "DecorView.h"

ANDROID_BEGIN

DecorView::DecorView(Context *context, int featureId) : FrameLayout(context),
    m_featureId(featureId) {
}

DecorView::~DecorView() {
}

bool DecorView::superDispatchTouchEvent(MotionEvent *ev) {
    return ViewGroup::dispatchTouchEvent(ev);
}

ANDROID_END