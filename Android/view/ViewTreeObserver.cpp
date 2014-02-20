//
//  ViewTreeObserver.cpp
//  Androidpp
//
//  Created by Saul Howard on 1/14/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#include "ViewTreeObserver.h"

ANDROID_BEGIN

int ViewTreeObserver::InternalInsetsInfo::hashCode() {
    int result = contentInsets.hashCode();
    result = 31 * result + visibleInsets.hashCode();
    result = 31 * result + touchableRegion.hashCode();
    result = 31 * result + mTouchableInsets;
    return result;
}

bool ViewTreeObserver::InternalInsetsInfo::equals(InternalInsetsInfo *other) {
    if (this == other) return true;
    if (other == NULL) return false;
    
    return mTouchableInsets == other->mTouchableInsets &&
    contentInsets == other->contentInsets &&
    visibleInsets == other->visibleInsets &&
    touchableRegion == other->touchableRegion;
}

void ViewTreeObserver::InternalInsetsInfo::reset() {
    contentInsets.MakeEmpty();
    visibleInsets.MakeEmpty();
    touchableRegion.setEmpty();
    mTouchableInsets = TOUCHABLE_INSETS_FRAME;
}

void ViewTreeObserver::InternalInsetsInfo::set(InternalInsetsInfo other) {
    contentInsets.set(other.contentInsets);
    visibleInsets.set(other.visibleInsets);
    touchableRegion.set(other.touchableRegion);
    mTouchableInsets = other.mTouchableInsets;
}

void ViewTreeObserver::merge(ViewTreeObserver *observer) {
    if (!observer->mOnWindowAttachListeners.empty()) {
        if (!mOnWindowAttachListeners.empty()) {
            mOnWindowAttachListeners.resize(mOnWindowAttachListeners.size() + observer->mOnWindowAttachListeners.size());
            mOnWindowAttachListeners.insert(mOnWindowAttachListeners.end(), observer->mOnWindowAttachListeners.begin(), observer->mOnWindowAttachListeners.end());
        } else {
            mOnWindowAttachListeners = observer->mOnWindowAttachListeners;
        }
    }
    
    if (!observer->mOnWindowFocusListeners.empty()) {
        if (!mOnWindowFocusListeners.empty()) {
            mOnWindowFocusListeners.resize(mOnWindowFocusListeners.size() + observer->mOnWindowFocusListeners.size());
            mOnWindowFocusListeners.insert(mOnWindowFocusListeners.end(), observer->mOnWindowFocusListeners.begin(), observer->mOnWindowFocusListeners.end());
        } else {
            mOnWindowFocusListeners = observer->mOnWindowFocusListeners;
        }
    }
    
    if (!observer->mOnGlobalFocusListeners.empty()) {
        if (!mOnGlobalFocusListeners.empty()) {
            mOnGlobalFocusListeners.resize(mOnGlobalFocusListeners.size() + observer->mOnGlobalFocusListeners.size());
            mOnGlobalFocusListeners.insert(mOnGlobalFocusListeners.end(), observer->mOnGlobalFocusListeners.begin(), observer->mOnGlobalFocusListeners.end());
        } else {
            mOnGlobalFocusListeners = observer->mOnGlobalFocusListeners;
        }
    }
    
    if (!observer->mOnGlobalLayoutListeners.empty()) {
        if (!mOnGlobalLayoutListeners.empty()) {
            mOnGlobalLayoutListeners.resize(mOnGlobalLayoutListeners.size() + observer->mOnGlobalLayoutListeners.size());
            mOnGlobalLayoutListeners.insert(mOnGlobalLayoutListeners.end(), observer->mOnGlobalLayoutListeners.begin(), observer->mOnGlobalLayoutListeners.end());
        } else {
            mOnGlobalLayoutListeners = observer->mOnGlobalLayoutListeners;
        }
    }
    
    if (!observer->mOnPreDrawListeners.empty()) {
        if (!mOnPreDrawListeners.empty()) {
            mOnPreDrawListeners.resize(mOnPreDrawListeners.size() + observer->mOnPreDrawListeners.size());
            mOnPreDrawListeners.insert(mOnPreDrawListeners.end(), observer->mOnPreDrawListeners.begin(), observer->mOnPreDrawListeners.end());
        } else {
            mOnPreDrawListeners = observer->mOnPreDrawListeners;
        }
    }
    
    if (!observer->mOnTouchModeChangeListeners.empty()) {
        if (!mOnTouchModeChangeListeners.empty()) {
            mOnTouchModeChangeListeners.resize(mOnTouchModeChangeListeners.size() + observer->mOnTouchModeChangeListeners.size());
            mOnTouchModeChangeListeners.insert(mOnTouchModeChangeListeners.end(), observer->mOnTouchModeChangeListeners.begin(), observer->mOnTouchModeChangeListeners.end());
        } else {
            mOnTouchModeChangeListeners = observer->mOnTouchModeChangeListeners;
        }
    }
    
    if (!observer->mOnComputeInternalInsetsListeners.empty()) {
        if (!mOnComputeInternalInsetsListeners.empty()) {
            mOnComputeInternalInsetsListeners.resize(mOnComputeInternalInsetsListeners.size() + observer->mOnComputeInternalInsetsListeners.size());
            mOnComputeInternalInsetsListeners.insert(mOnComputeInternalInsetsListeners.end(), observer->mOnComputeInternalInsetsListeners.begin(), observer->mOnComputeInternalInsetsListeners.end());
        } else {
            mOnComputeInternalInsetsListeners = observer->mOnComputeInternalInsetsListeners;
        }
    }
    
    if (!observer->mOnScrollChangedListeners.empty()) {
        if (!mOnScrollChangedListeners.empty()) {
            mOnScrollChangedListeners.resize(mOnScrollChangedListeners.size() + observer->mOnScrollChangedListeners.size());
            mOnScrollChangedListeners.insert(mOnScrollChangedListeners.end(), observer->mOnScrollChangedListeners.begin(), observer->mOnScrollChangedListeners.end());
        } else {
            mOnScrollChangedListeners = observer->mOnScrollChangedListeners;
        }
    }
    
    observer->kill();
}

