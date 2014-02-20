//
//  SpannableString.h
//  Androidpp
//
//  Created by Saul Howard on 1/15/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef Androidpp_SpannableString_h
#define Androidpp_SpannableString_h

#include "AndroidMacros.h"

#include "Android/text/SpannableStringInternal.h"
#include "Android/text/CharSequence.h"
#include "Android/text/GetChars.h"
#include "Android/text/Spannable.h"

#include <unicode/unistr.h>

#include <memory>

using namespace icu;
using namespace std;

ANDROID_BEGIN

/**
 * This is the class for text whose content is immutable but to which
 * markup objects can be attached and detached.
 * For mutable text, see {@link SpannableStringBuilder}.
 */
class SpannableString : public SpannableStringInternal, public enable_shared_from_this<SpannableString>, public virtual CharSequence, public virtual GetChars, public virtual Spannable {
    
public:
    
    SpannableString(shared_ptr<CharSequence> source, int start, int end) : SpannableStringInternal(this, source, start, end) {
    }
    
    SpannableString(shared_ptr<CharSequence> source) : SpannableStringInternal(this, source, 0, source->length()) {
    }
    
    static shared_ptr<SpannableString> valueOf(shared_ptr<CharSequence> source);
    
    void setSpan(shared_ptr<Object> what, int start, int end, int flags) {
        SpannableStringInternal::setSpan(shared_from_this(), what, start, end, flags);
    }
    
    void removeSpan(shared_ptr<Object> what) {
        SpannableStringInternal::removeSpan(shared_from_this(), what);
    }
    
    shared_ptr<CharSequence> subSequence(int start, int end);
    
    int length() {
        return SpannableStringInternal::length();
    }
    
    UChar charAt(int i) {
        return SpannableStringInternal::charAt(i);
    }
    
    shared_ptr<String> toString() {
        return SpannableStringInternal::toString();
    }
    
    /* subclasses must do subSequence() to preserve type */
    
    void getChars(int start, int end, UnicodeString &dest, int off) {
        return SpannableStringInternal::getChars(start, end, dest, off);
    }
    
    int getSpanStart(shared_ptr<Object> what) {
        return SpannableStringInternal::getSpanStart(what);
    }
    
    int getSpanEnd(shared_ptr<Object> what) {
        return SpannableStringInternal::getSpanEnd(what);
    }
    
    int getSpanFlags(shared_ptr<Object> what) {
        return SpannableStringInternal::getSpanFlags(what);
    }
    
    vector<shared_ptr<Object>> getSpans(int queryStart, int queryEnd, string kind) {
        return SpannableStringInternal::getSpans(queryStart, queryEnd, kind);
    }
    
    int nextSpanTransition(int start, int limit, string kind) {
        return SpannableStringInternal::nextSpanTransition(start, limit, kind);
    }
};

ANDROID_END

#endif
