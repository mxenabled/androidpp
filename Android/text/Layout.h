//
//  Layout.h
//  Androidpp
//
//  Created by Saul Howard on 1/20/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef __Androidpp__Layout__
#define __Androidpp__Layout__

#include "AndroidMacros.h"

#include "Android/text/CharSequence.h"
#include "Android/text/GetChars.h"
#include "Android/text/TabStops.h"
#include "Android/text/Spanned.h"
#include "Android/text/SpanSet.h"
#include "Android/text/TextDirectionHeuristic.h"
#include "Android/text/TextUtils.h"
#include "Android/utils/Object.h"

#include <unicode/unistr.h>

#include <memory>
#include <vector>

using namespace icu;
using namespace std;

ANDROID_BEGIN

class Canvas;
class Directions;
class LineBackgroundSpan;
class Paint;
class ParagraphStyle;
class Path;
class Rect;
class TextPaint;

/**
 * A base class that manages text layout in visual elements on
 * the screen.
 * <p>For text that will be edited, use a {@link DynamicLayout},
 * which will be updated as the text changes.
 * For text that will not change, use a {@link StaticLayout}.
 */
class Layout {
    
public:
    
    enum Alignment {
        ALIGN_NORMAL,
        ALIGN_OPPOSITE,
        ALIGN_CENTER,
        /** @hide */
        ALIGN_LEFT,
        /** @hide */
        ALIGN_RIGHT,
    };
    
private:
    
    static vector<shared_ptr<ParagraphStyle>> NO_PARA_SPANS;
    
//    /* package */ static const EmojiFactory EMOJI_FACTORY = EmojiFactory.newAvailableInstance();
//    /* package */ static const int MIN_EMOJI, MAX_EMOJI;
    
//    static {
//        if (EMOJI_FACTORY != null) {
//            MIN_EMOJI = EMOJI_FACTORY.getMinimumAndroidPua();
//            MAX_EMOJI = EMOJI_FACTORY.getMaximumAndroidPua();
//        } else {
//            MIN_EMOJI = -1;
//            MAX_EMOJI = -1;
//        }
//    }
    
public:
    
    /**
     * Return how wide a layout must be in order to display the
     * specified text with one line per paragraph.
     */
    static float getDesiredWidth(shared_ptr<CharSequence> source,
                                 shared_ptr<TextPaint> paint);
    
    /**
     * Return how wide a layout must be in order to display the
     * specified text slice with one line per paragraph.
     */
    static float getDesiredWidth(shared_ptr<CharSequence> source,
                                        int start, int end,
                                 shared_ptr<TextPaint> paint);
    
protected:
    
    /**
     * Subclasses of Layout use this constructor to set the display text,
     * width, and other standard properties.
     * @param text the text to render
     * @param paint the default paint for the layout.  Styles can override
     * various attributes of the paint.
     * @param width the wrapping width for the text.
     * @param align whether to left, right, or center the text.  Styles can
     * override the alignment.
     * @param spacingMult factor by which to scale the font size to get the
     * default line spacing
     * @param spacingAdd amount to add to the default line spacing
     */
    Layout(shared_ptr<CharSequence> text, shared_ptr<TextPaint> paint,
                     int width, Alignment align,
           float spacingMult, float spacingAdd);
    
    /**
     * Subclasses of Layout use this constructor to set the display text,
     * width, and other standard properties.
     * @param text the text to render
     * @param paint the default paint for the layout.  Styles can override
     * various attributes of the paint.
     * @param width the wrapping width for the text.
     * @param align whether to left, right, or center the text.  Styles can
     * override the alignment.
     * @param spacingMult factor by which to scale the font size to get the
     * default line spacing
     * @param spacingAdd amount to add to the default line spacing
     *
     * @hide
     */
    Layout(shared_ptr<CharSequence> text, shared_ptr<TextPaint> paint,
                     int width, Alignment align, shared_ptr<TextDirectionHeuristic> textDir,
           float spacingMult, float spacingAdd);
    
protected:
    
    /**
     * Replace constructor properties of this Layout with new ones.  Be careful.
     */
    void replaceWith(shared_ptr<CharSequence> text, shared_ptr<TextPaint> paint,
                                   int width, Alignment align,
                     float spacingmult, float spacingadd);
    
public:
    
    /**
     * Draw this Layout on the specified Canvas.
     */
    void draw(shared_ptr<Canvas> c);
    
