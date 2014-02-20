//
//  HeaderViewListAdapter.cpp
//  Androidpp
//
//  Created by Saul Howard on 2/11/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#include "HeaderViewListAdapter.h"

#include "Android/widget/AdapterView.h"

ANDROID_BEGIN

vector<ListView::FixedViewInfo> HeaderViewListAdapter::EMPTY_INFO_LIST = vector<ListView::FixedViewInfo>();

HeaderViewListAdapter::HeaderViewListAdapter(vector<ListView::FixedViewInfo> &headerViewInfos,
                      vector<ListView::FixedViewInfo> &footerViewInfos,
                                             shared_ptr<ListAdapter> adapter) : mHeaderViewInfos(headerViewInfos), mFooterViewInfos(footerViewInfos) {
    mAdapter = adapter;
    mFilterableAdapter = dynamic_pointer_cast<Filterable>(mAdapter);
    mIsFilterable = (mFilterableAdapter != NULL);
    
    mAreAllFixedViewsSelectable =
    areAllListInfosSelectable(mHeaderViewInfos)
    && areAllListInfosSelectable(mFooterViewInfos);
}

bool HeaderViewListAdapter::areAllListInfosSelectable(vector<ListView::FixedViewInfo> infos) {

    for (ListView::FixedViewInfo info : infos) {
        if (!info.isSelectable) {
            return false;
        }
    }
    
    return true;
}

bool HeaderViewListAdapter::removeHeader(shared_ptr<View> v) {
    for (int i = 0; i < mHeaderViewInfos.size(); i++) {
        ListView::FixedViewInfo info = mHeaderViewInfos[i];
        if (info.view == v) {
            mHeaderViewInfos.erase(mHeaderViewInfos.begin() + i);
            
            mAreAllFixedViewsSelectable =
            areAllListInfosSelectable(mHeaderViewInfos)
            && areAllListInfosSelectable(mFooterViewInfos);
            
            return true;
        }
    }
    
    return false;
}

bool HeaderViewListAdapter::removeFooter(shared_ptr<View> v) {
    for (int i = 0; i < mFooterViewInfos.size(); i++) {
        ListView::FixedViewInfo info = mFooterViewInfos[i];
        if (info.view == v) {
            mFooterViewInfos.erase(mFooterViewInfos.begin() + i);
            
            mAreAllFixedViewsSelectable =
            areAllListInfosSelectable(mHeaderViewInfos)
            && areAllListInfosSelectable(mFooterViewInfos);
            
            return true;
        }
    }
    
    return false;
}

int HeaderViewListAdapter::getCount() {
    if (mAdapter != NULL) {
        return getFootersCount() + getHeadersCount() + mAdapter->getCount();
    } else {
        return getFootersCount() + getHeadersCount();
    }
}

bool HeaderViewListAdapter::areAllItemsEnabled() {
    if (mAdapter != NULL) {
        return mAreAllFixedViewsSelectable && mAdapter->areAllItemsEnabled();
    } else {
        return true;
    }
}

bool HeaderViewListAdapter::isEnabled(int position) {
    // Header (negative positions will throw an ArrayIndexOutOfBoundsException)
    int numHeaders = getHeadersCount();
    if (position < numHeaders) {
        return mHeaderViewInfos[position].isSelectable;
    }
    
    // Adapter
    const int adjPosition = position - numHeaders;
    int adapterCount = 0;
    if (mAdapter != NULL) {
        adapterCount = mAdapter->getCount();
        if (adjPosition < adapterCount) {
            return mAdapter->isEnabled(adjPosition);
        }
    }
    
    // Footer (off-limits positions will throw an ArrayIndexOutOfBoundsException)
    return mFooterViewInfos[adjPosition - adapterCount].isSelectable;
}

shared_ptr<Object> HeaderViewListAdapter::getItem(int position) {
    // Header (negative positions will throw an ArrayIndexOutOfBoundsException)
    int numHeaders = getHeadersCount();
    if (position < numHeaders) {
        return mHeaderViewInfos[position].data;
    }
    
    // Adapter
    const int adjPosition = position - numHeaders;
    int adapterCount = 0;
    if (mAdapter != NULL) {
        adapterCount = mAdapter->getCount();
        if (adjPosition < adapterCount) {
            return mAdapter->getItem(adjPosition);
        }
    }
    
    // Footer (off-limits positions will throw an ArrayIndexOutOfBoundsException)
    return mFooterViewInfos[adjPosition - adapterCount].data;
}

long HeaderViewListAdapter::getItemId(int position) {
    int numHeaders = getHeadersCount();
    if (mAdapter != NULL && position >= numHeaders) {
        int adjPosition = position - numHeaders;
        int adapterCount = mAdapter->getCount();
        if (adjPosition < adapterCount) {
            return mAdapter->getItemId(adjPosition);
        }
    }
    return -1;
}

bool HeaderViewListAdapter::hasStableIds() {
    if (mAdapter != NULL) {
        return mAdapter->hasStableIds();
    }
    return false;
}

shared_ptr<View> HeaderViewListAdapter::getView(int position, shared_ptr<View> convertView, ViewGroup *parent) {
    // Header (negative positions will throw an ArrayIndexOutOfBoundsException)
    int numHeaders = getHeadersCount();
    if (position < numHeaders) {
        return mHeaderViewInfos[position].view;
    }
    
    // Adapter
    const int adjPosition = position - numHeaders;
    int adapterCount = 0;
    if (mAdapter != NULL) {
        adapterCount = mAdapter->getCount();
        if (adjPosition < adapterCount) {
            return mAdapter->getView(adjPosition, convertView, parent);
        }
    }
    
    // Footer (off-limits positions will throw an ArrayIndexOutOfBoundsException)
    return mFooterViewInfos[adjPosition - adapterCount].view;
}

int HeaderViewListAdapter::getItemViewType(int position) {
    int numHeaders = getHeadersCount();
    if (mAdapter != NULL && position >= numHeaders) {
        int adjPosition = position - numHeaders;
        int adapterCount = mAdapter->getCount();
        if (adjPosition < adapterCount) {
            return mAdapter->getItemViewType(adjPosition);
        }
    }
    
    return ITEM_VIEW_TYPE_HEADER_OR_FOOTER;
}

int HeaderViewListAdapter::getViewTypeCount() {
    if (mAdapter != NULL) {
        return mAdapter->getViewTypeCount();
    }
    return 1;
}

void HeaderViewListAdapter::registerDataSetObserver(DataSetObserver *observer) {
    if (mAdapter != NULL) {
        mAdapter->registerDataSetObserver(observer);
    }
}

void HeaderViewListAdapter::unregisterDataSetObserver(DataSetObserver *observer) {
    if (mAdapter != NULL) {
        mAdapter->unregisterDataSetObserver(observer);
    }
}

shared_ptr<Filter> HeaderViewListAdapter::getFilter() {
    if (mIsFilterable) {
        return mFilterableAdapter->getFilter();
    }
    return NULL;
}

ANDROID_END