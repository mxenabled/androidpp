//
//  SpannableStringBuilder.cpp
//  Androidpp
//
//  Created by Saul Howard on 1/16/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#include "SpannableStringBuilder.h"

#include "Android/graphics/Canvas.h"
#include "Android/graphics/Paint.h"
#include "Android/text/InputFilter.h"
#include "Android/text/NoCopySpan.h"
#include "Android/text/String.h"
#include "Android/text/Selection.h"
#include "Android/text/SpanWatcher.h"
#include "Android/text/TextUtils.h"
#include "Android/text/TextWatcher.h"
#include "Android/utils/ArrayUtils.h"
#include "Android/utils/Exceptions.h"
#include "Android/utils/Object.h"
#include "Android/utils/System.h"

#include "cocos2d.h"

#include <memory>
#include <sstream>

ANDROID_BEGIN

vector<shared_ptr<InputFilter>> SpannableStringBuilder::NO_FILTERS = vector<shared_ptr<InputFilter>>(0);

SpannableStringBuilder::SpannableStringBuilder() : SpannableStringBuilder(make_shared<String>()) {
}

SpannableStringBuilder::SpannableStringBuilder(shared_ptr<CharSequence> text) : SpannableStringBuilder(text, 0, text->length()) {
}

SpannableStringBuilder::SpannableStringBuilder(shared_ptr<CharSequence> text, int start, int end) {
    
    mFilters = NO_FILTERS;
    
    int srclen = end - start;
    
    if (srclen < 0) throw StringIndexOutOfBoundsException();
    
    int len = ArrayUtils::idealCharArraySize(srclen + 1);
//    mText.resize(len);
    mGapStart = srclen;
    mGapLength = len - srclen;
    
    TextUtils::getChars(text, start, end, mText, 0);
    
    mSpanCount = 0;
    int alloc = ArrayUtils::idealIntArraySize(0);
    mSpans = vector<shared_ptr<Object>>(alloc);
    mSpanStarts = vector<int>(alloc);
    mSpanEnds = vector<int>(alloc);
    mSpanFlags = vector<int>(alloc);
    
    shared_ptr<Spanned> sp = dynamic_pointer_cast<Spanned>(text);
    
    if (sp != NULL) {
        vector<shared_ptr<Object>> spans = sp->getSpans(start, end, "Object");
        
        for (int i = 0; i < spans.size(); i++) {
            if (dynamic_pointer_cast<NoCopySpan>(spans[i]) != NULL) {
                continue;
            }
            
            int st = sp->getSpanStart(spans[i]) - start;
            int en = sp->getSpanEnd(spans[i]) - start;
            int fl = sp->getSpanFlags(spans[i]);
            
            if (st < 0)
                st = 0;
            if (st > end - start)
                st = end - start;
            
            if (en < 0)
                en = 0;
            if (en > end - start)
                en = end - start;
            
            setSpan(false, spans[i], st, en, fl);
        }
    }
}

shared_ptr<SpannableStringBuilder> SpannableStringBuilder::valueOf(shared_ptr<CharSequence> source) {
    
    shared_ptr<SpannableStringBuilder> ssb = dynamic_pointer_cast<SpannableStringBuilder>(source);
    
    if (ssb != NULL) {
        return ssb;
    } else {
        return make_shared<SpannableStringBuilder>(source);
    }
}

UChar SpannableStringBuilder::charAt(int where) {
    int len = length();
    if (where < 0) {
        stringstream err;
        err << "charAt: " << where << " < 0";
        throw IndexOutOfBoundsException(err.str());
    } else if (where >= len) {
        stringstream err;
        err << "charAt: " << where << " >= length " << len;
        throw IndexOutOfBoundsException(err.str());
    }
    
    if (where >= mGapStart)
        return mText[where + mGapLength];
    else
        return mText[where];
}

int SpannableStringBuilder::length() {
    return mText.length() - mGapLength;
}

void SpannableStringBuilder::resizeFor(int size) {
    const int oldLength = mText.length();
    const int newLength = ArrayUtils::idealCharArraySize(size + 1);
    const int delta = newLength - oldLength;
    if (delta == 0) return;
    
    UnicodeString newText(mText, 0, mGapStart);
    
    const int after = oldLength - (mGapStart + mGapLength);
//    System::arraycopy(mText, oldLength - after, newText, newLength - after, after);
    mText.replace(oldLength - after, after, newText.tempSubString(newLength - after));
    
    mGapLength += delta;
    if (mGapLength < 1)
        Exception("mGapLength < 1");
    
    for (int i = 0; i < mSpanCount; i++) {
        if (mSpanStarts[i] > mGapStart) mSpanStarts[i] += delta;
        if (mSpanEnds[i] > mGapStart) mSpanEnds[i] += delta;
    }
}

