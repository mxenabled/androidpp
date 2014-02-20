//
//  GetChars.h
//  Androidpp
//
//  Created by Saul Howard on 1/15/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef Androidpp_GetChars_h
#define Androidpp_GetChars_h

#include "AndroidMacros.h"
#include "Android/text/CharSequence.h"

#include <unicode/unistr.h>

using namespace icu;
using namespace std;

ANDROID_BEGIN

/**
 * Please implement this interface if your CharSequence has a
 * getChars() method like the one in String that is faster than
 * calling charAt() multiple times.
 */
class GetChars : public virtual CharSequence
{
    
public:
    
    /**
     * Exactly like String.getChars(): copy chars <code>start</code>
     * through <code>end - 1</code> from this CharSequence into <code>dest</code>
     * beginning at offset <code>destoff</code>.
     */
    virtual void getChars(int start, int end, UnicodeString &dest, int destoff) = 0;
};

ANDROID_END

#endif
