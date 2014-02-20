//
//  TextLine.h
//  Androidpp
//
//  Created by Saul Howard on 1/22/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef __Androidpp__TextLine__
#define __Androidpp__TextLine__

#include "AndroidMacros.h"

#include "Android/graphics/Bitmap.h"
#include "Android/graphics/Canvas.h"
#include "Android/graphics/Paint.h"
#include "Android/text/Character.h"
#include "Android/text/CharSequence.h"
#include "Android/text/Directions.h"
#include "Android/text/Spanned.h"
#include "Android/text/SpanSet.h"
#include "Android/text/String.h"
#include "Android/text/TabStops.h"
#include "Android/text/TextPaint.h"
#include "Android/text/style/CharacterStyle.h"
#include "Android/text/style/MetricAffectingSpan.h"
#include "Android/text/style/ReplacementSpan.h"
#include "Android/utils/ArrayUtils.h"

#include <unicode/unistr.h>

#include <memory>
#include <string>
#include <vector>

using namespace icu;
using namespace std;

ANDROID_BEGIN

/**
 * Represents a line of styled text, for measuring in visual order and
 * for rendering.
 *
 * <p>Get a new instance using obtain(), and when finished with it, return it
 * to the pool using recycle().
 *
 * <p>Call set to prepare the instance for use, then either draw, measure,
 * metrics, or caretToLeftRightOf.
 *
 * @hide
 */
template<class T>
class TextLine {
    
    friend class Layout;
    friend class BoringLayout;
    
private:
    
    static const bool DEBUG = false;
    
    shared_ptr<TextPaint> mPaint;
    shared_ptr<CharSequence> mText;
    int mStart;
    int mLen;
    int mDir;
    Directions mDirections;
    bool mHasTabs;
    shared_ptr<TabStops<T>> mTabs;
    UnicodeString mChars;
    bool mCharsValid;
    shared_ptr<Spanned> mSpanned;
    shared_ptr<TextPaint> mWorkPaint = make_shared<TextPaint>();
    SpanSet<MetricAffectingSpan> mMetricAffectingSpanSpanSet = SpanSet<MetricAffectingSpan>("MetricAffectingSpan");
    SpanSet<CharacterStyle> mCharacterStyleSpanSet = SpanSet<CharacterStyle>("CharacterStyle");
    SpanSet<ReplacementSpan> mReplacementSpanSpanSet = SpanSet<ReplacementSpan>("ReplacementSpan");
    
    static vector<shared_ptr<TextLine<T>>> sCached;
    
    /**
     * Returns a new TextLine from the shared pool.
     *
     * @return an uninitialized TextLine
     */
    static shared_ptr<TextLine<T>> obtain() {
        shared_ptr<TextLine<T>> tl;
        //    synchronized (sCached) {
        for (int i = sCached.size(); --i >= 0;) {
            if (sCached[i] != NULL) {
                tl = sCached[i];
                sCached[i] = NULL;
                return tl;
            }
        }
        //    }
        tl = make_shared<TextLine<T>>();
        if (DEBUG) {
            //        CCLOG("TLINE", "new: " + tl);
        }
        return tl;
    }
    
    /**
     * Puts a TextLine back into the shared pool. Do not use this TextLine once
     * it has been returned.
     * @param tl the textLine
     * @return NULL, as a convenience from clearing references to the provided
     * TextLine
     */
    static shared_ptr<TextLine> recycle(shared_ptr<TextLine> tl) {
        tl->mText = NULL;
        tl->mPaint = NULL;
        
        tl->mMetricAffectingSpanSpanSet.recycle();
        tl->mCharacterStyleSpanSet.recycle();
        tl->mReplacementSpanSpanSet.recycle();
        
        //    synchronized(sCached) {
        for (int i = 0; i < sCached.size(); ++i) {
            if (sCached[i] == NULL) {
                sCached[i] = tl;
                break;
            }
        }
        //    }
        return NULL;
    }
    
