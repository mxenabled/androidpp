//
//  TextUtils.h
//  Androidpp
//
//  Created by Saul Howard on 1/15/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef __Androidpp__TextUtils__
#define __Androidpp__TextUtils__

#include "AndroidMacros.h"

#include "Android/view/View.h"
#include "Android/text/Spanned.h"
#include "Android/text/Spannable.h"
#include "Android/utils/Object.h"

#include <utils/Mutex.h>

#include <unicode/unistr.h>

#include <string>
#include <vector>
#include <memory>

using namespace icu;
using namespace std;
using namespace android;

ANDROID_BEGIN

class CharSequence;
class MeasuredText;
class TextPaint;
class TextDirectionHeuristic;

class TextUtils {
    
private:
    
    static const uint16_t FIRST_RIGHT_TO_LEFT = 0x0590;
    
public:
    
    class EllipsizeCallback {
        
    public:
        
        /**
         * This method is called to report that the specified region of
         * text was ellipsized away by a call to {@link #ellipsize}.
         */
        virtual void ellipsized(int start, int end) = 0;
    };
    
    class TruncateAt : public Object {
        
    public:
        
        static shared_ptr<TruncateAt> START,
        MIDDLE,
        END,
        MARQUEE,
        END_SMALL;
    };
    
    /**
     * Copies the spans from the region <code>start...end</code> in
     * <code>source</code> to the region
     * <code>destoff...destoff+end-start</code> in <code>dest</code>.
     * Spans in <code>source</code> that begin before <code>start</code>
     * or end after <code>end</code> but overlap this range are trimmed
     * as if they began at <code>start</code> or ended at <code>end</code>.
     *
     * @throws IndexOutOfBoundsException if any of the copied spans
     * are out of range in <code>dest</code>.
     */
    static void copySpansFrom(shared_ptr<Spanned> &source, int start, int end,
                                     string kind, shared_ptr<Spannable> dest, int destoff);
    
    static bool doesNotNeedBidi(UnicodeString &text, int start, int len) {
        for (int i = start, e = i + len; i < e; i++) {
            if (text[i] >= FIRST_RIGHT_TO_LEFT) {
                return false;
            }
        }
        return true;
    }
    
    /**
     * Returns the original text if it fits in the specified width
     * given the properties of the specified Paint,
     * or, if it does not fit, a truncated
     * copy with ellipsis character added at the specified edge or center.
     */
    static shared_ptr<CharSequence> ellipsize(shared_ptr<CharSequence> text,
                                              shared_ptr<TextPaint> p,
                                              float avail, shared_ptr<TruncateAt> where);
    
    /**
     * Returns the original text if it fits in the specified width
     * given the properties of the specified Paint,
     * or, if it does not fit, a copy with ellipsis character added
     * at the specified edge or center.
     * If <code>preserveLength</code> is specified, the returned copy
     * will be padded with zero-width spaces to preserve the original
     * length and offsets instead of truncating.
     * If <code>callback</code> is non-null, it will be called to
     * report the start and end of the ellipsized range.  TextDirection
     * is determined by the first strong directional character.
     */
    static shared_ptr<CharSequence> ellipsize(shared_ptr<CharSequence> text,
                                              shared_ptr<TextPaint> paint,
                                              float avail, shared_ptr<TruncateAt> where,
                                              bool preserveLength,
                                              EllipsizeCallback *callback);
    
    /**
     * Returns the original text if it fits in the specified width
     * given the properties of the specified Paint,
     * or, if it does not fit, a copy with ellipsis character added
     * at the specified edge or center.
     * If <code>preserveLength</code> is specified, the returned copy
     * will be padded with zero-width spaces to preserve the original
     * length and offsets instead of truncating.
     * If <code>callback</code> is non-null, it will be called to
     * report the start and end of the ellipsized range.
     *
     * @hide
     */
    static shared_ptr<CharSequence> ellipsize(shared_ptr<CharSequence> text,
                                              shared_ptr<TextPaint> paint,
                                              float avail, shared_ptr<TruncateAt> where,
                                              bool preserveLength,
                                              EllipsizeCallback *callback,
                                              shared_ptr<TextDirectionHeuristic> textDir, UnicodeString ellipsis);
    
    static int getOffsetAfter(CharSequence &text, int offset);
    static int getOffsetBefore(CharSequence &text, int offset);
    static void getChars(shared_ptr<CharSequence> s, int start, int end, UnicodeString &dest, int destoff);
    
    static int getLayoutDirectionFromLocale() {
        return View::LAYOUT_DIRECTION_RTL;
    }
    
    static int indexOf(shared_ptr<CharSequence> s, UChar32 ch);
    static int indexOf(shared_ptr<CharSequence> s, UChar32 ch, int start);
    static int indexOf(shared_ptr<CharSequence> s, UChar32 ch, int start, int end);
    
    /**
     * Returns true if the string is null or 0-length.
     * @param str the string to be examined
     * @return true if str is null or zero length
     */
    static bool isEmpty(shared_ptr<CharSequence> str);
    
    /**
     * Pack 2 int values into a long, useful as a return value for a range
     * @see #unpackRangeStartFromLong(long)
     * @see #unpackRangeEndFromLong(long)
     * @hide
     */
    static long packRangeInLong(int start, int end) {
        return (((long long) start) << 32) | end;
    }
    
    static UnicodeString *obtain(int len);
    static void recycle(UnicodeString *temp);
    
    /**
     * Removes empty spans from the <code>spans</code> array.
     *
     * When parsing a Spanned using {@link Spanned#nextSpanTransition(int, int, Class)}, empty spans
     * will (correctly) create span transitions, and calling getSpans on a slice of text bounded by
     * one of these transitions will (correctly) include the empty overlapping span.
     *
     * However, these empty spans should not be taken into account when layouting or rendering the
     * string and this method provides a way to filter getSpans' results accordingly.
     *
     * @param spans A list of spans retrieved using {@link Spanned#getSpans(int, int, Class)} from
     * the <code>spanned</code>
     * @param spanned The Spanned from which spans were extracted
     * @return A subset of spans where empty spans ({@link Spanned#getSpanStart(Object)}  ==
     * {@link Spanned#getSpanEnd(Object)} have been removed. The initial order is preserved
     * @hide
     */
    static vector<shared_ptr<Object>> removeEmptySpans(vector<shared_ptr<Object>> &spans, shared_ptr<Spanned> spanned, string klass);
    
    static shared_ptr<CharSequence> stringOrSpannedString(shared_ptr<CharSequence> source);
    
    /**
     * Get the start value from a range packed in a long by {@link #packRangeInLong(int, int)}
     * @see #unpackRangeEndFromLong(long)
     * @see #packRangeInLong(int, int)
     * @hide
     */
    static int unpackRangeStartFromLong(long long range);
    
    /**
     * Get the end value from a range packed in a long by {@link #packRangeInLong(int, int)}
     * @see #unpackRangeStartFromLong(long)
     * @see #packRangeInLong(int, int)
     * @hide
     */
    static int unpackRangeEndFromLong(long long range);
    
private:
    
    static Mutex mLock;
    static UnicodeString *sTemp;
    
    static const UChar ZWNBS_CHAR = 0xFEFF;
    
    static float setPara(shared_ptr<MeasuredText> mt, shared_ptr<TextPaint> paint,
                         shared_ptr<CharSequence> text, int start, int end, shared_ptr<TextDirectionHeuristic> textDir);
};

ANDROID_END

#endif /* defined(__Androidpp__TextUtils__) */
