//
//  SpannedString.h
//  Androidpp
//
//  Created by Saul Howard on 1/17/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef Androidpp_SpannedString_h
#define Androidpp_SpannedString_h

#include "AndroidMacros.h"

#include "Android/text/SpannableStringInternal.h"
#include "Android/text/CharSequence.h"
#include "Android/text/GetChars.h"
#include "Android/text/Spanned.h"

#include <unicode/unistr.h>

#include <memory>

using namespace icu;
using namespace std;

ANDROID_BEGIN

/**
 * This is the class for text whose content and markup are immutable.
 * For mutable markup, see {@link SpannableString}; for mutable text,
 * see {@link SpannableStringBuilder}.
 */
class SpannedString : public enable_shared_from_this<SpannedString>, public SpannableStringInternal, public virtual CharSequence, public virtual GetChars, public virtual Spanned
{
    
public:
    
    SpannedString(shared_ptr<CharSequence> source) : SpannableStringInternal(dynamic_cast<Spannable*>(this), source, 0, source->length()) {
    }
    
    shared_ptr<CharSequence> subSequence(int start, int end);
    
    static shared_ptr<SpannedString> valueOf(shared_ptr<CharSequence> source);
    
    void setSpan(shared_ptr<Object> what, int start, int end, int flags) {
        SpannableStringInternal::setSpan(dynamic_pointer_cast<Spannable>(shared_from_this()), what, start, end, flags);
    }
    
    void removeSpan(shared_ptr<Object> what) {
        SpannableStringInternal::removeSpan(dynamic_pointer_cast<Spannable>(shared_from_this()), what);
    }
    
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
    
    SpannedString(shared_ptr<CharSequence> source, int start, int end) : SpannableStringInternal(dynamic_cast<Spannable*>(this), source, start, end) {
    }
};

ANDROID_END

#endif
