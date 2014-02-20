//
//  WrapperListAdapter.h
//  Androidpp
//
//  Created by Saul Howard on 2/11/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef Androidpp_WrapperListAdapter_h
#define Androidpp_WrapperListAdapter_h

#include "AndroidMacros.h"

#include "Android/widget/ListAdapter.h"

#include <memory>

using namespace std;

ANDROID_BEGIN

/**
 * List adapter that wraps another list adapter. The wrapped adapter can be retrieved
 * by calling {@link #getWrappedAdapter()}.
 *
 * @see ListView
 */
class WrapperListAdapter : public ListAdapter {
    
public:
    
    /**
     * Returns the adapter wrapped by this list adapter.
     *
     * @return The {@link android.widget.ListAdapter} wrapped by this adapter.
     */
    virtual shared_ptr<ListAdapter> getWrappedAdapter() = 0;
};

ANDROID_END

#endif
