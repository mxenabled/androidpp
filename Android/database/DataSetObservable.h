//
//  DataSetObservable.h
//  cocos2dx
//
//  Created by Saul Howard on 12/11/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef cocos2dx_DataSetObservable_h
#define cocos2dx_DataSetObservable_h

#include "AndroidMacros.h"

#include "Android/database/Observable.h"
#include "Android/database/DataSetObserver.h"

#include <mutex>

ANDROID_BEGIN

class DataSetObservable : public Observable<DataSetObserver> {
    
private:
    mutex _mutex;
    
public:
    /**
     * Invokes {@link DataSetObserver#onChanged} on each observer.
     * Called when the contents of the data set have changed.  The recipient
     * will obtain the new contents the next time it queries the data set.
     */
    void notifyChanged() {
        unique_lock<mutex> lock(_mutex);
        // since onChanged() is implemented by the app, it could do anything, including
        // removing itself from {@link mObservers} - and that could cause problems if
        // an iterator is used on the ArrayList {@link mObservers}.
        // to avoid such problems, just march thru the list in the reverse order.
        for (int i = mObservers.size() - 1; i >= 0; i--) {
            mObservers[i]->onChanged();
        }
    }
    
    /**
     * Invokes {@link DataSetObserver#onInvalidated} on each observer.
     * Called when the data set is no longer valid and cannot be queried again,
     * such as when the data set has been closed.
     */
    void notifyInvalidated() {
        unique_lock<mutex> lock(_mutex);
        for (int i = mObservers.size() - 1; i >= 0; i--) {
            mObservers[i]->onInvalidated();
        }
    }
};

ANDROID_END

#endif
