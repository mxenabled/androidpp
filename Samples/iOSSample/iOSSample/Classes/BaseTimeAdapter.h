//
//  BaseTimeAdapter.h
//  MoneyMobileX
//
//  Created by Saul Howard on 12/11/13.
//
//

#ifndef MoneyMobileX_BaseTimeAdapter_h
#define MoneyMobileX_BaseTimeAdapter_h

#include "AndroidMacros.h"

#include "Android/widget/BaseAdapter.h"

#include "TimeView.h"

#include <vector>
#include <algorithm>

using namespace std;

ANDROID_BEGIN

class DateTime;

template<class T>
class BaseTimeAdapter : public BaseAdapter {
    
private:
    
    typedef bool (*sortFunction)(T, T);
    
    vector<T> mObjects;
    sortFunction mComparator;
    
public:
    
    BaseTimeAdapter(vector<T> objects, sortFunction comparator) {
        mObjects = objects;
        mComparator = comparator;
        
        sort(mObjects.begin(), mObjects.end(), mComparator);
    }
    
    void setObjects(List<T> objects) {
        
        int previousSize = mObjects.size();
        mObjects = objects;
        
        if (previousSize != mObjects.size()) {
            notifyDataSetInvalidated();
        } else {
            notifyDataSetChanged();
        }
    }
    
    virtual void notifyDataSetChanged() {
        sort(mObjects.begin(), mObjects.end(), mComparator);
        BaseAdapter::notifyDataSetChanged();
    }
    
    virtual void notifyDataSetInvalidated() {
        sort(mObjects.begin(), mObjects.end(), mComparator);
        BaseAdapter::notifyDataSetInvalidated();
    }
    
    long getItemId(int position) {
        return position;
    }
    
    int getPosition(T item) {
        return mObjects.indexOf(item);
    }
    
    T *getItem(int position) {
        return mObjects.get(position);
    }
    
    int getCount() {
        return mObjects.size();
    }
    
    TimeView *getView(int position, View *view, ViewGroup *viewGroup) {
        return getView(position, (TimeView*) view, viewGroup, false);
    }
    
    virtual DateTime *getDate(int position);
    virtual TimeView *getView(int position, TimeView *view, ViewGroup *viewGroup, bool update);
};

ANDROID_END

#endif
