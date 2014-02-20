//
//  ParagraphStyle.h
//  Androidpp
//
//  Created by Saul Howard on 1/22/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef Androidpp_ParagraphStyle_h
#define Androidpp_ParagraphStyle_h

#include "AndroidMacros.h"

#include "Android/utils/Object.h"

ANDROID_BEGIN

class ParagraphStyle : public Object {
    
public:
    
    virtual string getType() {
        return "ParagraphStyle";
    };
};

ANDROID_END

#endif
