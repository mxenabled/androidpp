//
//  SingleLineTransformationMethod.h
//  Androidpp
//
//  Created by Saul Howard on 1/31/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef Androidpp_SingleLineTransformationMethod_h
#define Androidpp_SingleLineTransformationMethod_h

#include "AndroidMacros.h"

#include "Android/text/method/ReplacementTransformationMethod.h"

#include <unicode/unistr.h>

#include <memory>

using namespace icu;
using namespace std;

ANDROID_BEGIN

/**
 * This transformation method causes any newline characters (\n) to be
 * displayed as spaces instead of causing line breaks, and causes
 * carriage return characters (\r) to have no appearance.
 */
class SingleLineTransformationMethod : public ReplacementTransformationMethod {
    
private:
    
    static UnicodeString ORIGINAL;
    static UnicodeString REPLACEMENT;
    
protected:
    
    /**
     * The characters to be replaced are \n and \r.
     */
    UnicodeString getOriginal() {
        return ORIGINAL;
    }
    
    /**
     * The character \n is replaced with is space;
     * the character \r is replaced with is FEFF (zero width space).
     */
    UnicodeString getReplacement() {
        return REPLACEMENT;
    }
    
public:
    
    static shared_ptr<SingleLineTransformationMethod> getInstance() {
        if (sInstance != NULL)
            return sInstance;
        
        sInstance = make_shared<SingleLineTransformationMethod>();
        return sInstance;
    }
    
private:
    
    static shared_ptr<SingleLineTransformationMethod> sInstance;
};

UnicodeString SingleLineTransformationMethod::ORIGINAL = UNICODE_STRING_SIMPLE("\n\r");
UnicodeString SingleLineTransformationMethod::REPLACEMENT = UNICODE_STRING_SIMPLE(" \uFEFF");

shared_ptr<SingleLineTransformationMethod> SingleLineTransformationMethod::sInstance;

ANDROID_END

#endif