    /**
     * Draw this Layout on the specified canvas, with the highlight path drawn
     * between the background and the text.
     *
     * @param canvas the canvas
     * @param highlight the path of the highlight or cursor; can be null
     * @param highlightPaint the paint for the highlight
     * @param cursorOffsetVertical the amount to temporarily translate the
     *        canvas while rendering the highlight
     */
    virtual void draw(shared_ptr<Canvas> canvas, Path *highlight, Paint *highlightPaint,
              int cursorOffsetVertical);
    
    /**
     * @hide
     */
    void drawText(shared_ptr<Canvas> canvas, int firstLine, int lastLine);
    
    /**
     * @hide
     */
    void drawBackground(shared_ptr<Canvas> canvas, Path *highlight, Paint *highlightPaint,
                        int cursorOffsetVertical, int firstLine, int lastLine);
    
    /**
     * @param canvas
     * @return The range of lines that need to be drawn, possibly empty.
     * @hide
     */
    long long getLineRangeForDraw(shared_ptr<Canvas> canvas);
    
private:
    
    /**
     * Return the start position of the line, given the left and right bounds
     * of the margins.
     *
     * @param line the line index
     * @param left the left bounds (0, or leading margin if ltr para)
     * @param right the right bounds (width, minus leading margin if rtl para)
     * @return the start position of the line (to right of line if rtl para)
     */
    virtual int getLineStartPos(int line, int left, int right);
    
public:
    
    /**
     * Return the text that is displayed by this Layout.
     */
    shared_ptr<CharSequence> getText() {
        return mText;
    }
    
    /**
     * Return the base Paint properties for this layout.
     * Do NOT change the paint, which may result in funny
     * drawing for this layout.
     */
    shared_ptr<TextPaint> getPaint() {
        return mPaint;
    }
    
    /**
     * Return the width of this layout.
     */
    const int getWidth() {
        return mWidth;
    }
    
    /**
     * Return the width to which this Layout is ellipsizing, or
     * {@link #getWidth} if it is not doing anything special.
     */
    virtual int getEllipsizedWidth() {
        return mWidth;
    }
    
    /**
     * Increase the width of this layout to the specified width.
     * Be careful to use this only when you know it is appropriate&mdash;
     * it does not cause the text to reflow to use the full new width.
     */
    void increaseWidthTo(int wid);
    
    /**
     * Return the total height of this layout.
     */
    virtual int getHeight() {
        return getLineTop(getLineCount());
    }
    
    /**
     * Return the base alignment of this layout.
     */
    Alignment getAlignment() {
        return mAlignment;
    }
    
    /**
     * Return what the text height is multiplied by to get the line height.
     */
    float getSpacingMultiplier() {
        return mSpacingMult;
    }
    
    /**
     * Return the number of units of leading that are added to each line.
     */
    float getSpacingAdd() {
        return mSpacingAdd;
    }
    
    /**
     * Return the heuristic used to determine paragraph text direction.
     * @hide
     */
    shared_ptr<TextDirectionHeuristic> getTextDirectionHeuristic() {
        return mTextDir;
    }
    
    /**
     * Return the number of lines of text in this layout.
     */
    virtual int getLineCount() = 0;
    
    /**
     * Return the baseline for the specified line (0&hellip;getLineCount() - 1)
     * If bounds is not null, return the top, left, right, bottom extents
     * of the specified line in it.
     * @param line which line to examine (0..getLineCount() - 1)
     * @param bounds Optional. If not null, it returns the extent of the line
     * @return the Y-coordinate of the baseline
     */
    int getLineBounds(int line, Rect bounds);
    
    /**
     * Return the vertical position of the top of the specified line
     * (0&hellip;getLineCount()).
     * If the specified line is equal to the line count, returns the
     * bottom of the last line.
     */
    virtual int getLineTop(int line) = 0;
    
    /**
     * Return the descent of the specified line(0&hellip;getLineCount() - 1).
     */
    virtual int getLineDescent(int line) = 0;
    
    /**
     * Return the text offset of the beginning of the specified line (
     * 0&hellip;getLineCount()). If the specified line is equal to the line
     * count, returns the length of the text.
     */
    virtual int getLineStart(int line) = 0;
    
    /**
     * Returns the primary directionality of the paragraph containing the
     * specified line, either 1 for left-to-right lines, or -1 for right-to-left
     * lines (see {@link #DIR_LEFT_TO_RIGHT}, {@link #DIR_RIGHT_TO_LEFT}).
     */
    virtual int getParagraphDirection(int line) = 0;
    
