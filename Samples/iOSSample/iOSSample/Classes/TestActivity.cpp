/*
 * File:   TestActivity.cpp
 * Author: saulhoward
 *
 * Created on October 21, 2013, 10:04 AM
 */

#include "TestActivity.h"

#include "Android/widget/LinearLayout.h"

TestActivity::TestActivity(Context* context) : Activity(context) {
}

void TestActivity::onCreate() {
    Activity::onCreate();

    setContentView("res/layout/splash_view.xml");
    
    std::string testString("This is a test");
    
    mindroid::sp<Handler> handler = new Handler();
    handler->postDelayed([=] () { CCLOG("Handler test: %s", testString.c_str()); }, 2000);
    
    view = findViewById(std::string("view4"));
    if (view != NULL)
        view->setOnClickListener([] (View& view) { CCLOG("Clicked view %s", view.getId().c_str()); });
    
    LinearLayout *linear = (LinearLayout*) findViewById(std::string("linearlayout2"));
    if (linear != NULL)
        linear->setOnLongClickListener([] (View& view) { CCLOG("Long clicked linearlayout %s", view.getId().c_str()); });
}

TestActivity *TestActivity::create(Context *context) {
    TestActivity *pRet = new TestActivity(context);
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