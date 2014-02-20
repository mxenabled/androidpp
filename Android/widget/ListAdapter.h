//
//  ListAdapter.h
//  cocos2dx
//
//  Created by Saul Howard on 12/11/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef cocos2dx_ListAdapter_h
#define cocos2dx_ListAdapter_h

#include "AndroidMacros.h"

#include "Android/widget/Adapter.h"

ANDROID_BEGIN

class ListAdapter : virtual public Adapter {

public:
    
    /**
     * Indicates whether all the items in this adapter are enabled. If the
     * value returned by this method changes over time, there is no guarantee
     * it will take effect.  If true, it means all items are selectable and
     * clickable (there is no separator.)
     *
     * @return True if all items are enabled, false otherwise.
     *
     * @see #isEnabled(int)
     */
    virtual bool areAllItemsEnabled() = 0;
    
    /**
     * Returns true if the item at the specified position is not a separator.
     * (A separator is a non-selectable, non-clickable item).
     *
     * The result is unspecified if position is invalid. An {@link ArrayIndexOutOfBoundsException}
     * should be thrown in that case for fast failure.
     *
     * @param position Index of the item
     *
     * @return True if the item is not a separator
     *
     * @see #areAllItemsEnabled()
     */
    virtual bool isEnabled(int position) = 0;
};

ANDROID_END

#endif
