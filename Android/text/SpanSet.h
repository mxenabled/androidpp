//
//  SpanSet.h
//  Androidpp
//
//  Created by Saul Howard on 1/20/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef __Androidpp__SpanSet__
#define __Androidpp__SpanSet__

#include "AndroidMacros.h"

#include "Android/utils/Object.h"
#include "Android/text/Spanned.h"

#include <memory>
#include <string>
#include <vector>

using namespace std;

ANDROID_BEGIN

/**
 * A cached set of spans. Caches the result of {@link Spanned#getSpans(int, int, Class)} and then
 * provides faster access to {@link Spanned#nextSpanTransition(int, int, Class)}.
 *
 * Fields are left for a convenient direct access.
 *
 * Note that empty spans are ignored by this class.
 * @hide
 */
template<class T>
class SpanSet {
    
    template<class U>
    friend class TextLine;
    friend class Layout;
    
private:
    
    string classType;
    
    int numberOfSpans;
    vector<shared_ptr<T>> spans;
    vector<int> spanStarts;
    vector<int> spanEnds;
    vector<int> spanFlags;
    
public:
    
    SpanSet(string type) {
        classType = type;
        numberOfSpans = 0;
    }
    
    void init(shared_ptr<Spanned> spanned, int start, int limit) {
        vector<shared_ptr<Object>> allSpans = spanned->getSpans(start, limit, classType);
        const int length = allSpans.size();
        
        if (length > 0 && (spans.size() < length)) {
            // These arrays may end up being too large because of the discarded empty spans
            spans = vector<shared_ptr<T>>(length);
            spanStarts = vector<int>(length);
            spanEnds = vector<int>(length);
            spanFlags = vector<int>(length);
        }
        
        numberOfSpans = 0;
        for (int i = 0; i < length; i++) {
            shared_ptr<Object> span = allSpans[i];
            
            const int spanStart = spanned->getSpanStart(span);
            const int spanEnd = spanned->getSpanEnd(span);
            if (spanStart == spanEnd) continue;
            
            const int spanFlag = spanned->getSpanFlags(span);
            
            spans[numberOfSpans] = dynamic_pointer_cast<T>(span);
            spanStarts[numberOfSpans] = spanStart;
            spanEnds[numberOfSpans] = spanEnd;
            spanFlags[numberOfSpans] = spanFlag;
            
            numberOfSpans++;
        }
    }
    
    /**
     * Returns true if there are spans intersecting the given interval.
     * @param end must be strictly greater than start
     */
    bool hasSpansIntersecting(int start, int end) {
        for (int i = 0; i < numberOfSpans; i++) {
            // equal test is valid since both intervals are not empty by construction
            if (spanStarts[i] >= end || spanEnds[i] <= start) continue;
            return true;
        }
        return false;
    }
    
private:
    
    /**
     * Similar to {@link Spanned#nextSpanTransition(int, int, Class)}
     */
    int getNextTransition(int start, int limit) {
        for (int i = 0; i < numberOfSpans; i++) {
            const int spanStart = spanStarts[i];
            const int spanEnd = spanEnds[i];
            if (spanStart > start && spanStart < limit) limit = spanStart;
            if (spanEnd > start && spanEnd < limit) limit = spanEnd;
        }
        return limit;
    }
    
public:
    
    /**
     * Removes all internal references to the spans to avoid memory leaks.
     */
    void recycle() {
        // The spans array is guaranteed to be not null when numberOfSpans is > 0
        for (int i = 0; i < numberOfSpans; i++) {
            spans[i] = NULL; // prevent a leak: no reference kept when TextLine is recycled
        }
    }
};

ANDROID_END

#endif /* defined(__Androidpp__SpanSet__) */
