//
//  BoringLayout.cpp
//  Androidpp
//
//  Created by Saul Howard on 1/28/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#include "BoringLayout.h"

#include "Android/graphics/Canvas.h"
#include "Android/text/TextLine.h"
#include "Android/text/TextPaint.h"
#include "Android/text/CharSequence.h"
#include "Android/text/Directions.h"
#include "Android/text/String.h"
#include "Android/text/TextDirectionHeuristics.h"
#include "Android/text/style/TabStopSpan.h"

#include <unicode/unistr.h>

#include <math.h>
#include <sstream>

using namespace icu;

ANDROID_BEGIN

TextPaint BoringLayout::sTemp = TextPaint();

string BoringLayout::Metrics::toString() {
    stringstream str;
    str << Paint::FontMetricsInt::toString() << " width=" << width;
    return str.str();
}

shared_ptr<BoringLayout> BoringLayout::make(shared_ptr<CharSequence> source,
                         shared_ptr<TextPaint> paint, int outerwidth,
                         Alignment align,
                         float spacingmult, float spacingadd,
                         shared_ptr<Metrics> metrics, bool includepad) {
    return make_shared<BoringLayout>(source, paint, outerwidth, align,
                            spacingmult, spacingadd, metrics,
                            includepad);
}

shared_ptr<BoringLayout> BoringLayout::make(shared_ptr<CharSequence> source,
                         shared_ptr<TextPaint> paint, int outerwidth,
                         Alignment align,
                         float spacingmult, float spacingadd,
                         shared_ptr<Metrics> metrics, bool includepad,
                         shared_ptr<TextUtils::TruncateAt> ellipsize, int ellipsizedWidth) {
    return make_shared<BoringLayout>(source, paint, outerwidth, align,
                            spacingmult, spacingadd, metrics,
                            includepad, ellipsize, ellipsizedWidth);
}

shared_ptr<BoringLayout> BoringLayout::replaceOrMake(shared_ptr<CharSequence> source, shared_ptr<TextPaint> paint,
                           int outerwidth, Alignment align,
                           float spacingmult, float spacingadd,
                           shared_ptr<Metrics> metrics,
                           bool includepad) {
    replaceWith(source, paint, outerwidth, align, spacingmult,
                spacingadd);
    
    mEllipsizedWidth = outerwidth;
    mEllipsizedStart = 0;
    mEllipsizedCount = 0;
    
    init(source, paint, outerwidth, align, spacingmult, spacingadd,
         metrics, includepad, true);
    return shared_from_this();
}

shared_ptr<BoringLayout> BoringLayout::replaceOrMake(shared_ptr<CharSequence> source, shared_ptr<TextPaint> paint,
                           int outerwidth, Alignment align,
                           float spacingmult, float spacingadd,
                           shared_ptr<Metrics> metrics,
                           bool includepad,
                           shared_ptr<TextUtils::TruncateAt> ellipsize,
                           int ellipsizedWidth) {
    bool trust;
    
    if (ellipsize == NULL || ellipsize == TextUtils::TruncateAt::MARQUEE) {
        replaceWith(source, paint, outerwidth, align, spacingmult,
                    spacingadd);
        
        mEllipsizedWidth = outerwidth;
        mEllipsizedStart = 0;
        mEllipsizedCount = 0;
        trust = true;
    } else {
        replaceWith(TextUtils::ellipsize(source, paint, ellipsizedWidth,
                                         ellipsize, true, this),
                    paint, outerwidth, align, spacingmult,
                    spacingadd);
        
        mEllipsizedWidth = ellipsizedWidth;
        trust = false;
    }
    
    init(getText(), paint, outerwidth, align, spacingmult, spacingadd,
         metrics, includepad, trust);
    return shared_from_this();
}

