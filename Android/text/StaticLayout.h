//
//  StaticLayout.h
//  Androidpp
//
//  Created by Saul Howard on 1/30/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef __Androidpp__StaticLayout__
#define __Androidpp__StaticLayout__

#include "AndroidMacros.h"

#include "Android/graphics/Paint.h"
#include "Android/text/Layout.h"

#include <unicode/unistr.h>

#include <memory>
#include <vector>
#include <string>

using namespace icu;
using namespace std;

ANDROID_BEGIN

class CharSequence;
class Directions;
class LineHeightSpan;
class TextPaint;
class TextDirectionHeuristic;

/**
 * StaticLayout is a Layout for text that will not be edited after it
 * is laid out.  Use {@link DynamicLayout} for text that may change.
 * <p>This is used by widgets to control text layout. You should not need
 * to use this class directly unless you are implementing your own widget
 * or custom display object, or would be tempted to call
 * {@link android.graphics.Canvas#drawText(CharSequence, int, int,
 * float, float, android.graphics.Paint)
 * Canvas.drawText()} directly.</p>
 */
class StaticLayout : public Layout {
    
public:
    
    StaticLayout(shared_ptr<CharSequence> source, shared_ptr<TextPaint> paint,
                 int width,
                 Alignment align, float spacingmult, float spacingadd,
                 bool includepad);
    
    /**
     * @hide
     */
    StaticLayout(shared_ptr<CharSequence> source, shared_ptr<TextPaint> paint,
                 int width, Alignment align, shared_ptr<TextDirectionHeuristic> textDir,
                 float spacingmult, float spacingadd,
                 bool includepad);
    
    StaticLayout(shared_ptr<CharSequence> source, int bufstart, int bufend,
                 shared_ptr<TextPaint> paint, int outerwidth,
                 Alignment align,
                 float spacingmult, float spacingadd,
                 bool includepad);
    
    /**
     * @hide
     */
    StaticLayout(shared_ptr<CharSequence> source, int bufstart, int bufend,
                 shared_ptr<TextPaint> paint, int outerwidth,
                 Alignment align, shared_ptr<TextDirectionHeuristic> textDir,
                 float spacingmult, float spacingadd,
                 bool includepad);
    
    StaticLayout(shared_ptr<CharSequence> source, int bufstart, int bufend,
                 shared_ptr<TextPaint> paint, int outerwidth,
                 Alignment align,
                 float spacingmult, float spacingadd,
                 bool includepad,
                 shared_ptr<TextUtils::TruncateAt> ellipsize, int ellipsizedWidth);
    
    /**
     * @hide
     */
    StaticLayout(shared_ptr<CharSequence> source, int bufstart, int bufend,
                 shared_ptr<TextPaint> paint, int outerwidth,
                 Alignment align, shared_ptr<TextDirectionHeuristic> textDir,
                 float spacingmult, float spacingadd,
                 bool includepad,
                 shared_ptr<TextUtils::TruncateAt> ellipsize, int ellipsizedWidth, int maxLines);
    
protected:
    
    StaticLayout(shared_ptr<CharSequence> text);
    
    void generate(shared_ptr<CharSequence> source, int bufStart, int bufEnd,
                  shared_ptr<TextPaint> paint, int outerWidth,
                  shared_ptr<TextDirectionHeuristic> textDir, float spacingmult,
                  float spacingadd, bool includepad,
                  bool trackpad, float ellipsizedWidth,
                  shared_ptr<TextUtils::TruncateAt> ellipsize);
    
private:
    
    /**
     * Returns true if the specified character is one of those specified
     * as being Ideographic (class ID) by the Unicode Line Breaking Algorithm
     * (http://www.unicode.org/unicode/reports/tr14/), and is therefore OK
     * to break between a pair of.
     *
     * @param includeNonStarters also return true for category NS
     *                           (non-starters), which can be broken
     *                           after but not before.
     */
    static bool isIdeographic(UChar c, bool includeNonStarters);
    