void SpannableStringBuilder::moveGapTo(int where) {
    if (where == mGapStart)
        return;
    
    bool atEnd = (where == length());
    
    if (where < mGapStart) {
        int overlap = mGapStart - where;
//        System::arraycopy(mText, where, mText, mGapStart + mGapLength - overlap, overlap);
        mText.replace(where, overlap, mText.tempSubString(mGapStart + mGapLength - overlap));
    } else {
        int overlap = where - mGapStart;
//        System::arraycopy(mText, where + mGapLength - overlap, mText, mGapStart, overlap);
        mText.replace(where + mGapLength - overlap, overlap, mText.tempSubString(mGapStart));
    }
    
    // XXX be more clever
    for (int i = 0; i < mSpanCount; i++) {
        int start = mSpanStarts[i];
        int end = mSpanEnds[i];
        
        if (start > mGapStart)
            start -= mGapLength;
        if (start > where)
            start += mGapLength;
        else if (start == where) {
            int flag = (mSpanFlags[i] & START_MASK) >> START_SHIFT;
            
            if (flag == POINT || (atEnd && flag == PARAGRAPH))
                start += mGapLength;
        }
        
        if (end > mGapStart)
            end -= mGapLength;
        if (end > where)
            end += mGapLength;
        else if (end == where) {
            int flag = (mSpanFlags[i] & END_MASK);
            
            if (flag == POINT || (atEnd && flag == PARAGRAPH))
                end += mGapLength;
        }
        
        mSpanStarts[i] = start;
        mSpanEnds[i] = end;
    }
    
    mGapStart = where;
}

// Documentation from interface
SpannableStringBuilder &SpannableStringBuilder::insert(int where, shared_ptr<CharSequence> tb, int start, int end) {
    return replace(where, where, tb, start, end);
}

// Documentation from interface
SpannableStringBuilder &SpannableStringBuilder::insert(int where, shared_ptr<CharSequence> tb) {
    return replace(where, where, tb, 0, tb->length());
}

// Documentation from interface
SpannableStringBuilder &SpannableStringBuilder::deletes(int start, int end) {
    SpannableStringBuilder &ret = replace(start, end, make_shared<String>(), 0, 0);
    
    if (mGapLength > 2 * length())
        resizeFor(length());
    
    return ret; // == this
}

// Documentation from interface
void SpannableStringBuilder::clear() {
    replace(0, length(), make_shared<String>(), 0, 0);
}

// Documentation from interface
void SpannableStringBuilder::clearSpans() {
    for (int i = mSpanCount - 1; i >= 0; i--) {
        shared_ptr<Object> what = mSpans[i];
        int ostart = mSpanStarts[i];
        int oend = mSpanEnds[i];
        
        if (ostart > mGapStart)
            ostart -= mGapLength;
        if (oend > mGapStart)
            oend -= mGapLength;
        
        mSpanCount = i;
        mSpans[i] = NULL;
        
        sendSpanRemoved(what, ostart, oend);
    }
}

// Documentation from interface
SpannableStringBuilder &SpannableStringBuilder::append(shared_ptr<CharSequence> text) {
    int length = this->length();
    return replace(length, length, text, 0, text->length());
}

// Documentation from interface
SpannableStringBuilder &SpannableStringBuilder::append(shared_ptr<CharSequence> text, int start, int end) {
    int length = this->length();
    return replace(length, length, text, start, end);
}

// Documentation from interface
SpannableStringBuilder &SpannableStringBuilder::append(char text) {
    return append(String::valueOf(text));
}

