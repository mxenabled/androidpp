//
//  Layout.cpp
//  Androidpp
//
//  Created by Saul Howard on 1/20/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#include "Layout.h"

#include "Android/graphics/Canvas.h"
#include "Android/graphics/Paint.h"
#include "Android/graphics/Path.h"
#include "Android/graphics/Rect.h"
#include "Android/text/AndroidBidi.h"
#include "Android/text/Directions.h"
#include "Android/text/MeasuredText.h"
#include "Android/text/TextDirectionHeuristics.h"
#include "Android/text/TextLine.h"
#include "Android/text/TextPaint.h"
#include "Android/text/style/AlignmentSpan.h"
#include "Android/text/style/LeadingMarginSpan.h"
#include "Android/text/style/LineBackgroundSpan.h"
#include "Android/text/style/TabStopSpan.h"
#include "Android/text/SpannableString.h"
#include "Android/text/String.h"
#include "Android/utils/ArrayUtils.h"
#include "Android/utils/Exceptions.h"
#include "Android/utils/System.h"

#include <cmath>
#include <math.h>
#include <sstream>

ANDROID_BEGIN

const int Layout::TAB_INCREMENT = 20;

Rect Layout::sTempRect = Rect();

vector<shared_ptr<ParagraphStyle>> Layout::NO_PARA_SPANS = vector<shared_ptr<ParagraphStyle>>();

const vector<UChar> Layout::ELLIPSIS_NORMAL = { 0x2026 }; // this is "..."
const vector<UChar> Layout::ELLIPSIS_TWO_DOTS = { 0x2025 };

float Layout::getDesiredWidth(shared_ptr<CharSequence> source,
                      shared_ptr<TextPaint> paint) {
    return getDesiredWidth(source, 0, source->length(), paint);
}

float Layout::getDesiredWidth(shared_ptr<CharSequence> source,
                      int start, int end,
                      shared_ptr<TextPaint> paint) {
    float need = 0;
    
    int next;
    for (int i = start; i <= end; i = next) {
        next = TextUtils::indexOf(source, '\n', i, end);
        
        if (next < 0)
            next = end;
        
        // note, omits trailing paragraph char
        float w = measurePara(paint, source, i, next);
        
        if (w > need)
            need = w;
        
        next++;
    }
    
    return need;
}

Layout::Layout(shared_ptr<CharSequence> text, shared_ptr<TextPaint> paint,
       int width, Alignment align,
               float spacingMult, float spacingAdd) : Layout(text, paint, width, align, TextDirectionHeuristics::FIRSTSTRONG_LTR,
                                                             spacingMult, spacingAdd) {
}

Layout::Layout(shared_ptr<CharSequence> text, shared_ptr<TextPaint> paint,
       int width, Alignment align, shared_ptr<TextDirectionHeuristic> textDir,
               float spacingMult, float spacingAdd) : mLineBackgroundSpans("LineBackgroundSpan") {
    
    if (width < 0) {
        stringstream str;
        str << "Layout: " << width << " < 0";
        throw IllegalArgumentException(str.str());
    }
    
    // Ensure paint doesn't have baselineShift set.
    // While normally we don't modify the paint the user passed in,
    // we were already doing this in Styled.drawUniformRun with both
    // baselineShift and bgColor.  We probably should reevaluate bgColor.
    if (paint != NULL) {
        paint->bgColor = 0;
        paint->baselineShift = 0;
    }
    
    mText = text;
    mPaint = paint;
    mWorkPaint = make_shared<TextPaint>();
    mWidth = width;
    mAlignment = align;
    mSpacingMult = spacingMult;
    mSpacingAdd = spacingAdd;
    mSpannedText = dynamic_pointer_cast<Spanned>(text);
    mTextDir = textDir;
}

void Layout::replaceWith(shared_ptr<CharSequence> text, shared_ptr<TextPaint> paint,
                 int width, Alignment align,
                 float spacingmult, float spacingadd) {
    if (width < 0) {
        stringstream str;
        str << "Layout: " << width << " < 0";
        throw IllegalArgumentException(str.str());
    }
    
    mText = text;
    mPaint = paint;
    mWidth = width;
    mAlignment = align;
    mSpacingMult = spacingmult;
    mSpacingAdd = spacingadd;
    mSpannedText = dynamic_pointer_cast<Spanned>(text);
}

void Layout::draw(shared_ptr<Canvas> c) {
    draw(c, NULL, NULL, 0);
}

void Layout::draw(shared_ptr<Canvas> canvas, Path *highlight, Paint *highlightPaint,
          int cursorOffsetVertical) {
    const long long lineRange = getLineRangeForDraw(canvas);
    int firstLine = TextUtils::unpackRangeStartFromLong(lineRange);
    int lastLine = TextUtils::unpackRangeEndFromLong(lineRange);
    if (lastLine < 0) return;
    
    drawBackground(canvas, highlight, highlightPaint, cursorOffsetVertical,
                   firstLine, lastLine);
    drawText(canvas, firstLine, lastLine);
}

