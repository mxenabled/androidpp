//
//  Resource.h
//  cocos2dx
//
//  Created by Saul Howard on 11/4/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef __cocos2dx__Resources__
#define __cocos2dx__Resources__

#include "AndroidMacros.h"

#include "Android/text/CharSequence.h"
#include "Android/utils/DisplayMetrics.h"
#include "Android/content/res/Configuration.h"

#include <string>
#include <map>
#include <memory>

using namespace std;

ANDROID_BEGIN

class ColorStateList;
class Drawable;
class AttributeSet;

class Resources {
public:
    Resources() : Resources(0, 0, 0) {}
    Resources(int width, int height, int densityDpi);
    virtual ~Resources();
    
    int getColor(string colorId);
    DisplayMetrics getDisplayMetrics();
    shared_ptr<Drawable> getDrawable(string name);
    string getBitmapPath(string name);
    string getString(string stringId);
    shared_ptr<CharSequence> getText(string resId);
    shared_ptr<ColorStateList> getColorStateList(string colorStateListId);
    
    /**
     * Return the current configuration that is in effect for this resource
     * object.  The returned object should be treated as read-only.
     *
     * @return The resource's current configuration.
     */
    Configuration getConfiguration();
    
private:
    
    static map<string, string> s_drawables;
    static map<int, string> s_resolutions;
    static map<string, string> s_strings;
    static map<string, string> s_colors;
    static map<string, shared_ptr<ColorStateList>> s_colorStateLists;
    
    DisplayMetrics m_displayMetrics;
    
    shared_ptr<ColorStateList> getCachedColorStateList(string key);
    void loadStrings();
    void loadColors();
    void loadValues(const char* file, map<string, string> &map, const char* type);
    Configuration mConfiguration = Configuration();
};

ANDROID_END

#endif /* defined(__cocos2dx__Resource__) */