    int outt(shared_ptr<CharSequence> text, int start, int end,
             int above, int below, int top, int bottom, int v,
             float spacingmult, float spacingadd,
             vector<shared_ptr<LineHeightSpan>> chooseHt, vector<int> chooseHtv,
             shared_ptr<Paint::FontMetricsInt> fm, bool hasTabOrEmoji,
             bool needMultiply, vector<int8_t> chdirs, int dir,
             bool easy, int bufEnd, bool includePad,
             bool trackPad, UnicodeString chs,
             vector<float> widths, int widthStart, shared_ptr<TextUtils::TruncateAt> ellipsize,
             float ellipsisWidth, float textWidth,
             shared_ptr<TextPaint> paint, bool moreChars);
    
    void calculateEllipsis(int lineStart, int lineEnd,
                           vector<float> widths, int widthStart,
                           float avail, shared_ptr<TextUtils::TruncateAt> where,
                           int line, float textWidth, shared_ptr<TextPaint> paint,
                           bool forceEllipsis);
    
public:
    
    // Override the base class so we can directly access our members,
    // rather than relying on member functions.
    // The logic mirrors that of Layout.getLineForVertical
    // FIXME: It may be faster to do a linear search for layouts without many lines.
    virtual int getLineForVertical(int vertical);
    
    virtual int getLineCount();
    
    virtual int getLineTop(int line);
    
    virtual int getLineDescent(int line);
    
    virtual int getLineStart(int line);
    
    virtual int getParagraphDirection(int line);
    
    virtual bool getLineContainsTab(int line);
    
    Directions getLineDirections(int line);
    
    virtual int getTopPadding();
    
    virtual int getBottomPadding();
    
    virtual int getEllipsisCount(int line);
    
    virtual int getEllipsisStart(int line);
    
    virtual int getEllipsizedWidth();
    
private:
    
    void prepare();
    
    void finish();
    
    shared_ptr<CharSequence> createText(shared_ptr<CharSequence> source, shared_ptr<TextUtils::TruncateAt> ellipsize);
    
    int mLineCount = 0;
    int mTopPadding = 0, mBottomPadding = 0;
    int mColumns = 0;
    int mEllipsizedWidth = 0;
    
    static const int COLUMNS_NORMAL = 3;
    static const int COLUMNS_ELLIPSIZE = 5;
    static const int START = 0;
    static const int DIR = START;
    static const int TAB = START;
    static const int TOP = 1;
    static const int DESCENT = 2;
    static const int ELLIPSIS_START = 3;
    static const int ELLIPSIS_COUNT = 4;
    
    vector<int> mLines;
    vector<Directions> mLineDirections;
    int mMaximumVisibleLineCount = INT_MAX;
    
    static const int START_MASK = 0x1FFFFFFF;
    static const int DIR_SHIFT  = 30;
    static const int TAB_MASK   = 0x20000000;
    
    static const UChar CHAR_FIRST_CJK = 0x2E80;
    
    static const UChar CHAR_NEW_LINE = '\n';
    static const UChar CHAR_TAB = '\t';
    static const UChar CHAR_SPACE = ' ';
    static const UChar CHAR_SLASH = '/';
    static const UChar CHAR_HYPHEN = '-';
    static const UChar CHAR_ZWSP = 0x200B;
    
    static const double EXTRA_ROUNDING;
    
    static const int CHAR_FIRST_HIGH_SURROGATE = 0xD800;
    static const int CHAR_LAST_LOW_SURROGATE = 0xDFFF;
    
    /*
     * This is reused across calls to generate()
     */
    shared_ptr<MeasuredText> mMeasured;
    shared_ptr<Paint::FontMetricsInt> mFontMetricsInt = make_shared<Paint::FontMetricsInt>();
};

ANDROID_END

#endif /* defined(__Androidpp__StaticLayout__) */
