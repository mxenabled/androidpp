//
//  SpannableStringInternal.cpp
//  Androidpp
//
//  Created by Saul Howard on 1/15/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#include "SpannableStringInternal.h"

#include "Android/text/Spanned.h"
#include "Android/text/SpanWatcher.h"
#include "Android/utils/ArrayUtils.h"
#include "Android/utils/Exceptions.h"
#include "Android/utils/System.h"

#include <algorithm>
#include <sstream>

ANDROID_BEGIN

vector<shared_ptr<Object>> SpannableStringInternal::EMPTY = vector<shared_ptr<Object>>(0);

SpannableStringInternal::SpannableStringInternal(Spannable *sharedThis, shared_ptr<CharSequence> source, int start, int end) {
    
    if (start == 0 && end == source->length())
        mText = source->toString();
    else
        mText = source->toString()->substring(start, end);
    
    int initial = 4;
    mSpans = vector<shared_ptr<Object>>(initial);
    mSpanData = vector<int>(initial * 3);
    
    shared_ptr<Spanned> sp = dynamic_pointer_cast<Spanned>(source);
    if (sp != NULL) {
        vector<shared_ptr<Object>> spans = sp->getSpans(start, end, "Object");
        
        for (int i = 0; i < spans.size(); i++) {
            int st = sp->getSpanStart(spans[i]);
            int en = sp->getSpanEnd(spans[i]);
            int fl = sp->getSpanFlags(spans[i]);
            
            if (st < start)
                st = start;
            if (en > end)
                en = end;
            
            // TOODO: We cannot have a reference to shared_from_this() in the constructor
//            setSpan(sharedThis, spans[i], st - start, en - start, fl);
        }
    }
}

void SpannableStringInternal::getChars(int start, int end, UnicodeString &dest, int off) {
    int count = off;
    for (int i = start; i <= end; i++, count++) {
        dest.setCharAt(count, mText->charAt(i));
    }
}

int SpannableStringInternal::getSpanStart(shared_ptr<Object> what) {
    int count = mSpanCount;
    vector<shared_ptr<Object>> &spans = mSpans;
    vector<int> data = mSpanData;
    
    for (int i = count - 1; i >= 0; i--) {
        if (spans[i] == what) {
            return data[i * COLUMNS + START];
        }
    }
    
    return -1;
}

int SpannableStringInternal::getSpanEnd(shared_ptr<Object> what) {
    int count = mSpanCount;
    vector<shared_ptr<Object>> &spans = mSpans;
    vector<int> data = mSpanData;
    
    for (int i = count - 1; i >= 0; i--) {
        if (spans[i] == what) {
            return data[i * COLUMNS + END];
        }
    }
    
    return -1;
}

int SpannableStringInternal::getSpanFlags(shared_ptr<Object> what) {
    int count = mSpanCount;
    vector<shared_ptr<Object>> &spans = mSpans;
    vector<int> data = mSpanData;
    
    for (int i = count - 1; i >= 0; i--) {
        if (spans[i] == what) {
            return data[i * COLUMNS + FLAGS];
        }
    }
    
    return 0;
}

