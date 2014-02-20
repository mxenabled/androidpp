//
//  TimeView.h
//  MoneyMobileX
//
//  Created by Saul Howard on 12/11/13.
//
//

#ifndef MoneyMobileX_TimeView_h
#define MoneyMobileX_TimeView_h

#include "AndroidMacros.h"

#include "Android/view/View.h"

ANDROID_BEGIN

class TimeView : public View {
    
public:
    
    TimeView(Context *context) : TimeView(context, NULL) {
    }
    
    TimeView(Context *context, AttributeSet *attrs) : View(context, attrs) {
    }
    
    virtual void setViewAlpha(int alpha) = 0;
    virtual int getViewAlpha() = 0;
    virtual bool isTouchingView(int x, int y) = 0;
    virtual int getViewCenterX() = 0;
    virtual int getViewCenterY() = 0;
    virtual float getRadius() = 0;
    virtual float getRadiusPercent() = 0;
    virtual void setDeleted() = 0;
};

ANDROID_END

#endif