void SpannableStringBuilder::change(int start, int end, shared_ptr<CharSequence> cs, int csStart, int csEnd) {
    // Can be negative
    const int replacedLength = end - start;
    const int replacementLength = csEnd - csStart;
    const int nbNewChars = replacementLength - replacedLength;
    
    for (int i = mSpanCount - 1; i >= 0; i--) {
        int spanStart = mSpanStarts[i];
        if (spanStart > mGapStart)
            spanStart -= mGapLength;
        
        int spanEnd = mSpanEnds[i];
        if (spanEnd > mGapStart)
            spanEnd -= mGapLength;
        
        if ((mSpanFlags[i] & SPAN_PARAGRAPH) == SPAN_PARAGRAPH) {
            int ost = spanStart;
            int oen = spanEnd;
            int clen = length();
            
            if (spanStart > start && spanStart <= end) {
                for (spanStart = end; spanStart < clen; spanStart++)
                    if (spanStart > end && charAt(spanStart - 1) == '\n')
                        break;
            }
            
            if (spanEnd > start && spanEnd <= end) {
                for (spanEnd = end; spanEnd < clen; spanEnd++)
                    if (spanEnd > end && charAt(spanEnd - 1) == '\n')
                        break;
            }
            
            if (spanStart != ost || spanEnd != oen)
                setSpan(false, mSpans[i], spanStart, spanEnd, mSpanFlags[i]);
        }
        
        int flags = 0;
        if (spanStart == start) flags |= SPAN_START_AT_START;
        else if (spanStart == end + nbNewChars) flags |= SPAN_START_AT_END;
        if (spanEnd == start) flags |= SPAN_END_AT_START;
        else if (spanEnd == end + nbNewChars) flags |= SPAN_END_AT_END;
        mSpanFlags[i] |= flags;
    }
    
    moveGapTo(end);
    
    if (nbNewChars >= mGapLength) {
        resizeFor(mText.length() + nbNewChars - mGapLength);
    }
    
    const bool textIsRemoved = replacementLength == 0;
    // The removal pass needs to be done before the gap is updated in order to broadcast the
    // correct previous positions to the correct intersecting SpanWatchers
    if (replacedLength > 0) { // no need for span fixup on pure insertion
        // A for loop will not work because the array is being modified
        // Do not iterate in reverse to keep the SpanWatchers notified in ordering
        // Also, a removed SpanWatcher should not get notified of removed spans located
        // further in the span array.
        int i = 0;
        while (i < mSpanCount) {
            if ((mSpanFlags[i] & Spanned::SPAN_EXCLUSIVE_EXCLUSIVE) ==
                Spanned::SPAN_EXCLUSIVE_EXCLUSIVE &&
                mSpanStarts[i] >= start && mSpanStarts[i] < mGapStart + mGapLength &&
                mSpanEnds[i] >= start && mSpanEnds[i] < mGapStart + mGapLength &&
                // This condition indicates that the span would become empty
                (textIsRemoved || mSpanStarts[i] > start || mSpanEnds[i] < mGapStart)) {
                removeSpan(i);
                continue; // do not increment i, spans will be shifted left in the array
            }
            
            i++;
        }
    }
    
    mGapStart += nbNewChars;
    mGapLength -= nbNewChars;
    
    if (mGapLength < 1)
        Exception("mGapLength < 1");
    
    TextUtils::getChars(cs, csStart, csEnd, mText, start);
    
    if (replacedLength > 0) { // no need for span fixup on pure insertion
        const bool atEnd = (mGapStart + mGapLength == mText.length());
        
        for (int i = 0; i < mSpanCount; i++) {
            const int startFlag = (mSpanFlags[i] & START_MASK) >> START_SHIFT;
            mSpanStarts[i] = updatedIntervalBound(mSpanStarts[i], start, nbNewChars, startFlag,
                                                  atEnd, textIsRemoved);
            
            const int endFlag = (mSpanFlags[i] & END_MASK);
            mSpanEnds[i] = updatedIntervalBound(mSpanEnds[i], start, nbNewChars, endFlag,
                                                atEnd, textIsRemoved);
        }
    }
    
    mSpanCountBeforeAdd = mSpanCount;
    
    try {
        Spanned *sp = dynamic_cast<Spanned*>(cs.get());
        vector<shared_ptr<Object>> spans = sp->getSpans(csStart, csEnd, "Object");
        
        for (int i = 0; i < spans.size(); i++) {
            int st = sp->getSpanStart(spans[i]);
            int en = sp->getSpanEnd(spans[i]);
            
            if (st < csStart) st = csStart;
            if (en > csEnd) en = csEnd;
            
            // Add span only if this object is not yet used as a span in this string
            if (getSpanStart(spans[i]) < 0) {
                setSpan(false, spans[i], st - csStart + start, en - csStart + start,
                        sp->getSpanFlags(spans[i]));
            }
        }
    } catch(bad_cast exc) {}
}

int SpannableStringBuilder::updatedIntervalBound(int offset, int start, int nbNewChars, int flag, bool atEnd,
                         bool textIsRemoved) {
    if (offset >= start && offset < mGapStart + mGapLength) {
        if (flag == POINT) {
            // A POINT located inside the replaced range should be moved to the end of the
            // replaced text.
            // The exception is when the point is at the start of the range and we are doing a
            // text replacement (as opposed to a deletion): the point stays there.
            if (textIsRemoved || offset > start) {
                return mGapStart + mGapLength;
            }
        } else {
            if (flag == PARAGRAPH) {
                if (atEnd) {
                    return mGapStart + mGapLength;
                }
            } else { // MARK
                // MARKs should be moved to the start, with the exception of a mark located at
                // the end of the range (which will be < mGapStart + mGapLength since mGapLength
                // is > 0, which should stay 'unchanged' at the end of the replaced text.
                if (textIsRemoved || offset < mGapStart - nbNewChars) {
                    return start;
                } else {
                    // Move to the end of replaced text (needed if nbNewChars != 0)
                    return mGapStart;
                }
            }
        }
    }
    return offset;
}

void SpannableStringBuilder::removeSpan(int i) {
    shared_ptr<Object> object = mSpans[i];
    
    int start = mSpanStarts[i];
    int end = mSpanEnds[i];
    
    if (start > mGapStart) start -= mGapLength;
    if (end > mGapStart) end -= mGapLength;
    
    int count = mSpanCount - (i + 1);
    System::arraycopy(mSpans, i + 1, mSpans, i, count);
    System::arraycopy(mSpanStarts, i + 1, mSpanStarts, i, count);
    System::arraycopy(mSpanEnds, i + 1, mSpanEnds, i, count);
    System::arraycopy(mSpanFlags, i + 1, mSpanFlags, i, count);
    
    mSpanCount--;
    
    mSpans[mSpanCount] = NULL;
    
    sendSpanRemoved(object, start, end);
}

