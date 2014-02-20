//
//  BoringLayout.h
//  Androidpp
//
//  Created by Saul Howard on 1/28/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef __Androidpp__BoringLayout__
#define __Androidpp__BoringLayout__

#include "AndroidMacros.h"

#include "Android/graphics/Paint.h"
#include "Android/text/Layout.h"
#include "Android/text/TextUtils.h"

#include <string>
#include <memory>

using namespace std;

ANDROID_BEGIN

class CharSequence;
class Directions;
class TextPaint;
class String;

/**
 * A BoringLayout is a very simple Layout implementation for text that
 * fits on a single line and is all left-to-right characters.
 * You will probably never want to make one of these yourself;
 * if you do, be sure to call {@link #isBoring} first to make sure
 * the text meets the criteria.
 * <p>This class is used by widgets to control text layout. You should not need
 * to use this class directly unless you are implementing your own widget
 * or custom display object, in which case
 * you are encouraged to use a Layout instead of calling
 * {@link android.graphics.Canvas#drawText(CharSequence, int, int, float, float, android.graphics.Paint)
 *  Canvas.drawText()} directly.</p>
 */
class BoringLayout : public Layout, public TextUtils::EllipsizeCallback, public enable_shared_from_this<BoringLayout> {
    
public:
    
    class Metrics : public Paint::FontMetricsInt {
    public:
        
        int width;
        
        virtual string toString();
    };
    
    static shared_ptr<BoringLayout> make(shared_ptr<CharSequence> source,
                                    shared_ptr<TextPaint> paint, int outerwidth,
                                    Alignment align,
                                    float spacingmult, float spacingadd,
                                    shared_ptr<Metrics> metrics, bool includepad);
    
    static shared_ptr<BoringLayout> make(shared_ptr<CharSequence> source,
                                    shared_ptr<TextPaint> paint, int outerwidth,
                                    Alignment align,
                                    float spacingmult, float spacingadd,
                                    shared_ptr<Metrics> metrics, bool includepad,
                             shared_ptr<TextUtils::TruncateAt> ellipsize, int ellipsizedWidth);
    
    /**
     * Returns a BoringLayout for the specified text, potentially reusing
     * this one if it is already suitable.  The caller must make sure that
     * no one is still using this Layout.
     */
    shared_ptr<BoringLayout> replaceOrMake(shared_ptr<CharSequence> source, shared_ptr<TextPaint> paint,
                                      int outerwidth, Alignment align,
                                      float spacingmult, float spacingadd,
                                      shared_ptr<Metrics> metrics,
                                      bool includepad);
    
    /**
     * Returns a BoringLayout for the specified text, potentially reusing
     * this one if it is already suitable.  The caller must make sure that
     * no one is still using this Layout.
     */
    shared_ptr<BoringLayout> replaceOrMake(shared_ptr<CharSequence> source, shared_ptr<TextPaint> paint,
                                      int outerwidth, Alignment align,
                                      float spacingmult, float spacingadd,
                                      shared_ptr<Metrics> metrics,
                                      bool includepad,
                               shared_ptr<TextUtils::TruncateAt> ellipsize,
                                      int ellipsizedWidth);
    
    BoringLayout(shared_ptr<CharSequence> source,
                        shared_ptr<TextPaint> paint, int outerwidth,
                        Alignment align,
                        float spacingmult, float spacingadd,
                        shared_ptr<Metrics> metrics, bool includepad);
    
    BoringLayout(shared_ptr<CharSequence> source,
                        shared_ptr<TextPaint> paint, int outerwidth,
                        Alignment align,
                        float spacingmult, float spacingadd,
                        shared_ptr<Metrics> metrics, bool includepad,
                        shared_ptr<TextUtils::TruncateAt> ellipsize, int ellipsizedWidth);
    
private:
    
    void init(shared_ptr<CharSequence> source,
                            shared_ptr<TextPaint> paint, int outerwidth,
                            Alignment align,
                            float spacingmult, float spacingadd,
                            shared_ptr<Metrics> metrics, bool includepad,
                            bool trustWidth);
    
public:
    
    /**
     * Returns null if not boring; the width, ascent, and descent if boring.
     */
    static shared_ptr<Metrics> isBoring(shared_ptr<CharSequence> text,
                                   shared_ptr<TextPaint> paint);
    
    /**
     * Returns null if not boring; the width, ascent, and descent if boring.
     * @hide
     */
    static shared_ptr<Metrics> isBoring(shared_ptr<CharSequence> text,
                                   shared_ptr<TextPaint> paint,
                                   shared_ptr<TextDirectionHeuristic> textDir);
    
    /**
     * Returns null if not boring; the width, ascent, and descent in the
     * provided Metrics object (or a new one if the provided one was null)
     * if boring.
     */
    static shared_ptr<Metrics> isBoring(shared_ptr<CharSequence> text, shared_ptr<TextPaint> paint, shared_ptr<Metrics> metrics);
    
    /**
     * Returns null if not boring; the width, ascent, and descent in the
     * provided Metrics object (or a new one if the provided one was null)
     * if boring.
     * @hide
     */
    static shared_ptr<Metrics> isBoring(shared_ptr<CharSequence> text, shared_ptr<TextPaint> paint,
                                   shared_ptr<TextDirectionHeuristic> textDir, shared_ptr<Metrics> metrics);
    
    virtual int getHeight();
    
    virtual int getLineCount();
    
    virtual int getLineTop(int line);
    
    virtual int getLineDescent(int line);
    
    virtual int getLineStart(int line);
    
    virtual int getParagraphDirection(int line);
    
    virtual bool getLineContainsTab(int line);
    
    virtual float getLineMax(int line);
    
    Directions getLineDirections(int line);
    
    virtual int getTopPadding();
    
    virtual int getBottomPadding();
    
    virtual int getEllipsisCount(int line);
    
    virtual int getEllipsisStart(int line);
    
    virtual int getEllipsizedWidth();
    
    // Override draw so it will be faster.
    virtual void draw(shared_ptr<Canvas> c, Path *highlight, Paint *highlightpaint,
                     int cursorOffset);
    
    /**
     * Callback for the ellipsizer to report what region it ellipsized.
     */
    void ellipsized(int start, int end);
    
private:
    
    static const uint16_t FIRST_RIGHT_TO_LEFT = 0x0590;
    
    shared_ptr<String> mDirect;
    shared_ptr<Paint> mPaint;
    
    /* package */ int mBottom = 0, mDesc = 0;   // for Direct
    int mTopPadding = 0, mBottomPadding = 0;
    float mMax = 0;
    int mEllipsizedWidth = 0, mEllipsizedStart = 0, mEllipsizedCount = 0;
    
    static TextPaint sTemp;
};

ANDROID_END

#endif /* defined(__Androidpp__BoringLayout__) */