    /**
     * Initializes a TextLine and prepares it for use.
     *
     * @param paint the base paint for the line
     * @param text the text, can be Styled
     * @param start the start of the line relative to the text
     * @param limit the limit of the line relative to the text
     * @param dir the paragraph direction of this line
     * @param directions the directions information of this line
     * @param hasTabs true if the line might contain tabs or emoji
     * @param tabStops the tabStops. Can be NULL.
     */
    void set(shared_ptr<TextPaint> paint, shared_ptr<CharSequence> text, int start, int limit, int dir,
             Directions directions, bool hasTabs, shared_ptr<TabStops<T>> tabStops) {
        mPaint = paint;
        mText = text;
        mStart = start;
        mLen = limit - start;
        mDir = dir;
        mDirections = directions;
        mHasTabs = hasTabs;
        mSpanned = NULL;
        
        bool hasReplacement = false;
        mSpanned = dynamic_pointer_cast<Spanned>(text);
        if (mSpanned != NULL) {
            mReplacementSpanSpanSet.init(mSpanned, start, limit);
            hasReplacement = mReplacementSpanSpanSet.numberOfSpans > 0;
        }
        
        mCharsValid = hasReplacement || hasTabs || directions != Directions::DIRS_ALL_LEFT_TO_RIGHT;
        
        if (mCharsValid) {
            TextUtils::getChars(text, start, limit, mChars, 0);
            if (hasReplacement) {
                // Handle these all at once so we don't have to do it as we go.
                // Replace the first character of each replacement run with the
                // object-replacement character and the remainder with zero width
                // non-break space aka BOM.  Cursor movement code skips these
                // zero-width characters.
                UnicodeString &chars = mChars;
                for (int i = start, inext; i < limit; i = inext) {
                    inext = mReplacementSpanSpanSet.getNextTransition(i, limit);
                    if (mReplacementSpanSpanSet.hasSpansIntersecting(i, inext)) {
                        // transition into a span
                        chars.setCharAt(i - start, 0xfffc);
                        for (int j = i - start + 1, e = inext - start; j < e; ++j) {
                            chars.setCharAt(j, 0xfeff); // used as ZWNBS, marks positions to skip
                        }
                    }
                }
            }
        }
        mTabs = tabStops;
    }
    
    /**
     * Renders the TextLine.
     *
     * @param c the canvas to render on
     * @param x the leading margin position
     * @param top the top of the line
     * @param y the baseline
     * @param bottom the bottom of the line
     */
    void draw(shared_ptr<Canvas> c, float x, int top, int y, int bottom) {
        if (!mHasTabs) {
            if (mDirections == Directions::DIRS_ALL_LEFT_TO_RIGHT) {
                drawRun(c, 0, mLen, false, x, top, y, bottom, false);
                return;
            }
            if (mDirections == Directions::DIRS_ALL_RIGHT_TO_LEFT) {
                drawRun(c, 0, mLen, true, x, top, y, bottom, false);
                return;
            }
        }
        
        float h = 0;
        vector<int> runs = mDirections.mDirections;
        RectF emojiRect;
        
        int lastRunIndex = runs.size() - 2;
        for (int i = 0; i < runs.size(); i += 2) {
            int runStart = runs[i];
            int runLimit = runStart + (runs[i+1] & Directions::RUN_LENGTH_MASK);
            if (runLimit > mLen) {
                runLimit = mLen;
            }
            bool runIsRtl = (runs[i+1] & Directions::RUN_RTL_FLAG) != 0;
            
            int segstart = runStart;
            for (int j = mHasTabs ? runStart : runLimit; j <= runLimit; j++) {
                int codept = 0;
                Bitmap *bm = NULL;
                
                if (mHasTabs && j < runLimit) {
                    codept = mChars[j];
                    if (codept >= 0xd800 && codept < 0xdc00 && j + 1 < runLimit) {
                        codept = Character::codePointAt(mChars, j);
//                        if (codept >= MIN_EMOJI && codept <= MAX_EMOJI) {
//                            bm = EMOJI_FACTORY.getBitmapFromAndroidPua(codept);
//                        } else
                        if (codept > 0xffff) {
                            ++j;
                            continue;
                        }
                    }
                }
                
                if (j == runLimit || codept == '\t' || bm != NULL) {
                    h += drawRun(c, segstart, j, runIsRtl, x+h, top, y, bottom,
                                 i != lastRunIndex || j != mLen);
                    
                    if (codept == '\t') {
                        h = mDir * nextTab(h * mDir);
                    } else if (bm != NULL) {
//                        float bmAscent = ascent(j);
//                        float bitmapHeight = bm.getHeight();
//                        float scale = -bmAscent / bitmapHeight;
//                        float width = bm.getWidth() * scale;
//                        
//                        if (emojiRect == NULL) {
//                            emojiRect = new RectF();
//                        }
//                        emojiRect.set(x + h, y + bmAscent,
//                                      x + h + width, y);
//                        c->drawBitmap(bm, NULL, emojiRect, mPaint);
//                        h += width;
//                        j++;
                    }
                    segstart = j + 1;
                }
            }
        }
    }
    
    /**
     * Returns metrics information for the entire line.
     *
     * @param fmi receives font metrics information, can be NULL
     * @return the signed width of the line
     */
    float metrics(shared_ptr<Paint::FontMetricsInt> fmi) {
        return measure(mLen, false, fmi);
    }
    
