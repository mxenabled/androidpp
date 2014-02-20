//
//  TabStops.h
//  Androidpp
//
//  Created by Saul Howard on 1/22/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef Androidpp_TabStops_h
#define Androidpp_TabStops_h

#include "AndroidMacros.h"

#include "Android/text/style/TabStopSpan.h"
#include "Android/utils/Object.h"

#include <algorithm>
#include <vector>
#include <memory>

using namespace std;

ANDROID_BEGIN

class TabStopsBase {
    
public:
    
    static float nextDefaultStop(float h, int inc) {
        return ((int) ((h + inc) / inc)) * inc;
    }
};

template<class T>
class TabStops : public TabStopsBase {
    
private:
    
    vector<int> mStops;
    int mNumStops;
    int mIncrement;
    
public:
    
    TabStops(int increment, vector<shared_ptr<T>> spans) {
        reset(increment, spans);
    }
    
    void reset(int increment, vector<shared_ptr<T>> spans) {
        
        mIncrement = increment;
        
        int ns = 0;
        vector<int> &stops = mStops;
        
        for (shared_ptr<T> o : spans) {
            
            shared_ptr<TabStopSpan> tss = dynamic_pointer_cast<TabStopSpan>(o);
            
            if (tss != NULL) {
                if (ns == stops.size()) {
                    vector<int> nstops = vector<int>(ns * 2);
                    for (int i = 0; i < ns; ++i) {
                        nstops[i] = stops[i];
                    }
                    stops = nstops;
                }
                stops[ns++] = tss->getTabStop();
            }
        }
        
        if (ns > 1) {
            sort(stops.begin(), stops.begin() + ns);
        }
        
        if (stops != mStops) {
            mStops = stops;
        }
        
        mNumStops = ns;
    }
    
    float nextTab(float h) {
        int ns = mNumStops;
        if (ns > 0) {
            vector<int> &stops = mStops;
            for (int i = 0; i < ns; ++i) {
                int stop = stops[i];
                if (stop > h) {
                    return stop;
                }
            }
        }
        return nextDefaultStop(h, mIncrement);
    }
};

ANDROID_END

#endif
