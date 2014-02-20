//
//  Spanned.h
//  Androidpp
//
//  Created by Saul Howard on 1/14/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef Androidpp_Spanned_h
#define Androidpp_Spanned_h

#include "AndroidMacros.h"

#include "Android/text/CharSequence.h"
#include "Android/utils/Object.h"

#include <string>
#include <vector>
#include <memory>

using namespace std;

ANDROID_BEGIN

/**
 * This is the interface for text that has markup objects attached to
 * ranges of it.  Not all text classes have mutable markup or text;
 * see {@link Spannable} for mutable markup and {@link Editable} for
 * mutable text.
 */
class Spanned : public virtual CharSequence {
    
public:
    
    /**
     * Bitmask of bits that are relevent for controlling point/mark behavior
     * of spans.
     *
     * MARK and POINT are conceptually located <i>between</i> two adjacent characters.
     * A MARK is "attached" to the character before, while a POINT will stick to the character
     * after. The insertion cursor is conceptually located between the MARK and the POINT.
     *
     * As a result, inserting a new character between a MARK and a POINT will leave the MARK
     * unchanged, while the POINT will be shifted, now located after the inserted character and
     * still glued to the same character after it.
     *
     * Depending on whether the insertion happens at the beginning or the end of a span, the span
     * will hence be expanded to <i>include</i> the new character (when the span is using a MARK at
     * its beginning or a POINT at its end) or it will be <i>excluded</i>.
     *
     * Note that <i>before</i> and <i>after</i> here refer to offsets in the String, which are
     * independent from the visual representation of the text (left-to-right or right-to-left).
     */
    static const int SPAN_POINT_MARK_MASK = 0x33;
    
    /**
     * 0-length spans with type SPAN_MARK_MARK behave like text marks:
     * they remain at their original offset when text is inserted
     * at that offset. Conceptually, the text is added after the mark.
     */
    static const int SPAN_MARK_MARK =   0x11;
    /**
     * SPAN_MARK_POINT is a synonym for {@link #SPAN_INCLUSIVE_INCLUSIVE}.
     */
    static const int SPAN_MARK_POINT =  0x12;
    /**
     * SPAN_POINT_MARK is a synonym for {@link #SPAN_EXCLUSIVE_EXCLUSIVE}.
     */
    static const int SPAN_POINT_MARK =  0x21;
    
    /**
     * 0-length spans with type SPAN_POINT_POINT behave like cursors:
     * they are pushed forward by the length of the insertion when text
     * is inserted at their offset.
     * The text is conceptually inserted before the point.
     */
    static const int SPAN_POINT_POINT = 0x22;
    
    /**
     * SPAN_PARAGRAPH behaves like SPAN_INCLUSIVE_EXCLUSIVE
     * (SPAN_MARK_MARK), except that if either end of the span is
     * at the end of the buffer, that end behaves like _POINT
     * instead (so SPAN_INCLUSIVE_INCLUSIVE if it starts in the
     * middle and ends at the end, or SPAN_EXCLUSIVE_INCLUSIVE
     * if it both starts and ends at the end).
     * <p>
     * Its endpoints must be the start or end of the buffer or
     * immediately after a \n character, and if the \n
     * that anchors it is deleted, the endpoint is pulled to the
     * next \n that follows in the buffer (or to the end of
     * the buffer).
     */
    static const int SPAN_PARAGRAPH =   0x33;
    
    /**
     * Non-0-length spans of type SPAN_INCLUSIVE_EXCLUSIVE expand
     * to include text inserted at their starting point but not at their
     * ending point.  When 0-length, they behave like marks.
     */
    static const int SPAN_INCLUSIVE_EXCLUSIVE = SPAN_MARK_MARK;
    
    /**
     * Spans of type SPAN_INCLUSIVE_INCLUSIVE expand
     * to include text inserted at either their starting or ending point.
     */
    static const int SPAN_INCLUSIVE_INCLUSIVE = SPAN_MARK_POINT;
    
