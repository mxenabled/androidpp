//
//  Resource.cpp
//  cocos2dx
//
//  Created by Saul Howard on 11/4/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#include "Resources.h"

#include "Android/content/res/ColorStateList.h"
#include "Android/utils/CCPullParser.h"
#include "Android/graphics/drawable/BitmapDrawable.h"
#include "Android/graphics/drawable/Drawable.h"
#include "Android/graphics/drawable/ColorDrawable.h"
#include "Android/text/String.h"
#include "Android/utils/AttributeSet.h"
#include "Android/utils/System.h"

#include <unicode/unistr.h>

using namespace icu;

ANDROID_BEGIN

map<int, string> Resources::s_resolutions = {
    {0, ""},
    {120, "-ldpi"},
    {160, "-mdpi"},
    {213, "-tvdpi"},
    {240, "-hdpi"},
    {320, "-xhdpi"},
    {480, "-xxhdpi"}
};

map<string, string> Resources::s_drawables = map<string, string>();
map<string, string> Resources::s_strings = map<string, string>();
map<string, string> Resources::s_colors = map<string, string>();
map<string, shared_ptr<ColorStateList>> Resources::s_colorStateLists = map<string, shared_ptr<ColorStateList>>();

Resources::Resources(int width, int height, int densityDpi) : m_displayMetrics() {
    m_displayMetrics.widthPixels = width;
    m_displayMetrics.heightPixels = height;
    m_displayMetrics.densityDpi = densityDpi;
    m_displayMetrics.density = densityDpi * DisplayMetrics::DENSITY_DEFAULT_SCALE;
    m_displayMetrics.scaledDensity = m_displayMetrics.density;
    m_displayMetrics.xdpi = densityDpi;
    m_displayMetrics.ydpi = densityDpi;
    mConfiguration.setToDefaults();
    
    loadStrings();
    loadColors();
}

Resources::~Resources() {
    
}

shared_ptr<ColorStateList> Resources::getCachedColorStateList(string key) {
    // Check our cache first
    map<string, shared_ptr<ColorStateList>>::iterator cached = s_colorStateLists.find(key);
    if (cached != s_colorStateLists.end()) {
        return cached->second;
    }
    
    return NULL;
}

int Resources::getColor(string colorId) {
    
    // Check our cache first
    map<string, string>::iterator cached = s_colors.find(colorId);
    if (cached != s_colors.end()) {
        return Color::parseColor(cached->second);
    }
    
    return -1;
}

DisplayMetrics Resources::getDisplayMetrics() {
    return m_displayMetrics;
}

shared_ptr<ColorStateList> Resources::getColorStateList(string colorStateListId) {
    
    shared_ptr<ColorStateList> csl;
    
    int color = getColor(colorStateListId);
    
    if (color != -1) {
        
        csl = ColorStateList::valueOf(color);
        
        return csl;
    }
    
    csl = getCachedColorStateList(colorStateListId);
    if (csl != NULL) {
        return csl;
    }
    
    cocos2d::CCFileUtils *fileUtils = cocos2d::CCFileUtils::sharedFileUtils();
    
    string path;
    
    size_t found = colorStateListId.find("@color/");
    if (found != string::npos) {
        colorStateListId = colorStateListId.substr(7, colorStateListId.size());
        path = "res/color/" + colorStateListId + ".xml";
    }
    
    found = colorStateListId.find("@drawable/");
    if (found != string::npos) {
        colorStateListId = colorStateListId.substr(10, colorStateListId.size());
        path = "res/drawable/" + colorStateListId + ".xml";
    }
    
    CCPullParser parser = CCPullParser();
    
    if (!parser.init(path.c_str())) {
        csl = ColorStateList::createFromXml(this, parser);
    }
    
    if (csl != NULL) {
        s_colorStateLists.insert(pair<string, shared_ptr<ColorStateList>>(colorStateListId, csl));
    }
    
    return csl;
}

shared_ptr<Drawable> Resources::getDrawable(string name) {
    
    if (name.find("@drawable/") != std::string::npos) {
        
        name = name.substr(10, name.size());
        
        cocos2d::CCFileUtils *fileUtils = cocos2d::CCFileUtils::sharedFileUtils();
        
        string tempPath("res/drawable/" + name + ".xml");
        
        // TODO: deal with xml drawables
        if (fileUtils->isFileExist(tempPath)) {
            
            CCPullParser parser = CCPullParser();
            
            if (parser.init(tempPath.c_str())) {
                return Drawable::createFromXmlInner(this, parser);
            }
            
            return NULL;
        }
        
        string path = getBitmapPath(name);
        
        return make_shared<BitmapDrawable>(path.c_str());
        
    } else {
        
        // We have a color to parse, TODO: account for @color/white values
        int color = Color::parseColor(name);
        return make_shared<ColorDrawable>(color);
    }
    
    return NULL;
}

string Resources::getBitmapPath(string name) {
    
    int dpi = getDisplayMetrics().densityDpi;
    cocos2d::CCFileUtils *fileUtils = cocos2d::CCFileUtils::sharedFileUtils();
    
    // Check our cache first
    map<string, string>::iterator cached = s_drawables.find(name);
    if (cached != s_drawables.end()) {
        return cached->second;
    }
    
    int closest = dpi;
    string path;
    typedef map<int, string>::iterator it_type;
    
    // Find the highest resolution resource we have
    for (it_type iterator = s_resolutions.begin(); iterator != s_resolutions.end(); iterator++) {
        
        int distance = abs(iterator->first - dpi);
        string tempPath("res/drawable" + iterator->second + "/" + name + ".png");
        
        if (closest > distance && fileUtils->isFileExist(tempPath)) {
            closest = distance;
            path = tempPath;
        }
    }
    
    if (path.empty()) return NULL;
    
    s_drawables.insert(pair<string, string>(name, path));
    
    return path;
}

Configuration Resources::getConfiguration() {
    return mConfiguration;
}

string Resources::getString(string stringId) {
    
    // Check our cache first
    map<string, string>::iterator cached = s_strings.find(stringId);
    if (cached != s_strings.end()) {
        return cached->second;
    }
    
    return string();
}

shared_ptr<CharSequence> Resources::getText(string resId) {
    string value = getString(resId);
    
    UnicodeString val = System::convert(value);
    
    return make_shared<String>(val);
}

void Resources::loadStrings() {
    loadValues("res/values/strings.xml", s_strings, "string");
}

void Resources::loadColors() {
    loadValues("res/values/color.xml", s_colors, "color");
}

void Resources::loadValues(const char* file, map<string, string> &map, const char* type) {
    
    CCPullParser parser = CCPullParser();
    
    if (parser.init(file)) {
        
        string res(parser.getName());
        
        if (res.compare("resources") == 0 && parser.hasChildren()) {
            
            parser.firstChild();
            
            do {
                string node(parser.getName());
                
                // Temporary condition because we only support string value nodes
                if (node.compare(type) == 0) {
                    
                    parser.loadAttributes();
                    
                    string name(parser.getAttributeValue("name"));
                    string value(parser.getValue());
                    
                    map.insert(pair<string, string>(name, value));
                }
                
            } while (parser.next());
        }
    }
}

ANDROID_END