void Layout::drawText(shared_ptr<Canvas> canvas, int firstLine, int lastLine) {
    int previousLineBottom = getLineTop(firstLine);
    int previousLineEnd = getLineStart(firstLine);
    vector<shared_ptr<ParagraphStyle>> spans = NO_PARA_SPANS;
    int spanEnd = 0;
    shared_ptr<TextPaint> paint = mPaint;
    shared_ptr<CharSequence> buf = mText;
    
    Alignment &paraAlign = mAlignment;
    shared_ptr<TabStops<ParagraphStyle>> tabStops;
    bool tabStopsIsInitialized = false;
    
    shared_ptr<TextLine<ParagraphStyle>> tl = TextLine<ParagraphStyle>::obtain();
    
    // Draw the lines, one at a time.
    // The baseline is the top of the following line minus the current line's descent.
    for (int i = firstLine; i <= lastLine; i++) {
        int start = previousLineEnd;
        previousLineEnd = getLineStart(i + 1);
        int end = getLineVisibleEnd(i, start, previousLineEnd);
        
        int ltop = previousLineBottom;
        int lbottom = getLineTop(i+1);
        previousLineBottom = lbottom;
        int lbaseline = lbottom - getLineDescent(i);
        
        int dir = getParagraphDirection(i);
        int left = 0;
        int right = mWidth;
        
        if (mSpannedText != 0) {
            int textLength = buf->length();
            bool isFirstParaLine = (start == 0 || buf->charAt(start - 1) == '\n');
            
            // New batch of paragraph styles, collect into spans array.
            // Compute the alignment, last alignment style wins.
            // Reset tabStops, we'll rebuild if we encounter a line with
            // tabs.
            // We expect paragraph spans to be relatively infrequent, use
            // spanEnd so that we can check less frequently.  Since
            // paragraph styles ought to apply to entire paragraphs, we can
            // just collect the ones present at the start of the paragraph.
            // If spanEnd is before the end of the paragraph, that's not
            // our problem.
            if (start >= spanEnd && (i == firstLine || isFirstParaLine)) {
                spanEnd = mSpannedText->nextSpanTransition(start, textLength, "ParagraphStyle");
                spans = getParagraphSpans<ParagraphStyle>(mSpannedText, start, spanEnd, "ParagraphStyle");
                
                paraAlign = mAlignment;
                shared_ptr<AlignmentSpan> alignmentSpan;
                for (int n = spans.size() - 1; n >= 0; n--) {
                    alignmentSpan = dynamic_pointer_cast<AlignmentSpan>(spans[n]);
                    if (alignmentSpan != NULL) {
                        paraAlign = alignmentSpan->getAlignment();
                        break;
                    }
                }
                
                tabStopsIsInitialized = false;
            }
            
            // Draw all leading margin spans.  Adjust left or right according
            // to the paragraph direction of the line.
            const int length = spans.size();
            for (int n = 0; n < length; n++) {
                shared_ptr<LeadingMarginSpan> margin = dynamic_pointer_cast<LeadingMarginSpan>(spans[n]);
                if (margin != NULL) {
                    bool useFirstLineMargin = isFirstParaLine;
                    shared_ptr<LeadingMarginSpan2> margin2 = dynamic_pointer_cast<LeadingMarginSpan2>(margin);
                    if (margin2 != NULL) {
                        int count = margin2->getLeadingMarginLineCount();
                        int startLine = getLineForOffset(mSpannedText->getSpanStart(margin));
                        useFirstLineMargin = i < startLine + count;
                    }
                    
                    if (dir == Directions::DIR_RIGHT_TO_LEFT) {
                        margin->drawLeadingMargin(canvas, paint, right, dir, ltop,
                                                 lbaseline, lbottom, buf,
                                                 start, end, isFirstParaLine, this);
                        right -= margin->getLeadingMargin(useFirstLineMargin);
                    } else {
                        margin->drawLeadingMargin(canvas, paint, left, dir, ltop,
                                                 lbaseline, lbottom, buf,
                                                 start, end, isFirstParaLine, this);
                        left += margin->getLeadingMargin(useFirstLineMargin);
                    }
                }
            }
        }
        
        bool hasTabOrEmoji = getLineContainsTab(i);
        // Can't tell if we have tabs for sure, currently
        if (hasTabOrEmoji && !tabStopsIsInitialized) {
            if (tabStops == NULL) {
                tabStops = make_shared<TabStops<ParagraphStyle>>(TAB_INCREMENT, spans);
            } else {
                tabStops->reset(TAB_INCREMENT, spans);
            }
            tabStopsIsInitialized = true;
        }
        
        // Determine whether the line aligns to normal, opposite, or center.
        Alignment align = paraAlign;
        if (align == Alignment::ALIGN_LEFT) {
            align = (dir == Directions::DIR_LEFT_TO_RIGHT) ?
            Alignment::ALIGN_NORMAL : Alignment::ALIGN_OPPOSITE;
        } else if (align == Alignment::ALIGN_RIGHT) {
            align = (dir == Directions::DIR_LEFT_TO_RIGHT) ?
            Alignment::ALIGN_OPPOSITE : Alignment::ALIGN_NORMAL;
        }
        
        int x = 0;
        if (align == Alignment::ALIGN_NORMAL) {
            if (dir == Directions::DIR_LEFT_TO_RIGHT) {
                x = left;
            } else {
                x = right;
            }
        } else {
            int max = (int) getLineExtent<ParagraphStyle>(i, tabStops, false);
            if (align == Alignment::ALIGN_OPPOSITE) {
                if (dir == Directions::DIR_LEFT_TO_RIGHT) {
                    x = right - max;
                } else {
                    x = left - max;
                }
            } else { // Alignment::ALIGN_CENTER
                max = max & ~1;
                x = (right + left - max) >> 1;
            }
        }
        
        Directions directions = getLineDirections(i);
        if (directions == Directions::DIRS_ALL_LEFT_TO_RIGHT && !mSpannedText && !hasTabOrEmoji) {
            // XXX: assumes there's nothing additional to be done
            canvas->drawText(buf, start, end, x, lbaseline, paint.get());
        } else {
            tl->set(paint, buf, start, end, dir, directions, hasTabOrEmoji, tabStops);
            tl->draw(canvas, x, ltop, lbaseline, lbottom);
        }
    }
    
    TextLine<ParagraphStyle>::recycle(tl);
}

