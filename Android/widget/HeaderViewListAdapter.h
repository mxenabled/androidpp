//
//  HeaderViewListAdapter.h
//  Androidpp
//
//  Created by Saul Howard on 2/11/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef __Androidpp__HeaderViewListAdapter__
#define __Androidpp__HeaderViewListAdapter__

#include "AndroidMacros.h"

#include "Android/widget/Filterable.h"
#include "Android/widget/ListView.h"
#include "Android/widget/WrapperListAdapter.h"

#include <memory>
#include <vector>

using namespace std;

ANDROID_BEGIN

class Filter;
class Object;
class View;

/**
 * ListAdapter used when a ListView has header views. This ListAdapter
 * wraps another one and also keeps track of the header views and their
 * associated data objects.
 *<p>This is intended as a base class; you will probably not need to
 * use this class directly in your own code.
 */
class HeaderViewListAdapter : public WrapperListAdapter, public Filterable {
    
private:
    
    shared_ptr<ListAdapter> mAdapter;
    shared_ptr<Filterable> mFilterableAdapter;
    
    /**
     * The item view type returned by {@link Adapter#getItemViewType(int)} when
     * the item is a header or footer.
     */
    static const int ITEM_VIEW_TYPE_HEADER_OR_FOOTER = -2;
    
protected:
    
    // These two vector are assumed to NOT be NULL.
    // They are indeed created when declared in ListView and then shared.
    vector<ListView::FixedViewInfo> &mHeaderViewInfos;
    vector<ListView::FixedViewInfo> &mFooterViewInfos;
    
    // Used as a placeholder in case the provided info views are indeed NULL.
    // Currently only used by some CTS tests, which may be removed.
    static vector<ListView::FixedViewInfo> EMPTY_INFO_LIST;
    
    bool mAreAllFixedViewsSelectable;
    
private:
    
    bool mIsFilterable = false;
    
public:
    
    HeaderViewListAdapter(vector<ListView::FixedViewInfo> &headerViewInfos,
                                 vector<ListView::FixedViewInfo> &footerViewInfos,
                          shared_ptr<ListAdapter> adapter);
    
    int getHeadersCount() {
        return mHeaderViewInfos.size();
    }
    
    int getFootersCount() {
        return mFooterViewInfos.size();
    }
    
    bool isEmpty() {
        return mAdapter == NULL || mAdapter->isEmpty();
    }
    
private:
    
    bool areAllListInfosSelectable(vector<ListView::FixedViewInfo> infos);
    
public:
    
    bool removeHeader(shared_ptr<View> v);
    
    bool removeFooter(shared_ptr<View> v);
    
    int getCount();
    
    bool areAllItemsEnabled();
    
    bool isEnabled(int position);
    
    shared_ptr<Object> getItem(int position);
    
    long getItemId(int position);
    
    bool hasStableIds();
    
    shared_ptr<View> getView(int position, shared_ptr<View> convertView, ViewGroup *parent);
    
    int getItemViewType(int position);
    
    int getViewTypeCount();
    
    void registerDataSetObserver(DataSetObserver *observer);
    
    void unregisterDataSetObserver(DataSetObserver *observer);
    
    shared_ptr<Filter> getFilter();
    
    shared_ptr<ListAdapter> getWrappedAdapter() {
        return mAdapter;
    }
};

ANDROID_END

#endif /* defined(__Androidpp__HeaderViewListAdapter__) */
