//
//  Region.h
//  Androidpp
//
//  Created by Saul Howard on 1/14/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef Androidpp_Region_h
#define Androidpp_Region_h

#include "AndroidMacros.h"

#include <SkRegion.h>

ANDROID_BEGIN

class Region : public SkRegion {
    
public:
    
    int hashCode() {
        return 0;
    }
};

ANDROID_END

#endif
