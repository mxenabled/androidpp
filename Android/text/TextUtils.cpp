//
//  TextUtils.cpp
//  Androidpp
//
//  Created by Saul Howard on 1/15/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#include "TextUtils.h"

#include "Android/internal/R.h"
#include "Android/text/CharSequence.h"
#include "Android/text/MeasuredText.h"
#include "Android/text/SpannedString.h"
#include "Android/text/SpannableString.h"
#include "Android/text/SpannableStringBuilder.h"
#include "Android/text/String.h"
//#include "Android/text/StringBuilder.h"
#include "Android/text/TextDirectionHeuristic.h"
#include "Android/text/TextDirectionHeuristics.h"
#include "Android/text/TextPaint.h"
#include "Android/utils/ArrayUtils.h"
#include "Android/utils/System.h"

#include <vector>
#include <memory>

#include "cocos2d.h"

ANDROID_BEGIN

Mutex TextUtils::mLock;

UnicodeString *TextUtils::sTemp = NULL;

shared_ptr<TextUtils::TruncateAt> TextUtils::TruncateAt::START = make_shared<TruncateAt>();
shared_ptr<TextUtils::TruncateAt> TextUtils::TruncateAt::MIDDLE = make_shared<TruncateAt>();
shared_ptr<TextUtils::TruncateAt> TextUtils::TruncateAt::END = make_shared<TruncateAt>();
shared_ptr<TextUtils::TruncateAt> TextUtils::TruncateAt::MARQUEE = make_shared<TruncateAt>();
shared_ptr<TextUtils::TruncateAt> TextUtils::TruncateAt::END_SMALL = make_shared<TruncateAt>();

void TextUtils::getChars(shared_ptr<CharSequence> s, int start, int end, UnicodeString &dest, int destoff) {
    
    for (int i = start; i < end; i++) {
        dest.setCharAt(destoff++, s->charAt(i));
    }
}

void TextUtils::copySpansFrom(shared_ptr<Spanned> &source, int start, int end, string kind,
                   shared_ptr<Spannable> dest, int destoff) {
    if (kind.empty()) {
        kind = "Object";
    }
    
    vector<shared_ptr<Object>> spans = source->getSpans(start, end, kind);
    
    for (int i = 0; i < spans.size(); i++) {
        int st = source->getSpanStart(spans[i]);
        int en = source->getSpanEnd(spans[i]);
        int fl = source->getSpanFlags(spans[i]);
        
        if (st < start)
            st = start;
        if (en > end)
            en = end;
        
        dest->setSpan(spans[i], st - start + destoff, en - start + destoff, fl);
    }
}

shared_ptr<CharSequence> TextUtils::ellipsize(shared_ptr<CharSequence> text,
                                     shared_ptr<TextPaint> p,
                                     float avail, shared_ptr<TruncateAt> where) {
    return ellipsize(text, p, avail, where, false, NULL);
}

shared_ptr<CharSequence> TextUtils::ellipsize(shared_ptr<CharSequence> text,
                                     shared_ptr<TextPaint> paint,
                                     float avail, shared_ptr<TruncateAt> where,
                                     bool preserveLength,
                                     EllipsizeCallback *callback) {
    
    const char16_t ellipsis = (where == TruncateAt::END_SMALL) ?
        R::string::ellipsis_two_dots :
        R::string::ellipsis;
    
    return ellipsize(text, paint, avail, where, preserveLength, callback,
                     TextDirectionHeuristics::FIRSTSTRONG_LTR,
                     UnicodeString(ellipsis, 1));
}