void ViewTreeObserver::addOnWindowAttachListener(OnWindowAttachListener *listener) {
    checkIsAlive();
    
    if (mOnWindowAttachListeners.empty()) {
        mOnWindowAttachListeners = vector<OnWindowAttachListener*>();
    }
    
    mOnWindowAttachListeners.push_back(listener);
}

void ViewTreeObserver::removeOnWindowAttachListener(OnWindowAttachListener *victim) {
    checkIsAlive();
    if (mOnWindowAttachListeners.empty()) {
        return;
    }
    mOnWindowAttachListeners.erase(remove(mOnWindowAttachListeners.begin(), mOnWindowAttachListeners.end(), victim), mOnWindowAttachListeners.end());
}

void ViewTreeObserver::addOnWindowFocusChangeListener(OnWindowFocusChangeListener *listener) {
    checkIsAlive();
    
    if (mOnWindowFocusListeners.empty()) {
        mOnWindowFocusListeners = vector<OnWindowFocusChangeListener*>();
    }
    
    mOnWindowFocusListeners.push_back(listener);
}

void ViewTreeObserver::removeOnWindowFocusChangeListener(OnWindowFocusChangeListener *victim) {
    checkIsAlive();
    if (mOnWindowFocusListeners.empty()) {
        return;
    }
    mOnWindowFocusListeners.erase(remove(mOnWindowFocusListeners.begin(), mOnWindowFocusListeners.end(), victim), mOnWindowFocusListeners.end());
}

void ViewTreeObserver::addOnGlobalFocusChangeListener(OnGlobalFocusChangeListener *listener) {
    checkIsAlive();
    
    if (mOnGlobalFocusListeners.empty()) {
        mOnGlobalFocusListeners = vector<OnGlobalFocusChangeListener*>();
    }
    
    mOnGlobalFocusListeners.push_back(listener);
}

void ViewTreeObserver::removeOnGlobalFocusChangeListener(OnGlobalFocusChangeListener *victim) {
    checkIsAlive();
    if (mOnGlobalFocusListeners.empty()) {
        return;
    }
    
    mOnGlobalFocusListeners.erase(remove(mOnGlobalFocusListeners.begin(), mOnGlobalFocusListeners.end(), victim), mOnGlobalFocusListeners.end());
}

void ViewTreeObserver::addOnGlobalLayoutListener(OnGlobalLayoutListener *listener) {
    checkIsAlive();
    
    if (mOnGlobalLayoutListeners.empty()) {
        mOnGlobalLayoutListeners = vector<OnGlobalLayoutListener*>();
    }
    
    mOnGlobalLayoutListeners.push_back(listener);
}

void ViewTreeObserver::removeOnGlobalLayoutListener(OnGlobalLayoutListener *victim) {
    checkIsAlive();
    if (mOnGlobalLayoutListeners.empty()) {
        return;
    }
    
    mOnGlobalLayoutListeners.erase(remove(mOnGlobalLayoutListeners.begin(), mOnGlobalLayoutListeners.end(), victim), mOnGlobalLayoutListeners.end());
}

void ViewTreeObserver::addOnPreDrawListener(OnPreDrawListener *listener) {
    checkIsAlive();
    
    if (mOnPreDrawListeners.empty()) {
        mOnPreDrawListeners = vector<OnPreDrawListener*>();
    }
    
    mOnPreDrawListeners.push_back(listener);
}

