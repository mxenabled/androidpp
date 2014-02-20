//
//  BaseAdapter.h
//  cocos2dx
//
//  Created by Saul Howard on 12/11/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef cocos2dx_BaseAdapter_h
#define cocos2dx_BaseAdapter_h

#include "AndroidMacros.h"

#include "Android/database/DataSetObservable.h"
#include "Android/widget/ListAdapter.h"
#include "Android/widget/SpinnerAdapter.h"

ANDROID_BEGIN

class DataSetObserver;

class BaseAdapter : public ListAdapter, public SpinnerAdapter {
  
private:
    DataSetObservable mDataSetObservable;
    
public:
    virtual bool hasStableIds() {
        return false;
    }
    
    virtual void registerDataSetObserver(DataSetObserver *observer) {
        mDataSetObservable.registerObserver(observer);
    }
    
    virtual void unregisterDataSetObserver(DataSetObserver *observer) {
        mDataSetObservable.unregisterObserver(observer);
    }
    
    /**
     * Notifies the attached observers that the underlying data has been changed
     * and any View reflecting the data set should refresh itself.
     */
    virtual void notifyDataSetChanged() {
        mDataSetObservable.notifyChanged();
    }
    
    /**
     * Notifies the attached observers that the underlying data is no longer valid
     * or available. Once invoked this adapter is no longer valid and should
     * not report further data set changes.
     */
    virtual void notifyDataSetInvalidated() {
        mDataSetObservable.notifyInvalidated();
    }
    
    virtual bool areAllItemsEnabled() {
        return true;
    }
    
    virtual bool isEnabled(int position) {
        return true;
    }
    
    virtual shared_ptr<View> getDropDownView(int position, shared_ptr<View> convertView, ViewGroup *parent) {
        return getView(position, convertView, parent);
    }
    
    virtual int getItemViewType(int position) {
        return 0;
    }
    
    virtual int getViewTypeCount() {
        return 1;
    }
    
    virtual bool isEmpty() {
        return getCount() == 0;
    }
};

ANDROID_END

#endif
