/*
 * File:   Window.h
 * Author: saulhoward
 *
 * Created on October 18, 2013, 5:59 PM
 */

#ifndef WINDOW_H
#define	WINDOW_H

#include "AndroidMacros.h"

#include "Android/app/DecorView.h"
#include "Android/content/Context.h"
#include "Android/view/LayoutInflater.h"
#include "Android/view/View.h"
#include "Android/view/ViewGroup.h"
#include "Android/view/LayoutParams.h"
#include "Android/view/ViewRootImpl.h"

#include <memory>

using namespace std;

ANDROID_BEGIN

class Activity;
class MotionEvent;

class Window {

    friend class Activity;
    
public:
    Window(Activity *activity);
    virtual ~Window();

    View *findViewById(std::string id) { return getDecorView()->findViewById(id); }
    Context *getContext() { return mContext; }
    shared_ptr<View> getDecorView() { return m_decor; }
    /**
     * Quick access to the {@link LayoutInflater} instance that this Window
     * retrieved from its Context.
     *
     * @return LayoutInflater The shared LayoutInflater.
     */
    LayoutInflater *getLayoutInflater() { return m_layoutInflater; }
    void setContentView(const char* layoutResource);
    void setContentView(shared_ptr<View> view, LayoutParams *params);
    bool superDispatchTouchEvent(MotionEvent *ev);
private:
    Activity *m_activity = NULL;

    Context *mContext = NULL;
    LayoutInflater *m_layoutInflater = NULL;
    shared_ptr<ViewGroup> m_contentParent;
    shared_ptr<DecorView> m_decor;
	shared_ptr<ViewRootImpl> m_root;

    void installDecor();
protected:
    shared_ptr<DecorView> generateDecor();
    shared_ptr<ViewGroup> generateLayout(shared_ptr<DecorView> decor);
};

ANDROID_END

#endif	/* WINDOW_H */

