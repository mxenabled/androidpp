//
//  System.h
//  Androidpp
//
//  Created by Saul Howard on 1/15/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef Androidpp_System_h
#define Androidpp_System_h

#include "AndroidMacros.h"

#include <vector>

#include <string>
#include <locale>
#include <iostream>
#include <algorithm>

#include <unicode/unistr.h>

using namespace icu;
using namespace std;

ANDROID_BEGIN

class System {
    
public:
    
    template<class T>
    static void arraycopy(vector<T> &src, int srcPos, vector<T> &dst, int dstPos, int length) {
        typedef typename vector<T>::iterator Iter;
        Iter srcIter = src.begin() + srcPos;
        Iter dstIter = dst.begin() + dstPos;
        
        for (int count = 0; count < length; count++) {
            dst.insert(dstIter, *srcIter);
            ++dstIter;
            ++srcIter;
        }
    }
    
    static string convert(UnicodeString value) {
        
        char buf[value.length()];
        value.extract(0, value.length(), &buf[0]);
        
        return string(buf);
    }
    
    static UnicodeString convertChars(const char* value, int32_t length) {
        UnicodeString val = UnicodeString::fromUTF8(StringPiece(value));
        return val;
    }
    
    static UnicodeString convert(string value) {
        return convertChars(value.c_str(), value.length());
    }
    
    // templated version of my_equal so it could work with both char and wchar_t
    template<typename charT>
    struct caseInsensitive {
        caseInsensitive(const locale& loc) : loc_(loc) {}
        bool operator()(charT ch1, charT ch2) {
            return toupper(ch1, loc_) == toupper(ch2, loc_);
        }
    private:
        const locale& loc_;
    };
    
    // find substring (case insensitive)
    template<typename T>
    static int find(const T& str1, const T& str2, const locale& loc = locale()) {
        typename T::const_iterator it = search(str1.begin(), str1.end(), str2.begin(), str2.end(), caseInsensitive<typename T::value_type>(loc));
        if (it != str1.end())
            return it - str1.begin();
        else
            return -1; // not found
    }
};

ANDROID_END

#endif