void Layout::drawBackground(shared_ptr<Canvas> canvas, Path *highlight, Paint *highlightPaint,
                    int cursorOffsetVertical, int firstLine, int lastLine) {
    // First, draw LineBackgroundSpans.
    // LineBackgroundSpans know nothing about the alignment, margins, or
    // direction of the layout or line.  XXX: Should they?
    // They are evaluated at each line.
    if (mSpannedText) {

        shared_ptr<Spanned> buffer = mSpannedText;
        int textLength = buffer->length();
        mLineBackgroundSpans.init(buffer, 0, textLength);
        
        if (mLineBackgroundSpans.numberOfSpans > 0) {
            int previousLineBottom = getLineTop(firstLine);
            int previousLineEnd = getLineStart(firstLine);
            vector<shared_ptr<ParagraphStyle>> spans = NO_PARA_SPANS;
            int spansLength = 0;
            shared_ptr<TextPaint> paint = mPaint;
            int spanEnd = 0;
            const int width = mWidth;
            for (int i = firstLine; i <= lastLine; i++) {
                int start = previousLineEnd;
                int end = getLineStart(i + 1);
                previousLineEnd = end;
                
                int ltop = previousLineBottom;
                int lbottom = getLineTop(i + 1);
                previousLineBottom = lbottom;
                int lbaseline = lbottom - getLineDescent(i);
                
                if (start >= spanEnd) {
                    // These should be infrequent, so we'll use this so that
                    // we don't have to check as often.
                    spanEnd = mLineBackgroundSpans.getNextTransition(start, textLength);
                    // All LineBackgroundSpans on a line contribute to its background.
                    spansLength = 0;
                    // Duplication of the logic of getParagraphSpans
                    if (start != end || start == 0) {
                        // Equivalent to a getSpans(start, end), but filling the 'spans' local
                        // array instead to reduce memory allocation
                        for (int j = 0; j < mLineBackgroundSpans.numberOfSpans; j++) {
                            // equal test is valid since both intervals are not empty by
                            // construction
                            if (mLineBackgroundSpans.spanStarts[j] >= end ||
                                mLineBackgroundSpans.spanEnds[j] <= start) continue;
                            if (spansLength == spans.size()) {
                                // The spans array needs to be expanded
                                int newSize = ArrayUtils::idealObjectArraySize(2 * spansLength);
                                vector<shared_ptr<ParagraphStyle>> newSpans = vector<shared_ptr<ParagraphStyle>>(newSize);
                                System::arraycopy(spans, 0, newSpans, 0, spansLength);
                                spans = newSpans;
                            }
                            spans[spansLength++] = mLineBackgroundSpans.spans[j];
                        }
                    }
                }
                
                for (int n = 0; n < spansLength; n++) {
                    shared_ptr<LineBackgroundSpan> lineBackgroundSpan = dynamic_pointer_cast<LineBackgroundSpan>(spans[n]);
                    lineBackgroundSpan->drawBackground(canvas, paint.get(), 0, width,
                                                      ltop, lbaseline, lbottom,
                                                      buffer.get(), start, end, i);
                }
            }
        }
        mLineBackgroundSpans.recycle();
    }
    
    // There can be a highlight even without spans if we are drawing
    // a non-spanned transformation of a spanned editing buffer->
    if (highlight != NULL) {
        if (cursorOffsetVertical != 0) canvas->translate(0, cursorOffsetVertical);
        canvas->drawPath(highlight, highlightPaint);
        if (cursorOffsetVertical != 0) canvas->translate(0, -cursorOffsetVertical);
    }
}

long long Layout::getLineRangeForDraw(shared_ptr<Canvas> canvas) {
    int dtop, dbottom;
    
    // TODO: synchronization here
//    synchronized (sTempRect) {
        if (!canvas->getClipBounds(&sTempRect)) {
            // Negative range end used as a special flag
            return TextUtils::packRangeInLong(0, -1);
        }
        
        dtop = sTempRect.top();
        dbottom = sTempRect.bottom();
//    }
    
    const int top = max(dtop, 0);
    const int bottom = min(getLineTop(getLineCount()), dbottom);
    
    if (top >= bottom) return TextUtils::packRangeInLong(0, -1);
    return TextUtils::packRangeInLong(getLineForVertical(top), getLineForVertical(bottom));
}

int Layout::getLineStartPos(int line, int left, int right) {
    // Adjust the point at which to start rendering depending on the
    // alignment of the paragraph.
    Alignment align = getParagraphAlignment(line);
    int dir = getParagraphDirection(line);
    
    if (align == Alignment::ALIGN_LEFT) {
        align = (dir == Directions::DIR_LEFT_TO_RIGHT) ? Alignment::ALIGN_NORMAL : Alignment::ALIGN_OPPOSITE;
    } else if (align == Alignment::ALIGN_RIGHT) {
        align = (dir == Directions::DIR_LEFT_TO_RIGHT) ? Alignment::ALIGN_OPPOSITE : Alignment::ALIGN_NORMAL;
    }
    
    int x;
    if (align == Alignment::ALIGN_NORMAL) {
        if (dir == Directions::DIR_LEFT_TO_RIGHT) {
            x = left;
        } else {
            x = right;
        }
    } else {
        shared_ptr<TabStops<TabStopSpan>> tabStops;
        if (mSpannedText && getLineContainsTab(line)) {
            shared_ptr<Spanned> spanned = mSpannedText;
            int start = getLineStart(line);
            int spanEnd = spanned->nextSpanTransition(start, spanned->length(),
                                                     "TabStopSpan");
            vector<shared_ptr<TabStopSpan>> tabSpans = getParagraphSpans<TabStopSpan>(spanned, start, spanEnd,
                                                       "TabStopSpan");
            if (tabSpans.size() > 0) {
                tabStops = make_shared<TabStops<TabStopSpan>>(TAB_INCREMENT, tabSpans);
            }
        }
        int max = (int)getLineExtent(line, tabStops, false);
        if (align == Alignment::ALIGN_OPPOSITE) {
            if (dir == Directions::DIR_LEFT_TO_RIGHT) {
                x = right - max;
            } else {
                // max is negative here
                x = left - max;
            }
        } else { // Alignment::ALIGN_CENTER
            max = max & ~1;
            x = (left + right - max) >> 1;
        }
    }
    return x;
}

void Layout::increaseWidthTo(int wid) {
    if (wid < mWidth) {
        throw RuntimeException("attempted to reduce Layout width");
    }
    
    mWidth = wid;
}

int Layout::getLineBounds(int line, Rect bounds) {
    
    bounds.setLTRB(0, getLineTop(line), mWidth, getLineTop(line + 1));
    
    return getLineBaseline(line);
}