void ViewTreeObserver::removeOnPreDrawListener(OnPreDrawListener *victim) {
    checkIsAlive();
    if (mOnPreDrawListeners.empty()) {
        return;
    }
    
    mOnPreDrawListeners.erase(remove(mOnPreDrawListeners.begin(), mOnPreDrawListeners.end(), victim), mOnPreDrawListeners.end());
}

void ViewTreeObserver::addOnDrawListener(OnDrawListener *listener) {
    checkIsAlive();
    
    if (mOnDrawListeners.empty()) {
        mOnDrawListeners = vector<OnDrawListener*>();
    }
    
    mOnDrawListeners.push_back(listener);
}

void ViewTreeObserver::removeOnDrawListener(OnDrawListener *victim) {
    checkIsAlive();
    if (mOnDrawListeners.empty()) {
        return;
    }
    
    mOnDrawListeners.erase(remove(mOnDrawListeners.begin(), mOnDrawListeners.end(), victim), mOnDrawListeners.end());
}

void ViewTreeObserver::addOnScrollChangedListener(OnScrollChangedListener *listener) {
    checkIsAlive();
    
    if (mOnScrollChangedListeners.empty()) {
        mOnScrollChangedListeners = vector<OnScrollChangedListener*>();
    }
    
    mOnScrollChangedListeners.push_back(listener);
}

void ViewTreeObserver::removeOnScrollChangedListener(OnScrollChangedListener *victim) {
    checkIsAlive();
    if (mOnScrollChangedListeners.empty()) {
        return;
    }
    
    mOnScrollChangedListeners.erase(remove(mOnScrollChangedListeners.begin(), mOnScrollChangedListeners.end(), victim), mOnScrollChangedListeners.end());
}

void ViewTreeObserver::addOnTouchModeChangeListener(OnTouchModeChangeListener *listener) {
    checkIsAlive();
    
    if (mOnTouchModeChangeListeners.empty()) {
        mOnTouchModeChangeListeners = vector<OnTouchModeChangeListener*>();
    }
    
    mOnTouchModeChangeListeners.push_back(listener);
}

void ViewTreeObserver::removeOnTouchModeChangeListener(OnTouchModeChangeListener *victim) {
    checkIsAlive();
    if (mOnTouchModeChangeListeners.empty()) {
        return;
    }
    
    mOnTouchModeChangeListeners.erase(remove(mOnTouchModeChangeListeners.begin(), mOnTouchModeChangeListeners.end(), victim), mOnTouchModeChangeListeners.end());
}

void ViewTreeObserver::addOnComputeInternalInsetsListener(OnComputeInternalInsetsListener *listener) {
    checkIsAlive();
    
    if (mOnComputeInternalInsetsListeners.empty()) {
        mOnComputeInternalInsetsListeners = vector<OnComputeInternalInsetsListener*>();
    }
    
    mOnComputeInternalInsetsListeners.push_back(listener);
}

void ViewTreeObserver::removeOnComputeInternalInsetsListener(OnComputeInternalInsetsListener *victim) {
    checkIsAlive();
    if (mOnComputeInternalInsetsListeners.empty()) {
        return;
    }
    
    mOnComputeInternalInsetsListeners.erase(remove(mOnComputeInternalInsetsListeners.begin(), mOnComputeInternalInsetsListeners.end(), victim), mOnComputeInternalInsetsListeners.end());
}

void ViewTreeObserver::checkIsAlive() {
    if (!mAlive) {
        throw IllegalStateException("This ViewTreeObserver is not alive, call getViewTreeObserver() again");
    }
}

void ViewTreeObserver::dispatchOnWindowAttachedChange(bool attached) {
    // NOTE: because of the use of vector, we *must* use an iterator to
    // perform the dispatching. The iterator is a safe guard against listeners that
    // could mutate the list by calling the various add/remove methods. This prevents
    // the array from being modified while we iterate it.
    const vector<OnWindowAttachListener*> &listeners = mOnWindowAttachListeners;
    if (listeners.size() > 0) {
        typedef vector<OnWindowAttachListener*>::const_iterator iter;
        for (iter it = listeners.begin(); it != listeners.end(); ++it) {
            OnWindowAttachListener *listener = *it;
            if (attached) listener->onWindowAttached();
            else listener->onWindowDetached();
        }
    }
}

void ViewTreeObserver::dispatchOnWindowFocusChange(bool hasFocus) {
    // NOTE: because of the use of vector, we *must* use an iterator to
    // perform the dispatching. The iterator is a safe guard against listeners that
    // could mutate the list by calling the various add/remove methods. This prevents
    // the array from being modified while we iterate it.
    const vector<OnWindowFocusChangeListener*> &listeners
    = mOnWindowFocusListeners;
    if (listeners.size() > 0) {
        typedef vector<OnWindowFocusChangeListener*>::const_iterator iter;
        for (iter it = listeners.begin(); it != listeners.end(); ++it) {
            OnWindowFocusChangeListener *listener = *it;
            listener->onWindowFocusChanged(hasFocus);
        }
    }
}

