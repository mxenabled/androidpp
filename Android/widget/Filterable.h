//
//  Filterable.h
//  Androidpp
//
//  Created by Saul Howard on 2/11/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef Androidpp_Filterable_h
#define Androidpp_Filterable_h

#include "AndroidMacros.h"

#include <memory>

using namespace std;

ANDROID_BEGIN

class Filter;

/**
 * <p>Defines a filterable behavior. A filterable class can have its data
 * constrained by a filter. Filterable classes are usually
 * {@link android.widget.Adapter} implementations.</p>
 *
 * @see android.widget.Filter
 */
class Filterable {
    
public:
    
    /**
     * <p>Returns a filter that can be used to constrain data with a filtering
     * pattern.</p>
     *
     * <p>This method is usually implemented by {@link android.widget.Adapter}
     * classes.</p>
     *
     * @return a filter used to constrain data
     */
    virtual shared_ptr<Filter> getFilter() = 0;
};

ANDROID_END

#endif