bool Layout::isLevelBoundary(int offset) {
    int line = getLineForOffset(offset);
    Directions dirs = getLineDirections(line);
    if (dirs == Directions::DIRS_ALL_LEFT_TO_RIGHT || dirs == Directions::DIRS_ALL_RIGHT_TO_LEFT) {
        return false;
    }
    
    vector<int> runs = dirs.mDirections;
    int lineStart = getLineStart(line);
    int lineEnd = getLineEnd(line);
    if (offset == lineStart || offset == lineEnd) {
        int paraLevel = getParagraphDirection(line) == 1 ? 0 : 1;
        int runIndex = offset == lineStart ? 0 : runs.size() - 2;
        return ((runs[runIndex + 1] >> Directions::RUN_LEVEL_SHIFT) & Directions::RUN_LEVEL_MASK) != paraLevel;
    }
    
    offset -= lineStart;
    for (int i = 0; i < runs.size(); i += 2) {
        if (offset == runs[i]) {
            return true;
        }
    }
    return false;
}

bool Layout::isRtlCharAt(int offset) {
    int line = getLineForOffset(offset);
    Directions dirs = getLineDirections(line);
    if (dirs == Directions::DIRS_ALL_LEFT_TO_RIGHT) {
        return false;
    }
    if (dirs == Directions::DIRS_ALL_RIGHT_TO_LEFT) {
        return  true;
    }
    vector<int> runs = dirs.mDirections;
    int lineStart = getLineStart(line);
    for (int i = 0; i < runs.size(); i += 2) {
        int start = lineStart + (runs[i] & Directions::RUN_LENGTH_MASK);
        // No need to test the end as an offset after the last run should return the value
        // corresponding of the last run
        if (offset >= start) {
            int level = (runs[i+1] >> Directions::RUN_LEVEL_SHIFT) & Directions::RUN_LEVEL_MASK;
            return ((level & 1) != 0);
        }
    }
    // Should happen only if the offset is "out of bounds"
    return false;
}

bool Layout::primaryIsTrailingPrevious(int offset) {
    int line = getLineForOffset(offset);
    int lineStart = getLineStart(line);
    int lineEnd = getLineEnd(line);
    vector<int> runs = getLineDirections(line).mDirections;
    
    int levelAt = -1;
    for (int i = 0; i < runs.size(); i += 2) {
        int start = lineStart + runs[i];
        int limit = start + (runs[i+1] & Directions::RUN_LENGTH_MASK);
        if (limit > lineEnd) {
            limit = lineEnd;
        }
        if (offset >= start && offset < limit) {
            if (offset > start) {
                // Previous character is at same level, so don't use trailing.
                return false;
            }
            levelAt = (runs[i+1] >> Directions::RUN_LEVEL_SHIFT) & Directions::RUN_LEVEL_MASK;
            break;
        }
    }
    if (levelAt == -1) {
        // Offset was limit of line.
        levelAt = getParagraphDirection(line) == 1 ? 0 : 1;
    }
    
    // At level boundary, check previous level.
    int levelBefore = -1;
    if (offset == lineStart) {
        levelBefore = getParagraphDirection(line) == 1 ? 0 : 1;
    } else {
        offset -= 1;
        for (int i = 0; i < runs.size(); i += 2) {
            int start = lineStart + runs[i];
            int limit = start + (runs[i+1] & Directions::RUN_LENGTH_MASK);
            if (limit > lineEnd) {
                limit = lineEnd;
            }
            if (offset >= start && offset < limit) {
                levelBefore = (runs[i+1] >> Directions::RUN_LEVEL_SHIFT) & Directions::RUN_LEVEL_MASK;
                break;
            }
        }
    }
    
    return levelBefore < levelAt;
}

float Layout::getHorizontal(int offset, bool trailing, bool clamped) {
    int line = getLineForOffset(offset);
    
    return getHorizontal(offset, trailing, line, clamped);
}

float Layout::getHorizontal(int offset, bool trailing, int line, bool clamped) {
    int start = getLineStart(line);
    int end = getLineEnd(line);
    int dir = getParagraphDirection(line);
    bool hasTabOrEmoji = getLineContainsTab(line);
    Directions directions = getLineDirections(line);
    
    shared_ptr<TabStops<TabStopSpan>> tabStops;
    if (hasTabOrEmoji && isSpanned()) {
        // Just checking this line should be good enough, tabs should be
        // consistent across all lines in a paragraph.
        vector<shared_ptr<TabStopSpan>> tabs = getParagraphSpans<TabStopSpan>(mSpannedText, start, end, "TabStopSpan");
        if (tabs.size() > 0) {
            tabStops = make_shared<TabStops<TabStopSpan>>(TAB_INCREMENT, tabs); // XXX should reuse
        }
    }
    
    shared_ptr<TextLine<TabStopSpan>> tl = TextLine<TabStopSpan>::obtain();
    tl->set(mPaint, mText, start, end, dir, directions, hasTabOrEmoji, tabStops);
    float wid = tl->measure(offset - start, trailing, NULL);
    TextLine<TabStopSpan>::recycle(tl);
    
    if (clamped && wid > mWidth) {
        wid = mWidth;
    }
    int left = getParagraphLeft(line);
    int right = getParagraphRight(line);
    
    return getLineStartPos(line, left, right) + wid;
}

float Layout::getLineLeft(int line) {
    int dir = getParagraphDirection(line);
    Alignment align = getParagraphAlignment(line);
    
    if (align == Alignment::ALIGN_LEFT) {
        return 0;
    } else if (align == Alignment::ALIGN_NORMAL) {
        if (dir == Directions::DIR_RIGHT_TO_LEFT)
            return getParagraphRight(line) - getLineMax(line);
        else
            return 0;
    } else if (align == Alignment::ALIGN_RIGHT) {
        return mWidth - getLineMax(line);
    } else if (align == Alignment::ALIGN_OPPOSITE) {
        if (dir == Directions::DIR_RIGHT_TO_LEFT)
            return 0;
        else
            return mWidth - getLineMax(line);
    } else { /* align == Alignment::ALIGN_CENTER */
        int left = getParagraphLeft(line);
        int right = getParagraphRight(line);
        int max = ((int) getLineMax(line)) & ~1;
        
        return left + ((right - left) - max) / 2;
    }
}

