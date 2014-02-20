//
//  StaticLayout.cpp
//  Androidpp
//
//  Created by Saul Howard on 1/30/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#include "StaticLayout.h"

#include "Android/text/AndroidBidi.h"
#include "Android/text/Character.h"
#include "Android/text/CharSequence.h"
#include "Android/text/Directions.h"
#include "Android/text/style/LeadingMarginSpan.h"
#include "Android/text/style/LineHeightSpan.h"
#include "Android/text/style/TabStopSpan.h"
#include "Android/text/TabStops.h"
#include "Android/text/MeasuredText.h"
#include "Android/text/Spanned.h"
#include "Android/text/TextPaint.h"
#include "Android/text/TextDirectionHeuristic.h"
#include "Android/text/TextDirectionHeuristics.h"
#include "Android/text/TextUtils.h"
#include "Android/utils/ArrayUtils.h"
#include "Android/utils/System.h"

ANDROID_BEGIN

const double StaticLayout::EXTRA_ROUNDING = 0.5;

StaticLayout::StaticLayout(shared_ptr<CharSequence> source, shared_ptr<TextPaint> paint,
                           int width,
                           Alignment align, float spacingmult, float spacingadd,
                           bool includepad) : StaticLayout(source, 0, source->length(), paint, width, align,
                                                           spacingmult, spacingadd, includepad) {
}

StaticLayout::StaticLayout(shared_ptr<CharSequence> source, shared_ptr<TextPaint> paint,
                           int width, Alignment align, shared_ptr<TextDirectionHeuristic> textDir,
                           float spacingmult, float spacingadd,
                           bool includepad) : StaticLayout(source, 0, source->length(), paint, width, align, textDir,
                                                           spacingmult, spacingadd, includepad) {
}

StaticLayout::StaticLayout(shared_ptr<CharSequence> source, int bufstart, int bufend,
                           shared_ptr<TextPaint> paint, int outerwidth,
                           Alignment align,
                           float spacingmult, float spacingadd,
                           bool includepad) : StaticLayout(source, bufstart, bufend, paint, outerwidth, align,
                                                           spacingmult, spacingadd, includepad, NULL, 0) {
}

StaticLayout::StaticLayout(shared_ptr<CharSequence> source, int bufstart, int bufend,
                           shared_ptr<TextPaint> paint, int outerwidth,
                           Alignment align, shared_ptr<TextDirectionHeuristic> textDir,
                           float spacingmult, float spacingadd,
                           bool includepad) : StaticLayout(source, bufstart, bufend, paint, outerwidth, align, textDir,
                                                           spacingmult, spacingadd, includepad, NULL, 0, INT_MAX) {
}

StaticLayout::StaticLayout(shared_ptr<CharSequence> source, int bufstart, int bufend,
                           shared_ptr<TextPaint> paint, int outerwidth,
                           Alignment align,
                           float spacingmult, float spacingadd,
                           bool includepad,
                           shared_ptr<TextUtils::TruncateAt> ellipsize, int ellipsizedWidth) : StaticLayout(source, bufstart, bufend, paint, outerwidth, align,
                                                                                                            TextDirectionHeuristics::FIRSTSTRONG_LTR,
                                                                                                            spacingmult, spacingadd, includepad, ellipsize, ellipsizedWidth, INT_MAX) {
}

StaticLayout::StaticLayout(shared_ptr<CharSequence> source, int bufstart, int bufend,
                           shared_ptr<TextPaint> paint, int outerwidth,
                           Alignment align, shared_ptr<TextDirectionHeuristic> textDir,
                           float spacingmult, float spacingadd,
                           bool includepad,
                           shared_ptr<TextUtils::TruncateAt> ellipsize, int ellipsizedWidth, int maxLines) : Layout(createText(source, ellipsize),
                                                                                                                    paint, outerwidth, align, textDir, spacingmult, spacingadd) {
    if (ellipsize != NULL) {
        shared_ptr<Ellipsizer> e = dynamic_pointer_cast<Ellipsizer>(getText());
        
        e->mLayout = this;
        e->mWidth = ellipsizedWidth;
        e->mMethod = ellipsize;
        mEllipsizedWidth = ellipsizedWidth;
        
        mColumns = COLUMNS_ELLIPSIZE;
    } else {
        mColumns = COLUMNS_NORMAL;
        mEllipsizedWidth = outerwidth;
    }
    
    mLines = vector<int>(ArrayUtils::idealIntArraySize(2 * mColumns));
    mLineDirections = vector<Directions>(ArrayUtils::idealIntArraySize(2 * mColumns));
    mMaximumVisibleLineCount = maxLines;
    
    mMeasured = MeasuredText::obtain();
    
    generate(source, bufstart, bufend, paint, outerwidth, textDir, spacingmult,
             spacingadd, includepad, includepad, ellipsizedWidth,
             ellipsize);
    
    mMeasured = MeasuredText::recycle(mMeasured);
}

