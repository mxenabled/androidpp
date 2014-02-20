//
//  LineHeightSpan.h
//  Androidpp
//
//  Created by Saul Howard on 1/22/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef Androidpp_LineHeightSpan_h
#define Androidpp_LineHeightSpan_h

#include "AndroidMacros.h"

#include "Android/graphics/Paint.h"
#include "Android/text/style/ParagraphStyle.h"
#include "Android/text/style/WrapTogetherSpan.h"

#include <memory>

using namespace std;

ANDROID_BEGIN

class CharSequence;
class TextPaint;

class LineHeightSpan : public virtual ParagraphStyle, public WrapTogetherSpan {
    
public:
    
    virtual void chooseHeight(shared_ptr<CharSequence> text, int start, int end,
                              int spanstartv, int v,
                              shared_ptr<Paint::FontMetricsInt> fm) = 0;
    
    virtual string getType() {
        return "LineHeightSpan";
    };
    
    class WithDensity;
};

class LineHeightSpan::WithDensity : public LineHeightSpan {
    
public:
    
    virtual void chooseHeight(shared_ptr<CharSequence> text, int start, int end,
                              int spanstartv, int v,
                              shared_ptr<Paint::FontMetricsInt> fm, shared_ptr<TextPaint> paint) = 0;
};

ANDROID_END

#endif