    /**
     * Returns information about a position on the line.
     *
     * @param offset the line-relative character offset, between 0 and the
     * line length, inclusive
     * @param trailing true to measure the trailing edge of the character
     * before offset, false to measure the leading edge of the character
     * at offset.
     * @param fmi receives metrics information about the requested
     * character, can be NULL.
     * @return the signed offset from the leading margin to the requested
     * character edge.
     */
    float measure(int offset, bool trailing, shared_ptr<Paint::FontMetricsInt> fmi) {
        int target = trailing ? offset - 1 : offset;
        if (target < 0) {
            return 0;
        }
        
        float h = 0;
        
        if (!mHasTabs) {
            if (mDirections == Directions::DIRS_ALL_LEFT_TO_RIGHT) {
                return measureRun(0, offset, mLen, false, fmi);
            }
            if (mDirections == Directions::DIRS_ALL_RIGHT_TO_LEFT) {
                return measureRun(0, offset, mLen, true, fmi);
            }
        }
        
        UnicodeString &chars = mChars;
        vector<int> runs = mDirections.mDirections;
        for (int i = 0; i < runs.size(); i += 2) {
            int runStart = runs[i];
            int runLimit = runStart + (runs[i+1] & Directions::RUN_LENGTH_MASK);
            if (runLimit > mLen) {
                runLimit = mLen;
            }
            bool runIsRtl = (runs[i+1] & Directions::RUN_RTL_FLAG) != 0;
            
            int segstart = runStart;
            for (int j = mHasTabs ? runStart : runLimit; j <= runLimit; j++) {
                int32_t codept = 0;
                Bitmap *bm = NULL;
                
                if (mHasTabs && j < runLimit) {
                    codept = chars[j];
                    if (codept >= 0xd800 && codept < 0xdc00 && j + 1 < runLimit) {
                        codept = Character::codePointAt(chars, j);
//                        if (codept >= MIN_EMOJI && codept <= MAX_EMOJI) {
//                            bm = EMOJI_FACTORY.getBitmapFromAndroidPua(codept);
//                        } else
                        if (codept > 0xffff) {
                            ++j;
                            continue;
                        }
                    }
                }
                
                if (j == runLimit || codept == '\t' || bm != NULL) {
                    bool inSegment = target >= segstart && target < j;
                    
                    bool advance = (mDir == Directions::DIR_RIGHT_TO_LEFT) == runIsRtl;
                    if (inSegment && advance) {
                        return h += measureRun(segstart, offset, j, runIsRtl, fmi);
                    }
                    
                    float w = measureRun(segstart, j, j, runIsRtl, fmi);
                    h += advance ? w : -w;
                    
                    if (inSegment) {
                        return h += measureRun(segstart, offset, j, runIsRtl, NULL);
                    }
                    
                    if (codept == '\t') {
                        if (offset == j) {
                            return h;
                        }
                        h = mDir * nextTab(h * mDir);
                        if (target == j) {
                            return h;
                        }
                    }
                    
                    if (bm != NULL) {
                        float bmAscent = ascent(j);
                        float wid = bm->width() * -bmAscent / bm->height();
                        h += mDir * wid;
                        j++;
                    }
                    
                    segstart = j + 1;
                }
            }
        }
        
        return h;
    }
    
    /**
     * Draws a unidirectional (but possibly multi-styled) run of text.
     *
     *
     * @param c the canvas to draw on
     * @param start the line-relative start
     * @param limit the line-relative limit
     * @param runIsRtl true if the run is right-to-left
     * @param x the position of the run that is closest to the leading margin
     * @param top the top of the line
     * @param y the baseline
     * @param bottom the bottom of the line
     * @param needWidth true if the width value is required.
     * @return the signed width of the run, based on the paragraph direction.
     * Only valid if needWidth is true.
     */
    float drawRun(shared_ptr<Canvas> c, int start,
                          int limit, bool runIsRtl, float x, int top, int y, int bottom,
                  bool needWidth) {
        
        if ((mDir == Directions::DIR_LEFT_TO_RIGHT) == runIsRtl) {
            float w = -measureRun(start, limit, limit, runIsRtl, NULL);
            handleRun(start, limit, limit, runIsRtl, c, x + w, top,
                      y, bottom, NULL, false);
            return w;
        }
        
        return handleRun(start, limit, limit, runIsRtl, c, x, top,
                         y, bottom, NULL, needWidth);
    }
    
    /**
     * Measures a unidirectional (but possibly multi-styled) run of text.
     *
     *
     * @param start the line-relative start of the run
     * @param offset the offset to measure to, between start and limit inclusive
     * @param limit the line-relative limit of the run
     * @param runIsRtl true if the run is right-to-left
     * @param fmi receives metrics information about the requested
     * run, can be NULL.
     * @return the signed width from the start of the run to the leading edge
     * of the character at offset, based on the run (not paragraph) direction
     */
    float measureRun(int start, int offset, int limit, bool runIsRtl,
                     shared_ptr<Paint::FontMetricsInt> fmi) {
        return handleRun(start, offset, limit, runIsRtl, NULL, 0, 0, 0, 0, fmi, true);
    }
    
