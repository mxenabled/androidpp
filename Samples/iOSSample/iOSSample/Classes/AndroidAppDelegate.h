//
//  MoneyMobileXAppDelegate.h
//  MoneyMobileX
//
//  Created by Saul Howard on 10/30/13.
//  Copyright __MyCompanyName__ 2013. All rights reserved.
//

#ifndef  _APP_DELEGATE_H_
#define  _APP_DELEGATE_H_

#include "CCApplication.h"
#include "cocos2d.h"

using namespace cocos2d;

namespace androidcpp {
    class Context;
}

/**
@brief    The cocos2d Application.

The reason to implement with private inheritance is to hide some interface details of CCDirector.
*/
class  AndroidAppDelegate : private cocos2d::CCApplication
{
public:
    AndroidAppDelegate(int width, int height);
    AndroidAppDelegate();
    virtual ~AndroidAppDelegate();

    /**
    @brief    Implement CCDirector and CCScene init code here.
    @return true    Initialize success, app continue.
    @return false   Initialize failed, app terminate.
    */
    virtual bool applicationDidFinishLaunching();

    /**
    @brief  The function is called when the application enters the background
    @param  the pointer of the application instance
    */
    virtual void applicationDidEnterBackground();

    /**
    @brief  The function is called when the application enters the foreground
    @param  the pointer of the application instance
    */
    virtual void applicationWillEnterForeground();
    
    
    void setStartingScene(CCScene *pScene);
    CCScene *startingScene;
    
private:
    
    androidcpp::Context *m_context = NULL;
    
    int m_width = 0;
    int m_height = 0;

};

#endif // _APP_DELEGATE_H_

