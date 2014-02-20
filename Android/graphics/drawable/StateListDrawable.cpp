//
//  StateListDrawable.cpp
//  cocos2dx
//
//  Created by Saul Howard on 11/22/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#include "StateListDrawable.h"

#include "Android/internal/R.h"

ANDROID_BEGIN

/**
 * Add a new image/string ID to the set of images.
 *
 * @param stateSet - An array of resource Ids to associate with the image.
 *                 Switch to this image by calling setState().
 * @param drawable -The image to show.
 */
void StateListDrawable::addState(vector<int> stateSet, shared_ptr<Drawable> drawable) {
    if (drawable != NULL) {
        mStateListState.addStateSet(stateSet, drawable);
        // in case the new state matches our current state...
        onStateChange(getState());
    }
}

bool StateListDrawable::isStateful() {
    return true;
}

bool StateListDrawable::onStateChange(vector<int> stateSet) {
    int idx = mStateListState.indexOfStateSet(stateSet);
    if (idx < 0) {
        idx = mStateListState.indexOfStateSet(StateSet::WILD_CARD);
    }
    if (selectDrawable(idx)) {
        return true;
    }
    return DrawableContainer::onStateChange(stateSet);
}

void StateListDrawable::inflate(Resources *r, CCPullParser &parser, AttributeSet *attrs) {
    DrawableContainer::inflateWithAttributes(r, parser, attrs, R::styleable::visible);
    
    attrs->loadAttributes();
    
    mStateListState.setVariablePadding(attrs->getAttributeBooleanValue(R::styleable::variablePadding, false));
    mStateListState.setConstantSize(attrs->getAttributeBooleanValue(R::styleable::constantSize, false));
    mStateListState.setEnterFadeDuration(attrs->getAttributeIntValue(R::styleable::enterFadeDuration, 0));
    mStateListState.setExitFadeDuration(attrs->getAttributeIntValue(R::styleable::exitFadeDuration, 0));
    
    setDither(attrs->getAttributeBooleanValue(R::styleable::dither, DEFAULT_DITHER));
    
    if (parser.hasChildren()) {
        
        parser.firstChild();
        
        do {
            
            attrs->loadAttributes();
            
            string drawableRes;
            int j = 0;
            int numAttrs = attrs->getAttributeCount();
            
            vector<int> states = vector<int>(numAttrs);
            
            for (int i = 0; i < numAttrs; i++) {
                
                string name(attrs->getAttributeName(i));
                
                if (name.empty()) break;
                if (name.compare("drawable") == 0) {
                    drawableRes = string(attrs->getAttributeValue(name.c_str()));
                } else {
                    int stateResId = StateSet::getStateValue(name);
                    states[j++] = attrs->getAttributeBooleanValue(name.c_str(), false) ? stateResId : -stateResId;
                }
            }
            
            StateSet::trimStateSet(states, j);
            
            shared_ptr<Drawable> dr = NULL;
            if (!drawableRes.empty()) {
                dr = r->getDrawable(drawableRes);
            } else {
                parser.firstChild();
                dr = Drawable::createFromXmlInner(r, parser);
                parser.toParent();
            }
            
            mStateListState.addStateSet(states, dr);
            
        } while (parser.next());
        
        parser.toParent();
    }
    
    onStateChange(getState());
}

StateListDrawable::StateListState StateListDrawable::getStateListState() {
    return mStateListState;
}

/**
 * Gets the number of states contained in this drawable.
 *
 * @return The number of states contained in this drawable.
 * @hide pending API council
 * @see #getStateSet(int)
 * @see #getStateDrawable(int)
 */
int StateListDrawable::getStateCount() {
    return mStateListState.getChildCount();
}

/**
 * Gets the state set at an index.
 *
 * @param index The index of the state set.
 * @return The state set at the index.
 * @hide pending API council
 * @see #getStateCount()
 * @see #getStateDrawable(int)
 */
vector<int> StateListDrawable::getStateSet(int index) {
    return mStateListState.mStateSets[index];
}

/**
 * Gets the drawable at an index.
 *
 * @param index The index of the drawable.
 * @return The drawable at the index.
 * @hide pending API council
 * @see #getStateCount()
 * @see #getStateSet(int)
 */
shared_ptr<Drawable> StateListDrawable::getStateDrawable(int index) {
    return mStateListState.getChildren()[index];
}

/**
 * Gets the index of the drawable with the provided state set.
 *
 * @param stateSet the state set to look up
 * @return the index of the provided state set, or -1 if not found
 * @hide pending API council
 * @see #getStateDrawable(int)
 * @see #getStateSet(int)
 */
int StateListDrawable::getStateDrawableIndex(vector<int> stateSet) {
    return mStateListState.indexOfStateSet(stateSet);
}

shared_ptr<Drawable> StateListDrawable::mutate() {
    
    if (!mMutated && DrawableContainer::mutate() == shared_from_this()) {
        
        const vector<vector<int>> &sets = mStateListState.mStateSets;
        const int count = sets.size();
        
        mStateListState.mStateSets = vector<vector<int>>(count);
        
        for (int i = 0; i < count; i++) {
            const vector<int> set = sets[i];
            if (!set.empty()) {
                mStateListState.mStateSets[i] = set;
            }
        }
        mMutated = true;
    }
    return shared_from_this();
}

StateListDrawable::StateListDrawable(StateListState state, Resources *res) {
    mStateListState = StateListState(state, this, res);
    setConstantState(&mStateListState);
    onStateChange(getState());
}

ANDROID_END