// Documentation from interface
SpannableStringBuilder &SpannableStringBuilder::replace(int start, int end, shared_ptr<CharSequence> tb) {
    return replace(start, end, tb, 0, tb->length());
}

// Documentation from interface
SpannableStringBuilder &SpannableStringBuilder::replace(const int start, const int end,
                               shared_ptr<CharSequence> tb, int tbstart, int tbend) {
    checkRange("replace", start, end);
    
    int filtercount = mFilters.size();
    for (int i = 0; i < filtercount; i++) {
        shared_ptr<CharSequence> repl = mFilters[i]->filter(tb, tbstart, tbend, shared_from_this(), start, end);
        
        if (repl != NULL) {
            tb = repl;
            tbstart = 0;
            tbend = repl->length();
        }
    }
    
    const int origLen = end - start;
    const int newLen = tbend - tbstart;
    
    if (origLen == 0 && newLen == 0 && !hasNonExclusiveExclusiveSpanAt(tb, tbstart)) {
        // This is a no-op iif there are no spans in tb that would be added (with a 0-length)
        // Early exit so that the text watchers do not get notified
        return *this;
    }
    
    vector<shared_ptr<Object>> textWatchers = getSpans(start, start + origLen, "TextWatcher");
    sendBeforeTextChanged(textWatchers, start, origLen, newLen);
    
    // Try to keep the cursor / selection at the same relative position during
    // a text replacement. If replaced or replacement text length is zero, this
    // is already taken care of.
    bool adjustSelection = origLen != 0 && newLen != 0;
    int selectionStart = 0;
    int selectionEnd = 0;
    if (adjustSelection) {
        selectionStart = Selection::getSelectionStart(shared_from_this());
        selectionEnd = Selection::getSelectionEnd(shared_from_this());
    }
    
    change(start, end, tb, tbstart, tbend);
    
    if (adjustSelection) {
        if (selectionStart > start && selectionStart < end) {
            const int offset = (selectionStart - start) * newLen / origLen;
            selectionStart = start + offset;
            
            setSpan(false, make_shared<Object>(Selection::SELECTION_START), selectionStart, selectionStart,
                    Spanned::SPAN_POINT_POINT);
        }
        if (selectionEnd > start && selectionEnd < end) {
            const int offset = (selectionEnd - start) * newLen / origLen;
            selectionEnd = start + offset;
            
            setSpan(false, make_shared<Object>(Selection::SELECTION_END), selectionEnd, selectionEnd,
                    Spanned::SPAN_POINT_POINT);
        }
    }
    
    sendTextChanged(textWatchers, start, origLen, newLen);
    sendAfterTextChanged(textWatchers);
    
    // Span watchers need to be called after text watchers, which may update the layout
    sendToSpanWatchers(start, end, newLen - origLen);
    
    return *this;
}

bool SpannableStringBuilder::hasNonExclusiveExclusiveSpanAt(shared_ptr<CharSequence> text, int offset) {
    
    try {
        Spanned *spanned = dynamic_cast<Spanned*>(text.get());
        vector<shared_ptr<Object>> spans = spanned->getSpans(offset, offset, "Object");
        const int length = spans.size();
        for (int i = 0; i < length; i++) {
            shared_ptr<Object> span = spans[i];
            int flags = spanned->getSpanFlags(span);
            if (flags != Spanned::SPAN_EXCLUSIVE_EXCLUSIVE) return true;
        }
    } catch (bad_cast exc) {
    }
    
    return false;
}

void SpannableStringBuilder::sendToSpanWatchers(int replaceStart, int replaceEnd, int nbNewChars) {
    for (int i = 0; i < mSpanCountBeforeAdd; i++) {
        int spanStart = mSpanStarts[i];
        int spanEnd = mSpanEnds[i];
        if (spanStart > mGapStart) spanStart -= mGapLength;
        if (spanEnd > mGapStart) spanEnd -= mGapLength;
        int spanFlags = mSpanFlags[i];
        
        int newReplaceEnd = replaceEnd + nbNewChars;
        bool spanChanged = false;
        
        int previousSpanStart = spanStart;
        if (spanStart > newReplaceEnd) {
            if (nbNewChars != 0) {
                previousSpanStart -= nbNewChars;
                spanChanged = true;
            }
        } else if (spanStart >= replaceStart) {
            // No change if span start was already at replace interval boundaries before replace
            if ((spanStart != replaceStart ||
                 ((spanFlags & SPAN_START_AT_START) != SPAN_START_AT_START)) &&
                (spanStart != newReplaceEnd ||
                 ((spanFlags & SPAN_START_AT_END) != SPAN_START_AT_END))) {
                    // TODO A correct previousSpanStart cannot be computed at this point.
                    // It would require to save all the previous spans' positions before the replace
                    // Using an invalid -1 value to convey this would break the broacast range
                    spanChanged = true;
                }
        }
        
        int previousSpanEnd = spanEnd;
        if (spanEnd > newReplaceEnd) {
            if (nbNewChars != 0) {
                previousSpanEnd -= nbNewChars;
                spanChanged = true;
            }
        } else if (spanEnd >= replaceStart) {
            // No change if span start was already at replace interval boundaries before replace
            if ((spanEnd != replaceStart ||
                 ((spanFlags & SPAN_END_AT_START) != SPAN_END_AT_START)) &&
                (spanEnd != newReplaceEnd ||
                 ((spanFlags & SPAN_END_AT_END) != SPAN_END_AT_END))) {
                    // TODO same as above for previousSpanEnd
                    spanChanged = true;
                }
        }
        
        if (spanChanged) {
            sendSpanChanged(mSpans[i], previousSpanStart, previousSpanEnd, spanStart, spanEnd);
        }
        mSpanFlags[i] &= ~SPAN_START_END_MASK;
    }
    
    // The spans starting at mIntermediateSpanCount were added from the replacement text
    for (int i = mSpanCountBeforeAdd; i < mSpanCount; i++) {
        int spanStart = mSpanStarts[i];
        int spanEnd = mSpanEnds[i];
        if (spanStart > mGapStart) spanStart -= mGapLength;
        if (spanEnd > mGapStart) spanEnd -= mGapLength;
        sendSpanAdded(mSpans[i], spanStart, spanEnd);
    }
}

