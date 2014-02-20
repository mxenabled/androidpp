//
//  CustomView.h
//  MoneyMobileX
//
//  Created by Saul Howard on 12/2/13.
//
//

#ifndef __MoneyMobileX__CustomView__
#define __MoneyMobileX__CustomView__

#include "Android/view/View.h"
#include "Android/graphics/Paint.h"

#include <unicode/unistr.h>

#include "cocos2d.h"

using namespace icu;
using namespace androidcpp;

class CustomView : public View {
    
public:
    
    CustomView(Context *context, AttributeSet *attrs) : View(context, attrs), m_text("This is a test") {
        m_paint.setAntiAlias(true);
        m_paint.setColor(Color::WHITE);
        m_paint.setTextSize(28);
        m_width = m_paint.measureText(m_text);
    }
    
protected:
    
    virtual void onDraw(shared_ptr<Canvas> canvas);
    
private:
    
    float m_width = 0.0f;
    Paint m_paint;
    UnicodeString m_text;
};

#define CL_TYPE CustomView // the type we want to register
#define CL_BASE_TYPE View
#define CL_TYPE_NAME "CustomView" // the name of the type (need not be the same as the type)
#include "Android/utils/cl_reg.h" // include the supermacro

#endif /* defined(__MoneyMobileX__CustomView__) */
