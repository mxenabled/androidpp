//
//  SpannableStringBuilder.h
//  Androidpp
//
//  Created by Saul Howard on 1/16/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef __Androidpp__SpannableStringBuilder__
#define __Androidpp__SpannableStringBuilder__

#include "AndroidMacros.h"

#include "Android/text/CharSequence.h"
#include "Android/text/GetChars.h"
#include "Android/text/Spannable.h"
#include "Android/text/Editable.h"
#include "Android/text/Appendable.h"
#include "Android/text/GraphicsOperations.h"

#include <unicode/unistr.h>

#include <string>
#include <vector>
#include <memory>

using namespace icu;
using namespace std;

ANDROID_BEGIN

class Object;
class TextWatcher;

/**
 * This is the class for text whose content and markup can both be changed.
 */
class SpannableStringBuilder : public enable_shared_from_this<SpannableStringBuilder>, public virtual CharSequence, public virtual GetChars, public virtual Spannable, public virtual Editable,
public virtual Appendable, public virtual GraphicsOperations {
    
public:
    
    /**
     * Create a new SpannableStringBuilder with empty contents
     */
    SpannableStringBuilder();
    
    /**
     * Create a new SpannableStringBuilder containing a copy of the
     * specified text, including its spans if any.
     */
    SpannableStringBuilder(shared_ptr<CharSequence> text);
    
    /**
     * Create a new SpannableStringBuilder containing a copy of the
     * specified slice of the specified text, including its spans if any.
     */
    SpannableStringBuilder(shared_ptr<CharSequence> text, int start, int end);
    
    static shared_ptr<SpannableStringBuilder> valueOf(shared_ptr<CharSequence> source);
    
    /**
     * Return the char at the specified offset within the buffer.
     */
    UChar charAt(int where);
    
    /**
     * Return the number of chars in the buffer.
     */
    int length();
    
private:
    
    void resizeFor(int size);
    
    void moveGapTo(int where);
    
public:
    
    // Documentation from interface
    SpannableStringBuilder &insert(int where, shared_ptr<CharSequence> tb, int start, int end);
    
    // Documentation from interface
    SpannableStringBuilder &insert(int where, shared_ptr<CharSequence> tb);
    
    // Documentation from interface
    SpannableStringBuilder &deletes(int start, int end);
    
    // Documentation from interface
    void clear();
    
    // Documentation from interface
    void clearSpans();
    
    // Documentation from interface
    SpannableStringBuilder &append(shared_ptr<CharSequence> text);
    
    // Documentation from interface
    SpannableStringBuilder &append(shared_ptr<CharSequence> text, int start, int end);
    
    // Documentation from interface
    SpannableStringBuilder &append(char text);
    
private:
    
    void change(int start, int end, shared_ptr<CharSequence> cs, int csStart, int csEnd);
    
    int updatedIntervalBound(int offset, int start, int nbNewChars, int flag, bool atEnd,
                                     bool textIsRemoved);
    
    void removeSpan(int i);
    
public:
    
    // Documentation from interface
    SpannableStringBuilder &replace(int start, int end, shared_ptr<CharSequence> tb);
    
    // Documentation from interface
    SpannableStringBuilder &replace(const int start, const int end,
                                          shared_ptr<CharSequence> tb, int tbstart, int tbend);
    
private:
    
    static bool hasNonExclusiveExclusiveSpanAt(shared_ptr<CharSequence> text, int offset);
    
    void sendToSpanWatchers(int replaceStart, int replaceEnd, int nbNewChars);
    
public:
    
    /**
     * Mark the specified range of text with the specified object.
     * The flags determine how the span will behave when text is
     * inserted at the start or end of the span's range.
     */
    void setSpan(shared_ptr<Object> what, int start, int end, int flags);
    
private:
    
    void setSpan(bool send, shared_ptr<Object> what, int start, int end, int flags);
    
public:
    
    /**
     * Remove the specified markup object from the buffer.
     */
    void removeSpan(shared_ptr<Object> what);
    
    /**
     * Return the buffer offset of the beginning of the specified
     * markup object, or -1 if it is not attached to this buffer.
     */
    int getSpanStart(shared_ptr<Object> what);
    
    /**
     * Return the buffer offset of the end of the specified
     * markup object, or -1 if it is not attached to this buffer.
     */
    int getSpanEnd(shared_ptr<Object> what);
    
    /**
     * Return the flags of the end of the specified
     * markup object, or 0 if it is not attached to this buffer.
     */
    int getSpanFlags(shared_ptr<Object> what);
    
    /**
     * Return an array of the spans of the specified type that overlap
     * the specified range of the buffer.  The kind may be Object.class to get
     * a list of all the spans regardless of type.
     */
    vector<shared_ptr<Object>> getSpans(int queryStart, int queryEnd, string kind);
    
    /**
     * Return the next offset after <code>start</code> but less than or
     * equal to <code>limit</code> where a span of the specified type
     * begins or ends.
     */
    int nextSpanTransition(int start, int limit, string kind);
    
    /**
     * Return a new CharSequence containing a copy of the specified
     * range of this buffer, including the overlapping spans.
     */
    shared_ptr<CharSequence> subSequence(int start, int end);
    
    /**
     * Copy the specified range of chars from this buffer into the
     * specified array, beginning at the specified offset.
     */
    void getChars(int start, int end, UnicodeString &dest, int destoff);
    
    /**
     * Return a String containing a copy of the chars in this buffer.
     */
    shared_ptr<String> toString();
    
    /**
     * Return a String containing a copy of the chars in this buffer, limited to the
     * [start, end[ range.
     * @hide
     */
    UnicodeString substring(int start, int end);
    
private:
    
    void sendBeforeTextChanged(vector<shared_ptr<Object>> watchers, int start, int before, int after);
    
    void sendTextChanged(vector<shared_ptr<Object>> watchers, int start, int before, int after);
    
    void sendAfterTextChanged(vector<shared_ptr<Object>> watchers);
    
    void sendSpanAdded(shared_ptr<Object> what, int start, int end);
    
    void sendSpanRemoved(shared_ptr<Object> what, int start, int end);
    
    void sendSpanChanged(shared_ptr<Object> what, int oldStart, int oldEnd, int start, int end);
    
    static string region(int start, int end);
    
    void checkRange(string operation, int start, int end);
    
public:
    
    /**
     * Don't call this yourself -- exists for Canvas to use internally.
     * {@hide}
     */
    void drawText(shared_ptr<Canvas> c, int start, int end, float x, float y, Paint *p);
    
    /**
     * Don't call this yourself -- exists for Canvas to use internally.
     * {@hide}
     */
    void drawTextRun(shared_ptr<Canvas> c, int start, int end, int contextStart, int contextEnd,
                            float x, float y, int flags, Paint *p);
    
    /**
     * Don't call this yourself -- exists for Paint to use internally.
     * {@hide}
     */
    float measureText(int start, int end, Paint *p);
    
    /**
     * Don't call this yourself -- exists for Paint to use internally.
     * {@hide}
     */
    int getTextWidths(int start, int end, vector<float> widths, Paint *p);
    
    /**
     * Don't call this yourself -- exists for Paint to use internally.
     * {@hide}
     */
    float getTextRunAdvances(int start, int end, int contextStart, int contextEnd, int flags,
                                    vector<float> advances, int advancesPos, Paint *p);
    
    /**
     * Returns the next cursor position in the run.  This avoids placing the cursor between
     * surrogates, between characters that form conjuncts, between base characters and combining
     * marks, or within a reordering cluster.
     *
     * <p>The context is the shaping context for cursor movement, generally the bounds of the metric
     * span enclosing the cursor in the direction of movement.
     * <code>contextStart</code>, <code>contextEnd</code> and <code>offset</code> are relative to
     * the start of the string.</p>
     *
     * <p>If cursorOpt is CURSOR_AT and the offset is not a valid cursor position,
     * this returns -1.  Otherwise this will never return a value before contextStart or after
     * contextEnd.</p>
     *
     * @param contextStart the start index of the context
     * @param contextEnd the (non-inclusive) end index of the context
     * @param flags either DIRECTION_RTL or DIRECTION_LTR
     * @param offset the cursor position to move from
     * @param cursorOpt how to move the cursor, one of CURSOR_AFTER,
     * CURSOR_AT_OR_AFTER, CURSOR_BEFORE,
     * CURSOR_AT_OR_BEFORE, or CURSOR_AT
     * @param p the Paint object that is requesting this information
     * @return the offset of the next position, or -1
     * @deprecated This is an internal method, refrain from using it in your code
     */
    int getTextRunCursor(int contextStart, int contextEnd, int flags, int offset, int cursorOpt, Paint *p);
    
    // Documentation from interface
    void setFilters(vector<shared_ptr<InputFilter>> filters);
    
    // Documentation from interface
    vector<shared_ptr<InputFilter>> getFilters();
    
private:
    
    static vector<shared_ptr<InputFilter>> NO_FILTERS;
    vector<shared_ptr<InputFilter>> mFilters;
    
    UnicodeString mText;
    int mGapStart;
    int mGapLength;
    
    vector<shared_ptr<Object>> mSpans;
    vector<int> mSpanStarts;
    vector<int> mSpanEnds;
    vector<int> mSpanFlags;
    int mSpanCount;
    int mSpanCountBeforeAdd;
    
    // TODO These value are tightly related to the SPAN_MARK/POINT values in {@link Spanned}
    static const int MARK = 1;
    static const int POINT = 2;
    static const int PARAGRAPH = 3;
    
    static const int START_MASK = 0xF0;
    static const int END_MASK = 0x0F;
    static const int START_SHIFT = 4;
    
    // These bits are not (currently) used by SPANNED flags
    static const int SPAN_START_AT_START = 0x1000;
    static const int SPAN_START_AT_END = 0x2000;
    static const int SPAN_END_AT_START = 0x4000;
    static const int SPAN_END_AT_END = 0x8000;
    static const int SPAN_START_END_MASK = 0xF000;
};

ANDROID_END

#endif /* defined(__Androidpp__SpannableStringBuilder__) */