BoringLayout::BoringLayout(shared_ptr<CharSequence> source,
                           shared_ptr<TextPaint> paint, int outerwidth,
                           Alignment align,
                           float spacingmult, float spacingadd,
                           shared_ptr<Metrics> metrics, bool includepad) : Layout(source, paint, outerwidth, align, spacingmult, spacingadd) {
    
    mEllipsizedWidth = outerwidth;
    mEllipsizedStart = 0;
    mEllipsizedCount = 0;
    
    init(source, paint, outerwidth, align, spacingmult, spacingadd,
         metrics, includepad, true);
}

BoringLayout::BoringLayout(shared_ptr<CharSequence> source,
                           shared_ptr<TextPaint> paint, int outerwidth,
                           Alignment align,
                           float spacingmult, float spacingadd,
                           shared_ptr<Metrics> metrics, bool includepad,
                           shared_ptr<TextUtils::TruncateAt> ellipsize, int ellipsizedWidth) : Layout(source, paint, outerwidth, align, spacingmult, spacingadd) {
    
    bool trust;
    
    if (ellipsize == NULL || ellipsize == TextUtils::TruncateAt::MARQUEE) {
        mEllipsizedWidth = outerwidth;
        mEllipsizedStart = 0;
        mEllipsizedCount = 0;
        trust = true;
    } else {
        replaceWith(TextUtils::ellipsize(source, paint, ellipsizedWidth,
                                         ellipsize, true, this),
                    paint, outerwidth, align, spacingmult,
                    spacingadd);
        
        
        mEllipsizedWidth = ellipsizedWidth;
        trust = false;
    }
    
    init(getText(), paint, outerwidth, align, spacingmult, spacingadd,
         metrics, includepad, trust);
}

void BoringLayout::init(shared_ptr<CharSequence> source,
          shared_ptr<TextPaint> paint, int outerwidth,
          Alignment align,
          float spacingmult, float spacingadd,
          shared_ptr<Metrics> metrics, bool includepad,
          bool trustWidth) {
    int spacing;
    
    
    
    if (align == Alignment::ALIGN_NORMAL) {
        mDirect = source->toString();
    }
    
    mPaint = paint;
    
    if (includepad) {
        spacing = metrics->bottom - metrics->top;
    } else {
        spacing = metrics->descent - metrics->ascent;
    }
    
    if (spacingmult != 1 || spacingadd != 0) {
        spacing = (int)(spacing * spacingmult + spacingadd + 0.5f);
    }
    
    mBottom = spacing;
    
    if (includepad) {
        mDesc = spacing + metrics->top;
    } else {
        mDesc = spacing + metrics->ascent;
    }
    
    if (trustWidth) {
        mMax = metrics->width;
    } else {
        shared_ptr<TextLine<TabStopSpan>> line = TextLine<TabStopSpan>::obtain();
        line->set(paint, source, 0, source->length(), Directions::DIR_LEFT_TO_RIGHT,
                 Directions::DIRS_ALL_LEFT_TO_RIGHT, false, NULL);
        mMax = (int) ceil(line->metrics(NULL));
        TextLine<TabStopSpan>::recycle(line);
    }
    
    if (includepad) {
        mTopPadding = metrics->top - metrics->ascent;
        mBottomPadding = metrics->bottom - metrics->descent;
    }
}

shared_ptr<BoringLayout::Metrics> BoringLayout::isBoring(shared_ptr<CharSequence> text,
                        shared_ptr<TextPaint> paint) {
    return isBoring(text, paint, TextDirectionHeuristics::FIRSTSTRONG_LTR, NULL);
}

shared_ptr<BoringLayout::Metrics> BoringLayout::isBoring(shared_ptr<CharSequence> text,
                        shared_ptr<TextPaint> paint,
                        shared_ptr<TextDirectionHeuristic> textDir) {
    return isBoring(text, paint, textDir, NULL);
}

shared_ptr<BoringLayout::Metrics> BoringLayout::isBoring(shared_ptr<CharSequence> text, shared_ptr<TextPaint> paint, shared_ptr<Metrics> metrics) {
    return isBoring(text, paint, TextDirectionHeuristics::FIRSTSTRONG_LTR, metrics);
}

