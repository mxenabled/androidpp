//
//  MeasuredText.cpp
//  Androidpp
//
//  Created by Saul Howard on 1/23/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#include "MeasuredText.h"

#include "Android/graphics/Canvas.h"
#include "Android/text/AndroidBidi.h"
#include "Android/text/CharSequence.h"
#include "Android/text/Directions.h"
#include "Android/text/Spanned.h"
#include "Android/text/TextPaint.h"
#include "Android/text/style/MetricAffectingSpan.h"
#include "Android/text/style/ReplacementSpan.h"
#include "Android/text/TextDirectionHeuristic.h"
#include "Android/text/TextDirectionHeuristics.h"
#include "Android/text/TextUtils.h"
#include "Android/utils/ArrayUtils.h"

ANDROID_BEGIN

vector<shared_ptr<MeasuredText>> MeasuredText::sCached = vector<shared_ptr<MeasuredText>>(3);

MeasuredText::MeasuredText() {
    mWorkPaint = make_shared<TextPaint>();
}

shared_ptr<MeasuredText> MeasuredText::obtain() {
    shared_ptr<MeasuredText> mt;
//    synchronized (sLock) {
        for (int i = sCached.size(); --i >= 0;) {
            if (sCached[i] != NULL) {
                mt = sCached[i];
                sCached[i] = NULL;
                return mt;
            }
        }
//    }
    mt = make_shared<MeasuredText>();
    if (localLOGV) {
//        Log.v("MEAS", "new: " + mt);
    }
    return mt;
}

shared_ptr<MeasuredText> MeasuredText::recycle(shared_ptr<MeasuredText> mt) {
    mt->mText = NULL;
    if (mt->mLen < 1000) {
//        synchronized(sLock) {
            for (int i = 0; i < sCached.size(); ++i) {
                if (sCached[i] == NULL) {
                    sCached[i] = mt;
                    mt->mText = NULL;
                    break;
                }
//            }
        }
    }
    return NULL;
}

void MeasuredText::setPos(int pos) {
    mPos = pos - mTextStart;
}

/**
 * Analyzes text for bidirectional runs.  Allocates working buffers.
 */
void MeasuredText::setPara(shared_ptr<CharSequence> text, int start, int end, shared_ptr<TextDirectionHeuristic> textDir) {
    mText = text;
    mTextStart = start;
    
    int len = end - start;
    mLen = len;
    mPos = 0;
    
    if (mWidths.size() < len) {
        mWidths = vector<float>(ArrayUtils::idealFloatArraySize(len));
    }
    if (mChars.length() < len) {
        int newLen = ArrayUtils::idealCharArraySize(len);
        mChars.getBuffer(newLen);
        mChars.releaseBuffer(newLen);
    }
    TextUtils::getChars(text, start, end, mChars, 0);
    
    shared_ptr<Spanned> spanned = dynamic_pointer_cast<Spanned>(text);
    if (spanned != NULL) {
        vector<shared_ptr<Object>> spans = spanned->getSpans(start, end, "ReplacementSpan");
        
        for (int i = 0; i < spans.size(); i++) {
            int startInPara = spanned->getSpanStart(spans[i]) - start;
            int endInPara = spanned->getSpanEnd(spans[i]) - start;
            // The span interval may be larger and must be restricted to [start, end[
            if (startInPara < 0) startInPara = 0;
            if (endInPara > len) endInPara = len;
            for (int j = startInPara; j < endInPara; j++) {
                mChars.setCharAt(j, 0xFFFC); // object replacement character
            }
        }
    }
    
    if ((textDir == TextDirectionHeuristics::LTR ||
         textDir == TextDirectionHeuristics::FIRSTSTRONG_LTR ||
         textDir == TextDirectionHeuristics::ANYRTL_LTR) &&
        TextUtils::doesNotNeedBidi(mChars, 0, len)) {
        mDir = Directions::DIR_LEFT_TO_RIGHT;
        mEasy = true;
    } else {
        if (mLevels.size() < len) {
            mLevels = vector<int8_t>(ArrayUtils::idealByteArraySize(len));
        }
        int bidiRequest;
        if (textDir == TextDirectionHeuristics::LTR) {
            bidiRequest = Directions::DIR_REQUEST_LTR;
        } else if (textDir == TextDirectionHeuristics::RTL) {
            bidiRequest = Directions::DIR_REQUEST_RTL;
        } else if (textDir == TextDirectionHeuristics::FIRSTSTRONG_LTR) {
            bidiRequest = Directions::DIR_REQUEST_DEFAULT_LTR;
        } else if (textDir == TextDirectionHeuristics::FIRSTSTRONG_RTL) {
            bidiRequest = Directions::DIR_REQUEST_DEFAULT_RTL;
        } else {
            bool isRtl = textDir->isRtl(mChars, 0, len);
            bidiRequest = isRtl ? Directions::DIR_REQUEST_RTL : Directions::DIR_REQUEST_LTR;
        }
        mDir = AndroidBidi::bidi(bidiRequest, mChars, mLevels, len, false);
        mEasy = false;
    }
}

