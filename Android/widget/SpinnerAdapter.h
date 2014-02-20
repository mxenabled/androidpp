//
//  SpinnerAdapter.h
//  cocos2dx
//
//  Created by Saul Howard on 12/11/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef cocos2dx_SpinnerAdapter_h
#define cocos2dx_SpinnerAdapter_h

#include "AndroidMacros.h"

#include "Android/widget/Adapter.h"

ANDROID_BEGIN

class SpinnerAdapter : virtual public Adapter {
    
public:
    
    /**
     * <p>Get a {@link android.view.View} that displays in the drop down popup
     * the data at the specified position in the data set.</p>
     *
     * @param position      index of the item whose view we want.
     * @param convertView   the old view to reuse, if possible. Note: You should
     *        check that this view is non-null and of an appropriate type before
     *        using. If it is not possible to convert this view to display the
     *        correct data, this method can create a new view.
     * @param parent the parent that this view will eventually be attached to
     * @return a {@link android.view.View} corresponding to the data at the
     *         specified position.
     */
    virtual shared_ptr<View> getDropDownView(int position, shared_ptr<View> convertView, ViewGroup *parent) = 0;
};

ANDROID_END

#endif
