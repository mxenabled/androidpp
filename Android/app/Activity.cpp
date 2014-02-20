/*
 * File:   Activity.cpp
 * Author: saulhoward
 *
 * Created on October 18, 2013, 12:22 PM
 */

#include "Activity.h"

#include "Android/app/Window.h"
#include "Android/view/LayoutInflater.h"
#include "Android/view/MotionEvent.h"

ANDROID_BEGIN

Activity::Activity(Context *context) : Context(context->m_resources) {

    mContext = context;
    m_window = new Window(this);
}

Activity::~Activity() {
}

View *Activity::findViewById(std::string id) {
    return getWindow()->findViewById(id);
}

LayoutInflater *Activity::getLayoutInflater() {
    return getWindow()->getLayoutInflater();
}

bool Activity::init() {

    if (CCScene::init()) {
        onCreate();
    }

    return true;
}

void Activity::onCreate() {
    m_called = true;
}

void Activity::onEnter() {
    CCScene::onEnter();
    this->registerWithTouchDispatcher();
    onResume();
}

void Activity::onExit() {
    CCScene::onExit();
    onPause();
}

void Activity::setContentView(const char* layoutResource) {
    getWindow()->setContentView(layoutResource);
}

Activity *Activity::create(Context *context) {
    Activity *pRet = new Activity(context);
    if (pRet && pRet->init())
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        CC_SAFE_DELETE(pRet);
        return NULL;
    }
}

bool Activity::dispatchTouchEvent(MotionEvent *ev) {
    
    if (getWindow()->superDispatchTouchEvent(ev)) {
        return true;
    }
    
    return onTouchEvent(ev);
}

bool Activity::onTouchEvent(androidcpp::MotionEvent *ev) {
    return false;
}

bool Activity::visit() {
    if (m_window && m_window->m_root != NULL) {
//        m_window->m_root->performDraw();
//        bool dirty = true;
        bool dirty = m_window->m_root->m_isDirty;
        m_window->m_root->m_isDirty = false;
        return dirty;
    }
    
    return false;
}

/// Touch and Accelerometer related

void Activity::registerWithTouchDispatcher()
{
    CCTouchDispatcher* pDispatcher = CCDirector::sharedDirector()->getTouchDispatcher();
    pDispatcher->addStandardDelegate(this, 0);
}

bool Activity::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    CCSet set;
    set.addObject(pTouch);
    MotionEvent event;
    event.init(&set, MotionEvent::ACTION_DOWN);
    
    dispatchTouchEvent(&event);
    
    CC_UNUSED_PARAM(pTouch);
    CC_UNUSED_PARAM(pEvent);
    CCAssert(false, "Layer#ccTouchBegan override me");
    return true;
}

void Activity::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
    CCSet set;
    set.addObject(pTouch);
    MotionEvent event;
    event.init(&set, MotionEvent::ACTION_MOVE);
    
    dispatchTouchEvent(&event);
    
    CC_UNUSED_PARAM(pTouch);
    CC_UNUSED_PARAM(pEvent);
}

void Activity::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
    CCSet set;
    set.addObject(pTouch);
    MotionEvent event;
    event.init(&set, MotionEvent::ACTION_UP);
    
    dispatchTouchEvent(&event);
    
    CC_UNUSED_PARAM(pTouch);
    CC_UNUSED_PARAM(pEvent);
}

void Activity::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent)
{
    CCSet set;
    set.addObject(pTouch);
    MotionEvent event;
    event.init(&set, MotionEvent::ACTION_CANCEL);
    
    dispatchTouchEvent(&event);
    
    CC_UNUSED_PARAM(pTouch);
    CC_UNUSED_PARAM(pEvent);
}

void Activity::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent)
{
    MotionEvent event;
    event.init(pTouches, MotionEvent::ACTION_DOWN);
    
    dispatchTouchEvent(&event);
    
    CC_UNUSED_PARAM(pTouches);
    CC_UNUSED_PARAM(pEvent);
}

void Activity::ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent)
{
    MotionEvent event;
    event.init(pTouches, MotionEvent::ACTION_MOVE);
    
    dispatchTouchEvent(&event);
    
    CC_UNUSED_PARAM(pTouches);
    CC_UNUSED_PARAM(pEvent);
}

void Activity::ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent)
{
    MotionEvent event;
    event.init(pTouches, MotionEvent::ACTION_UP);
    
    dispatchTouchEvent(&event);
    
    CC_UNUSED_PARAM(pTouches);
    CC_UNUSED_PARAM(pEvent);
}

void Activity::ccTouchesCancelled(CCSet *pTouches, CCEvent *pEvent)
{
    MotionEvent event;
    event.init(pTouches, MotionEvent::ACTION_CANCEL);
    
    dispatchTouchEvent(&event);
    
    CC_UNUSED_PARAM(pTouches);
    CC_UNUSED_PARAM(pEvent);
}

ANDROID_END