float Layout::getLineRight(int line) {
    int dir = getParagraphDirection(line);
    Alignment align = getParagraphAlignment(line);
    
    if (align == Alignment::ALIGN_LEFT) {
        return getParagraphLeft(line) + getLineMax(line);
    } else if (align == Alignment::ALIGN_NORMAL) {
        if (dir == Directions::DIR_RIGHT_TO_LEFT)
            return mWidth;
        else
            return getParagraphLeft(line) + getLineMax(line);
    } else if (align == Alignment::ALIGN_RIGHT) {
        return mWidth;
    } else if (align == Alignment::ALIGN_OPPOSITE) {
        if (dir == Directions::DIR_RIGHT_TO_LEFT)
            return getLineMax(line);
        else
            return mWidth;
    } else { /* align == Alignment::ALIGN_CENTER */
        int left = getParagraphLeft(line);
        int right = getParagraphRight(line);
        int max = ((int) getLineMax(line)) & ~1;
        
        return right - ((right - left) - max) / 2;
    }
}

float Layout::getLineMax(int line) {
    float margin = getParagraphLeadingMargin(line);
    float signedExtent = getLineExtent(line, false);
    return margin + signedExtent >= 0 ? signedExtent : -signedExtent;
}

float Layout::getLineWidth(int line) {
    float margin = getParagraphLeadingMargin(line);
    float signedExtent = getLineExtent(line, true);
    return margin + signedExtent >= 0 ? signedExtent : -signedExtent;
}

float Layout::getLineExtent(int line, bool full) {
    int start = getLineStart(line);
    int end = full ? getLineEnd(line) : getLineVisibleEnd(line);
    
    bool hasTabsOrEmoji = getLineContainsTab(line);
    shared_ptr<TabStops<TabStopSpan>> tabStops;
    if (hasTabsOrEmoji && isSpanned()) {
        // Just checking this line should be good enough, tabs should be
        // consistent across all lines in a paragraph.
        vector<shared_ptr<TabStopSpan>> tabs = getParagraphSpans<TabStopSpan>(mSpannedText, start, end, "TabStopSpan");
        if (tabs.size() > 0) {
            tabStops = make_shared<TabStops<TabStopSpan>>(TAB_INCREMENT, tabs); // XXX should reuse
        }
    }
    Directions directions = getLineDirections(line);
    // Returned directions can actually be NULL
//    if (directions == NULL) {
//        return 0f;
//    }
    int dir = getParagraphDirection(line);
    
    shared_ptr<TextLine<TabStopSpan>> tl = TextLine<TabStopSpan>::obtain();
    tl->set(mPaint, mText, start, end, dir, directions, hasTabsOrEmoji, tabStops);
    float width = tl->metrics(NULL);
    TextLine<TabStopSpan>::recycle(tl);
    return width;
}

template<class T>
float Layout::getLineExtent(int line, shared_ptr<TabStops<T>> tabStops, bool full) {
    int start = getLineStart(line);
    int end = full ? getLineEnd(line) : getLineVisibleEnd(line);
    bool hasTabsOrEmoji = getLineContainsTab(line);
    Directions directions = getLineDirections(line);
    int dir = getParagraphDirection(line);
    
    shared_ptr<TextLine<T>> tl = TextLine<T>::obtain();
    tl->set(mPaint, mText, start, end, dir, directions, hasTabsOrEmoji, tabStops);
    float width = tl->metrics(NULL);
    TextLine<T>::recycle(tl);
    return width;
}

int Layout::getLineForVertical(int vertical) {
    int high = getLineCount(), low = -1, guess;
    
    while (high - low > 1) {
        guess = (high + low) / 2;
        
        if (getLineTop(guess) > vertical)
            high = guess;
        else
            low = guess;
    }
    
    if (low < 0)
        return 0;
    else
        return low;
}

int Layout::getLineForOffset(int offset) {
    int high = getLineCount(), low = -1, guess;
    
    while (high - low > 1) {
        guess = (high + low) / 2;
        
        if (getLineStart(guess) > offset)
            high = guess;
        else
            low = guess;
    }
    
    if (low < 0)
        return 0;
    else
        return low;
}

int Layout::getOffsetForHorizontal(int line, float horiz) {
    int max = getLineEnd(line) - 1;
    int min = getLineStart(line);
    Directions dirs = getLineDirections(line);
    
    if (line == getLineCount() - 1)
        max++;
    
    int best = min;
    float bestdist = abs(getPrimaryHorizontal(best) - horiz);
    
    for (int i = 0; i < dirs.mDirections.size(); i += 2) {
        int here = min + dirs.mDirections[i];
        int there = here + (dirs.mDirections[i+1] & Directions::RUN_LENGTH_MASK);
        int swap = (dirs.mDirections[i+1] & Directions::RUN_RTL_FLAG) != 0 ? -1 : 1;
        
        if (there > max)
            there = max;
        int high = there - 1 + 1, low = here + 1 - 1, guess;
        
        while (high - low > 1) {
            guess = (high + low) / 2;
            int adguess = getOffsetAtStartOf(guess);
            
            if (getPrimaryHorizontal(adguess) * swap >= horiz * swap)
                high = guess;
            else
                low = guess;
        }
        
        if (low < here + 1)
            low = here + 1;
        
        if (low < there) {
            low = getOffsetAtStartOf(low);
            
            float dist = abs(getPrimaryHorizontal(low) - horiz);
            
            int aft = TextUtils::getOffsetAfter(*mText.get(), low);
            if (aft < there) {
                float other = abs(getPrimaryHorizontal(aft) - horiz);
                
                if (other < dist) {
                    dist = other;
                    low = aft;
                }
            }
            
            if (dist < bestdist) {
                bestdist = dist;
                best = low;
            }
        }
        
        float dist = abs(getPrimaryHorizontal(here) - horiz);
        
        if (dist < bestdist) {
            bestdist = dist;
            best = here;
        }
    }
    
    float dist = abs(getPrimaryHorizontal(max) - horiz);
    
    if (dist < bestdist) {
        bestdist = dist;
        best = max;
    }
    
    return best;
}

int Layout::getLineVisibleEnd(int line, int start, int end) {
    shared_ptr<CharSequence> text = mText;
    char ch;
    if (line == getLineCount() - 1) {
        return end;
    }
    
    for (; end > start; end--) {
        ch = text->charAt(end - 1);
        
        if (ch == '\n') {
            return end - 1;
        }
        
        if (ch != ' ' && ch != '\t') {
            break;
        }
        
    }
    
    return end;
}

