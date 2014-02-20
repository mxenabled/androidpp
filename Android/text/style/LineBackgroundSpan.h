//
//  LineBackgroundSpan.h
//  Androidpp
//
//  Created by Saul Howard on 1/22/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef Androidpp_LineBackgroundSpan_h
#define Androidpp_LineBackgroundSpan_h

#include "AndroidMacros.h"

#include "Android/text/style/ParagraphStyle.h"

#include <memory>

using namespace std;

ANDROID_BEGIN

class Canvas;
class Paint;
class CharSequence;

class LineBackgroundSpan : public ParagraphStyle {
public:
    virtual void drawBackground(shared_ptr<Canvas> c, Paint *p,
                               int left, int right,
                               int top, int baseline, int bottom,
                               CharSequence *text, int start, int end,
                               int lnum) = 0;
    
    virtual string getType() {
        return "LineBackgroundSpan";
    };
};

ANDROID_END

#endif
