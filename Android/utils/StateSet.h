//
//  StateSet.h
//  cocos2dx
//
//  Created by Saul Howard on 11/22/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef cocos2dx_StateSet_h
#define cocos2dx_StateSet_h

#include "AndroidMacros.h"

#include <vector>
#include <string>

using namespace std;

ANDROID_BEGIN

class StateSet {
    
public:
    StateSet() {}
    
    static const vector<int> WILD_CARD;
    static const vector<int> NOTHING;
    
    /**
     * Return whether the stateSetOrSpec is matched by all StateSets.
     *
     * @param stateSetOrSpec a state set or state spec.
     */
    static bool isWildCard(vector<int> stateSetOrSpec) {
        return stateSetOrSpec.size() == 0 || stateSetOrSpec[0] == 0;
    }
    
    static int getStateValue(string state);
    
    /**
     * Return whether the stateSet matches the desired stateSpec.
     *
     * @param stateSpec an array of required (if positive) or
     *        prohibited (if negative) {@link android.view.View} states.
     * @param stateSet an array of {@link android.view.View} states
     */
    static bool stateSetMatches(vector<int> stateSpec, vector<int> stateSet) {
        
        if (stateSet.empty()) {
            return (stateSpec.empty() || isWildCard(stateSpec));
        }
        
        typedef vector<int>::const_iterator iter;
        
        for (iter it = stateSpec.begin(); it != stateSpec.end(); ++it) {
            int stateSpecState = (*it);
            
            if (stateSpecState == 0) {
                // We've reached the end of the cases to match against.
                return true;
            }
            bool mustMatch = false;
            if (stateSpecState > 0) {
                mustMatch = true;
            } else {
                // We use negative values to indicate must-NOT-match states.
                mustMatch = false;
                stateSpecState = -stateSpecState;
            }
            bool found = false;
            for (iter jt = stateSet.begin(); jt != stateSet.end(); ++jt) {
                const int state = *jt;
                if (state == 0) {
                    // We've reached the end of states to match.
                    if (mustMatch) {
                        // We didn't find this must-match state.
                        return false;
                    } else {
                        // Continue checking other must-not-match states.
                        break;
                    }
                }
                if (state == stateSpecState) {
                    if (mustMatch) {
                        found = true;
                        // Continue checking other other must-match states.
                        break;
                    } else {
                        // Any match of a must-not-match state returns false.
                        return false;
                    }
                }
            }
            if (mustMatch && !found) {
                // We've reached the end of states to match and we didn't
                // find a must-match state.
                return false;
            }
        }
        return true;
    }
    
    /**
     * Return whether the state matches the desired stateSpec.
     *
     * @param stateSpec an array of required (if positive) or
     *        prohibited (if negative) {@link android.view.View} states.
     * @param state a {@link android.view.View} state
     */
    static bool stateSetMatches(vector<int> stateSpec, int state) {
        typedef vector<int>::const_iterator iter;
        for (iter it = stateSpec.begin(); it != stateSpec.end(); ++it) {
            const int stateSpecState = *it;
            if (stateSpecState == 0) {
                // We've reached the end of the cases to match against.
                return true;
            }
            if (stateSpecState > 0) {
                if (state != stateSpecState) {
                    return false;
                }
            } else {
                // We use negative values to indicate must-NOT-match states.
                if (state == -stateSpecState) {
                    // We matched a must-not-match case.
                    return false;
                }
            }
        }
        return true;
    }
    
    static void trimStateSet(vector<int> &states, int newSize) {
        if (states.size() == newSize) {
            return;
        }
        states.resize(newSize);
    }
};

ANDROID_END

#endif