int Layout::getOffsetToLeftRightOf(int caret, bool toLeft) {
    int line = getLineForOffset(caret);
    int lineStart = getLineStart(line);
    int lineEnd = getLineEnd(line);
    int lineDir = getParagraphDirection(line);
    
    bool lineChanged = false;
    bool advance = toLeft == (lineDir == Directions::DIR_RIGHT_TO_LEFT);
    // if walking off line, look at the line we're headed to
    if (advance) {
        if (caret == lineEnd) {
            if (line < getLineCount() - 1) {
                lineChanged = true;
                ++line;
            } else {
                return caret; // at very end, don't move
            }
        }
    } else {
        if (caret == lineStart) {
            if (line > 0) {
                lineChanged = true;
                --line;
            } else {
                return caret; // at very start, don't move
            }
        }
    }
    
    if (lineChanged) {
        lineStart = getLineStart(line);
        lineEnd = getLineEnd(line);
        int newDir = getParagraphDirection(line);
        if (newDir != lineDir) {
            // unusual case.  we want to walk onto the line, but it runs
            // in a different direction than this one, so we fake movement
            // in the opposite direction.
            toLeft = !toLeft;
            lineDir = newDir;
        }
    }
    
    Directions directions = getLineDirections(line);
    
    shared_ptr<TextLine<TabStopSpan>> tl = TextLine<TabStopSpan>::obtain();
    // XXX: we don't care about tabs
    tl->set(mPaint, mText, lineStart, lineEnd, lineDir, directions, false, NULL);
    caret = lineStart + tl->getOffsetToLeftRightOf(caret - lineStart, toLeft);
    tl = TextLine<TabStopSpan>::recycle(tl);
    return caret;
}

int Layout::getOffsetAtStartOf(int offset) {
    // XXX this probably should skip local reorderings and
    // zero-width characters, look at callers
    if (offset == 0)
        return 0;
    
    shared_ptr<CharSequence> text = mText;
    UChar c = text->charAt(offset);
    
    if (c >= 0xDC00 && c <= 0xDFFF) {
        UChar c1 = text->charAt(offset - 1);
        
        if (c1 >= 0xD800 && c1 <= 0xDBFF)
            offset -= 1;
    }
    
    if (mSpannedText != NULL) {
        vector<shared_ptr<Object>> spans = mSpannedText->getSpans(offset, offset, "ReplacementSpan");
        
        for (int i = 0; i < spans.size(); i++) {
            int start = mSpannedText->getSpanStart(spans[i]);
            int end = mSpannedText->getSpanEnd(spans[i]);
            
            if (start < offset && end > offset)
                offset = start;
        }
    }
    
    return offset;
}

bool Layout::shouldClampCursor(int line) {
    // Only clamp cursor position in left-aligned displays.
    switch (getParagraphAlignment(line)) {
        case ALIGN_LEFT:
            return true;
        case ALIGN_NORMAL:
            return getParagraphDirection(line) > 0;
        default:
            return false;
    }
    
}

// TODO: didn't implement because we don't need it right now
void Layout::getCursorPath(int point, Path *dest,
                   shared_ptr<CharSequence> editingBuffer) {
//    dest->reset();
//    
//    int line = getLineForOffset(point);
//    int top = getLineTop(line);
//    int bottom = getLineTop(line+1);
//    
//    bool clamped = shouldClampCursor(line);
//    float h1 = getPrimaryHorizontal(point, clamped) - 0.5f;
//    float h2 = isLevelBoundary(point) ? getSecondaryHorizontal(point, clamped) - 0.5f : h1;
//    
//    int caps = TextKeyListener::getMetaState(editingBuffer, TextKeyListener::META_SHIFT_ON) |
//    TextKeyListener::getMetaState(editingBuffer, TextKeyListener::META_SELECTING);
//    int fn = TextKeyListener::getMetaState(editingBuffer, TextKeyListener::META_ALT_ON);
//    int dist = 0;
//    
//    if (caps != 0 || fn != 0) {
//        dist = (bottom - top) >> 2;
//        
//        if (fn != 0)
//            top += dist;
//        if (caps != 0)
//            bottom -= dist;
//    }
//    
//    if (h1 < 0.5f)
//        h1 = 0.5f;
//    if (h2 < 0.5f)
//        h2 = 0.5f;
//    
//    if (fabs(h1 - h2) <= FLT_EPSILON) {
//        dest->moveTo(h1, top);
//        dest->lineTo(h1, bottom);
//    } else {
//        dest->moveTo(h1, top);
//        dest->lineTo(h1, (top + bottom) >> 1);
//        
//        dest->moveTo(h2, (top + bottom) >> 1);
//        dest->lineTo(h2, bottom);
//    }
//    
//    if (caps == 2) {
//        dest->moveTo(h2, bottom);
//        dest->lineTo(h2 - dist, bottom + dist);
//        dest->lineTo(h2, bottom);
//        dest->lineTo(h2 + dist, bottom + dist);
//    } else if (caps == 1) {
//        dest->moveTo(h2, bottom);
//        dest->lineTo(h2 - dist, bottom + dist);
//        
//        dest->moveTo(h2 - dist, bottom + dist - 0.5f);
//        dest->lineTo(h2 + dist, bottom + dist - 0.5f);
//        
//        dest->moveTo(h2 + dist, bottom + dist);
//        dest->lineTo(h2, bottom);
//    }
//    
//    if (fn == 2) {
//        dest->moveTo(h1, top);
//        dest->lineTo(h1 - dist, top - dist);
//        dest->lineTo(h1, top);
//        dest->lineTo(h1 + dist, top - dist);
//    } else if (fn == 1) {
//        dest->moveTo(h1, top);
//        dest->lineTo(h1 - dist, top - dist);
//        
//        dest->moveTo(h1 - dist, top - dist + 0.5f);
//        dest->lineTo(h1 + dist, top - dist + 0.5f);
//        
//        dest->moveTo(h1 + dist, top - dist);
//        dest->lineTo(h1, top);
//    }
}

