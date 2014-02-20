//
//  NoCopySpan.h
//  Androidpp
//
//  Created by Saul Howard on 1/15/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef Androidpp_NoCopySpan_h
#define Androidpp_NoCopySpan_h

#include "AndroidMacros.h"

ANDROID_BEGIN

/**
 * This interface should be added to a span object that should not be copied
 * into a new Spenned when performing a slice or copy operation on the original
 * Spanned it was placed in.
 */
class NoCopySpan {
    
public:
    
    /**
     * Convenience equivalent for when you would just want a new Object() for
     * a span but want it to be no-copy.  Use this instead.
     */
    class Concrete;
};

class NoCopySpan::Concrete : public NoCopySpan {
};

ANDROID_END

#endif