float MeasuredText::addStyleRun(shared_ptr<TextPaint> paint, int len, shared_ptr<Paint::FontMetricsInt> fm) {
    
    paint->getFontMetricsInt(fm);
    
    int p = mPos;
    mPos = p + len;
    
    if (mEasy) {
        int flags = mDir == Directions::DIR_LEFT_TO_RIGHT
        ? Canvas::DIRECTION_LTR : Canvas::DIRECTION_RTL;
        return paint->getTextRunAdvances(mChars, p, mPos, p, mPos, flags, &mWidths, p);
    }
    
    float totalAdvance = 0;
    int level = mLevels[p];
    for (int q = p, i = p + 1, e = p + len;; ++i) {
        if (i == e || mLevels[i] != level) {
            int flags = (level & 0x1) == 0 ? Canvas::DIRECTION_LTR : Canvas::DIRECTION_RTL;
            totalAdvance +=
            paint->getTextRunAdvances(mChars, q, i, q, i, flags, &mWidths, q);
            if (i == e) {
                break;
            }
            q = i;
            level = mLevels[i];
        }
    }
    return totalAdvance;
}

float MeasuredText::addStyleRun(shared_ptr<TextPaint> paint, vector<shared_ptr<Object>> spans, int len,
                  shared_ptr<Paint::FontMetricsInt> fm) {
    
    shared_ptr<TextPaint> &workPaint = mWorkPaint;
    workPaint = paint;
    // XXX paint should not have a baseline shift, but...
    workPaint->baselineShift = 0;
    
    shared_ptr<ReplacementSpan> replacement;
    for (int i = 0; i < spans.size(); i++) {
        shared_ptr<MetricAffectingSpan> span = dynamic_pointer_cast<MetricAffectingSpan>(spans[i]);
        replacement = dynamic_pointer_cast<ReplacementSpan>(span);
        if (replacement == NULL) {
            span->updateMeasureState(workPaint);
        }
    }
    
    float wid;
    if (replacement == NULL) {
        wid = addStyleRun(workPaint, len, fm);
    } else {
        // Use original text.  Shouldn't matter.
        wid = replacement->getSize(workPaint.get(), mText.get(), mTextStart + mPos,
                                  mTextStart + mPos + len, fm);
        vector<float> &w = mWidths;
        w[mPos] = wid;
        for (int i = mPos + 1, e = mPos + len; i < e; i++)
            w[i] = 0;
        mPos += len;
    }
    
    if (workPaint->baselineShift < 0) {
        fm->ascent += workPaint->baselineShift;
        fm->top += workPaint->baselineShift;
    } else {
        fm->descent += workPaint->baselineShift;
        fm->bottom += workPaint->baselineShift;
    }
    
    return wid;
}

int MeasuredText::breakText(int limit, bool forwards, float width) {
    vector<float> &w = mWidths;
    if (forwards) {
        int i = 0;
        while (i < limit) {
            width -= w[i];
            if (width < 0.0f) break;
            i++;
        }
        while (i > 0 && mChars[i - 1] == ' ') i--;
        return i;
    } else {
        int i = limit - 1;
        while (i >= 0) {
            width -= w[i];
            if (width < 0.0f) break;
            i--;
        }
        while (i < limit - 1 && mChars[i + 1] == ' ') i++;
        return limit - i - 1;
    }
}

float MeasuredText::measure(int start, int limit) {
    float width = 0;
    vector<float> &w = mWidths;
    for (int i = start; i < limit; ++i) {
        width += w[i];
    }
    return width;
}

ANDROID_END