shared_ptr<CharSequence> StaticLayout::createText(shared_ptr<CharSequence> source, shared_ptr<TextUtils::TruncateAt> ellipsize) {
    if (ellipsize == NULL) {
        return source;
    } else if (dynamic_pointer_cast<Spanned>(source) != NULL) {
        return make_shared<SpannedEllipsizer>(source);
    } else {
        return make_shared<Ellipsizer>(source);
    }
}

StaticLayout::StaticLayout(shared_ptr<CharSequence> text) : Layout(text, NULL, 0, ALIGN_CENTER, 0, 0) {
    
    mColumns = COLUMNS_ELLIPSIZE;
    mLines = vector<int>(ArrayUtils::idealIntArraySize(2 * mColumns));
    mLineDirections = vector<Directions>(ArrayUtils::idealIntArraySize(2 * mColumns));
    // FIXME This is never recycled
    mMeasured = MeasuredText::obtain();
}

void StaticLayout::generate(shared_ptr<CharSequence> source, int bufStart, int bufEnd,
                            shared_ptr<TextPaint> paint, int outerWidth,
                            shared_ptr<TextDirectionHeuristic> textDir, float spacingmult,
                            float spacingadd, bool includepad,
                            bool trackpad, float ellipsizedWidth,
                            shared_ptr<TextUtils::TruncateAt> ellipsize) {
    mLineCount = 0;
    
    int v = 0;
    bool needMultiply = (spacingmult != 1 || spacingadd != 0);
    
    shared_ptr<Paint::FontMetricsInt> &fm = mFontMetricsInt;
    vector<int> chooseHtv;
    
    shared_ptr<MeasuredText> &measured = mMeasured;
    
    shared_ptr<Spanned> spanned = dynamic_pointer_cast<Spanned>(source);
    
    int DEFAULT_DIR = Directions::DIR_LEFT_TO_RIGHT; // XXX
    
    int paraEnd;
    for (int paraStart = bufStart; paraStart <= bufEnd; paraStart = paraEnd) {
        paraEnd = TextUtils::indexOf(source, CHAR_NEW_LINE, paraStart, bufEnd);
        if (paraEnd < 0)
            paraEnd = bufEnd;
        else
            paraEnd++;
        
        int firstWidthLineLimit = mLineCount + 1;
        int firstWidth = outerWidth;
        int restWidth = outerWidth;
        
        vector<shared_ptr<LineHeightSpan>> chooseHt;
        
        if (spanned != NULL) {
            vector<shared_ptr<LeadingMarginSpan>> sp = getParagraphSpans<LeadingMarginSpan>(spanned, paraStart, paraEnd, "LeadingMarginSpan");
            for (int i = 0; i < sp.size(); i++) {
                shared_ptr<LeadingMarginSpan> &lms = sp[i];
                firstWidth -= sp[i]->getLeadingMargin(true);
                restWidth -= sp[i]->getLeadingMargin(false);
                
                // LeadingMarginSpan2 is odd.  The count affects all
                // leading margin spans, not just this particular one,
                // and start from the top of the span, not the top of the
                // paragraph.
                shared_ptr<LeadingMarginSpan2> lms2 = dynamic_pointer_cast<LeadingMarginSpan2>(lms);
                if (lms2 != NULL) {
                    int lmsFirstLine = getLineForOffset(spanned->getSpanStart(lms2));
                    firstWidthLineLimit = lmsFirstLine + lms2->getLeadingMarginLineCount();
                }
            }
            
            chooseHt = getParagraphSpans<LineHeightSpan>(spanned, paraStart, paraEnd, "LineHeightSpan");
            
            if (chooseHt.size() != 0) {
                if (chooseHtv.size() < chooseHt.size()) {
                    chooseHtv = vector<int>(ArrayUtils::idealIntArraySize(chooseHt.size()));
                }
                
                for (int i = 0; i < chooseHt.size(); i++) {
                    int o = spanned->getSpanStart(chooseHt[i]);
                    
                    if (o < paraStart) {
                        // starts in this layout, before the
                        // current paragraph
                        
                        chooseHtv[i] = getLineTop(getLineForOffset(o));
                    } else {
                        // starts in this paragraph
                        
                        chooseHtv[i] = v;
                    }
                }
            }
        }
        
        measured->setPara(source, paraStart, paraEnd, textDir);
        UnicodeString &chs = measured->mChars;
        vector<float> &widths = measured->mWidths;
        vector<int8_t> &chdirs = measured->mLevels;
        int dir = measured->mDir;
        bool easy = measured->mEasy;
        
        int width = firstWidth;
        
        float w = 0;
        // here is the offset of the starting character of the line we are currently measuring
        int here = paraStart;
        
        // ok is a character offset located after a word separator (space, tab, number...) where
        // we would prefer to cut the current line. Equals to here when no such break was found.
        int ok = paraStart;
        float okWidth = w;
        int okAscent = 0, okDescent = 0, okTop = 0, okBottom = 0;
        
        // fit is a character offset such that the [here, fit[ range fits in the allowed width.
        // We will cut the line there if no ok position is found.
        int fit = paraStart;
        float fitWidth = w;
        int fitAscent = 0, fitDescent = 0, fitTop = 0, fitBottom = 0;
        
        bool hasTabOrEmoji = false;
        bool hasTab = false;
        shared_ptr<TabStops<TabStopSpan>> tabStops;
        
        for (int spanStart = paraStart, spanEnd; spanStart < paraEnd; spanStart = spanEnd) {
            
            if (spanned == NULL) {
                spanEnd = paraEnd;
                int spanLen = spanEnd - spanStart;
                measured->addStyleRun(paint, spanLen, fm);
            } else {
                spanEnd = spanned->nextSpanTransition(spanStart, paraEnd,
                                                     "MetricAffectingSpan");
                int spanLen = spanEnd - spanStart;
                vector<shared_ptr<Object>> spans = spanned->getSpans(spanStart, spanEnd, "MetricAffectingSpan");
                spans = TextUtils::removeEmptySpans(spans, spanned, "MetricAffectingSpan");
                measured->addStyleRun(paint, spans, spanLen, fm);
            }
            
            int fmTop = fm->top;
            int fmBottom = fm->bottom;
            int fmAscent = fm->ascent;
            int fmDescent = fm->descent;
            
            for (int j = spanStart; j < spanEnd; j++) {
                UChar c = chs[j - paraStart];
                
                if (c == CHAR_NEW_LINE) {
                    // intentionally left empty
                } else if (c == CHAR_TAB) {
                    if (hasTab == false) {
                        hasTab = true;
                        hasTabOrEmoji = true;
                        if (spanned != NULL) {
                            // First tab this para, check for tabstops
                            vector<shared_ptr<TabStopSpan>> spans = getParagraphSpans<TabStopSpan>(spanned, paraStart, paraEnd, "TabStopSpan");
                            if (spans.size() > 0) {
                                tabStops = make_shared<TabStops<TabStopSpan>>(Layout::TAB_INCREMENT, spans);
                            }
                        }
                    }
                    if (tabStops != NULL) {
                        w = tabStops->nextTab(w);
                    } else {
                        w = TabStops<TabStopSpan>::nextDefaultStop(w, Layout::TAB_INCREMENT);
                    }
                } else if (c >= CHAR_FIRST_HIGH_SURROGATE && c <= CHAR_LAST_LOW_SURROGATE
                           && j + 1 < spanEnd) {
                    int emoji = Character::codePointAt(chs, j - paraStart);
                    
                    //                        if (emoji >= MIN_EMOJI && emoji <= MAX_EMOJI) {
                    //                            Bitmap bm = EMOJI_FACTORY.getBitmapFromAndroidPua(emoji);
                    //
                    //                            if (bm != NULL) {
                    //                                Paint whichPaint;
                    //
                    //                                if (spanned == NULL) {
                    //                                    whichPaint = paint;
                    //                                } else {
                    //                                    whichPaint = mWorkPaint;
                    //                                }
                    //
                    //                                float wid = bm.getWidth() * -whichPaint::ascent() / bm.getHeight();
                    //
                    //                                w += wid;
                    //                                hasTabOrEmoji = true;
                    //                                j++;
                    //                            } else {
                    //                                w += widths[j - paraStart];
                    //                            }
                    //                        } else {
                    w += widths[j - paraStart];
                    //                        }
                } else {
                    w += widths[j - paraStart];
                }
                
                bool isSpaceOrTab = c == CHAR_SPACE || c == CHAR_TAB || c == CHAR_ZWSP;
                
                if (w <= width || isSpaceOrTab) {
                    fitWidth = w;
                    fit = j + 1;
                    
                    if (fmTop < fitTop)
                        fitTop = fmTop;
                    if (fmAscent < fitAscent)
                        fitAscent = fmAscent;
                    if (fmDescent > fitDescent)
                        fitDescent = fmDescent;
                    if (fmBottom > fitBottom)
                        fitBottom = fmBottom;
                    
                    // From the Unicode Line Breaking Algorithm (at least approximately)
                    bool isLineBreak = isSpaceOrTab ||
                    // / is class SY and - is class HY, except when followed by a digit
                    ((c == CHAR_SLASH || c == CHAR_HYPHEN) &&
                     (j + 1 >= spanEnd /*|| !Character::isDigit(chs[j + 1 - paraStart])*/)) ||
                    // Ideographs are class ID: breakpoints when adjacent, except for NS
                    // (non-starters), which can be broken after but not before
                    (c >= CHAR_FIRST_CJK && isIdeographic(c, true) &&
                     j + 1 < spanEnd && isIdeographic(chs[j + 1 - paraStart], false));
                    
                    if (isLineBreak) {
                        okWidth = w;
                        ok = j + 1;
                        
                        if (fitTop < okTop)
                            okTop = fitTop;
                        if (fitAscent < okAscent)
                            okAscent = fitAscent;
                        if (fitDescent > okDescent)
                            okDescent = fitDescent;
                        if (fitBottom > okBottom)
                            okBottom = fitBottom;
                    }
                } else {
                    const bool moreChars = (j + 1 < spanEnd);
                    int endPos;
                    int above, below, top, bottom;
                    float currentTextWidth;
                    
                    if (ok != here) {
                        endPos = ok;
                        above = okAscent;
                        below = okDescent;
                        top = okTop;
                        bottom = okBottom;
                        currentTextWidth = okWidth;
                    } else if (fit != here) {
                        endPos = fit;
                        above = fitAscent;
                        below = fitDescent;
                        top = fitTop;
                        bottom = fitBottom;
                        currentTextWidth = fitWidth;
                    } else {
                        endPos = here + 1;
                        above = fm->ascent;
                        below = fm->descent;
                        top = fm->top;
                        bottom = fm->bottom;
                        currentTextWidth = widths[here - paraStart];
                    }
                    
                    v = outt(source, here, endPos,
                             above, below, top, bottom,
                             v, spacingmult, spacingadd, chooseHt, chooseHtv, fm, hasTabOrEmoji,
                             needMultiply, chdirs, dir, easy, bufEnd, includepad, trackpad,
                             chs, widths, paraStart, ellipsize, ellipsizedWidth,
                             currentTextWidth, paint, moreChars);
                    
                    here = endPos;
                    j = here - 1; // restart j-span loop from here, compensating for the j++
                    ok = fit = here;
                    w = 0;
                    fitAscent = fitDescent = fitTop = fitBottom = 0;
                    okAscent = okDescent = okTop = okBottom = 0;
                    
                    if (--firstWidthLineLimit <= 0) {
                        width = restWidth;
                    }
                    
                    if (here < spanStart) {
                        // The text was cut before the beginning of the current span range.
                        // Exit the span loop, and get spanStart to start over from here.
                        measured->setPos(here);
                        spanEnd = here;
                        break;
                    }
                    
                    if (mLineCount >= mMaximumVisibleLineCount) {
                        break;
                    }
                }
            }
        }
        
        if (paraEnd != here && mLineCount < mMaximumVisibleLineCount) {
            if ((fitTop | fitBottom | fitDescent | fitAscent) == 0) {
                paint->getFontMetricsInt(fm);
                
                fitTop = fm->top;
                fitBottom = fm->bottom;
                fitAscent = fm->ascent;
                fitDescent = fm->descent;
            }
            
            // Log.e("text", "output rest " + here + " to " + end);
            
            v = outt(source,
                     here, paraEnd, fitAscent, fitDescent,
                     fitTop, fitBottom,
                     v,
                     spacingmult, spacingadd, chooseHt,
                     chooseHtv, fm, hasTabOrEmoji,
                     needMultiply, chdirs, dir, easy, bufEnd,
                     includepad, trackpad, chs,
                     widths, paraStart, ellipsize,
                     ellipsizedWidth, w, paint, paraEnd != bufEnd);
        }
        
        paraStart = paraEnd;
        
        if (paraEnd == bufEnd)
            break;
    }
    
    if ((bufEnd == bufStart || source->charAt(bufEnd - 1) == CHAR_NEW_LINE) &&
        mLineCount < mMaximumVisibleLineCount) {
        // Log.e("text", "output last " + bufEnd);
        
        paint->getFontMetricsInt(fm);
        
        v = outt(source,
                 bufEnd, bufEnd, fm->ascent, fm->descent,
                 fm->top, fm->bottom,
                 v,
                 spacingmult, spacingadd, vector<shared_ptr<LineHeightSpan>>(),
                 vector<int>(), fm, false,
                 needMultiply, vector<int8_t>(), DEFAULT_DIR, true, bufEnd,
                 includepad, trackpad, UnicodeString(),
                 vector<float>(), bufStart, ellipsize,
                 ellipsizedWidth, 0, paint, false);
    }
}


