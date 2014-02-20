//
//  TextDirectionHeuristic.h
//  Androidpp
//
//  Created by Saul Howard on 1/20/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef Androidpp_TextDirectionHeuristic_h
#define Androidpp_TextDirectionHeuristic_h

#include "AndroidMacros.h"

#include <unicode/unistr.h>

#include <memory>

using namespace icu;
using namespace std;

ANDROID_BEGIN

class CharSequence;

/**
 * Interface for objects that use a heuristic for guessing at the paragraph direction by examining text.
 */
class TextDirectionHeuristic {
    
public:
    
    /**
     * Guess if a chars array is in the RTL direction or not.
     *
     * @param array the char array.
     * @param start start index, inclusive.
     * @param count the length to check, must not be negative and not greater than
     *          {@code array.length - start}.
     * @return true if all chars in the range are to be considered in a RTL direction,
     *          false otherwise.
     */
    virtual bool isRtl(UnicodeString array, int start, int count) = 0;
    
    /**
     * Guess if a {@code CharSequence} is in the RTL direction or not.
     *
     * @param cs the CharSequence.
     * @param start start index, inclusive.
     * @param count the length to check, must not be negative and not greater than
     *            {@code CharSequence.length() - start}.
     * @return true if all chars in the range are to be considered in a RTL direction,
     *          false otherwise.
     */
    virtual bool isRtl(shared_ptr<CharSequence> cs, int start, int count) = 0;
};

ANDROID_END

#endif