void Layout::addSelection(int line, int start, int end,
                  int top, int bottom, Path *dest) {
    int linestart = getLineStart(line);
    int lineend = getLineEnd(line);
    Directions dirs = getLineDirections(line);
    
    if (lineend > linestart && mText->charAt(lineend - 1) == '\n')
        lineend--;
    
    for (int i = 0; i < dirs.mDirections.size(); i += 2) {
        int here = linestart + dirs.mDirections[i];
        int there = here + (dirs.mDirections[i+1] & Directions::RUN_LENGTH_MASK);
        
        if (there > lineend)
            there = lineend;
        
        if (start <= there && end >= here) {
            int st = max(start, here);
            int en = min(end, there);
            
            if (st != en) {
                float h1 = getHorizontal(st, false, line, false /* not clamped */);
                float h2 = getHorizontal(en, true, line, false /* not clamped */);
                
                float left = min(h1, h2);
                float right = max(h1, h2);
                
                dest->addRect(left, top, right, bottom, Path::Direction::kCW_Direction);
            }
        }
    }
}

void Layout::getSelectionPath(int start, int end, Path *dest) {
    dest->reset();
    
    if (start == end)
        return;
    
    if (end < start) {
        int temp = end;
        end = start;
        start = temp;
    }
    
    int startline = getLineForOffset(start);
    int endline = getLineForOffset(end);
    
    int top = getLineTop(startline);
    int bottom = getLineBottom(endline);
    
    if (startline == endline) {
        addSelection(startline, start, end, top, bottom, dest);
    } else {
        const float width = mWidth;
        
        addSelection(startline, start, getLineEnd(startline),
                     top, getLineBottom(startline), dest);
        
        if (getParagraphDirection(startline) == Directions::DIR_RIGHT_TO_LEFT)
            dest->addRect(getLineLeft(startline), top,
                          0, getLineBottom(startline), Path::Direction::kCW_Direction);
        else
            dest->addRect(getLineRight(startline), top,
                          width, getLineBottom(startline), Path::Direction::kCW_Direction);
        
        for (int i = startline + 1; i < endline; i++) {
            top = getLineTop(i);
            bottom = getLineBottom(i);
            dest->addRect(0, top, width, bottom, Path::Direction::kCW_Direction);
        }
        
        top = getLineTop(endline);
        bottom = getLineBottom(endline);
        
        addSelection(endline, getLineStart(endline), end,
                     top, bottom, dest);
        
        if (getParagraphDirection(endline) == Directions::DIR_RIGHT_TO_LEFT)
            dest->addRect(width, top, getLineRight(endline), bottom, Path::Direction::kCW_Direction);
        else
            dest->addRect(0, top, getLineLeft(endline), bottom, Path::Direction::kCW_Direction);
    }
}

Layout::Alignment Layout::getParagraphAlignment(int line) {
    Alignment align = mAlignment;
    
    if (mSpannedText) {
        shared_ptr<Spanned> sp = mSpannedText;
        vector<shared_ptr<AlignmentSpan>> spans = getParagraphSpans<AlignmentSpan>(sp, getLineStart(line),
                                                  getLineEnd(line), "AlignmentSpan");
        
        int spanLength = spans.size();
        if (spanLength > 0) {
            align = spans[spanLength-1]->getAlignment();
        }
    }
    
    return align;
}

int Layout::getParagraphLeft(int line) {
    int left = 0;
    int dir = getParagraphDirection(line);
    if (dir == Directions::DIR_RIGHT_TO_LEFT || !mSpannedText) {
        return left; // leading margin has no impact, or no styles
    }
    return getParagraphLeadingMargin(line);
}

int Layout::getParagraphRight(int line) {
    int right = mWidth;
    int dir = getParagraphDirection(line);
    if (dir == Directions::DIR_LEFT_TO_RIGHT || !mSpannedText) {
        return right; // leading margin has no impact, or no styles
    }
    return right - getParagraphLeadingMargin(line);
}

int Layout::getParagraphLeadingMargin(int line) {
    if (!mSpannedText) {
        return 0;
    }
    shared_ptr<Spanned> &spanned = mSpannedText;
    
    int lineStart = getLineStart(line);
    int lineEnd = getLineEnd(line);
    int spanEnd = spanned->nextSpanTransition(lineStart, lineEnd, "LeadingMarginSpan");
    vector<shared_ptr<LeadingMarginSpan>> spans = getParagraphSpans<LeadingMarginSpan>(spanned, lineStart, spanEnd, "LeadingMarginSpan");
    
    if (spans.size() == 0) {
        return 0; // no leading margin span;
    }
    
    int margin = 0;
    
    bool isFirstParaLine = lineStart == 0 ||
    spanned->charAt(lineStart - 1) == '\n';
    
    for (int i = 0; i < spans.size(); i++) {
        shared_ptr<LeadingMarginSpan> span = spans[i];
        bool useFirstLineMargin = isFirstParaLine;
        shared_ptr<LeadingMarginSpan2> span2 = dynamic_pointer_cast<LeadingMarginSpan2>(span);
        if (span2 != NULL) {
            int spStart = spanned->getSpanStart(span);
            int spanLine = getLineForOffset(spStart);
            int count = span2->getLeadingMarginLineCount();
            useFirstLineMargin = line < spanLine + count;
        }
        margin += span->getLeadingMargin(useFirstLineMargin);
    }
    
    return margin;
}

