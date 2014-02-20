//
//  ColorStateList.cpp
//  Androidpp
//
//  Created by Saul Howard on 1/15/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#include "ColorStateList.h"

#include "Android/content/res/Resources.h"
#include "Android/internal/R.h"
#include "Android/utils/ArrayUtils.h"
#include "Android/utils/CCPullParser.h"
#include "Android/utils/AttributeSet.h"
#include "Android/utils/Exceptions.h"
#include "Android/utils/StateSet.h"
#include "Android/utils/System.h"

#include <sstream>

ANDROID_BEGIN

vector<vector<int>> ColorStateList::EMPTY = vector<vector<int>> { vector<int>(0) };
map<int, shared_ptr<ColorStateList>> ColorStateList::sCache = map<int, shared_ptr<ColorStateList>>();

ColorStateList::ColorStateList(vector<vector<int>> states, vector<int> colors) {
    mStateSpecs = states;
    mColors = colors;
    
    if (states.size() > 0) {
        mDefaultColor = colors[0];
        
        for (int i = 0; i < states.size(); i++) {
            if (states[i].size() == 0) {
                mDefaultColor = colors[i];
            }
        }
    }
}

/**
 * Creates or retrieves a ColorStateList that always returns a single color.
 */
shared_ptr<ColorStateList> ColorStateList::valueOf(int color) {
    // TODO: should we collect these eventually?
//    synchronized (sCache) {
        shared_ptr<ColorStateList> ref = sCache[color];
        
        if (ref != NULL) {
            return ref;
        }
        
        ref = make_shared<ColorStateList>(EMPTY, vector<int> { color });
        sCache[color] = ref;
        return ref;
//    }
}

/**
 * Create a ColorStateList from an XML document, given a set of {@link Resources}.
 */
shared_ptr<ColorStateList> ColorStateList::createFromXml(Resources *r, CCPullParser &parser) {
    
    AttributeSet *attrs = (AttributeSet*) &parser;
    
    return createFromXmlInner(r, parser, attrs);
}

/* Create from inside an XML document.  Called on a parser positioned at
 * a tag in an XML document, tries to create a ColorStateList from that tag.
 * Returns NULL if the tag is not a valid ColorStateList.
 */
shared_ptr<ColorStateList> ColorStateList::createFromXmlInner(Resources *r, CCPullParser &parser, AttributeSet *attrs) {
    
    shared_ptr<ColorStateList> colorStateList = NULL;
    
    string name = parser.getName();
    
    if (name.compare("selector") == 0) {
        colorStateList = make_shared<ColorStateList>();
    } else {
        string err("invalid drawable tag ");
        err.append(name);
        throw CCPullParserException(err);
    }
    
    colorStateList->inflate(r, parser, attrs);
    return colorStateList;
}

/**
 * Creates a new ColorStateList that has the same states and
 * colors as this one but where each color has the specified alpha value
 * (0-255).
 */
shared_ptr<ColorStateList> ColorStateList::withAlpha(int alpha) {
    vector<int> colors = vector<int>(mColors.size());
    
    int len = colors.size();
    for (int i = 0; i < len; i++) {
        colors[i] = (mColors[i] & 0xFFFFFF) | (alpha << 24);
    }
    
    return make_shared<ColorStateList>(mStateSpecs, colors);
}

/**
 * Fill in this object based on the contents of an XML "selector" element.
 */
void ColorStateList::inflate(Resources *r, CCPullParser &parser, AttributeSet *attrs) {
    
    attrs->loadAttributes();
    
    int type;
    
    int listAllocated = 20;
    int listSize = 0;
    vector<int> colorList = vector<int>(listAllocated);
    vector<vector<int>> stateSpecList = vector<vector<int>>(listAllocated);
    
    if (parser.hasChildren()) {
        
        parser.firstChild();
        
        do {
            
            attrs->loadAttributes();
            const std::string name(parser.getName());
            
            if (name.compare("item") != 0) {
                continue;
            }
            
            int color = 0xffff0000;
            bool haveColor = false;
            
            int i;
            int j = 0;
            
            const int numAttrs = attrs->getAttributeCount();
            vector<int> stateSpec = vector<int>(numAttrs);
            
            for (i = 0; i < numAttrs; i++) {
                
                string name(attrs->getAttributeName(i));
                
                if (name.empty()) break;
                
                if (name.compare("color") == 0) {
                    
                    color = attrs->getAttributeColorValue(r, R::styleable::color, 0);
                    haveColor = true;
                    
                } else {
                    int stateResId = StateSet::getStateValue(name);
                    stateSpec[j++] = attrs->getAttributeBooleanValue(name.c_str(), false) ? stateResId : -stateResId;
                }
            }
            
            StateSet::trimStateSet(stateSpec, j);
            
            if (!haveColor) {
                throw CCPullParserException("<item> tag requires a 'android:color' attribute.");
            }
            
            if (listSize == 0 || stateSpec.size() == 0) {
                mDefaultColor = color;
            }
            
            if (listSize + 1 >= listAllocated) {
                listAllocated = ArrayUtils::idealIntArraySize(listSize + 1);
                
                vector<int> ncolor = vector<int>(listAllocated);
                System::arraycopy(colorList, 0, ncolor, 0, listSize);
                
                vector<vector<int>> nstate = vector<vector<int>>(listAllocated);
                System::arraycopy(stateSpecList, 0, nstate, 0, listSize);
                
                colorList = ncolor;
                stateSpecList = nstate;
            }
            
            colorList[listSize] = color;
            stateSpecList[listSize] = stateSpec;
            listSize++;
            
        } while (parser.next());
        
        parser.toParent();
    }
    
    mColors = vector<int>(listSize);
    mStateSpecs = vector<vector<int>>(listSize);
    System::arraycopy(colorList, 0, mColors, 0, listSize);
    System::arraycopy(stateSpecList, 0, mStateSpecs, 0, listSize);
}

/**
 * Return the color associated with the given set of {@link android.view.View} states.
 *
 * @param stateSet an array of {@link android.view.View} states
 * @param defaultColor the color to return if there's not state spec in this
 * {@link ColorStateList} that matches the stateSet.
 *
 * @return the color associated with that set of states in this {@link ColorStateList}.
 */
int ColorStateList::getColorForState(vector<int> stateSet, int defaultColor) {
    const int setLength = mStateSpecs.size();
    for (int i = 0; i < setLength; i++) {
        vector<int> stateSpec = mStateSpecs[i];
        if (StateSet::stateSetMatches(stateSpec, stateSet)) {
            return mColors[i];
        }
    }
    return defaultColor;
}

string ColorStateList::toString() {
    stringstream str;
    str << "ColorStateList{mStateSpecs=mDefaultColor=" << mDefaultColor;
    return str.str();
//    return "ColorStateList{" +
//    "mStateSpecs=" + Arrays.deepToString(mStateSpecs) +
//    "mColors=" + Arrays.toString(mColors) +
//    "mDefaultColor=" + mDefaultColor + '}';
}

ANDROID_END