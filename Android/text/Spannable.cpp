//
//  Spannable.c
//  Androidpp
//
//  Created by Saul Howard on 1/15/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#include "Spannable.h"

#include "Android/text/SpannableString.h"

ANDROID_BEGIN

Spannable::Factory Spannable::Factory::sInstance = Spannable::Factory();

shared_ptr<Spannable>Spannable::Factory::newSpannable(shared_ptr<CharSequence> source) {
    return make_shared<SpannableString>(source);
}

ANDROID_END
