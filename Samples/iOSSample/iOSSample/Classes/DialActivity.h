//
//  DialActivity.h
//  MoneyMobileX
//
//  Created by Saul Howard on 1/6/14.
//
//

#ifndef __MoneyMobileX__DialActivity__
#define __MoneyMobileX__DialActivity__

#include "Android.h"

#include "DialView.h"

using namespace androidcpp;

class DialActivity : public Activity {
public:
    DialActivity(Context *context);
    virtual ~DialActivity() {}
    
    virtual void onCreate();
    
    static DialActivity *create(Context *context);
};


#endif /* defined(__MoneyMobileX__DialActivity__) */
