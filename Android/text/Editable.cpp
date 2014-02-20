//
//  Editable.c
//  Androidpp
//
//  Created by Saul Howard on 1/15/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#include "Editable.h"

#include "Android/text/SpannableStringBuilder.h"

ANDROID_BEGIN

Editable::Factory Editable::Factory::sInstance = Editable::Factory();

shared_ptr<Editable> Editable::Factory::newEditable(shared_ptr<CharSequence> source) {
    return make_shared<SpannableStringBuilder>(source);
}

ANDROID_END