shared_ptr<BoringLayout::Metrics> BoringLayout::isBoring(shared_ptr<CharSequence> text, shared_ptr<TextPaint> paint,
                        shared_ptr<TextDirectionHeuristic> textDir, shared_ptr<Metrics> metrics) {
    UnicodeString *temp = TextUtils::obtain(500);
    int length = text->length();
    bool boring = true;
    
    for (int i = 0; i < length; i += 500) {
        int j = i + 500;
        
        if (j > length)
            j = length;
        
        TextUtils::getChars(text, i, j, *temp, 0);
        
        int n = j - i;
        
        for (int a = 0; a < n; a++) {
            UChar c = temp->charAt(a);
            
            if (c == '\n' || c == '\t' || c >= FIRST_RIGHT_TO_LEFT) {
                boring = false;
                goto outer;
            }
        }
        
        if (textDir != NULL && textDir->isRtl(*temp, 0, n)) {
            boring = false;
            goto outer;
        }
    }
    
outer:
    
    TextUtils::recycle(temp);
    
    shared_ptr<Spanned> sp = dynamic_pointer_cast<Spanned>(text);
    if (boring && sp != NULL) {
        vector<shared_ptr<Object>> styles = sp->getSpans(0, length, "ParagraphStyle");
        if (styles.size() > 0) {
            boring = false;
        }
    }
    
    if (boring) {
        shared_ptr<Metrics> fm = metrics;
        if (fm == NULL) {
            fm = make_shared<Metrics>();
        }
        
        shared_ptr<TextLine<TabStopSpan>> line = TextLine<TabStopSpan>::obtain();
        line->set(paint, text, 0, length, Directions::DIR_LEFT_TO_RIGHT,
                 Directions::DIRS_ALL_LEFT_TO_RIGHT, false, NULL);
        fm->width = (int) ceil(line->metrics(fm));
        TextLine<TabStopSpan>::recycle(line);
        
        return fm;
    } else {
        return NULL;
    }
}

int BoringLayout::getHeight() {
    return mBottom;
}

int BoringLayout::getLineCount() {
    return 1;
}

int BoringLayout::getLineTop(int line) {
    if (line == 0)
        return 0;
    else
        return mBottom;
}

int BoringLayout::getLineDescent(int line) {
    return mDesc;
}

int BoringLayout::getLineStart(int line) {
    if (line == 0)
        return 0;
    else
        return getText()->length();
}

int BoringLayout::getParagraphDirection(int line) {
    return Directions::DIR_LEFT_TO_RIGHT;
}

bool BoringLayout::getLineContainsTab(int line) {
    return false;
}

float BoringLayout::getLineMax(int line) {
    return mMax;
}

Directions BoringLayout::getLineDirections(int line) {
    return Directions::DIRS_ALL_LEFT_TO_RIGHT;
}

int BoringLayout::getTopPadding() {
    return mTopPadding;
}

int BoringLayout::getBottomPadding() {
    return mBottomPadding;
}

int BoringLayout::getEllipsisCount(int line) {
    return mEllipsizedCount;
}

int BoringLayout::getEllipsisStart(int line) {
    return mEllipsizedStart;
}

int BoringLayout::getEllipsizedWidth() {
    return mEllipsizedWidth;
}

// Override draw so it will be faster.
void BoringLayout::draw(shared_ptr<Canvas> c, Path *highlight, Paint *highlightpaint,
                  int cursorOffset) {
    
    if (mDirect != NULL && highlight == NULL) {
        c->drawText(mDirect, 0, mBottom - mDesc, mPaint.get());
    } else {
        Layout::draw(c, highlight, highlightpaint, cursorOffset);
    }
}

void BoringLayout::ellipsized(int start, int end) {
    mEllipsizedStart = start;
    mEllipsizedCount = end - start;
}

ANDROID_END