//
//  StateListDrawable.h
//  cocos2dx
//
//  Created by Saul Howard on 11/22/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef __cocos2dx__StateListDrawable__
#define __cocos2dx__StateListDrawable__

#include "AndroidMacros.h"

#include "Android/graphics/drawable/DrawableContainer.h"
#include "Android/graphics/drawable/DrawableContainerState.h"

#include "Android/utils/StateSet.h"

#include <vector>

using namespace std;

ANDROID_BEGIN

class StateListDrawable : public DrawableContainer {

public:
    
    StateListDrawable() : StateListDrawable(StateListState(), NULL) {
    }
    
    void addState(vector<int> stateSet, shared_ptr<Drawable> drawable);
    
    int getStateCount();
    shared_ptr<Drawable> getStateDrawable(int index);
    int getStateDrawableIndex(vector<int> stateSet);
    vector<int> getStateSet(int index);
    
    virtual void inflate(Resources *r, CCPullParser &parser, AttributeSet *attrs);
    virtual bool isStateful();
    virtual shared_ptr<Drawable> mutate();
    
protected:
    
    virtual bool onStateChange(vector<int> stateSet);
    
private:
    
    class StateListState : public DrawableContainerState {
        
        friend class StateListDrawable;
        
    public:
        vector<vector<int>> mStateSets;
        
        StateListState() {}
        
        StateListState(StateListState &orig, StateListDrawable *owner, Resources *res) : DrawableContainerState(&orig, owner, res) {
            
//            if (orig) {
                mStateSets = orig.mStateSets;
//            } else {
//                mStateSets = vector<vector<int>>(getChildren().size());
//            }
        }
        
        int addStateSet(vector<int> stateSet, shared_ptr<Drawable> drawable) {
            const int pos = addChild(drawable);
            mStateSets[pos] = stateSet;
            return pos;
        }
        
        virtual shared_ptr<Drawable> newDrawable() {
            return make_shared<StateListDrawable>(*this, (Resources*) NULL);
        }
        
        virtual shared_ptr<Drawable> newDrawable(Resources *res) {
            return make_shared<StateListDrawable>(*this, res);
        }
        
        virtual void growArray(int oldSize, int newSize) {
            DrawableContainerState::growArray(oldSize, newSize);
            mStateSets.resize(newSize);
        }
        
    private:
        
        int indexOfStateSet(vector<int> stateSet) {
            const vector<vector<int>> &stateSets = mStateSets;
            const int N = getChildCount();
            for (int i = 0; i < N; i++) {
                if (StateSet::stateSetMatches(stateSets[i], stateSet)) {
                    return i;
                }
            }
            return -1;
        }
    };
    
    static const bool DEFAULT_DITHER = true;
    StateListState mStateListState;
    bool mMutated = false;
    
    StateListState getStateListState();
    int getStateValue(string state);
    
public:
    
    StateListDrawable(StateListState state, Resources *res);
};

ANDROID_END

#endif /* defined(__cocos2dx__StateListDrawable__) */
