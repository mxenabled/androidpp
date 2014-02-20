//
//  SpanWatcher.h
//  Androidpp
//
//  Created by Saul Howard on 1/15/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef Androidpp_SpanWatcher_h
#define Androidpp_SpanWatcher_h

#include "AndroidMacros.h"

#include "Android/text/NoCopySpan.h"
#include "Android/utils/Object.h"

#include <memory>

using namespace std;

ANDROID_BEGIN

class Spannable;

/**
 * When an object of this type is attached to a Spannable, its methods
 * will be called to notify it that other markup objects have been
 * added, changed, or removed.
 */
class SpanWatcher : public virtual NoCopySpan, public virtual Object {
    
public:
    
    /**
     * This method is called to notify you that the specified object
     * has been attached to the specified range of the text.
     */
    virtual void onSpanAdded(shared_ptr<Spannable> text, shared_ptr<Object> what, int start, int end) = 0;
    /**
     * This method is called to notify you that the specified object
     * has been detached from the specified range of the text.
     */
    virtual void onSpanRemoved(shared_ptr<Spannable> text, shared_ptr<Object> what, int start, int end) = 0;
    /**
     * This method is called to notify you that the specified object
     * has been relocated from the range <code>ostart&hellip;oend</code>
     * to the new range <code>nstart&hellip;nend</code> of the text.
     */
    virtual void onSpanChanged(shared_ptr<Spannable> text, shared_ptr<Object> what, int ostart, int oend,
                              int nstart, int nend) = 0;
};

ANDROID_END

#endif
