//
//  WrapTogetherSpan.h
//  Androidpp
//
//  Created by Saul Howard on 1/22/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef Androidpp_WrapTogetherSpan_h
#define Androidpp_WrapTogetherSpan_h

#include "AndroidMacros.h"

ANDROID_BEGIN

class WrapTogetherSpan : public virtual ParagraphStyle {
    
public:
    
    virtual string getType() {
        return "WrapTogetherSpan";
    };
};

ANDROID_END

#endif
