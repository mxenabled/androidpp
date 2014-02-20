//
//  ColorStateList.h
//  Androidpp
//
//  Created by Saul Howard on 1/15/14.
//  Copyright (c) 2014 MoneyDesktop. All rights reserved.
//

#ifndef __Androidpp__ColorStateList__
#define __Androidpp__ColorStateList__

#include "AndroidMacros.h"

#include <vector>
#include <map>
#include <string>
#include <memory>

using namespace std;

ANDROID_BEGIN

class Resources;
class CCPullParser;
class AttributeSet;

/**
 *
 * Lets you map {@link android.view.View} state sets to colors.
 *
 * {@link android.content.res.ColorStateList}s are created from XML resource files defined in the
 * "color" subdirectory directory of an application's resource directory.  The XML file contains
 * a single "selector" element with a number of "item" elements inside.  For example:
 *
 * <pre>
 * &lt;selector xmlns:android="http://schemas.android.com/apk/res/android"&gt;
 *   &lt;item android:state_focused="true" android:color="@color/testcolor1"/&gt;
 *   &lt;item android:state_pressed="true" android:state_enabled="false" android:color="@color/testcolor2" /&gt;
 *   &lt;item android:state_enabled="false" android:color="@color/testcolor3" /&gt;
 *   &lt;item android:color="@color/testcolor5"/&gt;
 * &lt;/selector&gt;
 * </pre>
 *
 * This defines a set of state spec / color pairs where each state spec specifies a set of
 * states that a view must either be in or not be in and the color specifies the color associated
 * with that spec.  The list of state specs will be processed in order of the items in the XML file.
 * An item with no state spec is considered to match any set of states and is generally useful as
 * a final item to be used as a default.  Note that if you have such an item before any other items
 * in the list then any subsequent items will end up being ignored.
 * <p>For more information, see the guide to <a
 * href="{@docRoot}guide/topics/resources/color-list-resource.html">Color State
 * List Resource</a>.</p>
 */
class ColorStateList {
    
private:
    
    vector<vector<int>> mStateSpecs; // must be parallel to mColors
    vector<int> mColors;      // must be parallel to mStateSpecs
    int mDefaultColor = 0xffff0000;
    
    static vector<vector<int>> EMPTY;
    static map<int, shared_ptr<ColorStateList>> sCache;
    
public:
    
    ColorStateList() { }
    
    /**
     * Creates a ColorStateList that returns the specified mapping from
     * states to colors.
     */
    ColorStateList(vector<vector<int>> states, vector<int> colors);
    
    /**
     * Creates or retrieves a ColorStateList that always returns a single color.
     */
    static shared_ptr<ColorStateList> valueOf(int color);
    
    /**
     * Create a ColorStateList from an XML document, given a set of {@link Resources}.
     */
    static shared_ptr<ColorStateList> createFromXml(Resources *r, CCPullParser &parser);
    
    /* Create from inside an XML document.  Called on a parser positioned at
     * a tag in an XML document, tries to create a ColorStateList from that tag.
     * Returns null if the tag is not a valid ColorStateList.
     */
    static shared_ptr<ColorStateList> createFromXmlInner(Resources *r, CCPullParser &parser,
                                             AttributeSet *attrs);
    
    /**
     * Creates a new ColorStateList that has the same states and
     * colors as this one but where each color has the specified alpha value
     * (0-255).
     */
    shared_ptr<ColorStateList> withAlpha(int alpha);
    
private:
    
    /**
     * Fill in this object based on the contents of an XML "selector" element.
     */
    void inflate(Resources *r, CCPullParser &parser, AttributeSet *attrs);
    
public:
    
    bool isStateful() { return mStateSpecs.size() > 1; }
    
    /**
     * Return the color associated with the given set of {@link android.view.View} states.
     *
     * @param stateSet an array of {@link android.view.View} states
     * @param defaultColor the color to return if there's not state spec in this
     * {@link ColorStateList} that matches the stateSet.
     *
     * @return the color associated with that set of states in this {@link ColorStateList}.
     */
    int getColorForState(vector<int> stateSet, int defaultColor);
    
    /**
     * Return the default color in this {@link ColorStateList}.
     *
     * @return the default color in this {@link ColorStateList}.
     */
    int getDefaultColor() { return mDefaultColor; }
    
    string toString();
};

ANDROID_END

#endif /* defined(__Androidpp__ColorStateList__) */