shared_ptr<CharSequence> TextUtils::ellipsize(shared_ptr<CharSequence> text,
                                     shared_ptr<TextPaint> paint,
                                     float avail, shared_ptr<TruncateAt> where,
                                     bool preserveLength,
                                     EllipsizeCallback *callback,
                                     shared_ptr<TextDirectionHeuristic> textDir, UnicodeString ellipsis) {
    
    int len = text->length();
    
    shared_ptr<MeasuredText> mt = MeasuredText::obtain();
    
    shared_ptr<SpannableStringBuilder> ssb = make_shared<SpannableStringBuilder>();
    
    try {
        float width = setPara(mt, paint, text, 0, text->length(), textDir);
        
        if (width <= avail) {
            if (callback != NULL) {
                callback->ellipsized(0, 0);
            }
            
            return text;
        }
        
        // XXX assumes ellipsis string does not require shaping and
        // is unaffected by style
        float ellipsiswid = paint->measureText(ellipsis);
        avail -= ellipsiswid;
        
        int left = 0;
        int right = len;
        if (avail < 0) {
            // it all goes
        } else if (where == TruncateAt::START) {
            right = len - mt->breakText(len, false, avail);
        } else if (where == TruncateAt::END || where == TruncateAt::END_SMALL) {
            left = mt->breakText(len, true, avail);
        } else {
            right = len - mt->breakText(len, false, avail / 2);
            avail -= mt->measure(right, len);
            left = mt->breakText(right, true, avail);
        }
        
        if (callback != NULL) {
            callback->ellipsized(left, right);
        }
        
        UnicodeString buf = mt->mChars;
        shared_ptr<Spanned> sp = dynamic_pointer_cast<Spanned>(text);
        
        int remaining = len - (right - left);
        if (preserveLength) {
            if (remaining > 0) { // else eliminate the ellipsis too
                buf.setCharAt(left++, ellipsis[0]);
            }
            for (int i = left; i < right; i++) {
                buf.setCharAt(i, ZWNBS_CHAR);
            }
            shared_ptr<String> s = make_shared<String>(buf, 0, len);
            if (sp == NULL) {
                return s;
            }
            shared_ptr<SpannableString> ss = make_shared<SpannableString>(s);
            copySpansFrom(sp, 0, len, "Object", ss, 0);
            return ss;
        }
        
        if (remaining == 0) {
            return make_shared<String>();
        }
        
        if (sp == NULL) {
            UnicodeString sb;
            sb.append(buf, 0, left);
            sb.append(ellipsis);
            sb.append(buf, right, len - right);
            return make_shared<String>(sb);
        }
        ssb->append(text, 0, left);
        ssb->append(make_shared<String>(ellipsis));
        ssb->append(text, right, len);
        
    } catch(Exception exc) {
    }
    
    MeasuredText::recycle(mt);
    
    return ssb;
}

int TextUtils::getOffsetAfter(CharSequence &text, int offset) {
    int len = text.length();
    
    if (offset == len)
        return len;
    if (offset == len - 1)
        return len;
    
    char16_t c = text.charAt(offset);
    
    if (c >= 0xD800 && c <= 0xDBFF) {
        char16_t c1 = text.charAt(offset + 1);
        
        if (c1 >= 0xDC00 && c1 <= 0xDFFF)
            offset += 2;
        else
            offset += 1;
    } else {
        offset += 1;
    }
    
    try{
        Spanned &spanned = dynamic_cast<Spanned&>(text);
    
        vector<shared_ptr<Object>> spans = spanned.getSpans(offset, offset, "ReplacementSpan");
        
        for (int i = 0; i < spans.size(); i++) {
            int start = spanned.getSpanStart(spans[i]);
            int end = spanned.getSpanEnd(spans[i]);
            
            if (start < offset && end > offset)
                offset = end;
        }
    } catch (bad_cast exc) {}
    
    return offset;
}

int TextUtils::getOffsetBefore(CharSequence &text, int offset) {
    if (offset == 0)
        return 0;
    if (offset == 1)
        return 0;
    
    char16_t c = text.charAt(offset - 1);
    
    if (c >= 0xDC00 && c <= 0xDFFF) {
        char16_t c1 = text.charAt(offset - 2);
        
        if (c1 >= 0xD800 && c1 <= 0xDBFF)
            offset -= 2;
        else
            offset -= 1;
    } else {
        offset -= 1;
    }
    
    try{
        Spanned &spanned = dynamic_cast<Spanned&>(text);
        
        vector<shared_ptr<Object>> spans = spanned.getSpans(offset, offset, "ReplacementSpan");
        
        for (int i = 0; i < spans.size(); i++) {
            int start = spanned.getSpanStart(spans[i]);
            int end = spanned.getSpanEnd(spans[i]);
            
            if (start < offset && end > offset)
                offset = start;
        }
    } catch (bad_cast exc) {}
    
    return offset;
}

int TextUtils::indexOf(shared_ptr<CharSequence> s, UChar32 ch) {
    return indexOf(s, ch, 0);
}

int TextUtils::indexOf(shared_ptr<CharSequence> s, UChar32 ch, int start) {
    
    shared_ptr<String> string = dynamic_pointer_cast<String>(s);
    if (string != NULL) {
        return string->indexOf(ch, start);
    } else {
        return indexOf(s, ch, start, s->length());
    }
}