vector<shared_ptr<Object>> SpannableStringInternal::getSpans(int queryStart, int queryEnd, string kind) {
    int count = 0;
    
    int spanCount = mSpanCount;
    vector<shared_ptr<Object>> &spans = mSpans;
    vector<int> &data = mSpanData;
    vector<shared_ptr<Object>> ret;
    shared_ptr<Object> ret1 = NULL;
    
    for (int i = 0; i < spanCount; i++) {
        if (!kind.empty() && kind.compare(spans[i]->getType()) != 0) {
            continue;
        }
        
        int spanStart = data[i * COLUMNS + START];
        int spanEnd = data[i * COLUMNS + END];
        
        if (spanStart > queryEnd) {
            continue;
        }
        if (spanEnd < queryStart) {
            continue;
        }
        
        if (spanStart != spanEnd && queryStart != queryEnd) {
            if (spanStart == queryEnd) {
                continue;
            }
            if (spanEnd == queryStart) {
                continue;
            }
        }
        
        if (count == 0) {
            ret1 = spans[i];
            count++;
        } else {
            if (count == 1) {
                ret = vector<shared_ptr<Object>>(spanCount - i + 1);
                ret[0] = ret1;
            }
            
            int prio = data[i * COLUMNS + FLAGS] & Spanned::SPAN_PRIORITY;
            if (prio != 0) {
                int j;
                
                for (j = 0; j < count; j++) {
                    int p = getSpanFlags(ret[j]) & Spanned::SPAN_PRIORITY;
                    
                    if (prio > p) {
                        break;
                    }
                }
                
                System::arraycopy<shared_ptr<Object>>(ret, j, ret, j + 1, count - j);
                ret[j] = spans[i];
                count++;
            } else {
                ret[count++] = spans[i];
            }
        }
    }
    
    if (count == 0) {
        return vector<shared_ptr<Object>>();
    }
    if (count == 1) {
        ret = vector<shared_ptr<Object>>(1);
        ret[0] = ret1;
        return ret;
    }
    if (count == ret.size()) {
        return ret;
    }
    
    vector<shared_ptr<Object>> nret = vector<shared_ptr<Object>>(count);
    ret = nret;
    return nret;
}

int SpannableStringInternal::nextSpanTransition(int start, int limit, string kind) {
    int count = mSpanCount;
    vector<shared_ptr<Object>> &spans = mSpans;
    vector<int> data = mSpanData;
    
    if (kind.empty()) {
        kind = "Object";
    }
    
    for (int i = 0; i < count; i++) {
        int st = data[i * COLUMNS + START];
        int en = data[i * COLUMNS + END];
        
        if (st > start && st < limit && kind.compare(spans[i]->getType()) == 0)
            limit = st;
        if (en > start && en < limit && kind.compare(spans[i]->getType()) == 0)
            limit = en;
    }
    
    return limit;
}

void SpannableStringInternal::setSpan(shared_ptr<Spannable> sharedThis, shared_ptr<Object> what, int start, int end, int flags) {
    int nstart = start;
    int nend = end;
    
    checkRange("setSpan", start, end);
    
    if ((flags & Spannable::SPAN_PARAGRAPH) == Spannable::SPAN_PARAGRAPH) {
        if (start != 0 && start != length()) {
            char c = charAt(start - 1);
            
            if (c != '\n') {
                stringstream err;
                err << "PARAGRAPH span must start at paragraph boundary (" << start << " follows " << c << ")";
                throw RuntimeException(err.str());
            }
        }
        
        if (end != 0 && end != length()) {
            char c = charAt(end - 1);
            
            if (c != '\n') {
                stringstream err;
                err << "PARAGRAPH span must end at paragraph boundary (" << start << " follows " << c << ")";
                throw RuntimeException(err.str());
            }
        }
    }
    
    int count = mSpanCount;
    vector<shared_ptr<Object>> &spans = mSpans;
    vector<int> data = mSpanData;
    
    for (int i = 0; i < count; i++) {
        if (spans[i] == what) {
            int ostart = data[i * COLUMNS + START];
            int oend = data[i * COLUMNS + END];
            
            data[i * COLUMNS + START] = start;
            data[i * COLUMNS + END] = end;
            data[i * COLUMNS + FLAGS] = flags;
            
            sendSpanChanged(sharedThis, what, ostart, oend, nstart, nend);
            return;
        }
    }
    
    if (mSpanCount + 1 >= mSpans.size()) {
        int newsize = ArrayUtils::idealIntArraySize(mSpanCount + 1);
        vector<shared_ptr<Object>> newtags = vector<shared_ptr<Object>>(newsize);
        vector<int> newdata = vector<int>(newsize * 3);
        
        System::arraycopy<shared_ptr<Object>>(mSpans, 0, newtags, 0, mSpanCount);
        System::arraycopy<int>(mSpanData, 0, newdata, 0, mSpanCount * 3);
        
        mSpans = newtags;
        mSpanData = newdata;
    }
    
    mSpans[mSpanCount] = what;
    mSpanData[mSpanCount * COLUMNS + START] = start;
    mSpanData[mSpanCount * COLUMNS + END] = end;
    mSpanData[mSpanCount * COLUMNS + FLAGS] = flags;
    mSpanCount++;
    
    sendSpanAdded(sharedThis, what, nstart, nend);
}

