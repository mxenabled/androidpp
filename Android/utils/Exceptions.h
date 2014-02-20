//
//  Exceptions.h
//  cocos2dx
//
//  Created by Saul Howard on 12/11/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef cocos2dx_Exceptions_h
#define cocos2dx_Exceptions_h

#include "AndroidMacros.h"

#include "cocos2d.h"

#include <unicode/unistr.h>

#include <sstream>
#include <string>
#include <exception>

using namespace icu;
using namespace std;

ANDROID_BEGIN

struct Exception : public exception {
    
    string s;
    
    Exception(string ss) : s(ss) {
        CCLOG("Exception Thrown: %s", ss.c_str());
    }
    Exception() throw () {
        CCLOG("Exception Thrown");
    }
    
    const char* what() const throw() { return s.c_str(); }
};

struct ArrayIndexOutOfBoundsException : public Exception {
    ArrayIndexOutOfBoundsException() : Exception() {}
};

struct NullPointerException : public Exception {
    NullPointerException() : Exception() {}
    NullPointerException(string ss) : Exception(ss) {}
};

struct IllegalArgumentException : public Exception {
    IllegalArgumentException() : Exception() {}
    IllegalArgumentException(string ss) : Exception(ss) {}
};

struct IllegalStateException : public Exception {
    IllegalStateException(string ss) : Exception(ss) {}
};

struct CalledFromWrongThreadException : public Exception {
    CalledFromWrongThreadException(string ss) : Exception(ss) {}
};

struct RuntimeException : public Exception {
    RuntimeException(string ss) : Exception(ss) {}
};

struct IndexOutOfBoundsException : public Exception {
    IndexOutOfBoundsException() : Exception() {}
    IndexOutOfBoundsException(string ss) : Exception(ss) {}
};

struct CCPullParserException : public Exception {
    CCPullParserException(string ss) : Exception(ss) {}
};

struct UnsupportedOperationException : public Exception {
    UnsupportedOperationException(string ss) : Exception(ss) {}
};

struct StringIndexOutOfBoundsException : public Exception {
    StringIndexOutOfBoundsException() : Exception(createMessage(0)) {}
    StringIndexOutOfBoundsException(int index) : Exception(createMessage(index)) {}
    
    string createMessage(int index) {
        std::stringstream ss;
        ss << "Index: ";
        ss << index;
        ss << " Out of Bounds";
        
        return ss.str();
    }
};

struct NumberFormatException : public Exception {
    NumberFormatException(string ss) : Exception(ss) {}
    
    static NumberFormatException forInputString(UnicodeString string) {
        return NumberFormatException("could not convert");
    }
};

ANDROID_END

#endif
