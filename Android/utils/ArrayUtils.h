//
//  ArrayUtils.h
//  Androidpp
//
//  Created by Saul Howard on 1/15/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef Androidpp_ArrayUtils_h
#define Androidpp_ArrayUtils_h

#include "AndroidMacros.h"

ANDROID_BEGIN

class ArrayUtils {
    
public:
    
    static int idealCharArraySize(int need) {
        return idealByteArraySize(need * 2) / 2;
    }
    
    static int idealIntArraySize(int need) {
        return idealByteArraySize(need * 4) / 4;
    }
    
    static int idealByteArraySize(int need) {
        for (int i = 4; i < 32; i++)
            if (need <= (1 << i) - 12)
                return (1 << i) - 12;
        
        return need;
    }
    
    static int idealObjectArraySize(int need) {
        return idealByteArraySize(need * 4) / 4;
    }
    
    static int idealFloatArraySize(int need) {
        return idealByteArraySize(need * 4) / 4;
    }
};

ANDROID_END

#endif
