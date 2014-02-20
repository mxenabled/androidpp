//
//  Pool.h
//  cocos2dx
//
//  Created by Saul Howard on 12/11/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef cocos2dx_Pool_h
#define cocos2dx_Pool_h

#include "AndroidMacros.h"

#include "Android/utils/Exceptions.h"

#include <vector>
#include <algorithm>
#include <mutex>

using namespace std;

ANDROID_BEGIN

class Pools {
    
public:
    /**
     * Interface for managing a pool of objects.
     *
     * @param <T> The pooled type.
     */
    template<class T>
    class Pool {
        
    public:
        
        Pool() {}
        
        /**
         * @return An instance from the pool if such, null otherwise.
         */
        virtual T *acquire() = 0;
        
        /**
         * Release an instance to the pool.
         *
         * @param instance The instance to release.
         * @return Whether the instance was put in the pool.
         *
         * @throws IllegalStateException If the instance is already in the pool.
         */
        virtual bool release(T *instance) = 0;
    };
    
    /**
     * Simple (non-synchronized) pool of objects.
     *
     * @param <T> The pooled type.
     */
    template<class T>
    class SimplePool : public Pool<T> {
        
    protected:
        
        size_t mPoolSize = 0;
        
        vector<T*> mPool;
        
    public:
        
        SimplePool() : mPool(0) {}
        
        /**
         * Creates a new instance.
         *
         * @param maxPoolSize The max pool size.
         *
         * @throws IllegalArgumentException If the max pool size is less than zero.
         */
        SimplePool(int maxPoolSize) : mPool(maxPoolSize) {
            if (maxPoolSize <= 0) {
                throw IllegalArgumentException("The max pool size must be > 0");
            }
        }
        
        virtual T *acquire() {
            if (mPoolSize > 0) {
                const int lastPooledIndex = mPoolSize - 1;
                T *instance = mPool[lastPooledIndex];
                mPool[lastPooledIndex] = NULL;
                mPoolSize--;
                return instance;
            }
            return NULL;
        }
        
        virtual bool release(T *instance) {
            if (isInPool(instance)) {
                throw IllegalStateException("Already in the pool!");
            }
            if (mPoolSize < mPool.size()) {
                mPool[mPoolSize] = instance;
                mPoolSize++;
                return true;
            }
            return false;
        }
        
        bool isInPool(T *instance) const {
            return find(mPool.begin(), mPool.end(), instance) != mPool.end();
        }
    };
    
    /**
     * Synchronized) pool of objects.
     *
     * @param <T> The pooled type.
     */
    template<class T>
    class SynchronizedPool : public SimplePool<T> {
    
    private:
        
        mutable mutex _mutex;
        
    public:
        
        SynchronizedPool() : _mutex() {}
        
        SynchronizedPool(const SynchronizedPool& other) : _mutex() {
            lock_guard<mutex> _lock(_mutex);
            SimplePool<T>::mPool = other.mPool;
            SimplePool<T>::mPoolSize = other.mPoolSize;
            
        }
        
        SynchronizedPool& operator=(const SynchronizedPool& other) {
            if (this != &other) {
                lock_guard<mutex> _lock(_mutex);
                SimplePool<T>::mPool = other.mPool;
                SimplePool<T>::mPoolSize = other.mPoolSize;
            }
            
            return *this;
        }
        
        /**
         * Creates a new instance.
         *
         * @param maxPoolSize The max pool size.
         *
         * @throws IllegalArgumentException If the max pool size is less than zero.
         */
        SynchronizedPool(int maxPoolSize) : SimplePool<T>(maxPoolSize) {
        }
        
        virtual T *acquire() {
            unique_lock<mutex> lock(_mutex);
            return SimplePool<T>::acquire();
        }
        
        virtual bool release(T *element) {
            unique_lock<mutex> lock(_mutex);
            return SimplePool<T>::release(element);
        }
    };
};

ANDROID_END

#endif
