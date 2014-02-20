//
//  DataSetObserver.h
//  cocos2dx
//
//  Created by Saul Howard on 12/11/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef cocos2dx_DataSetObserver_h
#define cocos2dx_DataSetObserver_h

#include "AndroidMacros.h"

ANDROID_BEGIN

class DataSetObserver {
    
public:
    void onChanged() {
        // Do nothing
    }
    
    /**
     * This method is called when the entire data becomes invalid,
     * most likely through a call to {@link Cursor#deactivate()} or {@link Cursor#close()} on a
     * {@link Cursor}.
     */
    void onInvalidated() {
        // Do nothing
    }
};

ANDROID_END

#endif
