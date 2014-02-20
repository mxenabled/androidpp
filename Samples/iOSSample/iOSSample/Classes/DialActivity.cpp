//
//  DialActivity.cpp
//  MoneyMobileX
//
//  Created by Saul Howard on 1/6/14.
//
//

#include "DialActivity.h"



DialActivity::DialActivity(Context* context) : Activity(context) {
}

void DialActivity::onCreate() {
    Activity::onCreate();
    
    setContentView("res/layout/dial_layout.xml");
    
    shared_ptr<Drawable> dial = getResources()->loadDrawable("@drawable/circle_dial");
    
    DialView *dialView = (DialView*) findViewById(std::string("dialview"));
    dialView->setDialImage(dial);
}

DialActivity *DialActivity::create(Context *context) {
    DialActivity *pRet = new DialActivity(context);
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