    /**
     * Spans of type SPAN_EXCLUSIVE_EXCLUSIVE do not expand
     * to include text inserted at either their starting or ending point.
     * They can never have a length of 0 and are automatically removed
     * from the buffer if all the text they cover is removed.
     */
    static const int SPAN_EXCLUSIVE_EXCLUSIVE = SPAN_POINT_MARK;
    
    /**
     * Non-0-length spans of type SPAN_EXCLUSIVE_INCLUSIVE expand
     * to include text inserted at their ending point but not at their
     * starting point.  When 0-length, they behave like points.
     */
    static const int SPAN_EXCLUSIVE_INCLUSIVE = SPAN_POINT_POINT;
    
    /**
     * This flag is set on spans that are being used to apply temporary
     * styling information on the composing text of an input method, so that
     * they can be found and removed when the composing text is being
     * replaced.
     */
    static const int SPAN_COMPOSING = 0x100;
    
    /**
     * This flag will be set for intermediate span changes, meaning there
     * is guaranteed to be another change following it.  Typically it is
     * used for {@link Selection} which automatically uses this with the first
     * offset it sets when updating the selection.
     */
    static const int SPAN_INTERMEDIATE = 0x200;
    
    /**
     * The bits numbered SPAN_USER_SHIFT and above are available
     * for callers to use to store scalar data associated with their
     * span object.
     */
    static const int SPAN_USER_SHIFT = 24;
    /**
     * The bits specified by the SPAN_USER bitfield are available
     * for callers to use to store scalar data associated with their
     * span object.
     */
    static const int SPAN_USER = 0xFFFFFFFF << SPAN_USER_SHIFT;
    
    /**
     * The bits numbered just above SPAN_PRIORITY_SHIFT determine the order
     * of change notifications -- higher numbers go first.  You probably
     * don't need to set this; it is used so that when text changes, the
     * text layout gets the chance to update itself before any other
     * callbacks can inquire about the layout of the text.
     */
    static const int SPAN_PRIORITY_SHIFT = 16;
    /**
     * The bits specified by the SPAN_PRIORITY bitmap determine the order
     * of change notifications -- higher numbers go first.  You probably
     * don't need to set this; it is used so that when text changes, the
     * text layout gets the chance to update itself before any other
     * callbacks can inquire about the layout of the text.
     */
    static const int SPAN_PRIORITY = 0xFF << SPAN_PRIORITY_SHIFT;
    
    /**
     * Return an array of the markup objects attached to the specified
     * slice of this CharSequence and whose type is the specified type
     * or a subclass of it.  Specify Object.class for the type if you
     * want all the objects regardless of type.
     */
    virtual vector<shared_ptr<Object>> getSpans(int start, int end, string type) = 0;
    
    /**
     * Return the beginning of the range of text to which the specified
     * markup object is attached, or -1 if the object is not attached.
     */
    virtual int getSpanStart(shared_ptr<Object> tag) = 0;
    
    /**
     * Return the end of the range of text to which the specified
     * markup object is attached, or -1 if the object is not attached.
     */
    virtual int getSpanEnd(shared_ptr<Object> tag) = 0;
    
    /**
     * Return the flags that were specified when {@link Spannable#setSpan} was
     * used to attach the specified markup object, or 0 if the specified
     * object has not been attached.
     */
    virtual int getSpanFlags(shared_ptr<Object> tag) = 0;
    
    /**
     * Return the first offset greater than or equal to <code>start</code>
     * where a markup object of class <code>type</code> begins or ends,
     * or <code>limit</code> if there are no starts or ends greater than or
     * equal to <code>start</code> but less than <code>limit</code>.  Specify
     * <code>null</code> or Object.class for the type if you want every
     * transition regardless of type.
     */
    virtual int nextSpanTransition(int start, int limit, string type) = 0;
};

ANDROID_END

#endif