bool StaticLayout::isIdeographic(UChar c, bool includeNonStarters) {
    if (c >= 0x2E80 && c <= 0x2FFF) {
        return true; // CJK, KANGXI RADICALS, DESCRIPTION SYMBOLS
    }
    if (c == 0x3000) {
        return true; // IDEOGRAPHIC SPACE
    }
    if (c >= 0x3040 && c <= 0x309F) {
        if (!includeNonStarters) {
            switch (c) {
                case 0x3041: //  # HIRAGANA LETTER SMALL A
                case 0x3043: //  # HIRAGANA LETTER SMALL I
                case 0x3045: //  # HIRAGANA LETTER SMALL U
                case 0x3047: //  # HIRAGANA LETTER SMALL E
                case 0x3049: //  # HIRAGANA LETTER SMALL O
                case 0x3063: //  # HIRAGANA LETTER SMALL TU
                case 0x3083: //  # HIRAGANA LETTER SMALL YA
                case 0x3085: //  # HIRAGANA LETTER SMALL YU
                case 0x3087: //  # HIRAGANA LETTER SMALL YO
                case 0x308E: //  # HIRAGANA LETTER SMALL WA
                case 0x3095: //  # HIRAGANA LETTER SMALL KA
                case 0x3096: //  # HIRAGANA LETTER SMALL KE
                case 0x309B: //  # KATAKANA-HIRAGANA VOICED SOUND MARK
                case 0x309C: //  # KATAKANA-HIRAGANA SEMI-VOICED SOUND MARK
                case 0x309D: //  # HIRAGANA ITERATION MARK
                case 0x309E: //  # HIRAGANA VOICED ITERATION MARK
                    return false;
            }
        }
        return true; // Hiragana (except small characters)
    }
    if (c >= 0x30A0 && c <= 0x30FF) {
        if (!includeNonStarters) {
            switch (c) {
                case 0x30A0: //  # KATAKANA-HIRAGANA DOUBLE HYPHEN
                case 0x30A1: //  # KATAKANA LETTER SMALL A
                case 0x30A3: //  # KATAKANA LETTER SMALL I
                case 0x30A5: //  # KATAKANA LETTER SMALL U
                case 0x30A7: //  # KATAKANA LETTER SMALL E
                case 0x30A9: //  # KATAKANA LETTER SMALL O
                case 0x30C3: //  # KATAKANA LETTER SMALL TU
                case 0x30E3: //  # KATAKANA LETTER SMALL YA
                case 0x30E5: //  # KATAKANA LETTER SMALL YU
                case 0x30E7: //  # KATAKANA LETTER SMALL YO
                case 0x30EE: //  # KATAKANA LETTER SMALL WA
                case 0x30F5: //  # KATAKANA LETTER SMALL KA
                case 0x30F6: //  # KATAKANA LETTER SMALL KE
                case 0x30FB: //  # KATAKANA MIDDLE DOT
                case 0x30FC: //  # KATAKANA-HIRAGANA PROLONGED SOUND MARK
                case 0x30FD: //  # KATAKANA ITERATION MARK
                case 0x30FE: //  # KATAKANA VOICED ITERATION MARK
                    return false;
            }
        }
        return true; // Katakana (except small characters)
    }
    if (c >= 0x3400 && c <= 0x4DB5) {
        return true; // CJK UNIFIED IDEOGRAPHS EXTENSION A
    }
    if (c >= 0x4E00 && c <= 0x9FBB) {
        return true; // CJK UNIFIED IDEOGRAPHS
    }
    if (c >= 0xF900 && c <= 0xFAD9) {
        return true; // CJK COMPATIBILITY IDEOGRAPHS
    }
    if (c >= 0xA000 && c <= 0xA48F) {
        return true; // YI SYLLABLES
    }
    if (c >= 0xA490 && c <= 0xA4CF) {
        return true; // YI RADICALS
    }
    if (c >= 0xFE62 && c <= 0xFE66) {
        return true; // SMALL PLUS SIGN to SMALL EQUALS SIGN
    }
    if (c >= 0xFF10 && c <= 0xFF19) {
        return true; // WIDE DIGITS
    }
    
    return false;
}