void SpannableStringBuilder::setSpan(shared_ptr<Object> what, int start, int end, int flags) {
    setSpan(true, what, start, end, flags);
}

void SpannableStringBuilder::setSpan(bool send, shared_ptr<Object> what, int start, int end, int flags) {
    checkRange("setSpan", start, end);
    
    int flagsStart = (flags & START_MASK) >> START_SHIFT;
    if (flagsStart == PARAGRAPH) {
        if (start != 0 && start != length()) {
            char c = charAt(start - 1);
            
            if (c != '\n')
                throw RuntimeException("PARAGRAPH span must start at paragraph boundary");
        }
    }
    
    int flagsEnd = flags & END_MASK;
    if (flagsEnd == PARAGRAPH) {
        if (end != 0 && end != length()) {
            char c = charAt(end - 1);
            
            if (c != '\n')
                throw RuntimeException("PARAGRAPH span must end at paragraph boundary");
        }
    }
    
    // 0-length Spanned::SPAN_EXCLUSIVE_EXCLUSIVE
    if (flagsStart == POINT && flagsEnd == MARK && start == end) {
        if (send) CCLOG("SPAN_EXCLUSIVE_EXCLUSIVE spans cannot have a zero length");
        // Silently ignore invalid spans when they are created from this class.
        // This avoids the duplication of the above test code before all the
        // calls to setSpan that are done in this class
        return;
    }
    
    int nstart = start;
    int nend = end;
    
    if (start > mGapStart) {
        start += mGapLength;
    } else if (start == mGapStart) {
        if (flagsStart == POINT || (flagsStart == PARAGRAPH && start == length()))
            start += mGapLength;
    }
    
    if (end > mGapStart) {
        end += mGapLength;
    } else if (end == mGapStart) {
        if (flagsEnd == POINT || (flagsEnd == PARAGRAPH && end == length()))
            end += mGapLength;
    }
    
    int count = mSpanCount;
    vector<shared_ptr<Object>> &spans = mSpans;
    
    for (int i = 0; i < count; i++) {
        if (spans[i] == what) {
            int ostart = mSpanStarts[i];
            int oend = mSpanEnds[i];
            
            if (ostart > mGapStart)
                ostart -= mGapLength;
            if (oend > mGapStart)
                oend -= mGapLength;
            
            mSpanStarts[i] = start;
            mSpanEnds[i] = end;
            mSpanFlags[i] = flags;
            
            if (send) sendSpanChanged(what, ostart, oend, nstart, nend);
            
            return;
        }
    }
    
    if (mSpanCount + 1 >= mSpans.size()) {
        int newsize = ArrayUtils::idealIntArraySize(mSpanCount + 1);
        vector<shared_ptr<Object>> newspans = vector<shared_ptr<Object>>(newsize);
        vector<int> newspanstarts = vector<int>(newsize);
        vector<int> newspanends = vector<int>(newsize);
        vector<int> newspanflags = vector<int>(newsize);
        
        System::arraycopy(mSpans, 0, newspans, 0, mSpanCount);
        System::arraycopy(mSpanStarts, 0, newspanstarts, 0, mSpanCount);
        System::arraycopy(mSpanEnds, 0, newspanends, 0, mSpanCount);
        System::arraycopy(mSpanFlags, 0, newspanflags, 0, mSpanCount);
        
        mSpans = newspans;
        mSpanStarts = newspanstarts;
        mSpanEnds = newspanends;
        mSpanFlags = newspanflags;
    }
    
    mSpans[mSpanCount] = what;
    mSpanStarts[mSpanCount] = start;
    mSpanEnds[mSpanCount] = end;
    mSpanFlags[mSpanCount] = flags;
    mSpanCount++;
    
    if (send) sendSpanAdded(what, nstart, nend);
}

