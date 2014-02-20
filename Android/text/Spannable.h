//
//  Spannable.h
//  Androidpp
//
//  Created by Saul Howard on 1/15/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef Androidpp_Spannable_h
#define Androidpp_Spannable_h

#include "AndroidMacros.h"

#include "Android/text/CharSequence.h"
#include "Android/text/Spanned.h"
#include "Android/utils/Object.h"

#include <memory>

using namespace std;

ANDROID_BEGIN

/**
 * This is the interface for text to which markup objects can be
 * attached and detached.  Not all Spannable classes have mutable text;
 * see {@link Editable} for that.
 */
class Spannable : public virtual Spanned {
    
public:
    
    /**
     * Attach the specified markup object to the range <code>start&hellip;end</code>
     * of the text, or move the object to that range if it was already
     * attached elsewhere.  See {@link Spanned} for an explanation of
     * what the flags mean.  The object can be one that has meaning only
     * within your application, or it can be one that the text system will
     * use to affect text display or behavior.  Some noteworthy ones are
     * the subclasses of {@link android.text.style.CharacterStyle} and
     * {@link android.text.style.ParagraphStyle}, and
     * {@link android.text.TextWatcher} and
     * {@link android.text.SpanWatcher}.
     */
    virtual void setSpan(shared_ptr<Object> what, int start, int end, int flags) = 0;
    
    /**
     * Remove the specified object from the range of text to which it
     * was attached, if any.  It is OK to remove an object that was never
     * attached in the first place.
     */
    virtual void removeSpan(shared_ptr<Object> what) = 0;
    
    /**
     * Factory used by TextView to create new Spannables.  You can subclass
     * it to provide something other than SpannableString.
     */
    class Factory {
        
    private:
        
        static Spannable::Factory sInstance;
        
    public:
        
        /**
         * Returns the standard Spannable Factory.
         */
        static Spannable::Factory getInstance() {
            return sInstance;
        }
        
        /**
         * Returns a new SpannableString from the specified CharSequence.
         * You can override this to provide a different kind of Spannable.
         */
        shared_ptr<Spannable> newSpannable(shared_ptr<CharSequence> source);
    };
};

ANDROID_END

#endif
