//
//  InputFilter.cpp
//  Androidpp
//
//  Created by Saul Howard on 1/15/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#include "InputFilter.h"

#include "Android/text/Character.h"
#include "Android/text/CharSequence.h"
#include "Android/text/SpannableString.h"
#include "Android/text/String.h"
#include "Android/text/TextUtils.h"

#include <unicode/unistr.h>

#include <ctype.h>
#include <string>

ANDROID_BEGIN

shared_ptr<CharSequence> InputFilter::AllCaps::filter(shared_ptr<CharSequence> source, int start, int end,
                     shared_ptr<Spanned> dest, int dstart, int dend) {
    for (int i = start; i < end; i++) {
        if (islower(source->charAt(i))) {
            size_t size = end - start;
            UnicodeString v = UnicodeString(size, ' ', size);
            TextUtils::getChars(source, start, end, v, 0);
            shared_ptr<CharSequence> s = make_shared<String>(v)->toUpperCase();
            
            shared_ptr<SpannableString> sp = dynamic_pointer_cast<SpannableString>(s);
            if (sp != NULL) {
                shared_ptr<Spanned> spanned = dynamic_pointer_cast<Spanned>(source);
                TextUtils::copySpansFrom(spanned, start, end, string(), sp, 0);
                return sp;
            } else {
                return s;
            }
        }
    }
    
    return NULL; // keep original
}

shared_ptr<CharSequence> InputFilter::LengthFilter::filter(shared_ptr<CharSequence> source, int start, int end,
                           shared_ptr<Spanned> dest, int dstart, int dend) {
    int keep = mMax - (dest->length() - (dend - dstart));
    
    if (keep <= 0) {
        return make_shared<String>();
    } else if (keep >= end - start) {
        return NULL; // keep original
    } else {
        keep += start;
        if (Character::isHighSurrogate(source->charAt(keep - 1))) {
            --keep;
            if (keep == start) {
                return make_shared<String>();
            }
        }
        return source->subSequence(start, keep);
    }
}

ANDROID_END