void SpannableStringBuilder::removeSpan(shared_ptr<Object> what) {
    for (int i = mSpanCount - 1; i >= 0; i--) {
        if (mSpans[i] == what) {
            removeSpan(i);
            return;
        }
    }
}

int SpannableStringBuilder::getSpanStart(shared_ptr<Object> what) {
    int count = mSpanCount;
    vector<shared_ptr<Object>> &spans = mSpans;
    
    for (int i = count - 1; i >= 0; i--) {
        if (spans[i] == what) {
            int where = mSpanStarts[i];
            
            if (where > mGapStart)
                where -= mGapLength;
            
            return where;
        }
    }
    
    return -1;
}

int SpannableStringBuilder::getSpanEnd(shared_ptr<Object> what) {
    int count = mSpanCount;
    vector<shared_ptr<Object>> &spans = mSpans;
    
    for (int i = count - 1; i >= 0; i--) {
        if (spans[i] == what) {
            int where = mSpanEnds[i];
            
            if (where > mGapStart)
                where -= mGapLength;
            
            return where;
        }
    }
    
    return -1;
}

int SpannableStringBuilder::getSpanFlags(shared_ptr<Object> what) {
    int count = mSpanCount;
    vector<shared_ptr<Object>> &spans = mSpans;
    
    for (int i = count - 1; i >= 0; i--) {
        if (spans[i] == what) {
            return mSpanFlags[i];
        }
    }
    
    return 0;
}

vector<shared_ptr<Object>> SpannableStringBuilder::getSpans(int queryStart, int queryEnd, string kind) {
    if (kind.empty()) return vector<shared_ptr<Object>>();
    
    int &spanCount = mSpanCount;
    vector<shared_ptr<Object>> &spans = mSpans;
    vector<int> &starts = mSpanStarts;
    vector<int> &ends = mSpanEnds;
    vector<int> &flags = mSpanFlags;
    int &gapstart = mGapStart;
    int &gaplen = mGapLength;
    
    int count = 0;
    vector<shared_ptr<Object>> ret;
    shared_ptr<Object> ret1 = NULL;
    
    for (int i = 0; i < spanCount; i++) {
        int spanStart = starts[i];
        if (spanStart > gapstart) {
            spanStart -= gaplen;
        }
        if (spanStart > queryEnd) {
            continue;
        }
        
        int spanEnd = ends[i];
        if (spanEnd > gapstart) {
            spanEnd -= gaplen;
        }
        if (spanEnd < queryStart) {
            continue;
        }
        
        if (spanStart != spanEnd && queryStart != queryEnd) {
            if (spanStart == queryEnd)
                continue;
            if (spanEnd == queryStart)
                continue;
        }
        
        // Expensive test, should be performed after the previous tests
        if (kind.compare(spans[i]->getType()) != 0) continue;
        
        if (count == 0) {
            // Safe conversion thanks to the isInstance test above
            ret1 = spans[i];
            count++;
        } else {
            if (count == 1) {
                // Safe conversion, but requires a suppressWarning
                ret = vector<shared_ptr<Object>>(spanCount - i + 1);
                ret[0] = ret1;
            }
            
            int prio = flags[i] & SPAN_PRIORITY;
            if (prio != 0) {
                int j;
                
                for (j = 0; j < count; j++) {
                    int p = getSpanFlags(ret[j]) & SPAN_PRIORITY;
                    
                    if (prio > p) {
                        break;
                    }
                }
                
                System::arraycopy(ret, j, ret, j + 1, count - j);
                // Safe conversion thanks to the isInstance test above
                ret[j] = spans[i];
                count++;
            } else {
                // Safe conversion thanks to the isInstance test above
                ret[count++] = spans[i];
            }
        }
    }
    
    if (count == 0) {
        return vector<shared_ptr<Object>>();
    }
    if (count == 1) {
        // Safe conversion, but requires a suppressWarning
        ret = vector<shared_ptr<Object>>(1);
        ret[0] = ret1;
        return ret;
    }
    if (count == ret.size()) {
        return ret;
    }
    
    // Safe conversion, but requires a suppressWarning
    vector<shared_ptr<Object>> nret = vector<shared_ptr<Object>>(count);
    System::arraycopy(ret, 0, nret, 0, count);
    return nret;
}

int SpannableStringBuilder::nextSpanTransition(int start, int limit, string kind) {
    int count = mSpanCount;
    vector<shared_ptr<Object>> &spans = mSpans;
    vector<int> starts = mSpanStarts;
    vector<int> ends = mSpanEnds;
    int gapstart = mGapStart;
    int gaplen = mGapLength;
    
    if (kind.empty()) {
        kind = "Object";
    }
    
    for (int i = 0; i < count; i++) {
        int st = starts[i];
        int en = ends[i];
        
        if (st > gapstart)
            st -= gaplen;
        if (en > gapstart)
            en -= gaplen;
        
        if (st > start && st < limit && kind.compare(spans[i]->getType()) == 0)
            limit = st;
        if (en > start && en < limit && kind.compare(spans[i]->getType()) == 0)
            limit = en;
    }
    
    return limit;
}