    /**
     * Returns whether the specified line contains one or more
     * characters that need to be handled specially, like tabs
     * or emoji.
     */
    virtual bool getLineContainsTab(int line) = 0;
    
    /**
     * Returns the (negative) number of extra pixels of ascent padding in the
     * top line of the Layout.
     */
    virtual int getTopPadding() = 0;
    
    /**
     * Returns the number of extra pixels of descent padding in the
     * bottom line of the Layout.
     */
    virtual int getBottomPadding() = 0;
    
    
    /**
     * Returns true if the character at offset and the preceding character
     * are at different run levels (and thus there's a split caret).
     * @param offset the offset
     * @return true if at a level boundary
     * @hide
     */
    bool isLevelBoundary(int offset);
    
    /**
     * Returns true if the character at offset is right to left (RTL).
     * @param offset the offset
     * @return true if the character is RTL, false if it is LTR
     */
    bool isRtlCharAt(int offset);
    
private:
    
    bool primaryIsTrailingPrevious(int offset);
    
public:
    
    /**
     * Get the primary horizontal position for the specified text offset.
     * This is the location where a new character would be inserted in
     * the paragraph's primary direction.
     */
    float getPrimaryHorizontal(int offset) {
        return getPrimaryHorizontal(offset, false /* not clamped */);
    }
    
    /**
     * Get the primary horizontal position for the specified text offset, but
     * optionally clamp it so that it doesn't exceed the width of the layout.
     * @hide
     */
    float getPrimaryHorizontal(int offset, bool clamped) {
        bool trailing = primaryIsTrailingPrevious(offset);
        return getHorizontal(offset, trailing, clamped);
    }
    
    /**
     * Get the secondary horizontal position for the specified text offset.
     * This is the location where a new character would be inserted in
     * the direction other than the paragraph's primary direction.
     */
    float getSecondaryHorizontal(int offset) {
        return getSecondaryHorizontal(offset, false /* not clamped */);
    }
    
    /**
     * Get the secondary horizontal position for the specified text offset, but
     * optionally clamp it so that it doesn't exceed the width of the layout.
     * @hide
     */
    float getSecondaryHorizontal(int offset, bool clamped) {
        bool trailing = primaryIsTrailingPrevious(offset);
        return getHorizontal(offset, !trailing, clamped);
    }
    
private:
    
    float getHorizontal(int offset, bool trailing, bool clamped);
    
    float getHorizontal(int offset, bool trailing, int line, bool clamped);
    
public:
    
    /**
     * Get the leftmost position that should be exposed for horizontal
     * scrolling on the specified line.
     */
    float getLineLeft(int line);
    
    /**
     * Get the rightmost position that should be exposed for horizontal
     * scrolling on the specified line.
     */
    float getLineRight(int line);
    
    /**
     * Gets the unsigned horizontal extent of the specified line, including
     * leading margin indent, but excluding trailing whitespace.
     */
    virtual float getLineMax(int line);
    
    /**
     * Gets the unsigned horizontal extent of the specified line, including
     * leading margin indent and trailing whitespace.
     */
    float getLineWidth(int line);
    
public:
    
    /**
     * Get the line number corresponding to the specified vertical position.
     * If you ask for a position above 0, you get 0; if you ask for a position
     * below the bottom of the text, you get the last line.
     */
    // FIXME: It may be faster to do a linear search for layouts without many lines.
    virtual int getLineForVertical(int vertical);
    
    /**
     * Get the line number on which the specified text offset appears.
     * If you ask for a position before 0, you get 0; if you ask for a position
     * beyond the end of the text, you get the last line.
     */
    int getLineForOffset(int offset);
    
    /**
     * Get the character offset on the specified line whose position is
     * closest to the specified horizontal position.
     */
    int getOffsetForHorizontal(int line, float horiz);
    
    /**
     * Return the text offset after the last character on the specified line.
     */
    int getLineEnd(int line) {
        return getLineStart(line + 1);
    }
    
    /**
     * Return the text offset after the last visible character (so whitespace
     * is not counted) on the specified line.
     */
    int getLineVisibleEnd(int line) {
        return getLineVisibleEnd(line, getLineStart(line), getLineStart(line+1));
    }
    
private:
    
    int getLineVisibleEnd(int line, int start, int end);
    
public:
    
    /**
     * Return the vertical position of the bottom of the specified line.
     */
    int getLineBottom(int line) {
        return getLineTop(line + 1);
    }
    
