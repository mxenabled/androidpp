//
//  LeadingMarginSpan.h
//  Androidpp
//
//  Created by Saul Howard on 1/22/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef Androidpp_LeadingMarginSpan_h
#define Androidpp_LeadingMarginSpan_h

#include "AndroidMacros.h"

#include "Android/text/style/ParagraphStyle.h"
#include "Android/text/style/WrapTogetherSpan.h"

#include <memory>

using namespace std;

ANDROID_BEGIN

class Canvas;
class Paint;
class Layout;
class CharSequence;

/**
 * A paragraph style affecting the leading margin. There can be multiple leading
 * margin spans on a single paragraph; they will be rendered in order, each
 * adding its margin to the ones before it. The leading margin is on the right
 * for lines in a right-to-left paragraph.
 */
class LeadingMarginSpan : public virtual ParagraphStyle {
    
public:
    
    /**
     * Returns the amount by which to adjust the leading margin. Positive values
     * move away from the leading edge of the paragraph, negative values move
     * towards it.
     *
     * @param first true if the request is for the first line of a paragraph,
     * false for subsequent lines
     * @return the offset for the margin.
     */
    virtual int getLeadingMargin(bool first) = 0;
    
    /**
     * Renders the leading margin.  This is called before the margin has been
     * adjusted by the value returned by {@link #getLeadingMargin(bool)}.
     *
     * @param c the canvas
     * @param p the paint. The this should be left unchanged on exit.
     * @param x the current position of the margin
     * @param dir the base direction of the paragraph; if negative, the margin
     * is to the right of the text, otherwise it is to the left.
     * @param top the top of the line
     * @param baseline the baseline of the line
     * @param bottom the bottom of the line
     * @param text the text
     * @param start the start of the line
     * @param end the end of the line
     * @param first true if this is the first line of its paragraph
     * @param layout the layout containing this line
     */
    virtual void drawLeadingMargin(shared_ptr<Canvas> c, shared_ptr<Paint> p,
                                  int x, int dir,
                                  int top, int baseline, int bottom,
                                  shared_ptr<CharSequence> text, int start, int end,
                                  bool first, Layout *layout) = 0;
    
    virtual string getType() {
        return "LeadingMarginSpan";
    };
    
    /**
     * An extended version of {@link LeadingMarginSpan}, which allows
     * the implementor to specify the number of lines of text to which
     * this object is attached that the "first line of paragraph" margin
     * width will be applied to.
     */
    class LeadingMarginSpan2;
    
    /**
     * The standard implementation of LeadingMarginSpan, which adjusts the
     * margin but does not do any rendering.
     */
    class Standard;
};

class LeadingMarginSpan2 : public LeadingMarginSpan, public WrapTogetherSpan {
    
public:
    
    /**
     * Returns the number of lines of text to which this object is
     * attached that the "first line" margin will apply to.
     * Note that if this returns N, the first N lines of the region,
     * not the first N lines of each paragraph, will be given the
     * special margin width.
     */
    virtual int getLeadingMarginLineCount() = 0;
    
    
    virtual string getType() {
        return "LeadingMarginSpan2";
    };
};

class LeadingMarginSpan::Standard : public LeadingMarginSpan {

private:
    
    const int mFirst, mRest;

public:
    
    /**
     * Constructor taking separate indents for the first and subsequent
     * lines.
     *
     * @param first the indent for the first line of the paragraph
     * @param rest the indent for the remaining lines of the paragraph
     */
    Standard(int first, int rest) : mFirst(first), mRest(rest) {
    }
    
    /**
     * Constructor taking an indent for all lines.
     * @param every the indent of each line
     */
    Standard(int every) : Standard(every, every) {
    }
    
//    public Standard(Parcel src) {
//        mFirst = src.readInt();
//        mRest = src.readInt();
//    }
//
//    int getSpanTypeId() {
//        return TextUtils::LEADING_MARGIN_SPAN;
//    }
//    
//    int describeContents() {
//        return 0;
//    }
//    
//    public void writeToParcel(Parcel dest, int flags) {
//        dest.writeInt(mFirst);
//        dest.writeInt(mRest);
//    }
    
    int getLeadingMargin(bool first) {
        return first ? mFirst : mRest;
    }
    
    void drawLeadingMargin(shared_ptr<Canvas> c, shared_ptr<Paint> p,
                                  int x, int dir,
                                  int top, int baseline, int bottom,
                                  shared_ptr<CharSequence> text, int start, int end,
                                  bool first, Layout *layout) {
        ;
    }
};

ANDROID_END

#endif
