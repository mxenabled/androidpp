/*
 * File:   Window.cpp
 * Author: saulhoward
 *
 * Created on October 18, 2013, 5:59 PM
 */

#include "Window.h"

#include "Android/app/Activity.h"

#include <string.h>

ANDROID_BEGIN

Window::Window(Activity *activity) {
    m_activity = activity;
    mContext = activity->mContext;
    m_layoutInflater = LayoutInflater::from(mContext);
}

Window::~Window() {
}

shared_ptr<DecorView> Window::generateDecor() {
    return make_shared<DecorView>(getContext(), -1);
}

shared_ptr<ViewGroup> Window::generateLayout(shared_ptr<DecorView> decor) {

    int width = getContext()->getResources()->getDisplayMetrics().widthPixels;
    int height = getContext()->getResources()->getDisplayMetrics().heightPixels;
    
    m_root = make_shared<ViewRootImpl>(mContext);
    decor->setId(std::string("decor"));
    decor->setLayoutParams(new LayoutParams(width, height));
    m_root->setView(decor, NULL, NULL);

    return decor;
}

void Window::installDecor() {

    if (m_decor == NULL) {
        m_decor = generateDecor();
        m_decor->setIsRootNamespace(true);
//        if (!mInvalidatePanelMenuPosted && mInvalidatePanelMenuFeatures != 0) {
//            m_decor.postOnAnimation(mInvalidatePanelMenuRunnable);
//        }
    }

    if (m_contentParent == NULL) {
        m_contentParent = generateLayout(m_decor);
    }
}

void Window::setContentView(const char* layoutResource) {

    if (m_contentParent == NULL) {
        installDecor();
    } else {
        m_contentParent->removeAllViews();
    }

    m_layoutInflater->inflate(layoutResource, m_contentParent);
    
    m_contentParent->requestLayout();
}

void Window::setContentView(shared_ptr<View> view, LayoutParams *params) {

    if (m_contentParent == NULL) {
        installDecor();
    } else {
        m_contentParent->removeAllViews();
    }

    m_contentParent->addView(view, params);

    /*final Callback cb = getCallback();
    if (cb != NULL && !isDestroyed()) {
        cb.onContentChanged();
    }*/
}

bool Window::superDispatchTouchEvent(androidcpp::MotionEvent *ev) {
    return m_decor->superDispatchTouchEvent(ev);
}

ANDROID_END