    /**
     * Return the vertical position of the baseline of the specified line.
     */
    int getLineBaseline(int line) {
        // getLineTop(line+1) == getLineTop(line)
        return getLineTop(line+1) - getLineDescent(line);
    }
    
    /**
     * Get the ascent of the text on the specified line.
     * The return value is negative to match the Paint.ascent() convention.
     */
    int getLineAscent(int line) {
        // getLineTop(line+1) - getLineDescent(line) == getLineBaseLine(line)
        return getLineTop(line) - (getLineTop(line+1) - getLineDescent(line));
    }
    
    int getOffsetToLeftOf(int offset) {
        return getOffsetToLeftRightOf(offset, true);
    }
    
    int getOffsetToRightOf(int offset) {
        return getOffsetToLeftRightOf(offset, false);
    }
    
private:
    
    int getOffsetToLeftRightOf(int caret, bool toLeft);
    
    int getOffsetAtStartOf(int offset);
    
public:
    
    /**
     * Determine whether we should clamp cursor position. Currently it's
     * only robust for left-aligned displays.
     * @hide
     */
    bool shouldClampCursor(int line);
    
    /**
     * Fills in the specified Path with a representation of a cursor
     * at the specified offset.  This will often be a vertical line
     * but can be multiple discontinuous lines in text with multiple
     * directionalities.
     */
    void getCursorPath(int point, Path *dest,
                       shared_ptr<CharSequence> editingBuffer);
    
private:
    
    void addSelection(int line, int start, int end,
                      int top, int bottom, Path *dest);
    
public:
    
    /**
     * Fills in the specified Path with a representation of a highlight
     * between the specified offsets.  This will often be a rectangle
     * or a potentially discontinuous set of rectangles.  If the start
     * and end are the same, the returned path is empty.
     */
    void getSelectionPath(int start, int end, Path *dest);
    
    /**
     * Get the alignment of the specified paragraph, taking into account
     * markup attached to it.
     */
    Alignment getParagraphAlignment(int line);
    
    /**
     * Get the left edge of the specified paragraph, inset by left margins.
     */
    int getParagraphLeft(int line);
    
    /**
     * Get the right edge of the specified paragraph, inset by right margins.
     */
    int getParagraphRight(int line);
    
private:
    
    /**
     * Returns the effective leading margin (unsigned) for this line,
     * taking into account LeadingMarginSpan and LeadingMarginSpan2.
     * @param line the line index
     * @return the leading margin of this line
     */
    int getParagraphLeadingMargin(int line);
    
    /* package */
    static float measurePara(shared_ptr<TextPaint> paint, shared_ptr<CharSequence> text, int start, int end);
    
    /**
     * Like {@link #getLineExtent(int,TabStops,bool)} but determines the
     * tab stops instead of using the ones passed in.
     * @param line the index of the line
     * @param full whether to include trailing whitespace
     * @return the extent of the line
     */
    float getLineExtent(int line, bool full);
    
    /**
     * Returns the signed horizontal extent of the specified line, excluding
     * leading margin.  If full is false, excludes trailing whitespace.
     * @param line the index of the line
     * @param tabStops the tab stops, can be null if we know they're not used.
     * @param full whether to include trailing whitespace
     * @return the extent of the text on this line
     */
    template<class T>
    float getLineExtent(int line, shared_ptr<TabStops<T>> tabStops, bool full);
    
    /**
     * Returns the position of the next tab stop after h on the line.
     *
     * @param text the text
     * @param start start of the line
     * @param end limit of the line
     * @param h the current horizontal offset
     * @param tabs the tabs, can be null.  If it is null, any tabs in effect
     * on the line will be used.  If there are no tabs, a default offset
     * will be used to compute the tab stop.
     * @return the offset of the next tab stop.
     */
    static float nextTab(shared_ptr<CharSequence> text, int start, int end,
                         float h, vector<shared_ptr<Object>> &tabs);
    
protected:
    
    const bool isSpanned() {
        return (mSpannedText != NULL);
    }
    