int StaticLayout::outt(shared_ptr<CharSequence> text, int start, int end,
                       int above, int below, int top, int bottom, int v,
                       float spacingmult, float spacingadd,
                       vector<shared_ptr<LineHeightSpan>> chooseHt, vector<int> chooseHtv,
                       shared_ptr<Paint::FontMetricsInt> fm, bool hasTabOrEmoji,
                       bool needMultiply, vector<int8_t> chdirs, int dir,
                       bool easy, int bufEnd, bool includePad,
                       bool trackPad, UnicodeString chs,
                       vector<float> widths, int widthStart, shared_ptr<TextUtils::TruncateAt> ellipsize,
                       float ellipsisWidth, float textWidth,
                       shared_ptr<TextPaint> paint, bool moreChars) {
    int j = mLineCount;
    int off = j * mColumns;
    int want = off + mColumns + TOP;
    vector<int> &lines = mLines;
    
    if (want >= lines.size()) {
        int nlen = ArrayUtils::idealIntArraySize(want + 1);
        vector<int> grow = vector<int>(nlen);
        System::arraycopy(lines, 0, grow, 0, lines.size());
        mLines = grow;
        lines = grow;
        
        vector<Directions> grow2 = vector<Directions>(nlen);
        System::arraycopy(mLineDirections, 0, grow2, 0,
                         mLineDirections.size());
        mLineDirections = grow2;
    }
    
    if (!chooseHt.empty()) {
        fm->ascent = above;
        fm->descent = below;
        fm->top = top;
        fm->bottom = bottom;
        
        for (int i = 0; i < chooseHt.size(); i++) {
            shared_ptr<LineHeightSpan::WithDensity> withDensity = dynamic_pointer_cast<LineHeightSpan::WithDensity>(chooseHt[i]);
            if (withDensity != NULL) {
                withDensity->chooseHeight(text, start, end, chooseHtv[i], v, fm, paint);
                
            } else {
                chooseHt[i]->chooseHeight(text, start, end, chooseHtv[i], v, fm);
            }
        }
        
        above = fm->ascent;
        below = fm->descent;
        top = fm->top;
        bottom = fm->bottom;
    }
    
    if (j == 0) {
        if (trackPad) {
            mTopPadding = top - above;
        }
        
        if (includePad) {
            above = top;
        }
    }
    if (end == bufEnd) {
        if (trackPad) {
            mBottomPadding = bottom - below;
        }
        
        if (includePad) {
            below = bottom;
        }
    }
    
    int extra;
    
    if (needMultiply) {
        double ex = (below - above) * (spacingmult - 1) + spacingadd;
        if (ex >= 0) {
            extra = (int)(ex + EXTRA_ROUNDING);
        } else {
            extra = -(int)(-ex + EXTRA_ROUNDING);
        }
    } else {
        extra = 0;
    }
    
    lines[off + START] = start;
    lines[off + TOP] = v;
    lines[off + DESCENT] = below + extra;
    
    v += (below - above) + extra;
    lines[off + mColumns + START] = end;
    lines[off + mColumns + TOP] = v;
    
    if (hasTabOrEmoji)
        lines[off + TAB] |= TAB_MASK;
    
    lines[off + DIR] |= dir << DIR_SHIFT;
    Directions linedirs = Directions::DIRS_ALL_LEFT_TO_RIGHT;
    // easy means all chars < the first RTL, so no emoji, no nothing
    // XXX a run with no text or all spaces is easy but might be an empty
    // RTL paragraph.  Make sure easy is false if this is the case.
    if (easy) {
        mLineDirections[j] = linedirs;
    } else {
        mLineDirections[j] = AndroidBidi::directions(dir, chdirs, start - widthStart, chs,
                                                    start - widthStart, end - start);
    }
    
    if (ellipsize != NULL) {
        // If there is only one line, then do any type of ellipsis except when it is MARQUEE
        // if there are multiple lines, just allow END ellipsis on the last line
        bool firstLine = (j == 0);
        bool currentLineIsTheLastVisibleOne = (j + 1 == mMaximumVisibleLineCount);
        bool forceEllipsis = moreChars && (mLineCount + 1 == mMaximumVisibleLineCount);
        
        bool doEllipsis =
        (((mMaximumVisibleLineCount == 1 && moreChars) || (firstLine && !moreChars)) &&
         ellipsize != TextUtils::TruncateAt::MARQUEE) ||
        (!firstLine && (currentLineIsTheLastVisibleOne || !moreChars) &&
         ellipsize == TextUtils::TruncateAt::END);
        if (doEllipsis) {
            calculateEllipsis(start, end, widths, widthStart,
                              ellipsisWidth, ellipsize, j,
                              textWidth, paint, forceEllipsis);
        }
    }
    
    mLineCount++;
    return v;
}

