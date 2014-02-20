//
//  MoneyMobileXAppDelegate.cpp
//  MoneyMobileX
//
//  Created by Saul Howard on 10/30/13.
//  Copyright __MyCompanyName__ 2013. All rights reserved.
//

#include "AndroidAppDelegate.h"

#include "cocos2d.h"

#include "Android.h"
#include "TestActivity.h"
#include "DialActivity.h"

USING_NS_CC;

#ifdef BUILD_FOR_ANDROID

#include "platform/android/jni/JniHelper.h"

static JNIEnv* getJNIEnv(void)
{
    
    JavaVM* jvm = cocos2d::JniHelper::getJavaVM();
    if (NULL == jvm) {
        CCLOG("Failed to get JNIEnv. JniHelper::getJavaVM() is NULL");
        return NULL;
    }
    
    JNIEnv *env = NULL;
    // get jni environment
    jint ret = jvm->GetEnv((void**)&env, JNI_VERSION_1_4);
    
    switch (ret) {
        case JNI_OK :
            // Success!
            return env;
            
        case JNI_EDETACHED :
            // Thread not attached
            
            // TODO : If calling AttachCurrentThread() on a native thread
            // must call DetachCurrentThread() in future.
            // see: http://developer.android.com/guide/practices/design/jni.html
            
            if (jvm->AttachCurrentThread(&env, NULL) < 0)
            {
                CCLOG("Failed to get the environment using AttachCurrentThread()");
                return NULL;
            } else {
                // Success : Attached and obtained JNIEnv!
                return env;
            }
            
        case JNI_EVERSION :
            // Cannot recover from this error
            CCLOG("JNI interface version 1.4 not supported");
        default :
            CCLOG("Failed to get the environment using GetEnv()");
            return NULL;
    }
}
#endif

AndroidAppDelegate::AndroidAppDelegate(int width, int height) {
    m_width = width;
    m_height = height;
}

AndroidAppDelegate::AndroidAppDelegate()
{

}

AndroidAppDelegate::~AndroidAppDelegate()
{
}

bool AndroidAppDelegate::applicationDidFinishLaunching()
{
    // initialize director
    CCDirector *pDirector = CCDirector::sharedDirector();
    pDirector->setOpenGLView(CCEGLView::sharedOpenGLView());

    // turn on display FPS
    pDirector->setDisplayStats(false);

    // set FPS. the default value is 1.0/60 if you don't call this
    pDirector->setAnimationInterval(1.0 / 60);
    
    if (!m_context) {
        const CCSize winSize = CCDirector::sharedDirector()->getWinSize();
        Resources resource = Resources(winSize.width, winSize.height, CCDevice::getDPI());
        m_context = new androidcpp::Context(resource);
    }
    
	// 'scene' is an autorelease object
    androidcpp::Activity *testActivity = TestActivity::create(m_context);
//    androidcpp::Activity *testActivity = DialActivity::create(m_context);
    
    setStartingScene(testActivity);

    // run
    pDirector->runWithScene(testActivity);
    
    return true;
}

void AndroidAppDelegate::setStartingScene(CCScene *pScene) {
	startingScene = pScene;
	startingScene->retain();
    
	return;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AndroidAppDelegate::applicationDidEnterBackground()
{
    CCDirector::sharedDirector()->stopAnimation();
}

// this function will be called when the app is active again
void AndroidAppDelegate::applicationWillEnterForeground()
{
    CCDirector::sharedDirector()->startAnimation();
}
