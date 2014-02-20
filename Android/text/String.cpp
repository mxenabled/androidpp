//
//  String.cpp
//  Androidpp
//
//  Created by Saul Howard on 1/15/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#include "String.h"

#include "Android/utils/Exceptions.h"

#include <algorithm>

ANDROID_BEGIN

shared_ptr<String> String::valueOf(UChar c) {
    UnicodeString data(c, 1);
    return make_shared<String>(data);
}

UChar String::charAt(int index) {
    if ((index < 0) || (index >= length())) {
        throw StringIndexOutOfBoundsException(index);
    }
    return mValue[index];
}

int String::indexOf(shared_ptr<String> str) {
    return indexOf(str, 0);
}

int String::indexOf(shared_ptr<String> str, int fromIndex) {
    return indexOf(mValue, length(),
                   str->mValue, str->length(), fromIndex);
}

/**
 * Code shared by String and StringBuffer to do searches. The
 * source is the character array being searched, and the target
 * is the string being searched for.
 *
 * @param   source       the characters being searched.
 * @param   sourceOffset offset of the source string.
 * @param   sourceCount  count of the source string.
 * @param   target       the characters being searched for.
 * @param   targetOffset offset of the target string.
 * @param   targetCount  count of the target string.
 * @param   fromIndex    the index to begin searching from.
 */
int String::indexOf(UnicodeString source, int sourceCount,
                   UnicodeString target, int targetCount,
                   int fromIndex) {
    if (fromIndex >= sourceCount) {
        return (targetCount == 0 ? sourceCount : -1);
    }
    if (fromIndex < 0) {
        fromIndex = 0;
    }
    if (targetCount == 0) {
        return fromIndex;
    }
    
    UChar first  = target[0];
    int max = (sourceCount - targetCount);
    
    for (int i = fromIndex; i <= max; i++) {
        /* Look for first character. */
        if (source[i] != first) {
            while (++i <= max && source[i] != first);
        }
        
        /* Found first character, now look at the rest of v2 */
        if (i <= max) {
            int j = i + 1;
            int end = j + targetCount - 1;
            for (int k = 1; j < end && source[j] ==
                 target[k]; j++, k++);
            
            if (j == end) {
                /* Found whole string. */
                return i;
            }
        }
    }
    return -1;
}

int String::lastIndexOf(int32_t ch) {
    return lastIndexOf(ch, length() - 1);
}

int String::lastIndexOf(int32_t ch, int fromIndex) {
    int pos = mValue.lastIndexOf(ch, fromIndex);
    
    return pos;
}

shared_ptr<CharSequence> String::subSequence(int beginIndex, int endIndex) {
    return substring(beginIndex, endIndex);
}

shared_ptr<String> String::substring(int beginIndex) {
    return substring(beginIndex, mValue.length());
}

shared_ptr<String> String::substring(int beginIndex, int endIndex) {
    if (beginIndex < 0) {
        throw StringIndexOutOfBoundsException(beginIndex);
    }
    if (endIndex > mValue.length()) {
        throw StringIndexOutOfBoundsException(endIndex);
    }
    if (beginIndex > endIndex) {
        throw StringIndexOutOfBoundsException(endIndex - beginIndex);
    }
    return ((beginIndex == 0) && (endIndex == mValue.length())) ? shared_from_this() : make_shared<String>(mValue, beginIndex, endIndex - beginIndex);
}

shared_ptr<String> String::toUpperCase() {
    UnicodeString upper = mValue;
    upper.toUpper();
    return make_shared<String>(upper);
}

ANDROID_END