void StaticLayout::calculateEllipsis(int lineStart, int lineEnd,
                                     vector<float> widths, int widthStart,
                                     float avail, shared_ptr<TextUtils::TruncateAt> where,
                                     int line, float textWidth, shared_ptr<TextPaint> paint,
                                     bool forceEllipsis) {
    if (textWidth <= avail && !forceEllipsis) {
        // Everything fits!
        mLines[mColumns * line + ELLIPSIS_START] = 0;
        mLines[mColumns * line + ELLIPSIS_COUNT] = 0;
        return;
    }
    
    float ellipsisWidth = paint->measureText((where == TextUtils::TruncateAt::END_SMALL) ?
                                            &ELLIPSIS_TWO_DOTS[0] : &ELLIPSIS_NORMAL[0], 0, 1);
    int ellipsisStart = 0;
    int ellipsisCount = 0;
    int len = lineEnd - lineStart;
    
    // We only support start ellipsis on a single line
    if (where == TextUtils::TruncateAt::START) {
        if (mMaximumVisibleLineCount == 1) {
            float sum = 0;
            int i;
            
            for (i = len; i >= 0; i--) {
                float w = widths[i - 1 + lineStart - widthStart];
                
                if (w + sum + ellipsisWidth > avail) {
                    break;
                }
                
                sum += w;
            }
            
            ellipsisStart = 0;
            ellipsisCount = i;
        } else {
//            if (Log.isLoggable(TAG, Log.WARN)) {
//                Log.w(TAG, "Start Ellipsis only supported with one line");
//            }
        }
    } else if (where == TextUtils::TruncateAt::END || where == TextUtils::TruncateAt::MARQUEE ||
               where == TextUtils::TruncateAt::END_SMALL) {
        float sum = 0;
        int i;
        
        for (i = 0; i < len; i++) {
            float w = widths[i + lineStart - widthStart];
            
            if (w + sum + ellipsisWidth > avail) {
                break;
            }
            
            sum += w;
        }
        
        ellipsisStart = i;
        ellipsisCount = len - i;
        if (forceEllipsis && ellipsisCount == 0 && len > 0) {
            ellipsisStart = len - 1;
            ellipsisCount = 1;
        }
    } else {
        // where = TextUtils::TruncateAt::MIDDLE We only support middle ellipsis on a single line
        if (mMaximumVisibleLineCount == 1) {
            float lsum = 0, rsum = 0;
            int left = 0, right = len;
            
            float ravail = (avail - ellipsisWidth) / 2;
            for (right = len; right >= 0; right--) {
                float w = widths[right - 1 + lineStart - widthStart];
                
                if (w + rsum > ravail) {
                    break;
                }
                
                rsum += w;
            }
            
            float lavail = avail - ellipsisWidth - rsum;
            for (left = 0; left < right; left++) {
                float w = widths[left + lineStart - widthStart];
                
                if (w + lsum > lavail) {
                    break;
                }
                
                lsum += w;
            }
            
            ellipsisStart = left;
            ellipsisCount = right - left;
        } else {
//            if (Log.isLoggable(TAG, Log.WARN)) {
//                Log.w(TAG, "Middle Ellipsis only supported with one line");
//            }
        }
    }
    
    mLines[mColumns * line + ELLIPSIS_START] = ellipsisStart;
    mLines[mColumns * line + ELLIPSIS_COUNT] = ellipsisCount;
}

