/*
 * File:   Activity.h
 * Author: saulhoward
 *
 * Created on October 18, 2013, 12:22 PM
 */

#ifndef ACTIVITY_H
#define	ACTIVITY_H

#include "AndroidMacros.h"

#include "Android/content/Context.h"

#include "cocos2d.h"

#include <memory>
#include <string>

ANDROID_BEGIN

using namespace cocos2d;
using namespace std;

class Window;
class LayoutInflater;
class MotionEvent;

class Activity : public cocos2d::CCScene, public CCTouchDelegate, public Context {

    friend class Window;

public:

    Activity(Context *context);
    virtual ~Activity();

    View *findViewById(string id);
    Window *getWindow() { return m_window; }
    LayoutInflater *getLayoutInflater();
    
    // default implements are used to call script callback if exist
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
    
    // default implements are used to call script callback if exist
    virtual void ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent);
    virtual void ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent);
    virtual void ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent);
    virtual void ccTouchesCancelled(CCSet *pTouches, CCEvent *pEvent);
    
    bool dispatchTouchEvent(MotionEvent *ev);
    
    virtual bool init();

    virtual void onCreate();
    virtual void onEnter();
    virtual void onExit();
    
    virtual void onPause() {}
    virtual void onResume() {}
    virtual bool onTouchEvent(MotionEvent *ev);
    virtual bool visit(void);
    
    virtual void registerWithTouchDispatcher(void);
    void setContentView(const char* layoutResource);

    static Activity *create(Context *context);
private:
    Context *mContext = NULL;
    Window *m_window = NULL;
    bool m_called = false;
};

ANDROID_END

#endif	/* ACTIVITY_H */