shared_ptr<CharSequence> SpannableStringBuilder::subSequence(int start, int end) {
    return make_shared<SpannableStringBuilder>(shared_from_this(), start, end);
}

void SpannableStringBuilder::getChars(int start, int end, UnicodeString &dest, int destoff) {
    checkRange("getChars", start, end);
    
    if (end <= mGapStart) {
//        System::arraycopy(mText, start, dest, destoff, end - start);
        mText.replace(start, end - start, dest.tempSubString(destoff));
    } else if (start >= mGapStart) {
//        System::arraycopy(mText, start + mGapLength, dest, destoff, end - start);
        mText.replace(start + mGapLength, end - start, dest.tempSubString(destoff));
    } else {
//        System::arraycopy(mText, start, dest, destoff, mGapStart - start);
        mText.replace(start, mGapStart - start, dest.tempSubString(destoff));
//        System::arraycopy(mText, mGapStart + mGapLength,
//                         dest, destoff + (mGapStart - start),
//                          end - mGapStart);
        mText.replace(mGapStart + mGapLength, end - mGapStart, dest.tempSubString(destoff + (mGapStart - start)));
    }
}

shared_ptr<String> SpannableStringBuilder::toString() {
    int len = length();
    UnicodeString buf(len, ' ', 0);
    
    getChars(0, len, buf, 0);
    return make_shared<String>(buf);
}

UnicodeString SpannableStringBuilder::substring(int start, int end) {
    
    UnicodeString buf(end - start, ' ', 0);

    getChars(start, end, buf, 0);
    return buf;
}

void SpannableStringBuilder::sendBeforeTextChanged(vector<shared_ptr<Object>> watchers, int start, int before, int after) {
    int n = watchers.size();
    
    for (int i = 0; i < n; i++) {
        dynamic_pointer_cast<TextWatcher>(watchers[i])->beforeTextChanged(shared_from_this(), start, before, after);
    }
}

void SpannableStringBuilder::sendTextChanged(vector<shared_ptr<Object>> watchers, int start, int before, int after) {
    int n = watchers.size();
    
    for (int i = 0; i < n; i++) {
        dynamic_pointer_cast<TextWatcher>(watchers[i])->onTextChanged(shared_from_this(), start, before, after);
    }
}

void SpannableStringBuilder::sendAfterTextChanged(vector<shared_ptr<Object>> watchers) {
    int n = watchers.size();
    
    for (int i = 0; i < n; i++) {
        dynamic_pointer_cast<TextWatcher>(watchers[i])->afterTextChanged(shared_from_this());
    }
}

void SpannableStringBuilder::sendSpanAdded(shared_ptr<Object> what, int start, int end) {
    vector<shared_ptr<Object>> recip = getSpans(start, end, "SpanWatcher");
    int n = recip.size();
    
    for (int i = 0; i < n; i++) {
        dynamic_pointer_cast<SpanWatcher>(what)->onSpanAdded(shared_from_this(), what, start, end);
    }
}

void SpannableStringBuilder::sendSpanRemoved(shared_ptr<Object> what, int start, int end) {
    vector<shared_ptr<Object>> recip = getSpans(start, end, "SpanWatcher");
    int n = recip.size();
    
    for (int i = 0; i < n; i++) {
        dynamic_pointer_cast<SpanWatcher>(what)->onSpanRemoved(shared_from_this(), what, start, end);
    }
}

void SpannableStringBuilder::sendSpanChanged(shared_ptr<Object> what, int oldStart, int oldEnd, int start, int end) {
    // The bounds of a possible SpanWatcher are guaranteed to be set before this method is
    // called, so that the order of the span does not affect this broadcast.
    vector<shared_ptr<Object>> spanWatchers = getSpans(min(oldStart, start),
                                          min(max(oldEnd, end), length()), "SpanWatcher");
    int n = spanWatchers.size();
    for (int i = 0; i < n; i++) {
        dynamic_pointer_cast<SpanWatcher>(spanWatchers[i])->onSpanChanged(shared_from_this(), what, oldStart, oldEnd, start, end);
    }
}

string SpannableStringBuilder::region(int start, int end) {
    stringstream region;
    region << "(" << start << " ... " << end << ")";
    return region.str();
}

void SpannableStringBuilder::checkRange(string operation, int start, int end) {
    if (end < start) {
        throw IndexOutOfBoundsException(operation + " " +
                                            region(start, end) + " has end before start");
    }
    
    int len = length();
    
    if (start > len || end > len) {
        stringstream str;
        str << operation << " " << region(start, end) << " ends beyond length " << len;
        throw IndexOutOfBoundsException(str.str());
    }
    
    if (start < 0 || end < 0) {
        throw IndexOutOfBoundsException(operation + " " +
                                            region(start, end) + " starts before 0");
    }
}