    /**
     * Returns the same as <code>text.getSpans()</code>, except where
     * <code>start</code> and <code>end</code> are the same and are not
     * at the very beginning of the text, in which case an empty array
     * is returned instead.
     * <p>
     * This is needed because of the special case that <code>getSpans()</code>
     * on an empty range returns the spans adjacent to that range, which is
     * primarily for the sake of <code>TextWatchers</code> so they will get
     * notifications when text goes from empty to non-empty.  But it also
     * has the unfortunate side effect that if the text ends with an empty
     * paragraph, that paragraph accidentally picks up the styles of the
     * preceding paragraph (even though those styles will not be picked up
     * by new text that is inserted into the empty paragraph).
     * <p>
     * The reason it just checks whether <code>start</code> and <code>end</code>
     * is the same is that the only time a line can contain 0 characters
     * is if it is the const paragraph of the Layout; otherwise any line will
     * contain at least one printing or newline character.  The reason for the
     * additional check if <code>start</code> is greater than 0 is that
     * if the empty paragraph is the entire content of the buffer, paragraph
     * styles that are already applied to the buffer will apply to text that
     * is inserted into it.
     */
    template<class T>
    static vector<shared_ptr<T>> getParagraphSpans(shared_ptr<Spanned> text, int start, int end, string type) {
        if (start == end && start > 0) {
            return vector<shared_ptr<T>>();
        }
        
        vector<shared_ptr<Object>> spans = text->getSpans(start, end, type);
        vector<shared_ptr<T>> ret;
        
        typedef typename vector<shared_ptr<Object>>::iterator iter;
        
        for (iter it = spans.begin(); it != spans.end(); ++it) {
            ret.push_back(dynamic_pointer_cast<T>(*it));
        }
        
        return ret;
    }
    
    UChar getEllipsisChar(shared_ptr<TextUtils::TruncateAt> method);
    
    void ellipsize(int start, int end, int line,
                   UnicodeString &dest, int destoff, shared_ptr<TextUtils::TruncateAt> method);
    
public:
    
    /**
     * Returns the directional run information for the specified line.
     * The array alternates counts of characters in left-to-right
     * and right-to-left segments of the line.
     *
     * <p>NOTE: this is inadequate to support bidirectional text, and will change.
     */
    virtual Directions getLineDirections(int line) = 0;
    
    /**
     * Return the offset of the first character to be ellipsized away,
     * relative to the start of the line.  (So 0 if the beginning of the
     * line is ellipsized, not getLineStart().)
     */
    virtual int getEllipsisStart(int line) = 0;
    
    /**
     * Returns the number of characters to be ellipsized away, or 0 if
     * no ellipsis is to take place.
     */
    virtual int getEllipsisCount(int line) = 0;
    
protected:
    
    class Ellipsizer : public virtual CharSequence, public virtual GetChars {
        
        friend class StaticLayout;
        
    private:
        
        /* package */ shared_ptr<CharSequence> mText;
        /* package */ Layout *mLayout = NULL;
        /* package */ int mWidth = 0;
        /* package */ shared_ptr<TextUtils::TruncateAt> mMethod;
        
    public:
        
        Ellipsizer(shared_ptr<CharSequence> s);
        
        UChar charAt(int off);
        
        void getChars(int start, int end, UnicodeString &dest, int destoff);
        
        int length();
        
        shared_ptr<CharSequence> subSequence(int start, int end);
        
        shared_ptr<String> toString();
    };
    
    class SpannedEllipsizer : public Ellipsizer, public Spanned {
        
    private:
        
        shared_ptr<Spanned> mSpanned;
        
    public:
        
        SpannedEllipsizer(shared_ptr<CharSequence> display);
        
        vector<shared_ptr<Object>> getSpans(int start, int end, string type);
        
        int getSpanStart(shared_ptr<Object> tag);
        
        int getSpanEnd(shared_ptr<Object> tag);
        
        int getSpanFlags(shared_ptr<Object> tag);
        
        int nextSpanTransition(int start, int limit, string type);
        
        shared_ptr<CharSequence> subSequence(int start, int end);
    };
    
    /* package */ static const vector<UChar> ELLIPSIS_NORMAL; // this is "..."
    /* package */ static const vector<UChar> ELLIPSIS_TWO_DOTS; // this is ".."
    
    static const int TAB_INCREMENT;
    
private:
    
    shared_ptr<CharSequence> mText;
    shared_ptr<TextPaint> mPaint;
    shared_ptr<TextPaint> mWorkPaint;
    int mWidth = 0;
    Alignment mAlignment = Alignment::ALIGN_NORMAL;
    float mSpacingMult = 0;
    float mSpacingAdd = 0;
    static Rect sTempRect;
    shared_ptr<Spanned> mSpannedText;
    shared_ptr<TextDirectionHeuristic> mTextDir;
    SpanSet<LineBackgroundSpan> mLineBackgroundSpans;
};

ANDROID_END

#endif /* defined(__Androidpp__Layout__) */
