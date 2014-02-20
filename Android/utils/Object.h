//
//  Object.h
//  Androidpp
//
//  Created by Saul Howard on 1/15/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef Androidpp_Object_h
#define Androidpp_Object_h

#include "AndroidMacros.h"

#include <string>

using namespace std;

ANDROID_BEGIN

class String;

class Object {
    
public:
    
    virtual int hashCode() const {
        return 0;
    }
    
    virtual String toString() const;
    
    virtual bool equals(const Object *object) const {
        if (this == object) {
            return true;
        }
        if (object == NULL) {
            return false;
        }
        return false;
    }
    
    virtual string getType() {
        return "Object";
    };
};

ANDROID_END

#endif
