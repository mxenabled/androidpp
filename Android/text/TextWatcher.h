//
//  TextWatcher.h
//  Androidpp
//
//  Created by Saul Howard on 1/16/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef Androidpp_TextWatcher_h
#define Androidpp_TextWatcher_h

#include "AndroidMacros.h"

#include "Android/text/NoCopySpan.h"
#include "Android/utils/Object.h"

#include <memory>

ANDROID_BEGIN

class CharSequence;
class Editable;

/**
 * When an object of a type is attached to an Editable, its methods will
 * be called when the text is changed.
 */
class TextWatcher : public virtual NoCopySpan, public virtual Object {
    
public:
    
    /**
     * This method is called to notify you that, within <code>s</code>,
     * the <code>count</code> characters beginning at <code>start</code>
     * are about to be replaced by new text with length <code>after</code>.
     * It is an error to attempt to make changes to <code>s</code> from
     * this callback.
     */
    virtual void beforeTextChanged(shared_ptr<CharSequence> s, int start,
                                  int count, int after) = 0;
    /**
     * This method is called to notify you that, within <code>s</code>,
     * the <code>count</code> characters beginning at <code>start</code>
     * have just replaced old text that had length <code>before</code>.
     * It is an error to attempt to make changes to <code>s</code> from
     * this callback.
     */
    virtual void onTextChanged(shared_ptr<CharSequence> s, int start, int before, int count) = 0;
    
    /**
     * This method is called to notify you that, somewhere within
     * <code>s</code>, the text has been changed.
     * It is legitimate to make further changes to <code>s</code> from
     * this callback, but be careful not to get yourself into an infinite
     * loop, because any changes you make will cause this method to be
     * called again recursively.
     * (You are not told where the change took place because other
     * afterTextChanged() methods may already have made other changes
     * and invalidated the offsets.  But if you need to know here,
     * you can use {@link Spannable#setSpan} in {@link #onTextChanged}
     * to mark your place and then look up from here where the span
     * ended up.
     */
    virtual void afterTextChanged(shared_ptr<Editable> s) = 0;
};

ANDROID_END

#endif
