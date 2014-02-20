//
//  Observable.h
//  cocos2dx
//
//  Created by Saul Howard on 12/11/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef cocos2dx_Observable_h
#define cocos2dx_Observable_h

#include "AndroidMacros.h"

#include <vector>
#include <algorithm>
#include <mutex>

using namespace std;

ANDROID_BEGIN

template<class T>
class Observable {
    
private:
    mutex _mutex;
    
protected:
    
    /**
     * The list of observers.  An observer can be in the list at most
     * once and will never be null.
     */
    vector<T*> mObservers;

public:
    
    /**
     * Adds an observer to the list. The observer cannot be null and it must not already
     * be registered.
     * @param observer the observer to register
     * @throws IllegalArgumentException the observer is null
     * @throws IllegalStateException the observer is already registered
     */
    void registerObserver(T *observer) {
        if (observer == NULL) {
            return;
        }
        unique_lock<mutex> lock(_mutex);
        
        if (find(mObservers.begin(), mObservers.end(), observer) != mObservers.end()) {
            return;
        }
        mObservers.push_back(observer);
    }
    
    /**
     * Removes a previously registered observer. The observer must not be null and it
     * must already have been registered.
     * @param observer the observer to unregister
     * @throws IllegalArgumentException the observer is null
     * @throws IllegalStateException the observer is not yet registered
     */
    void unregisterObserver(T *observer) {
        
        if (observer == NULL) {
            return;
        }
        
        unique_lock<mutex> lock(_mutex);
        
        typename vector<T*>::iterator it;
        it = find(mObservers.begin(), mObservers.end(), observer);
        if (it == mObservers.end()) {
            return;
        }
        
        mObservers.erase(it);
    }
    
    /**
     * Remove all registered observers.
     */
    void unregisterAll() {
        unique_lock<mutex> lock(_mutex);
        mObservers.clear();
    }
};

ANDROID_END

#endif