void SpannableStringBuilder::drawText(shared_ptr<Canvas> c, int start, int end, float x, float y, Paint *p) {
    checkRange("drawText", start, end);
    
    if (end <= mGapStart) {
        c->drawText(mText, start, end - start, x, y, p);
    } else if (start >= mGapStart) {
        c->drawText(mText, start + mGapLength, end - start, x, y, p);
    } else {
        UnicodeString *buf = TextUtils::obtain(end - start);
        
        getChars(start, end, *buf, 0);
        c->drawText(*buf, 0, end - start, x, y, p);
        TextUtils::recycle(buf);
    }
}


void SpannableStringBuilder::drawTextRun(shared_ptr<Canvas> c, int start, int end, int contextStart, int contextEnd,
                 float x, float y, int flags, Paint *p) {
    checkRange("drawTextRun", start, end);
    
    int contextLen = contextEnd - contextStart;
    int len = end - start;
    if (contextEnd <= mGapStart) {
        c->drawTextRun(mText.getBuffer(), start, len, contextStart, contextLen, x, y, flags, p);
    } else if (contextStart >= mGapStart) {
        c->drawTextRun(mText.getBuffer(), start + mGapLength, len, contextStart + mGapLength,
                      contextLen, x, y, flags, p);
    } else {
        UnicodeString *buf = TextUtils::obtain(contextLen);
        getChars(contextStart, contextEnd, *buf, 0);
        c->drawTextRun(buf->getBuffer(), start - contextStart, len, 0, contextLen, x, y, flags, p);
        TextUtils::recycle(buf);
    }
}

float SpannableStringBuilder::measureText(int start, int end, Paint *p) {
    checkRange("measureText", start, end);
    
    float ret;
    
    if (end <= mGapStart) {
        ret = p->measureText(mText, start, end - start);
    } else if (start >= mGapStart) {
        ret = p->measureText(mText, start + mGapLength, end - start);
    } else {
        UnicodeString *buf = TextUtils::obtain(end - start);
        
        getChars(start, end, *buf, 0);
        ret = p->measureText(*buf, 0, end - start);
        TextUtils::recycle(buf);
    }
    
    return ret;
}

int SpannableStringBuilder::getTextWidths(int start, int end, vector<float> widths, Paint *p) {
    checkRange("getTextWidths", start, end);
    
    int ret;
    
    if (end <= mGapStart) {
        ret = p->getTextWidths(mText, start, end - start, widths);
    } else if (start >= mGapStart) {
        ret = p->getTextWidths(mText, start + mGapLength, end - start, widths);
    } else {
        UnicodeString *buf = TextUtils::obtain(end - start);
        
        getChars(start, end, *buf, 0);
        ret = p->getTextWidths(*buf, 0, end - start, widths);
        TextUtils::recycle(buf);
    }
    
    return ret;
}

float SpannableStringBuilder::getTextRunAdvances(int start, int end, int contextStart, int contextEnd, int flags,
                         vector<float> advances, int advancesPos, Paint *p) {
    
    float ret;
    
    int contextLen = contextEnd - contextStart;
    
    if (end <= mGapStart) {
        ret = p->getTextRunAdvances(mText, start, end, contextStart, contextEnd,
                                   flags, &advances, advancesPos);
    } else if (start >= mGapStart) {
        ret = p->getTextRunAdvances(mText, start + mGapLength, end,
                                   contextStart + mGapLength, contextEnd, flags, &advances, advancesPos);
    } else {
        UnicodeString *buf = TextUtils::obtain(contextLen);
        getChars(contextStart, contextEnd, *buf, 0);
        ret = p->getTextRunAdvances(*buf, start - contextStart, end,
                                   0, contextEnd, flags, &advances, advancesPos);
        TextUtils::recycle(buf);
    }
    
    return ret;
}

int SpannableStringBuilder::getTextRunCursor(int contextStart, int contextEnd, int flags, int offset,
                            int cursorOpt, Paint *p) {
    
    int ret;
    
    int contextLen = contextEnd - contextStart;
    if (contextEnd <= mGapStart) {
        ret = p->getTextRunCursor(mText, contextStart, contextLen,
                                 flags, offset, cursorOpt);
    } else if (contextStart >= mGapStart) {
        ret = p->getTextRunCursor(mText, contextStart + mGapLength, contextLen,
                                 flags, offset + mGapLength, cursorOpt) - mGapLength;
    } else {
        UnicodeString *buf = TextUtils::obtain(contextLen);
        getChars(contextStart, contextEnd, *buf, 0);
        ret = p->getTextRunCursor(*buf, 0, contextLen,
                                 flags, offset - contextStart, cursorOpt) + contextStart;
        TextUtils::recycle(buf);
    }
    
    return ret;
}

// Documentation from interface
void SpannableStringBuilder::setFilters(vector<shared_ptr<InputFilter>> filters) {
    mFilters = filters;
}

// Documentation from interface
vector<shared_ptr<InputFilter>> SpannableStringBuilder::getFilters() {
    return mFilters;
}

ANDROID_END