// Override the base class so we can directly access our members,
// rather than relying on member functions.
// The logic mirrors that of Layout.getLineForVertical
// FIXME: It may be faster to do a linear search for layouts without many lines.
int StaticLayout::getLineForVertical(int vertical) {
    int high = mLineCount;
    int low = -1;
    int guess;
    vector<int> &lines = mLines;
    while (high - low > 1) {
        guess = (high + low) >> 1;
        if (lines[mColumns * guess + TOP] > vertical){
            high = guess;
        } else {
            low = guess;
        }
    }
    if (low < 0) {
        return 0;
    } else {
        return low;
    }
}

int StaticLayout::getLineCount() {
    return mLineCount;
}

int StaticLayout::getLineTop(int line) {
    int top = mLines[mColumns * line + TOP];
    if (mMaximumVisibleLineCount > 0 && line >= mMaximumVisibleLineCount &&
        line != mLineCount) {
        top += getBottomPadding();
    }
    return top;
}

int StaticLayout::getLineDescent(int line) {
    int descent = mLines[mColumns * line + DESCENT];
    if (mMaximumVisibleLineCount > 0 && line >= mMaximumVisibleLineCount - 1 && // -1 intended
        line != mLineCount) {
        descent += getBottomPadding();
    }
    return descent;
}