    /**
     * Walk the cursor through this line, skipping conjuncts and
     * zero-width characters.
     *
     * <p>This function cannot properly walk the cursor off the ends of the line
     * since it does not know about any shaping on the previous/following line
     * that might affect the cursor position. Callers must either avoid these
     * situations or handle the result specially.
     *
     * @param cursor the starting position of the cursor, between 0 and the
     * length of the line, inclusive
     * @param toLeft true if the caret is moving to the left.
     * @return the new offset.  If it is less than 0 or greater than the length
     * of the line, the previous/following line should be examined to get the
     * actual offset.
     */
    int getOffsetToLeftRightOf(int cursor, bool toLeft) {
        // 1) The caret marks the leading edge of a character. The character
        // logically before it might be on a different level, and the active caret
        // position is on the character at the lower level. If that character
        // was the previous character, the caret is on its trailing edge.
        // 2) Take this character/edge and move it in the indicated direction.
        // This gives you a new character and a new edge.
        // 3) This position is between two visually adjacent characters.  One of
        // these might be at a lower level.  The active position is on the
        // character at the lower level.
        // 4) If the active position is on the trailing edge of the character,
        // the new caret position is the following logical character, else it
        // is the character.
        
        int lineStart = 0;
        int lineEnd = mLen;
        bool paraIsRtl = mDir == -1;
        vector<int> runs = mDirections.mDirections;
        
        int runIndex, runLevel = 0, runStart = lineStart, runLimit = lineEnd, newCaret = -1;
        bool trailing = false;
        
        if (cursor == lineStart) {
            runIndex = -2;
        } else if (cursor == lineEnd) {
            runIndex = runs.size();
        } else {
            // First, get information about the run containing the character with
            // the active caret.
            for (runIndex = 0; runIndex < runs.size(); runIndex += 2) {
                runStart = lineStart + runs[runIndex];
                if (cursor >= runStart) {
                    runLimit = runStart + (runs[runIndex+1] & Directions::RUN_LENGTH_MASK);
                    if (runLimit > lineEnd) {
                        runLimit = lineEnd;
                    }
                    if (cursor < runLimit) {
                        runLevel = (runs[runIndex+1] >> Directions::RUN_LEVEL_SHIFT) &
                        Directions::RUN_LEVEL_MASK;
                        if (cursor == runStart) {
                            // The caret is on a run boundary, see if we should
                            // use the position on the trailing edge of the previous
                            // logical character instead.
                            int prevRunIndex, prevRunLevel, prevRunStart, prevRunLimit;
                            int pos = cursor - 1;
                            for (prevRunIndex = 0; prevRunIndex < runs.size(); prevRunIndex += 2) {
                                prevRunStart = lineStart + runs[prevRunIndex];
                                if (pos >= prevRunStart) {
                                    prevRunLimit = prevRunStart +
                                    (runs[prevRunIndex+1] & Directions::RUN_LENGTH_MASK);
                                    if (prevRunLimit > lineEnd) {
                                        prevRunLimit = lineEnd;
                                    }
                                    if (pos < prevRunLimit) {
                                        prevRunLevel = (runs[prevRunIndex+1] >> Directions::RUN_LEVEL_SHIFT)
                                        & Directions::RUN_LEVEL_MASK;
                                        if (prevRunLevel < runLevel) {
                                            // Start from logically previous character.
                                            runIndex = prevRunIndex;
                                            runLevel = prevRunLevel;
                                            runStart = prevRunStart;
                                            runLimit = prevRunLimit;
                                            trailing = true;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                        break;
                    }
                }
            }
            
            // caret might be == lineEnd.  This is generally a space or paragraph
            // separator and has an associated run, but might be the end of
            // text, in which case it doesn't.  If that happens, we ran off the
            // end of the run list, and runIndex == runs.size().  In this case,
            // we are at a run boundary so we skip the below test.
            if (runIndex != runs.size()) {
                bool runIsRtl = (runLevel & 0x1) != 0;
                bool advance = toLeft == runIsRtl;
                if (cursor != (advance ? runLimit : runStart) || advance != trailing) {
                    // Moving within or into the run, so we can move logically.
                    newCaret = getOffsetBeforeAfter(runIndex, runStart, runLimit,
                                                    runIsRtl, cursor, advance);
                    // If the new position is internal to the run, we're at the strong
                    // position already so we're finished.
                    if (newCaret != (advance ? runLimit : runStart)) {
                        return newCaret;
                    }
                }
            }
        }
        
        // If newCaret is -1, we're starting at a run boundary and crossing
        // into another run. Otherwise we've arrived at a run boundary, and
        // need to figure out which character to attach to.  Note we might
        // need to run this twice, if we cross a run boundary and end up at
        // another run boundary.
        while (true) {
            bool advance = toLeft == paraIsRtl;
            int otherRunIndex = runIndex + (advance ? 2 : -2);
            if (otherRunIndex >= 0 && otherRunIndex < runs.size()) {
                int otherRunStart = lineStart + runs[otherRunIndex];
                int otherRunLimit = otherRunStart +
                (runs[otherRunIndex+1] & Directions::RUN_LENGTH_MASK);
                if (otherRunLimit > lineEnd) {
                    otherRunLimit = lineEnd;
                }
                int otherRunLevel = (runs[otherRunIndex+1] >> Directions::RUN_LEVEL_SHIFT) &
                Directions::RUN_LEVEL_MASK;
                bool otherRunIsRtl = (otherRunLevel & 1) != 0;
                
                advance = toLeft == otherRunIsRtl;
                if (newCaret == -1) {
                    newCaret = getOffsetBeforeAfter(otherRunIndex, otherRunStart,
                                                    otherRunLimit, otherRunIsRtl,
                                                    advance ? otherRunStart : otherRunLimit, advance);
                    if (newCaret == (advance ? otherRunLimit : otherRunStart)) {
                        // Crossed and ended up at a new boundary,
                        // repeat a second and const time.
                        runIndex = otherRunIndex;
                        runLevel = otherRunLevel;
                        continue;
                    }
                    break;
                }
                
                // The new caret is at a boundary.
                if (otherRunLevel < runLevel) {
                    // The strong character is in the other run.
                    newCaret = advance ? otherRunStart : otherRunLimit;
                }
                break;
            }
            
            if (newCaret == -1) {
                // We're walking off the end of the line.  The paragraph
                // level is always equal to or lower than any internal level, so
                // the boundaries get the strong caret.
                newCaret = advance ? mLen + 1 : -1;
                break;
            }
            
            // Else we've arrived at the end of the line.  That's a strong position.
            // We might have arrived here by crossing over a run with no internal
            // breaks and dropping out of the above loop before advancing one const
            // time, so reset the caret.
            // Note, we use '<=' below to handle a situation where the only run
            // on the line is a counter-directional run.  If we're not advancing,
            // we can end up at the 'lineEnd' position but the caret we want is at
            // the lineStart.
            if (newCaret <= lineEnd) {
                newCaret = advance ? lineEnd : lineStart;
            }
            break;
        }
        
        return newCaret;
    }
    
    /**
     * Returns the next valid offset within this directional run, skipping
     * conjuncts and zero-width characters.  This should not be called to walk
     * off the end of the line, since the returned values might not be valid
     * on neighboring lines.  If the returned offset is less than zero or
     * greater than the line length, the offset should be recomputed on the
     * preceding or following line, respectively.
     *
     * @param runIndex the run index
     * @param runStart the start of the run
     * @param runLimit the limit of the run
     * @param runIsRtl true if the run is right-to-left
     * @param offset the offset
     * @param after true if the new offset should logically follow the provided
     * offset
     * @return the new offset
     */
    int getOffsetBeforeAfter(int runIndex, int runStart, int runLimit,
                             bool runIsRtl, int offset, bool after) {
        
        if (runIndex < 0 || offset == (after ? mLen : 0)) {
            // Walking off end of line.  Since we don't know
            // what cursor positions are available on other lines, we can't
            // return accurate values.  These are a guess.
            if (after) {
                return TextUtils::getOffsetAfter(*mText.get(), offset + mStart) - mStart;
            }
            return TextUtils::getOffsetBefore(*mText.get(), offset + mStart) - mStart;
        }
        
        shared_ptr<TextPaint> wp = mWorkPaint;
        wp = mPaint;
        
        int spanStart = runStart;
        int spanLimit;
        if (mSpanned == NULL) {
            spanLimit = runLimit;
        } else {
            int target = after ? offset + 1 : offset;
            int limit = mStart + runLimit;
            while (true) {
                spanLimit = mSpanned->nextSpanTransition(mStart + spanStart, limit,
                                                        "MetricAffectingSpan") - mStart;
                if (spanLimit >= target) {
                    break;
                }
                spanStart = spanLimit;
            }
            
            vector<shared_ptr<Object>> spans = mSpanned->getSpans(mStart + spanStart,
                                                            mStart + spanLimit, "MetricAffectingSpan");
            spans = TextUtils::removeEmptySpans(spans, mSpanned, "MetricAffectingSpan");
            
            if (spans.size() > 0) {
                shared_ptr<ReplacementSpan> replacement;
                shared_ptr<MetricAffectingSpan> span;
                for (int j = 0; j < spans.size(); j++) {
                    span = dynamic_pointer_cast<MetricAffectingSpan>(spans[j]);
                    replacement = dynamic_pointer_cast<ReplacementSpan>(span);
                    if (replacement == NULL) {
                        span->updateMeasureState(wp);
                    }
                }
                
                if (replacement != NULL) {
                    // If we have a replacement span, we're moving either to
                    // the start or end of this span.
                    return after ? spanLimit : spanStart;
                }
            }
        }
        
        int flags = runIsRtl ? Paint::DIRECTION_RTL : Paint::DIRECTION_LTR;
        int cursorOpt = after ? Paint::CURSOR_AFTER : Paint::CURSOR_BEFORE;
        if (mCharsValid) {
            return wp->getTextRunCursor(mChars, spanStart, spanLimit - spanStart,
                                       flags, offset, cursorOpt);
        } else {
            return wp->getTextRunCursor(mText->toString()->toCharArray(), mStart + spanStart,
                                       mStart + spanLimit, flags, mStart + offset, cursorOpt) - mStart;
        }
    }
    
    /**
     * @param wp
     */
    static void expandMetricsFromPaint(shared_ptr<Paint::FontMetricsInt> fmi, shared_ptr<TextPaint> wp) {
        const int previousTop     = fmi->top;
        const int previousAscent  = fmi->ascent;
        const int previousDescent = fmi->descent;
        const int previousBottom  = fmi->bottom;
        const int previousLeading = fmi->leading;
        
        wp->getFontMetricsInt(fmi);
        
        updateMetrics(fmi, previousTop, previousAscent, previousDescent, previousBottom,
                      previousLeading);
    }
    
    static void updateMetrics(shared_ptr<Paint::FontMetricsInt> fmi, int previousTop, int previousAscent,
                              int previousDescent, int previousBottom, int previousLeading) {
        fmi->top     = min(fmi->top,     previousTop);
        fmi->ascent  = min(fmi->ascent,  previousAscent);
        fmi->descent = max(fmi->descent, previousDescent);
        fmi->bottom  = max(fmi->bottom,  previousBottom);
        fmi->leading = max(fmi->leading, previousLeading);
    }
    
    /**
     * Utility function for measuring and rendering text.  The text must
     * not include a tab or emoji.
     *
     * @param wp the working paint
     * @param start the start of the text
     * @param end the end of the text
     * @param runIsRtl true if the run is right-to-left
     * @param c the canvas, can be NULL if rendering is not needed
     * @param x the edge of the run closest to the leading margin
     * @param top the top of the line
     * @param y the baseline
     * @param bottom the bottom of the line
     * @param fmi receives metrics information, can be NULL
     * @param needWidth true if the width of the run is needed
     * @return the signed width of the run based on the run direction; only
     * valid if needWidth is true
     */
    float handleText(shared_ptr<TextPaint> wp, int start, int end,
                             int contextStart, int contextEnd, bool runIsRtl,
                             shared_ptr<Canvas> c, float x, int top, int y, int bottom,
                     shared_ptr<Paint::FontMetricsInt> fmi, bool needWidth) {
        
        // Get metrics first (even for empty strings or "0" width runs)
        if (fmi != NULL) {
            expandMetricsFromPaint(fmi, wp);
        }
        
        int runLen = end - start;
        // No need to do anything if the run width is "0"
        if (runLen == 0) {
            return 0.0f;
        }
        
        float ret = 0;
        
        int contextLen = contextEnd - contextStart;
        if (needWidth || (c != NULL && (wp->bgColor != 0 || wp->underlineColor != 0 || runIsRtl))) {
            int flags = runIsRtl ? Paint::DIRECTION_RTL : Paint::DIRECTION_LTR;
            if (mCharsValid) {
                ret = wp->getTextRunAdvances(mChars, start, end,
                                            contextStart, contextEnd, flags, NULL, 0);
            } else {
                int delta = mStart;
                ret = wp->getTextRunAdvances(mText->toString()->toCharArray(), delta + start,
                                            delta + end, delta + contextStart, delta + contextEnd,
                                            flags, NULL, 0);
            }
        }
        
        if (c != NULL) {
            if (runIsRtl) {
                x -= ret;
            }
            
            if (wp->bgColor != 0) {
                int previousColor = wp->getColor();
                Paint::Style previousStyle = wp->getStyle();
                
                wp->setColor(wp->bgColor);
                wp->setStyle(Paint::Style::kFill_Style);
                c->drawRect(x, top, x + ret, bottom, wp.get());
                
                wp->setStyle(previousStyle);
                wp->setColor(previousColor);
            }
            
            if (wp->underlineColor != 0) {
                // kStdUnderline_Offset = 1/9, defined in SkTextFormatParams.h
                float underlineTop = y + wp->baselineShift + (1.0f / 9.0f) * wp->getTextSize();
                
                int previousColor = wp->getColor();
                Paint::Style previousStyle = wp->getStyle();
                bool previousAntiAlias = wp->isAntiAlias();
                
                wp->setStyle(Paint::Style::kFill_Style);
                wp->setAntiAlias(true);
                
                wp->setColor(wp->underlineColor);
                c->drawRect(x, underlineTop, x + ret, underlineTop + wp->underlineThickness, wp.get());
                
                wp->setStyle(previousStyle);
                wp->setColor(previousColor);
                wp->setAntiAlias(previousAntiAlias);
            }
            
            drawTextRun(c, wp, start, end, contextStart, contextEnd, runIsRtl,
                        x, y + wp->baselineShift);
        }
        
        return runIsRtl ? -ret : ret;
    }
    
    /**
     * Utility function for measuring and rendering a replacement.
     *
     *
     * @param replacement the replacement
     * @param wp the work paint
     * @param start the start of the run
     * @param limit the limit of the run
     * @param runIsRtl true if the run is right-to-left
     * @param c the canvas, can be NULL if not rendering
     * @param x the edge of the replacement closest to the leading margin
     * @param top the top of the line
     * @param y the baseline
     * @param bottom the bottom of the line
     * @param fmi receives metrics information, can be NULL
     * @param needWidth true if the width of the replacement is needed
     * @return the signed width of the run based on the run direction; only
     * valid if needWidth is true
     */
    float handleReplacement(shared_ptr<ReplacementSpan> replacement, shared_ptr<TextPaint> wp,
                                    int start, int limit, bool runIsRtl, shared_ptr<Canvas> c,
                                    float x, int top, int y, int bottom, shared_ptr<Paint::FontMetricsInt> fmi,
                            bool needWidth) {
        
        float ret = 0;
        
        int textStart = mStart + start;
        int textLimit = mStart + limit;
        
        if (needWidth || (c != NULL && runIsRtl)) {
            int previousTop = 0;
            int previousAscent = 0;
            int previousDescent = 0;
            int previousBottom = 0;
            int previousLeading = 0;
            
            bool needUpdateMetrics = (fmi != NULL);
            
            if (needUpdateMetrics) {
                previousTop     = fmi->top;
                previousAscent  = fmi->ascent;
                previousDescent = fmi->descent;
                previousBottom  = fmi->bottom;
                previousLeading = fmi->leading;
            }
            
            ret = replacement->getSize(wp.get(), mText.get(), textStart, textLimit, fmi);
            
            if (needUpdateMetrics) {
                updateMetrics(fmi, previousTop, previousAscent, previousDescent, previousBottom,
                              previousLeading);
            }
        }
        
        if (c != NULL) {
            if (runIsRtl) {
                x -= ret;
            }
            replacement->draw(c, mText.get(), textStart, textLimit,
                             x, top, y, bottom, wp.get());
        }
        
        return runIsRtl ? -ret : ret;
    }
    
    /**
     * Utility function for handling a unidirectional run.  The run must not
     * contain tabs or emoji but can contain styles.
     *
     *
     * @param start the line-relative start of the run
     * @param measureLimit the offset to measure to, between start and limit inclusive
     * @param limit the limit of the run
     * @param runIsRtl true if the run is right-to-left
     * @param c the canvas, can be NULL
     * @param x the end of the run closest to the leading margin
     * @param top the top of the line
     * @param y the baseline
     * @param bottom the bottom of the line
     * @param fmi receives metrics information, can be NULL
     * @param needWidth true if the width is required
     * @return the signed width of the run based on the run direction; only
     * valid if needWidth is true
     */
    float handleRun(int start, int measureLimit,
                            int limit, bool runIsRtl, shared_ptr<Canvas> c, float x, int top, int y,
                    int bottom, shared_ptr<Paint::FontMetricsInt> fmi, bool needWidth) {
        
        // Case of an empty line, make sure we update fmi according to mPaint
        if (start == measureLimit) {
            shared_ptr<TextPaint> wp = mWorkPaint;
            wp = mPaint;
            if (fmi != NULL) {
                expandMetricsFromPaint(fmi, wp);
            }
            
            return 0.0f;
        }
        
        if (mSpanned == NULL) {
            shared_ptr<TextPaint> wp = mWorkPaint;
            wp = mPaint;
            const int mlimit = measureLimit;
            return handleText(wp, start, mlimit, start, limit, runIsRtl, c, x, top,
                              y, bottom, fmi, needWidth || mlimit < measureLimit);
        }
        
        mMetricAffectingSpanSpanSet.init(mSpanned, mStart + start, mStart + limit);
        mCharacterStyleSpanSet.init(mSpanned, mStart + start, mStart + limit);
        
        // Shaping needs to take into account context up to metric boundaries,
        // but rendering needs to take into account character style boundaries.
        // So we iterate through metric runs to get metric bounds,
        // then within each metric run iterate through character style runs
        // for the run bounds.
        const float originalX = x;
        for (int i = start, inext; i < measureLimit; i = inext) {
            shared_ptr<TextPaint> wp = mWorkPaint;
            wp = mPaint;
            
            inext = mMetricAffectingSpanSpanSet.getNextTransition(mStart + i, mStart + limit) -
            mStart;
            int mlimit = min(inext, measureLimit);
            
            shared_ptr<ReplacementSpan> replacement;
            
            for (int j = 0; j < mMetricAffectingSpanSpanSet.numberOfSpans; j++) {
                // Both intervals [spanStarts..spanEnds] and [mStart + i..mStart + mlimit] are NOT
                // empty by construction. This special case in getSpans() explains the >= & <= tests
                if ((mMetricAffectingSpanSpanSet.spanStarts[j] >= mStart + mlimit) ||
                    (mMetricAffectingSpanSpanSet.spanEnds[j] <= mStart + i)) continue;
                shared_ptr<MetricAffectingSpan> span = mMetricAffectingSpanSpanSet.spans[j];
                replacement = dynamic_pointer_cast<ReplacementSpan>(span);
                if (replacement == NULL) {
                    // We might have a replacement that uses the draw
                    // state, otherwise measure state would suffice.
                    span->updateDrawState(wp);
                }
            }
            
            if (replacement != NULL) {
                x += handleReplacement(replacement, wp, i, mlimit, runIsRtl, c, x, top, y,
                                       bottom, fmi, needWidth || mlimit < measureLimit);
                continue;
            }
            
            for (int j = i, jnext; j < mlimit; j = jnext) {
                jnext = mCharacterStyleSpanSet.getNextTransition(mStart + j, mStart + mlimit) -
                mStart;
                
                wp = mPaint;
                for (int k = 0; k < mCharacterStyleSpanSet.numberOfSpans; k++) {
                    // Intentionally using >= and <= as explained above
                    if ((mCharacterStyleSpanSet.spanStarts[k] >= mStart + jnext) ||
                        (mCharacterStyleSpanSet.spanEnds[k] <= mStart + j)) continue;
                    
                    shared_ptr<CharacterStyle> span = mCharacterStyleSpanSet.spans[k];
                    span->updateDrawState(wp);
                }
                
                x += handleText(wp, j, jnext, i, inext, runIsRtl, c, x,
                                top, y, bottom, fmi, needWidth || jnext < measureLimit);
            }
        }
        
        return x - originalX;
    }
    
    /**
     * Render a text run with the set-up paint.
     *
     * @param c the canvas
     * @param wp the paint used to render the text
     * @param start the start of the run
     * @param end the end of the run
     * @param contextStart the start of context for the run
     * @param contextEnd the end of the context for the run
     * @param runIsRtl true if the run is right-to-left
     * @param x the x position of the left edge of the run
     * @param y the baseline of the run
     */
    void drawTextRun(shared_ptr<Canvas> c, shared_ptr<TextPaint> wp, int start, int end,
                     int contextStart, int contextEnd, bool runIsRtl, float x, int y) {
        
        int flags = runIsRtl ? Canvas::DIRECTION_RTL : Canvas::DIRECTION_LTR;
        if (mCharsValid) {
            int count = end - start;
            int contextCount = contextEnd - contextStart;
            c->drawTextRun(mChars.getBuffer(), start, count, contextStart, contextCount,
                          x, y, flags, wp.get());
        } else {
            int delta = mStart;
            c->drawTextRun(mText, delta + start, delta + end,
                          delta + contextStart, delta + contextEnd, x, y, flags, wp.get());
        }
    }
    
    /**
     * Returns the ascent of the text at start.  This is used for scaling
     * emoji.
     *
     * @param pos the line-relative position
     * @return the ascent of the text at start
     */
    float ascent(int pos) {
        if (mSpanned == NULL) {
            return mPaint->ascent();
        }
        
        pos += mStart;
        vector<shared_ptr<Object>> spans = mSpanned->getSpans(pos, pos + 1, "MetricAffectingSpan");
        if (spans.size() == 0) {
            return mPaint->ascent();
        }
        
        shared_ptr<TextPaint> wp = mWorkPaint;
        wp = mPaint;
        shared_ptr<MetricAffectingSpan> span;
        
        for (int i = 0; i < spans.size(); i++) {
            span = dynamic_pointer_cast<MetricAffectingSpan>(spans[i]);
            span->updateMeasureState(wp);
        }
        return wp->ascent();
    }
    
    /**
     * Returns the next tab position.
     *
     * @param h the (unsigned) offset from the leading margin
     * @return the (unsigned) tab position after this offset
     */
    float nextTab(float h) {
        if (mTabs != NULL) {
            return mTabs->nextTab(h);
        }
        return TabStopsBase::nextDefaultStop(h, TAB_INCREMENT);
    }
    
    static const int TAB_INCREMENT = 20;
};

template<class T>
vector<shared_ptr<TextLine<T>>> TextLine<T>::sCached = vector<shared_ptr<TextLine<T>>>(3);

ANDROID_END

#endif /* defined(__Androidpp__TextLine__) */
