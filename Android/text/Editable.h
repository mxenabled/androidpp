//
//  Editable.h
//  Androidpp
//
//  Created by Saul Howard on 1/15/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef Androidpp_Editable_h
#define Androidpp_Editable_h

#include "AndroidMacros.h"

#include "Android/text/CharSequence.h"
#include "Android/text/GetChars.h"
#include "Android/text/Spannable.h"
#include "Android/text/Appendable.h"

#include <vector>
#include <memory>

using namespace std;

ANDROID_BEGIN

class InputFilter;

/**
 * This is the interface for text whose content and markup
 * can be changed (as opposed
 * to immutable text like Strings).  If you make a {@link DynamicLayout}
 * of an Editable, the layout will be reflowed as the text is changed.
 */
class Editable : public virtual CharSequence, public virtual GetChars, public virtual Spannable, public virtual Appendable
{
    
public:
    
    /**
     * Replaces the specified range (<code>st&hellip;en</code>) of text in this
     * Editable with a copy of the slice <code>start&hellip;end</code> from
     * <code>source</code>.  The destination slice may be empty, in which case
     * the operation is an insertion, or the source slice may be empty,
     * in which case the operation is a deletion.
     * <p>
     * Before the change is committed, each filter that was set with
     * {@link #setFilters} is given the opportunity to modify the
     * <code>source</code> text.
     * <p>
     * If <code>source</code>
     * is Spanned, the spans from it are preserved into the Editable.
     * Existing spans within the Editable that entirely cover the replaced
     * range are retained, but any that were strictly within the range
     * that was replaced are removed.  As a special case, the cursor
     * position is preserved even when the entire range where it is
     * located is replaced.
     * @return  a reference to this object.
     */
    virtual Editable &replace(int st, int en, shared_ptr<CharSequence> source, int start, int end) = 0;
    
    /**
     * Convenience for replace(st, en, text, 0, text.length())
     * @see #replace(int, int, CharSequence, int, int)
     */
    virtual Editable &replace(int st, int en, shared_ptr<CharSequence> text) = 0;
    
    /**
     * Convenience for replace(where, where, text, start, end)
     * @see #replace(int, int, CharSequence, int, int)
     */
    virtual Editable &insert(int where, shared_ptr<CharSequence> text, int start, int end) = 0;
    
    /**
     * Convenience for replace(where, where, text, 0, text.length());
     * @see #replace(int, int, CharSequence, int, int)
     */
    virtual Editable &insert(int where, shared_ptr<CharSequence> text) = 0;
    
    /**
     * Convenience for replace(st, en, "", 0, 0)
     * @see #replace(int, int, CharSequence, int, int)
     */
    virtual Editable &deletes(int st, int en) = 0;
    
    /**
     * Convenience for replace(length(), length(), text, 0, text.length())
     * @see #replace(int, int, CharSequence, int, int)
     */
    virtual Editable &append(shared_ptr<CharSequence> text) = 0;
    
    /**
     * Convenience for replace(length(), length(), text, start, end)
     * @see #replace(int, int, CharSequence, int, int)
     */
    virtual Editable &append(shared_ptr<CharSequence> text, int start, int end) = 0;
    
    /**
     * Convenience for append(String.valueOf(text)).
     * @see #replace(int, int, CharSequence, int, int)
     */
    virtual Editable &append(char text) = 0;
    
    /**
     * Convenience for replace(0, length(), "", 0, 0)
     * @see #replace(int, int, CharSequence, int, int)
     * Note that this clears the text, not the spans;
     * use {@link #clearSpans} if you need that.
     */
    virtual void clear() = 0;
    
    /**
     * Removes all spans from the Editable, as if by calling
     * {@link #removeSpan} on each of them.
     */
    virtual void clearSpans() = 0;
    
    /**
     * Sets the series of filters that will be called in succession
     * whenever the text of this Editable is changed, each of which has
     * the opportunity to limit or transform the text that is being inserted.
     */
    virtual void setFilters(vector<shared_ptr<InputFilter>> filters) = 0;
    
    /**
     * Returns the array of input filters that are currently applied
     * to changes to this Editable.
     */
    virtual vector<shared_ptr<InputFilter>> getFilters() = 0;
    
    /**
     * Factory used by TextView to create new Editables.  You can subclass
     * it to provide something other than SpannableStringBuilder.
     */
    class Factory {
    
    private:
        
        static Editable::Factory sInstance;
    
    public:
        
        /**
         * Returns the standard Editable Factory.
         */
        static Editable::Factory getInstance() {
            return sInstance;
        }
        
        /**
         * Returns a new SpannedStringBuilder from the specified
         * CharSequence.  You can override this to provide
         * a different kind of Spanned.
         */
        shared_ptr<Editable> newEditable(shared_ptr<CharSequence> source);
    };
};

ANDROID_END

#endif
