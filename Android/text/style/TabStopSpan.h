//
//  TabStopSpan.h
//  Androidpp
//
//  Created by Saul Howard on 1/22/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef Androidpp_TabStopSpan_h
#define Androidpp_TabStopSpan_h

#include "AndroidMacros.h"

#include "Android/text/style/ParagraphStyle.h"

ANDROID_BEGIN

/**
 * Represents a single tab stop on a line.
 */
class TabStopSpan : public ParagraphStyle {
    
public:
    
    /**
     * Returns the offset of the tab stop from the leading margin of the
     * line.
     * @return the offset
     */
    virtual int getTabStop() = 0;
    
    virtual string getType() {
        return "TabStopSpan";
    };
    
    /**
     * The default implementation of TabStopSpan.
     */
    class Standard;
};

class TabStopSpan::Standard : public TabStopSpan {
    
public:
    
    /**
     * Constructor.
     *
     * @param where the offset of the tab stop from the leading margin of
     *        the line
     */
    Standard(int where) {
        mTab = where;
    }
    
    int getTabStop() {
        return mTab;
    }
    
private:
    
    int mTab;
};

ANDROID_END

#endif
