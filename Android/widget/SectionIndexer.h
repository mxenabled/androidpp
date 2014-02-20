//
//  SectionIndexer.h
//  Androidpp
//
//  Created by Saul Howard on 2/11/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef Androidpp_SectionIndexer_h
#define Androidpp_SectionIndexer_h

#include "AndroidMacros.h"

#include <vector>
#include <memory>

using namespace std;

ANDROID_BEGIN

/**
 * Interface that should be implemented on Adapters to enable fast scrolling
 * in an {@link AbsListView} between sections of the list. A section is a group of list items
 * to jump to that have something in common. For example, they may begin with the
 * same letter or they may be songs from the same artist.
 */
class SectionIndexer {
    
public:
    
    /**
     * This provides the list view with an array of section objects. In the simplest
     * case these are Strings, each containing one letter of the alphabet.
     * They could be more complex objects that indicate the grouping for the adapter's
     * consumption. The list view will call toString() on the objects to get the
     * preview letter to display while scrolling.
     * @return the array of objects that indicate the different sections of the list.
     */
    virtual vector<shared_ptr<Object>> getSections() = 0;
    
    /**
     * Provides the starting index in the list for a given section.
     * @param section the index of the section to jump to.
     * @return the starting position of that section. If the section is out of bounds, the
     * position must be clipped to fall within the size of the list.
     */
    virtual int getPositionForSection(int section) = 0;
    
    /**
     * This is a reverse mapping to fetch the section index for a given position
     * in the list.
     * @param position the position for which to return the section
     * @return the section index. If the position is out of bounds, the section index
     * must be clipped to fall within the size of the section array.
     */
    virtual int getSectionForPosition(int position) = 0;
};

ANDROID_END

#endif
