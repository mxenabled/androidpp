//
//  SpannedString.c
//  Androidpp
//
//  Created by Saul Howard on 1/17/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#include "SpannedString.h"

ANDROID_BEGIN

shared_ptr<CharSequence> SpannedString::subSequence(int start, int end) {
    return make_shared<SpannedString>(shared_from_this(), start, end);
}

shared_ptr<SpannedString> SpannedString::valueOf(shared_ptr<CharSequence> source) {
    
    shared_ptr<SpannedString> ss = dynamic_pointer_cast<SpannedString>(source);
    if (ss != NULL) {
        return make_shared<SpannedString>(ss);
    } else {
        return make_shared<SpannedString>(source);
    }
}

ANDROID_END