void ViewTreeObserver::dispatchOnGlobalFocusChange(shared_ptr<View> oldFocus, shared_ptr<View> newFocus) {
    // NOTE: because of the use of vector, we *must* use an iterator to
    // perform the dispatching. The iterator is a safe guard against listeners that
    // could mutate the list by calling the various add/remove methods. This prevents
    // the array from being modified while we iterate it.
    const vector<OnGlobalFocusChangeListener*> &listeners = mOnGlobalFocusListeners;
    if (listeners.size() > 0) {
        typedef vector<OnGlobalFocusChangeListener*>::const_iterator iter;
        for (iter it = listeners.begin(); it != listeners.end(); ++it) {
            OnGlobalFocusChangeListener *listener = *it;
            listener->onGlobalFocusChanged(oldFocus, newFocus);
        }
    }
}

void ViewTreeObserver::dispatchOnGlobalLayout() {
    // NOTE: because of the use of vector, we *must* use an iterator to
    // perform the dispatching. The iterator is a safe guard against listeners that
    // could mutate the list by calling the various add/remove methods. This prevents
    // the array from being modified while we iterate it.
    const vector<OnGlobalLayoutListener*> &listeners = mOnGlobalLayoutListeners;
    if (listeners.size() > 0) {
        typedef vector<OnGlobalLayoutListener*>::const_iterator iter;
        for (iter it = listeners.begin(); it != listeners.end(); ++it) {
            OnGlobalLayoutListener *listener = *it;
            listener->onGlobalLayout();
        }
    }
}

bool ViewTreeObserver::dispatchOnPreDraw() {
    bool cancelDraw = false;
    const vector<OnPreDrawListener*> &listeners = mOnPreDrawListeners;
    if (listeners.size() > 0) {
        typedef vector<OnPreDrawListener*>::const_iterator iter;
        for (iter it = listeners.begin(); it != listeners.end(); ++it) {
            OnPreDrawListener *listener = *it;
            listener->onPreDraw();
        }
    }
    return cancelDraw;
}

void ViewTreeObserver::dispatchOnDraw() {
    const vector<OnDrawListener*> &listeners = mOnDrawListeners;
    int numListeners = listeners.size();
    for (int i = 0; i < numListeners; ++i) {
        listeners[i]->onDraw();
    }
}

void ViewTreeObserver::dispatchOnTouchModeChanged(bool inTouchMode) {
    const vector<OnTouchModeChangeListener*> &listeners =
    mOnTouchModeChangeListeners;
    if (listeners.size() > 0) {
        typedef vector<OnTouchModeChangeListener*>::const_iterator iter;
        for (iter it = listeners.begin(); it != listeners.end(); ++it) {
            OnTouchModeChangeListener *listener = *it;
            listener->onTouchModeChanged(inTouchMode);
        }
    }
}

void ViewTreeObserver::dispatchOnScrollChanged() {
    // NOTE: because of the use of vector, we *must* use an iterator to
    // perform the dispatching. The iterator is a safe guard against listeners that
    // could mutate the list by calling the various add/remove methods. This prevents
    // the array from being modified while we iterate it.
    const vector<OnScrollChangedListener*> &listeners = mOnScrollChangedListeners;
    if (listeners.size() > 0) {
        typedef vector<OnScrollChangedListener*>::const_iterator iter;
        for (iter it = listeners.begin(); it != listeners.end(); ++it) {
            OnScrollChangedListener *listener = *it;
            listener->onScrollChanged();
        }
    }
}

bool ViewTreeObserver::hasComputeInternalInsetsListeners() {
    const vector<OnComputeInternalInsetsListener*> &listeners =
    mOnComputeInternalInsetsListeners;
    return (listeners.size() > 0);
}

void ViewTreeObserver::dispatchOnComputeInternalInsets(InternalInsetsInfo inoutInfo) {
    // NOTE: because of the use of vector, we *must* use an iterator to
    // perform the dispatching. The iterator is a safe guard against listeners that
    // could mutate the list by calling the various add/remove methods. This prevents
    // the array from being modified while we iterate it.
    const vector<OnComputeInternalInsetsListener*> &listeners =
    mOnComputeInternalInsetsListeners;
    if (listeners.size() > 0) {
        typedef vector<OnComputeInternalInsetsListener*>::const_iterator iter;
        for (iter it = listeners.begin(); it != listeners.end(); ++it) {
            OnComputeInternalInsetsListener *listener = *it;
            listener->onComputeInternalInsets(inoutInfo);
        }
    }
}

ANDROID_END