//
//  MeasuredText.h
//  Androidpp
//
//  Created by Saul Howard on 1/23/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef __Androidpp__MeasuredText__
#define __Androidpp__MeasuredText__

#include "AndroidMacros.h"

#include "Android/graphics/Paint.h"
#include "Android/utils/Object.h"

#include <unicode/unistr.h>

#include <memory>
#include <vector>
#include <string>

using namespace icu;
using namespace std;

ANDROID_BEGIN

class CharSequence;
class TextPaint;
class MetricAffectingSpan;
class TextDirectionHeuristic;

class MeasuredText {
    
    friend class Layout;
    friend class StaticLayout;
    friend class TextUtils;
    
public:
    
    MeasuredText();
    
private:
    
    static const bool localLOGV = false;
    shared_ptr<CharSequence> mText;
    int mTextStart;
    vector<float> mWidths;
    UnicodeString mChars;
    vector<int8_t> mLevels;
    int mDir;
    bool mEasy;
    int mLen;
    
    int mPos;
    shared_ptr<TextPaint> mWorkPaint;
    
//    static const Object[] sLock = new Object[0];
    static vector<shared_ptr<MeasuredText>> sCached;
    
public:
    
    static shared_ptr<MeasuredText> obtain();
    
    static shared_ptr<MeasuredText> recycle(shared_ptr<MeasuredText> mt);
    
    void setPos(int pos);
    
    /**
     * Analyzes text for bidirectional runs.  Allocates working buffers.
     */
    void setPara(shared_ptr<CharSequence> text, int start, int end, shared_ptr<TextDirectionHeuristic> textDir);
    
    float addStyleRun(shared_ptr<TextPaint> paint, int len, shared_ptr<Paint::FontMetricsInt> fm);
    
    float addStyleRun(shared_ptr<TextPaint> paint, vector<shared_ptr<Object>> spans, int len,
                      shared_ptr<Paint::FontMetricsInt> fm);
    
    int breakText(int limit, bool forwards, float width);
    
    float measure(int start, int limit);
};

ANDROID_END

#endif /* defined(__Androidpp__MeasuredText__) */
