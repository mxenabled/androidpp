//
//  InputFilter.h
//  Androidpp
//
//  Created by Saul Howard on 1/15/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef __Androidpp__InputFilter__
#define __Androidpp__InputFilter__

#include "AndroidMacros.h"

#include "Android/text/CharSequence.h"
#include "Android/text/Spanned.h"

#include <memory>

using namespace std;

ANDROID_BEGIN

/**
 * InputFilters can be attached to {@link Editable}s to constrain the
 * changes that can be made to them.
 */
class InputFilter {
    
public:
    
    /**
     * This method is called when the buffer is going to replace the
     * range <code>dstart &hellip; dend</code> of <code>dest</code>
     * with the new text from the range <code>start &hellip; end</code>
     * of <code>source</code>.  Return the CharSequence that you would
     * like to have placed there instead, including an empty string
     * if appropriate, or <code>null</code> to accept the original
     * replacement.  Be careful to not to reject 0-length replacements,
     * as this is what happens when you delete text.  Also beware that
     * you should not attempt to make any changes to <code>dest</code>
     * from this method; you may only examine it for context.
     *
     * Note: If <var>source</var> is an instance of {@link Spanned} or
     * {@link Spannable}, the span objects in the <var>source</var> should be
     * copied into the filtered result (i.e. the non-null return value).
     * {@link TextUtils#copySpansFrom} can be used for convenience.
     */
    virtual shared_ptr<CharSequence> filter(shared_ptr<CharSequence> source, int start, int end,
                               shared_ptr<Spanned> dest, int dstart, int dend) = 0;
    
    /**
     * This filter will capitalize all the lower case letters that are added
     * through edits.
     */
    class AllCaps;
    
    /**
     * This filter will constrain edits not to make the length of the text
     * greater than the specified length.
     */
    class LengthFilter;
};

class InputFilter::AllCaps : public InputFilter {

public:
    
    shared_ptr<CharSequence> filter(shared_ptr<CharSequence> source, int start, int end,
                         shared_ptr<Spanned> dest, int dstart, int dend);
};

class InputFilter::LengthFilter : public InputFilter {
    
public:
    
    LengthFilter(int max) {
        mMax = max;
    }
    
    shared_ptr<CharSequence> filter(shared_ptr<CharSequence> source, int start, int end,
                         shared_ptr<Spanned> dest, int dstart, int dend);
    
private:
    
    int mMax;
};

ANDROID_END

#endif /* defined(__Androidpp__InputFilter__) */