float Layout::measurePara(shared_ptr<TextPaint> paint, shared_ptr<CharSequence> text, int start, int end) {
    
    float ret = 0;
    
    shared_ptr<MeasuredText> mt = MeasuredText::obtain();
    shared_ptr<TextLine<TabStopSpan>> tl = TextLine<TabStopSpan>::obtain();
    try {
        mt->setPara(text, start, end, TextDirectionHeuristics::LTR);
        Directions directions;
        int dir;
        if (mt->mEasy) {
            directions = Directions::DIRS_ALL_LEFT_TO_RIGHT;
            dir = Directions::DIR_LEFT_TO_RIGHT;
        } else {
            directions = AndroidBidi::directions(mt->mDir, mt->mLevels,
                                                0, mt->mChars, 0, mt->mLen);
            dir = mt->mDir;
        }
        UnicodeString &chars = mt->mChars;
        int len = mt->mLen;
        bool hasTabs = false;
        shared_ptr<TabStops<TabStopSpan>> tabStops;
        for (int i = 0; i < len; ++i) {
            if (chars[i] == '\t') {
                hasTabs = true;
                shared_ptr<Spanned> spanned = dynamic_pointer_cast<Spanned>(text);
                if (spanned != NULL) {
                    int spanEnd = spanned->nextSpanTransition(start, end, "TabStopSpan");
                    vector<shared_ptr<TabStopSpan>> spans = getParagraphSpans<TabStopSpan>(spanned, start, spanEnd, "TabStopSpan");
                    if (spans.size() > 0) {
                        tabStops = make_shared<TabStops<TabStopSpan>>(TAB_INCREMENT, spans);
                    }
                }
                break;
            }
        }
        
        tl->set(paint, text, start, end, dir, directions, hasTabs, tabStops);
        ret = tl->metrics(NULL);
        
    } catch (exception exc) {
    }
    
    TextLine<TabStopSpan>::recycle(tl);
    MeasuredText::recycle(mt);
    
    return ret;
}

float Layout::nextTab(shared_ptr<CharSequence> text, int start, int end,
              float h, vector<shared_ptr<Object>> &tabs) {
    
    float nh = FLT_MAX;
    bool alltabs = false;
    
    shared_ptr<Spanned> spanned = dynamic_pointer_cast<Spanned>(text);
    if (spanned != NULL) {
        if (tabs.empty()) {
            // TODO: this needs to be fixed but I don't think we are currently using this method
            tabs = getParagraphSpans<Object>(spanned, start, end, "TabStopSpan");
            alltabs = true;
        }
        
        for (int i = 0; i < tabs.size(); i++) {
            if (!alltabs) {
                if (dynamic_pointer_cast<TabStopSpan>(tabs[i]) == NULL)
                    continue;
            }
            
            int where = (dynamic_pointer_cast<TabStopSpan>(tabs[i]))->getTabStop();
            
            if (where < nh && where > h)
                nh = where;
        }
        
        if (nh != FLT_MAX)
            return nh;
    }
    
    return ((int) ((h + TAB_INCREMENT) / TAB_INCREMENT)) * TAB_INCREMENT;
}

UChar Layout::getEllipsisChar(shared_ptr<TextUtils::TruncateAt> method) {
    return (method == TextUtils::TruncateAt::END_SMALL) ?
    ELLIPSIS_TWO_DOTS[0] :
    ELLIPSIS_NORMAL[0];
}

void Layout::ellipsize(int start, int end, int line,
               UnicodeString &dest, int destoff, shared_ptr<TextUtils::TruncateAt> method) {
    int ellipsisCount = getEllipsisCount(line);
    
    if (ellipsisCount == 0) {
        return;
    }
    
    int ellipsisStart = getEllipsisStart(line);
    int linestart = getLineStart(line);
    
    for (int i = ellipsisStart; i < ellipsisStart + ellipsisCount; i++) {
        UChar c;
        
        if (i == ellipsisStart) {
            c = getEllipsisChar(method); // ellipsis
        } else {
            c = 0xFEFF; // 0-width space
        }
        
        int a = i + linestart;
        
        if (a >= start && a < end) {
            dest.setCharAt((destoff + a - start), c);
        }
    }
}

//bool Layout::Directions::operator== (Directions &rhs) {
//    return mDirections == rhs.mDirections;
//}

Layout::Ellipsizer::Ellipsizer(shared_ptr<CharSequence> s) {
    mText = s;
}

UChar Layout::Ellipsizer::charAt(int off) {
    UnicodeString *buf = TextUtils::obtain(1);
    getChars(off, off + 1, *buf, 0);
    UChar ret = buf->charAt(0);
    
    TextUtils::recycle(buf);
    return ret;
}

void Layout::Ellipsizer::getChars(int start, int end, UnicodeString &dest, int destoff) {
    int line1 = mLayout->getLineForOffset(start);
    int line2 = mLayout->getLineForOffset(end);
    
    TextUtils::getChars(mText, start, end, dest, destoff);
    
    for (int i = line1; i <= line2; i++) {
        mLayout->ellipsize(start, end, i, dest, destoff, mMethod);
    }
}

int Layout::Ellipsizer::length() {
    return mText->length();
}

shared_ptr<CharSequence> Layout::Ellipsizer::subSequence(int start, int end) {
    UnicodeString s;
    getChars(start, end, s, 0);
    return make_shared<String>(s);
}

shared_ptr<String> Layout::Ellipsizer::toString() {
    UnicodeString s;
    getChars(0, length(), s, 0);
    return make_shared<String>(s);
}

Layout::SpannedEllipsizer::SpannedEllipsizer(shared_ptr<CharSequence> display) : Ellipsizer(display) {
    mSpanned = dynamic_pointer_cast<Spanned>(display);
}

vector<shared_ptr<Object>> Layout::SpannedEllipsizer::getSpans(int start, int end, string type) {
    return mSpanned->getSpans(start, end, type);
}

int Layout::SpannedEllipsizer::getSpanStart(shared_ptr<Object> tag) {
    return mSpanned->getSpanStart(tag);
}

int Layout::SpannedEllipsizer::getSpanEnd(shared_ptr<Object> tag) {
    return mSpanned->getSpanEnd(tag);
}

int Layout::SpannedEllipsizer::getSpanFlags(shared_ptr<Object> tag) {
    return mSpanned->getSpanFlags(tag);
}

int Layout::SpannedEllipsizer::nextSpanTransition(int start, int limit, string type) {
    return mSpanned->nextSpanTransition(start, limit, type);
}

shared_ptr<CharSequence> Layout::SpannedEllipsizer::subSequence(int start, int end) {
    UnicodeString s;
    getChars(start, end, s, 0);
    
    shared_ptr<SpannableString> ss = make_shared<SpannableString>(make_shared<String>(s));
    TextUtils::copySpansFrom(mSpanned, start, end, "Object", ss, 0);
    return ss;
}

ANDROID_END