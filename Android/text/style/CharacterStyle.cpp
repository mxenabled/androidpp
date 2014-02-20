//
//  CharacterStyle.cpp
//  Androidpp
//
//  Created by Saul Howard on 1/22/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#include "CharacterStyle.h"

#include "Android/text/style/MetricAffectingSpan.h"

ANDROID_BEGIN

shared_ptr<CharacterStyle> CharacterStyle::wrap(shared_ptr<CharacterStyle> cs) {
    
    shared_ptr<MetricAffectingSpan> mas = dynamic_pointer_cast<MetricAffectingSpan>(cs);
    
    if (mas != NULL) {
        return make_shared<MetricAffectingSpan::Passthrough>(mas);
    } else {
        return make_shared<Passthrough>(cs);
    }
}

ANDROID_END