void SpannableStringInternal::removeSpan(shared_ptr<Spannable> sharedThis, shared_ptr<Object> what) {
    int count = mSpanCount;
    vector<shared_ptr<Object>> &spans = mSpans;
    vector<int> &data = mSpanData;
    
    for (int i = count - 1; i >= 0; i--) {
        if (spans[i] == what) {
            int ostart = data[i * COLUMNS + START];
            int oend = data[i * COLUMNS + END];
            
            int c = count - (i + 1);
            
            System::arraycopy(spans, i + 1, spans, i, c);
            System::arraycopy(data, (i + 1) * COLUMNS,
                             data, i * COLUMNS, c * COLUMNS);
            
            mSpanCount--;
            
            sendSpanRemoved(sharedThis, what, ostart, oend);
            return;
        }
    }
}

void SpannableStringInternal::sendSpanAdded(shared_ptr<Spannable> sharedThis, shared_ptr<Object> what, int start, int end) {
    vector<shared_ptr<Object>> recip = getSpans(start, end, "SpanWatcher");
    int n = recip.size();
    
    shared_ptr<SpanWatcher> sw;
    for (int i = 0; i < n; i++) {
        sw = dynamic_pointer_cast<SpanWatcher>(recip[i]);
        sw->onSpanAdded(sharedThis, what, start, end);
    }
}

void SpannableStringInternal::sendSpanRemoved(shared_ptr<Spannable> sharedThis, shared_ptr<Object> what, int start, int end) {
    vector<shared_ptr<Object>> recip = getSpans(start, end, "SpanWatcher");
    int n = recip.size();
    
    shared_ptr<SpanWatcher> sw;
    for (int i = 0; i < n; i++) {
        sw = dynamic_pointer_cast<SpanWatcher>(recip[i]);
        sw->onSpanRemoved(sharedThis, what, start, end);
    }
}

void SpannableStringInternal::sendSpanChanged(shared_ptr<Spannable> sharedThis, shared_ptr<Object> what, int s, int e, int st, int en) {
    vector<shared_ptr<Object>> recip = getSpans(min(s, st), max(e, en), "SpanWatcher");
    int n = recip.size();
    
    shared_ptr<SpanWatcher> sw;
    for (int i = 0; i < n; i++) {
        sw = dynamic_pointer_cast<SpanWatcher>(recip[i]);
        sw->onSpanChanged(sharedThis, what, s, e, st, en);
    }
}

string SpannableStringInternal::region(int start, int end) {
    stringstream region;
    region << "(" << start << " ... " << end << ")";
    return region.str();
}

void SpannableStringInternal::checkRange(const string operation, int start, int end) {
    if (end < start) {
        stringstream err;
        err << operation << " " << region(start, end) << " has end before start";
        throw IndexOutOfBoundsException(err.str());
    }
    
    int len = length();
    
    if (start > len || end > len) {
        stringstream err;
        err << operation << " " << region(start, end) << " ends beyond length " << len;
        throw IndexOutOfBoundsException(err.str());
    }
    
    if (start < 0 || end < 0) {
        stringstream err(operation);
        err << operation << " " << region(start, end) << " starts before 0";
        throw IndexOutOfBoundsException(err.str());
    }
}

ANDROID_END