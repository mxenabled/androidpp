//
//  TransformationMethod.h
//  Androidpp
//
//  Created by Saul Howard on 1/28/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef Androidpp_TransformationMethod_h
#define Androidpp_TransformationMethod_h

#include "AndroidMacros.h"

#include "Android/utils/Object.h"

#include <memory>

using namespace std;

ANDROID_BEGIN

class CharSequence;
class Rect;
class View;

/**
 * TextView uses TransformationMethods to do things like replacing the
 * characters of passwords with dots, or keeping the newline characters
 * from causing line breaks in single-line text fields.
 */
class TransformationMethod : public virtual Object {
    
public:
    
    /**
     * Returns a CharSequence that is a transformation of the source text --
     * for example, replacing each character with a dot in a password field.
     * Beware that the returned text must be exactly the same length as
     * the source text, and that if the source text is Editable, the returned
     * text must mirror it dynamically instead of doing a one-time copy.
     */
    virtual shared_ptr<CharSequence> getTransformation(shared_ptr<CharSequence> source, View *view) = 0;
    
    /**
     * This method is called when the TextView that uses this
     * TransformationMethod gains or loses focus.
     */
    virtual void onFocusChanged(View *view, shared_ptr<CharSequence> sourceText,
                               bool focused, int direction,
                               Rect previouslyFocusedRect) = 0;
};

ANDROID_END

#endif