int TextUtils::indexOf(shared_ptr<CharSequence> s, UChar32 ch, int start, int end) {
    
    const int INDEX_INCREMENT = 500;
    UnicodeString *temp = obtain(INDEX_INCREMENT);
    
    while (start < end) {
        int segend = start + INDEX_INCREMENT;
        if (segend > end)
            segend = end;
        
        getChars(s, start, segend, *temp, 0);
        
        int count = segend - start;
        for (int i = 0; i < count; i++) {
            if (temp->charAt(i) == ch) {
                recycle(temp);
                return i + start;
            }
        }
        
        start = segend;
    }
    
    recycle(temp);
    return -1;
}

bool TextUtils::isEmpty(shared_ptr<CharSequence> str) {
    if (str == NULL || str->length() == 0)
        return true;
    else
        return false;
}

UnicodeString *TextUtils::obtain(int len) {
    
    AutoMutex _l(mLock);
    
    UnicodeString *buf = sTemp;
    sTemp = NULL;
    
    if (buf == NULL) {
        buf = new UnicodeString(len, ' ', 0);
    }
    
    if (buf->length() < len) {
        buf->remove();
        buf->getBuffer(ArrayUtils::idealCharArraySize(len));
        buf->releaseBuffer(ArrayUtils::idealCharArraySize(len));
//        buf->resize(ArrayUtils::idealCharArraySize(len));
    }
    
    return buf;
}

void TextUtils::recycle(UnicodeString *temp) {
    if (temp->length() > 1000)
        return;
    
    AutoMutex _l(mLock);
    sTemp = temp;
}

vector<shared_ptr<Object>> TextUtils::removeEmptySpans(vector<shared_ptr<Object>> &spans, shared_ptr<Spanned> spanned, string klass) {
    
    vector<shared_ptr<Object>> copy;
    int count = 0;
    
    for (int i = 0; i < spans.size(); i++) {
        shared_ptr<Object> span = spans[i];
        const int start = spanned->getSpanStart(span);
        const int end = spanned->getSpanEnd(span);
        
        if (start == end) {
            if (copy.empty()) {
                copy.resize(spans.size() - 1);
                System::arraycopy(spans, 0, copy, 0, i);
                count = i;
            }
        } else {
            copy[count] = span;
            count++;
        }
    }
    
    if (!copy.empty()) {
        vector<shared_ptr<Object>> result = vector<shared_ptr<Object>>(count);
        System::arraycopy(copy, 0, result, 0, count);
        return result;
    } else {
        return spans;
    }
}

float TextUtils::setPara(shared_ptr<MeasuredText> mt, shared_ptr<TextPaint> paint,
                             shared_ptr<CharSequence> text, int start, int end, shared_ptr<TextDirectionHeuristic> textDir) {
    
    mt->setPara(text, start, end, textDir);
    
    float width;
    shared_ptr<Spanned> sp = dynamic_pointer_cast<Spanned>(text);
    int len = end - start;
    if (sp == NULL) {
        width = mt->addStyleRun(paint, len, NULL);
    } else {
        width = 0;
        int spanEnd;
        for (int spanStart = 0; spanStart < len; spanStart = spanEnd) {
            spanEnd = sp->nextSpanTransition(spanStart, len, "MetricAffectingSpan");
            vector<shared_ptr<Object>> spans = sp->getSpans(spanStart, spanEnd, "MetricAffectingSpan");
            spans = removeEmptySpans(spans, sp, "MetricAffectingSpan");
            width += mt->addStyleRun(paint, spans, spanEnd - spanStart, NULL);
        }
    }
    
    return width;
}

shared_ptr<CharSequence> TextUtils::stringOrSpannedString(shared_ptr<CharSequence> source) {
    
    if (source == NULL)
        return NULL;
    
    if (dynamic_pointer_cast<SpannedString>(source) != NULL)
        return source;
    
    if (dynamic_pointer_cast<Spanned>(source) != NULL)
        return make_shared<SpannedString>(source);
    
    return source->toString();
}

int TextUtils::unpackRangeStartFromLong(long long range) {
    return (int) (range >> 32);
}

int TextUtils::unpackRangeEndFromLong(long long range) {
    return (int) (range & 0x00000000FFFFFFFFL);
}

ANDROID_END