int StaticLayout::getLineStart(int line) {
    return mLines[mColumns * line + START] & START_MASK;
}

int StaticLayout::getParagraphDirection(int line) {
    return mLines[mColumns * line + DIR] >> DIR_SHIFT;
}

bool StaticLayout::getLineContainsTab(int line) {
    return (mLines[mColumns * line + TAB] & TAB_MASK) != 0;
}

Directions StaticLayout::getLineDirections(int line) {
    return mLineDirections[line];
}

int StaticLayout::getTopPadding() {
    return mTopPadding;
}

int StaticLayout::getBottomPadding() {
    return mBottomPadding;
}

int StaticLayout::getEllipsisCount(int line) {
    if (mColumns < COLUMNS_ELLIPSIZE) {
        return 0;
    }
    
    return mLines[mColumns * line + ELLIPSIS_COUNT];
}

int StaticLayout::getEllipsisStart(int line) {
    if (mColumns < COLUMNS_ELLIPSIZE) {
        return 0;
    }
    
    return mLines[mColumns * line + ELLIPSIS_START];
}

int StaticLayout::getEllipsizedWidth() {
    return mEllipsizedWidth;
}

void StaticLayout::prepare() {
    mMeasured = MeasuredText::obtain();
}

void StaticLayout::finish() {
    mMeasured = MeasuredText::recycle(mMeasured);
}

ANDROID_END