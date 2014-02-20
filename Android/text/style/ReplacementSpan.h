//
//  ReplacementSpan.h
//  Androidpp
//
//  Created by Saul Howard on 1/22/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef Androidpp_ReplacementSpan_h
#define Androidpp_ReplacementSpan_h

#include "AndroidMacros.h"

#include "Android/graphics/Paint.h"
#include "Android/text/style/MetricAffectingSpan.h"

#include <memory>

using namespace std;

ANDROID_BEGIN

class Paint;
class CharSequence;
class Canvas;

class ReplacementSpan : public MetricAffectingSpan {
    
public:
    
    virtual int getSize(Paint *paint, CharSequence *text,
                                int start, int end,
                        shared_ptr<Paint::FontMetricsInt> fm) = 0;
    
    virtual void draw(shared_ptr<Canvas> canvas, CharSequence *text,
                              int start, int end, float x,
                              int top, int y, int bottom, Paint *paint) = 0;
    
    /**
     * This method does nothing, since ReplacementSpans are measured
     * explicitly instead of affecting Paint properties.
     */
    virtual void updateMeasureState(shared_ptr<TextPaint> p) { }
    
    /**
     * This method does nothing, since ReplacementSpans are drawn
     * explicitly instead of affecting Paint properties.
     */
    virtual void updateDrawState(shared_ptr<TextPaint> ds) { }
    
    virtual string getType() {
        return "ReplacementSpan";
    };
};

ANDROID_END

#endif
