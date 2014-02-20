//
//  SpannableString.c
//  Androidpp
//
//  Created by Saul Howard on 1/15/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#include "SpannableString.h"

ANDROID_BEGIN

shared_ptr<SpannableString> SpannableString::valueOf(shared_ptr<CharSequence> source) {
    
    shared_ptr<SpannableString> ss = dynamic_pointer_cast<SpannableString>(source);
    if (ss != NULL) {
        return make_shared<SpannableString>(ss);
    } else {
        return make_shared<SpannableString>(source);
    }
}

shared_ptr<CharSequence> SpannableString::subSequence(int start, int end) {
    return make_shared<SpannableString>(shared_from_this(), start, end);
}

ANDROID_END